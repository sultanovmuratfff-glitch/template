#!/bin/bash

# Build script for Linux platform
# This script builds the viscopuppy project with all dependencies

set -euo pipefail

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to check if a command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to install system dependencies
install_system_deps() {
    print_info "Installing system dependencies..."
    
    if command_exists apt-get; then
        # Ubuntu/Debian
        sudo apt-get update
        sudo apt-get install -y \
            build-essential \
            cmake \
            ninja-build \
            python3-dev \
            python3-pip \
            python3-venv \
            libpython3-dev \
            git \
            wget \
            curl \
            pkg-config \
            clang \
            clang-tidy \
            clang-format \
            lld \
            ccache
    elif command_exists yum; then
        # RHEL/CentOS
        sudo yum groupinstall -y "Development Tools"
        sudo yum install -y \
            cmake \
            ninja-build \
            python3-devel \
            python3-pip \
            git \
            wget \
            curl \
            pkgconfig \
            clang \
            clang-tools-extra \
            lld \
            ccache
    elif command_exists dnf; then
        # Fedora
        sudo dnf groupinstall -y "Development Tools"
        sudo dnf install -y \
            cmake \
            ninja-build \
            python3-devel \
            python3-pip \
            git \
            wget \
            curl \
            pkgconfig \
            clang \
            clang-tools-extra \
            lld \
            ccache
    else
        print_warning "Unknown package manager. Please install dependencies manually."
    fi
}

# Function to setup uv (modern Python package manager)
setup_uv() {
    print_info "Setting up uv..."
    
    if ! command_exists uv; then
        print_info "Installing uv..."
        curl -LsSf https://astral.sh/uv/install.sh | sh
        export PATH="$HOME/.cargo/bin:$PATH"
    fi
    
    # Create virtual environment
    uv venv .venv --python 3.11
    source .venv/bin/activate
    
    # Install Python dependencies
    uv pip install -e .[dev]
}

# Function to setup Conan
setup_conan() {
    print_info "Setting up Conan..."
    
    if ! command_exists conan; then
        print_info "Installing Conan..."
        pip install conan
    fi
    
    # Create Conan profile if it doesn't exist
    if ! conan profile show default 2>/dev/null; then
        conan profile detect --force
    fi
    
    # Update Conan profile for C++23
    conan profile update settings.compiler.libcxx=libstdc++11 default
    conan profile update settings.cppstd=23 default
}

# Function to build the project
build_project() {
    local build_type="${1:-Release}"
    local generator="${2:-Ninja}"
    local build_dir="build-${build_type}"
    
    print_info "Building project with ${build_type} configuration..."
    
    # Create build directory
    mkdir -p "$build_dir"
    cd "$build_dir"
    
    # Configure with CMake
    cmake .. \
        -DCMAKE_BUILD_TYPE="$build_type" \
        -DCMAKE_CXX_STANDARD=23 \
        -DCMAKE_CXX_STANDARD_REQUIRED=ON \
        -DCMAKE_CXX_EXTENSIONS=OFF \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
        -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
        -G "$generator" \
        -DCMAKE_PREFIX_PATH="$HOME/.conan2" \
        -DCMAKE_MODULE_PATH="$HOME/.conan2" \
        -DCMAKE_INSTALL_PREFIX=../install
    
    # Build
    if [ "$generator" = "Ninja" ]; then
        ninja
    else
        cmake --build . --config "$build_type" --parallel "$(nproc)"
    fi
    
    # Run tests
    print_info "Running tests..."
    if [ "$generator" = "Ninja" ]; then
        ninja test
    else
        ctest --output-on-failure
    fi
    
    # Install
    print_info "Installing..."
    if [ "$generator" = "Ninja" ]; then
        ninja install
    else
        cmake --install . --config "$build_type"
    fi
    
    cd ..
}

# Function to run Python tests
run_python_tests() {
    print_info "Running Python tests..."
    
    source .venv/bin/activate
    
    # Install test dependencies
    uv pip install pytest pytest-benchmark numpy
    
    # Run tests
    python -m pytest python/tests/ -v --benchmark-skip
    
    # Run benchmarks (optional)
    # python -m pytest python/tests/ -v --benchmark-only
}

# Function to run code quality checks
run_code_quality() {
    print_info "Running code quality checks..."
    
    # Run pre-commit hooks
    if command_exists pre-commit; then
        pre-commit run --all-files
    fi
    
    # Run clang-tidy
    if command_exists clang-tidy; then
        print_info "Running clang-tidy..."
        find src include -name "*.cpp" -o -name "*.hpp" | xargs clang-tidy -p build-Release
    fi
    
    # Run cppcheck
    if command_exists cppcheck; then
        print_info "Running cppcheck..."
        cppcheck --enable=all --suppress=missingIncludeSystem --inconclusive src include
    fi
}

# Function to create distribution packages
create_distribution() {
    print_info "Creating distribution packages..."
    
    source .venv/bin/activate
    
    # Build Python wheel
    uv build
    
    # Build documentation (if any)
    if [ -f "docs/conf.py" ]; then
        uv pip install sphinx
        sphinx-build -b html docs docs/_build/html
    fi
}

# Function to clean build artifacts
clean_build() {
    print_info "Cleaning build artifacts..."
    
    rm -rf build-* install .venv .pytest_cache __pycache__ *.egg-info dist
    find . -name "*.pyc" -delete
    find . -name "__pycache__" -type d -exec rm -rf {} +
}

# Main function
main() {
    local action="${1:-all}"
    
    case "$action" in
        "deps")
            install_system_deps
            setup_uv
            setup_conan
            ;;
        "build")
            build_project "Release" "Ninja"
            ;;
        "debug")
            build_project "Debug" "Ninja"
            ;;
        "test")
            run_python_tests
            ;;
        "quality")
            run_code_quality
            ;;
        "dist")
            create_distribution
            ;;
        "clean")
            clean_build
            ;;
        "all")
            install_system_deps
            setup_uv
            setup_conan
            build_project "Release" "Ninja"
            run_python_tests
            run_code_quality
            create_distribution
            ;;
        "help"|"-h"|"--help")
            echo "Usage: $0 [action]"
            echo ""
            echo "Actions:"
            echo "  deps     - Install system dependencies and setup tools"
            echo "  build    - Build the project in Release mode"
            echo "  debug    - Build the project in Debug mode"
            echo "  test     - Run Python tests"
            echo "  quality  - Run code quality checks"
            echo "  dist     - Create distribution packages"
            echo "  clean    - Clean build artifacts"
            echo "  all      - Run all steps (default)"
            echo "  help     - Show this help message"
            ;;
        *)
            print_error "Unknown action: $action"
            echo "Use '$0 help' for usage information."
            exit 1
            ;;
    esac
    
    print_success "Action '$action' completed successfully!"
}

# Run main function with arguments
main "$@"