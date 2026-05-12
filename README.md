# Viscopuppy: High-Performance C++23 Math Library with Python Bindings

[![CI/CD Pipeline](https://github.com/yourusername/viscopuppy/actions/workflows/ci.yml/badge.svg)](https://github.com/yourusername/viscopuppy/actions/workflows/ci.yml)
[![PyPI version](https://img.shields.io/pypi/v/viscopuppy.svg)](https://pypi.org/project/viscopuppy/)
[![Python Versions](https://img.shields.io/pypi/pyversions/viscopuppy.svg)](https://pypi.org/project/viscopuppy/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Code Coverage](https://codecov.io/gh/yourusername/viscopuppy/branch/main/graph/badge.svg)](https://codecov.io/gh/yourusername/viscopuppy)

**Viscopuppy** is a modern, high-performance C++23 math library with seamless Python bindings using nanobind. Built with CMake, Conan, and scikit-build-core, it provides a robust foundation for scientific computing with zero-copy NumPy array support and SIMD acceleration.

## 🚀 Features

- **Modern C++23**: Leverages the latest C++ standards with concepts, ranges, and modules
- **Zero-copy NumPy integration**: Efficient data exchange between Python and C++
- **SIMD acceleration**: Automatic vectorization for performance-critical operations
- **Header-only library**: Easy integration into existing projects
- **Cross-platform**: Builds on Linux, macOS, and Windows
- **Comprehensive testing**: Unit tests, benchmarks, and integration tests
- **Modern tooling**: Uses uv, Conan, CMake, and scikit-build-core

## 📦 Installation

### From PyPI (Recommended)

```bash
pip install viscopuppy
```

### From Source

```bash
# Clone the repository
git clone https://github.com/yourusername/viscopuppy.git
cd viscopuppy

# Install with uv (modern Python package manager)
uv pip install -e .

# Or with pip
pip install -e .
```

### Development Installation

```bash
# Clone and setup development environment
git clone https://github.com/yourusername/viscopuppy.git
cd viscopuppy

# Install development dependencies
uv pip install -e .[dev]

# Run tests
pytest python/tests/ -v

# Run benchmarks
pytest python/tests/ -v --benchmark-only
```

## 🛠️ Build System

Viscopuppy uses a modern build system stack:

- **CMake**: Primary build system
- **Conan**: C++ dependency management
- **scikit-build-core**: Python package building
- **nanobind**: High-performance Python bindings
- **uv**: Modern Python package manager

### Building from Source

```bash
# Using the provided build script
./scripts/build-linux.sh all

# Or manually
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=23
cmake --build . --parallel
```

## 📚 Quick Start

### Python Usage

```python
import numpy as np
import viscopuppy as vp

# Create vectors
a = np.array([1.0, 2.0, 3.0], dtype=np.float64)
b = np.array([4.0, 5.0, 6.0], dtype=np.float64)

# Compute dot product
dot_product = vp.dot(a, b)
print(f"Dot product: {dot_product}")

# Compute cross product
cross_product = vp.cross(a, b)
print(f"Cross product: {cross_product}")

# Matrix operations
matrix = np.random.rand(3, 3).astype(np.float64)
vector = np.random.rand(3).astype(np.float64)

# Matrix-vector multiplication
result = vp.matvec(matrix, vector)
print(f"Matrix-vector product: {result}")

# Eigenvalue decomposition
eigenvalues, eigenvectors = vp.eig(matrix)
print(f"Eigenvalues: {eigenvalues}")

# Linear system solving
solution = vp.solve(matrix, vector)
print(f"Linear system solution: {solution}")
```

### C++ Usage

```cpp
#include <viscopuppy/mathlib.hpp>
#include <iostream>
#include <vector>

int main() {
    using namespace viscopuppy;
    
    // Create vectors
    std::vector<double> a = {1.0, 2.0, 3.0};
    std::vector<double> b = {4.0, 5.0, 6.0};
    
    // Compute dot product
    double dot = math::dot(a, b);
    std::cout << "Dot product: " << dot << std::endl;
    
    // Compute cross product
    auto cross = math::cross(a, b);
    std::cout << "Cross product: ";
    for (auto val : cross) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
    
    // Matrix operations
    math::Matrix<double> mat = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    math::Vector<double> vec = {1, 2, 3};
    
    // Matrix-vector multiplication
    auto result = math::matvec(mat, vec);
    std::cout << "Matrix-vector product: ";
    for (auto val : result) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
    
    return 0;
}
```

## 🏗️ Architecture

### Project Structure

```
viscopuppy/
├── include/viscopuppy/     # Public C++ headers
│   └── mathlib.hpp         # Main math library header
├── src/                    # C++ source files
│   ├── mathlib/           # Math library implementation
│   └── python/            # Python bindings
├── python/                # Python package
│   ├── __init__.py       # Package initialization
│   └── tests/            # Python tests
├── tests/                # C++ tests
├── examples/             # Usage examples
├── scripts/              # Build and utility scripts
└── .github/workflows/    # CI/CD pipelines
```

### Key Components

1. **Math Library (`include/viscopuppy/mathlib.hpp`)**: Header-only C++23 math library
2. **Python Bindings (`src/python/module.cpp`)**: nanobind-based Python interface
3. **Build System (`CMakeLists.txt`)**: Modern CMake configuration
4. **Dependency Management (`conanfile.py`)**: Conan package definitions
5. **Python Packaging (`pyproject.toml`)**: scikit-build-core configuration

## 🔧 Advanced Usage

### Custom Build Configuration

```bash
# Build with specific compiler
CC=clang CXX=clang++ cmake .. -DCMAKE_BUILD_TYPE=Release

# Build with AVX2 optimizations
cmake .. -DUSE_AVX2=ON -DCMAKE_BUILD_TYPE=Release

# Build with OpenMP support
cmake .. -DUSE_OPENMP=ON -DCMAKE_BUILD_TYPE=Release

# Build for specific Python version
cmake .. -DPython_EXECUTABLE=$(which python3.11)
```

### Using as a C++ Library

```cmake
# In your CMakeLists.txt
find_package(viscopuppy REQUIRED)
target_link_libraries(your_target PRIVATE viscopuppy::viscopuppy)
```

### Creating Custom Python Extensions

```python
# Example of extending viscopuppy with custom functions
import viscopuppy as vp
import numpy as np

def custom_function(x: np.ndarray) -> np.ndarray:
    """Custom function that uses viscopuppy internally."""
    # Use viscopuppy's optimized operations
    result = vp.some_operation(x)
    # Add custom logic
    return result * 2.0
```

## 📊 Performance

Viscopuppy is optimized for performance:

- **SIMD vectorization**: Automatic use of AVX2/AVX-512 when available
- **Cache-friendly algorithms**: Optimized memory access patterns
- **Zero-copy operations**: No data copying between Python and C++
- **Multithreading support**: Parallel execution for large datasets

### Benchmark Results

```bash
# Run benchmarks
pytest python/tests/ -v --benchmark-only

# Example benchmark output
# dot_product: mean=1.2μs, std=0.1μs
# cross_product: mean=1.5μs, std=0.2μs
# matvec_100x100: mean=15.3μs, std=1.2μs
```

## 🧪 Testing

### Running Tests

```bash
# Run all tests
./scripts/build-linux.sh test

# Run C++ tests only
cd build && ctest --output-on-failure

# Run Python tests only
pytest python/tests/ -v

# Run benchmarks
pytest python/tests/ -v --benchmark-only

# Run code quality checks
pre-commit run --all-files
```

### Test Coverage

```bash
# Generate coverage report
./scripts/build-linux.sh coverage

# View HTML report
open build-coverage/coverage/index.html
```

## 🔍 Code Quality

Viscopuppy maintains high code quality standards:

- **clang-format**: Consistent code formatting
- **clang-tidy**: Static analysis and linting
- **pre-commit**: Automated code quality hooks
- **cppcheck**: Additional static analysis
- **mypy**: Python type checking

### Code Quality Tools

```bash
# Format code
clang-format -i src/**/*.cpp include/**/*.hpp

# Run clang-tidy
clang-tidy -p build src/**/*.cpp include/**/*.hpp

# Run pre-commit hooks
pre-commit run --all-files

# Run mypy
mypy python/
```

## 🤝 Contributing

We welcome contributions! Please see our [Contributing Guide](CONTRIBUTING.md) for details.

### Development Workflow

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Run tests and code quality checks
5. Submit a pull request

### Code Style

- Follow the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
- Use `clang-format` for formatting
- Write comprehensive tests
- Add documentation for new features

## 📄 License

Viscopuppy is licensed under the MIT License. See [LICENSE](LICENSE) for details.

## 📚 Documentation

Full documentation is available at [https://viscopuppy.readthedocs.io](https://viscopuppy.readthedocs.io).

### Building Documentation

```bash
# Install documentation dependencies
uv pip install sphinx sphinx-rtd-theme

# Build documentation
sphinx-build -b html docs docs/_build/html

# View documentation
open docs/_build/html/index.html
```

## 🐛 Support

- **Issues**: [GitHub Issues](https://github.com/yourusername/viscopuppy/issues)
- **Discussions**: [GitHub Discussions](https://github.com/yourusername/viscopuppy/discussions)
- **Email**: support@viscopuppy.org

## 🙏 Acknowledgments

- [nanobind](https://github.com/wjakob/nanobind) for high-performance Python bindings
- [scikit-build-core](https://github.com/scikit-build/scikit-build-core) for Python package building
- [Conan](https://conan.io/) for C++ dependency management
- [Catch2](https://github.com/catchorg/Catch2) for C++ testing
- [pytest](https://docs.pytest.org/) for Python testing

## 📈 Roadmap

- [ ] GPU acceleration with CUDA/OpenCL
- [ ] Distributed computing support
- [ ] Additional math functions
- [ ] Machine learning utilities
- [ ] WebAssembly compilation target
- [ ] Julia language bindings

---

**Viscopuppy** - Modern C++23 math library with Python bindings. Fast, reliable, and easy to use.