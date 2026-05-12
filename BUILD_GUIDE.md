# Viscopuppy Build Guide

This guide provides comprehensive instructions for building, testing, and distributing the Viscopuppy project.

## Project Overview

Viscopuppy is a modern Python library built from C++23 code using:
- **CMake** as the build tool
- **Conan** as the package manager  
- **Scikit-build-core** as the Python CMake adaptor
- **Nanobind** as the Python binding module

## Prerequisites

### System Requirements
- Linux, macOS, or Windows with WSL2
- Python 3.9+
- C++23 compatible compiler (GCC 13+, Clang 16+, MSVC 2022+)
- CMake 3.25+
- Ninja (recommended) or Make
- Conan 2.0+

### Python Environment Setup

```bash
# Create and activate virtual environment
python3 -m venv .venv
source .venv/bin/activate  # On Windows: .venv\Scripts\activate

# Install core Python dependencies
pip install numpy scikit-build-core cmake ninja build wheel
```

## Building the Project

### Method 1: Development Installation (Recommended)

```bash
# Install in development mode
pip install -e . --no-build-isolation

# Or using scikit-build-core directly
python -m pip install -e . --no-build-isolation
```

### Method 2: Build Wheel Package

```bash
# Build wheel package
python -m build --wheel

# Install the built wheel
pip install dist/viscopuppy-*.whl
```

### Method 3: Manual CMake Build

```bash
# Create build directory
mkdir -p build && cd build

# Configure with CMake
cmake .. \
  -DCMAKE_CXX_STANDARD=23 \
  -DCMAKE_CXX_STANDARD_REQUIRED=ON \
  -DCMAKE_CXX_EXTENSIONS=OFF \
  -DCMAKE_BUILD_TYPE=Release \
  -DVISCOPUPPY_BUILD_PYTHON=ON \
  -DVISCOPUPPY_USE_CONAN=ON

# Build
cmake --build . --parallel

# Install (optional)
cmake --install .
```

## Testing

### Python Tests

```bash
# Install test dependencies
pip install pytest pytest-cov

# Run Python tests
python -m pytest python/tests/test_viscopuppy.py -v

# Run with coverage
python -m pytest python/tests/test_viscopuppy.py --cov=viscopuppy --cov-report=html
```

### C++ Tests

```bash
# Build and run C++ tests
mkdir -p build && cd build
cmake .. -DVISCOPUPPY_BUILD_TESTS=ON
cmake --build . --target test_mathlib
./tests/test_mathlib  # or .\tests\test_mathlib.exe on Windows
```

### Using the Build Script

```bash
# Make the script executable
chmod +x build_and_test.sh

# Run comprehensive build and test
./build_and_test.sh
```

## Conan Dependency Management

### Install Conan Dependencies

```bash
# Install Conan if not available
pip install conan

# Create Conan profile (if needed)
conan profile detect --force

# Install dependencies
conan install . --output-folder=build --build=missing
```

### Build with Conan

```bash
# Build using Conan
conan create . --build=missing

# Or install dependencies for CMake
conan install . -if build --build=missing
```

## Distribution

### Building Source Distribution

```bash
python -m build --sdist
```

### Building Universal Wheels

```bash
# Build wheel for current platform
python -m build --wheel

# For manylinux wheels (Linux only)
docker run --rm -v $(pwd):/io quay.io/pypa/manylinux2014_x86_64 /io/scripts/build-manylinux.sh
```

### Upload to PyPI

```bash
# Install twine
pip install twine

# Upload to TestPyPI
twine upload --repository testpypi dist/*

# Upload to PyPI
twine upload dist/*
```

## Development Workflow

### Code Quality Tools

```bash
# Install development dependencies
pip install -e ".[dev]"

# Run pre-commit hooks
pre-commit install
pre-commit run --all-files

# Format code
black python/ tests/
ruff check --fix python/ tests/

# Type checking
mypy python/
```

### Continuous Integration

The project includes GitHub Actions workflows in `.github/workflows/ci.yml` that:
1. Build and test on Linux, macOS, and Windows
2. Run Python and C++ tests
3. Build wheels for multiple Python versions
4. Upload artifacts

## Project Structure

```
viscopuppy/
├── CMakeLists.txt              # Root CMake configuration
├── conanfile.py               # Conan package definition
├── pyproject.toml            # Python package configuration
├── setup.py                  # Legacy setup.py (for compatibility)
├── build_and_test.sh         # Comprehensive build script
├── scripts/                  # Build scripts
│   └── build-linux.sh       # Linux build script
├── include/                  # C++ headers
│   └── viscopuppy/
│       └── mathlib.hpp      # Main library header
├── src/                      # C++ source code
│   ├── CMakeLists.txt
│   ├── mathlib/             # Math library implementation
│   └── python/              # Python bindings
│       ├── CMakeLists.txt
│       └── module.cpp       # Nanobind bindings
├── python/                   # Python package
│   ├── __init__.py
│   ├── _version.py
│   └── tests/
│       └── test_viscopuppy.py
├── tests/                    # C++ tests
│   ├── CMakeLists.txt
│   └── test_mathlib.cpp
├── examples/                 # Usage examples
│   ├── basic_usage.py
│   └── basic_usage.cpp
└── .github/workflows/       # CI/CD pipelines
    └── ci.yml
```

## Troubleshooting

### Common Issues

1. **CMake cannot find Python**
   ```bash
   export Python3_ROOT_DIR=$(python -c "import sys; print(sys.prefix)")
   ```

2. **Conan dependencies not found**
   ```bash
   conan profile detect --force
   conan install . --build=missing
   ```

3. **scikit-build-core experimental features**
   - Ensure `experimental = true` in `pyproject.toml`
   - Use `--no-build-isolation` flag

4. **Network issues during build**
   ```bash
   # Use offline mode for Conan
   conan install . --build=missing -o *:shared=False
   ```

### Platform-Specific Notes

**Linux:**
```bash
sudo apt-get install build-essential cmake ninja-build python3-dev
```

**macOS:**
```bash
brew install cmake ninja python@3.11
```

**Windows:**
- Install Visual Studio 2022 with C++ support
- Use x64 Native Tools Command Prompt
- Consider using WSL2 for Linux-like environment

## Advanced Configuration

### Custom CMake Options

Add to `pyproject.toml`:
```toml
[tool.scikit-build.cmake.define]
MY_CUSTOM_OPTION = "VALUE"
```

### Cross-Compilation

```bash
# Example for ARM Linux
conan install . --profile:host=armv8-linux-gnu --profile:build=default
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake
```

## Performance Optimization

### Release Build
```bash
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON
```

### SIMD Optimization
The library includes xsimd for automatic vectorization. Enable with:
```bash
cmake .. -DVISCOPUPPY_ENABLE_SIMD=ON
```

## Support

For issues and questions:
1. Check the [README.md](README.md)
2. Review existing GitHub issues
3. Submit new issues with build logs

## License

MIT License - See [LICENSE](LICENSE) file for details.