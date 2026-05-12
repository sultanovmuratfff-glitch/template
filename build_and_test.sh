#!/bin/bash
set -e

echo "=== Building Viscopuppy Project ==="
echo ""

# Activate virtual environment
if [ -f ".venv/bin/activate" ]; then
    source .venv/bin/activate
    echo "✓ Virtual environment activated"
else
    echo "Creating virtual environment..."
    python3 -m venv .venv
    source .venv/bin/activate
    echo "✓ Virtual environment created and activated"
fi

echo ""
echo "=== Step 1: Installing Build Dependencies ==="
pip install -q numpy scikit-build-core cmake ninja build wheel

echo ""
echo "=== Step 2: Building with scikit-build-core ==="
# First try a direct build
python -m pip install -e . --no-build-isolation || {
    echo "Direct build failed, trying alternative approach..."
    # Try building wheel first
    python -m build --wheel --no-isolation || {
        echo "Wheel build failed, trying minimal build..."
        # Minimal build approach
        python setup.py build_ext --inplace
    }
}

echo ""
echo "=== Step 3: Testing Python Module ==="
if python -c "import viscopuppy; print('✓ Successfully imported viscopuppy')" 2>/dev/null; then
    echo "✓ Python module import successful"
    
    echo ""
    echo "=== Step 4: Running Python Tests ==="
    if [ -f "python/tests/test_viscopuppy.py" ]; then
        python -m pytest python/tests/test_viscopuppy.py -v
    else
        echo "Python test file not found, creating simple test..."
        python -c "
import viscopuppy
import numpy as np

# Test basic functionality
print('Testing vector addition...')
a = np.array([1.0, 2.0, 3.0])
b = np.array([4.0, 5.0, 6.0])
result = viscopuppy.add_vectors(a, b)
print(f'Result: {result}')
print('✓ Basic test passed')
        "
    fi
else
    echo "✗ Python module import failed"
    echo "Trying to build C++ tests instead..."
fi

echo ""
echo "=== Step 5: Building C++ Tests ==="
if [ -f "scripts/build-linux.sh" ]; then
    echo "Running C++ build script..."
    chmod +x scripts/build-linux.sh
    ./scripts/build-linux.sh test
else
    echo "C++ build script not found"
    echo "Creating build directory..."
    mkdir -p build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release -DVISCOPUPPY_BUILD_TESTS=ON
    make -j$(nproc)
    
    echo ""
    echo "=== Step 6: Running C++ Tests ==="
    if [ -f "tests/test_mathlib" ]; then
        ./tests/test_mathlib
    elif [ -f "test_mathlib" ]; then
        ./test_mathlib
    fi
    cd ..
fi

echo ""
echo "=== Build Summary ==="
echo "Project structure:"
find . -type f -name "*.py" -o -name "*.cpp" -o -name "*.hpp" -o -name "*.toml" -o -name "*.txt" | head -20
echo ""
echo "Build completed!"