"""Tests for the viscopuppy Python package."""

import numpy as np
import pytest
import viscopuppy


def test_version():
    """Test that version is accessible."""
    assert hasattr(viscopuppy, "__version__")
    assert isinstance(viscopuppy.__version__, str)
    assert len(viscopuppy.__version__) > 0


def test_basic_arithmetic():
    """Test basic arithmetic operations."""
    # Addition
    assert viscopuppy.add(2, 3) == 5
    assert viscopuppy.add(2.5, 3.5) == 6.0
    
    # Multiplication
    assert viscopuppy.multiply(2, 3) == 6
    assert viscopuppy.multiply(2.5, 4.0) == 10.0
    
    # Safe division
    result = viscopuppy.safe_divide(10.0, 2.0)
    assert result == 5.0
    
    # Division by zero should raise an error
    with pytest.raises(RuntimeError, match="Division by zero"):
        viscopuppy.safe_divide(10.0, 0.0)
    
    # Factorial
    assert viscopuppy.factorial(0) == 1
    assert viscopuppy.factorial(1) == 1
    assert viscopuppy.factorial(5) == 120
    
    # Fibonacci
    assert viscopuppy.fibonacci(0) == 0
    assert viscopuppy.fibonacci(1) == 1
    assert viscopuppy.fibonacci(5) == 5
    assert viscopuppy.fibonacci(10) == 55


def test_numpy_array_operations():
    """Test NumPy array operations."""
    a = np.array([1.0, 2.0, 3.0], dtype=np.float64)
    b = np.array([4.0, 5.0, 6.0], dtype=np.float64)
    c = np.array([7.0, 8.0], dtype=np.float64)  # Different size
    
    # Vector addition
    result = viscopuppy.vector_add(a, b)
    assert isinstance(result, np.ndarray)
    assert result.shape == (3,)
    np.testing.assert_array_almost_equal(result, [5.0, 7.0, 9.0])
    
    # Vector addition with size mismatch
    with pytest.raises(RuntimeError, match="Array shapes must match"):
        viscopuppy.vector_add(a, c)
    
    # Vector multiplication
    result = viscopuppy.vector_multiply(a, b)
    assert isinstance(result, np.ndarray)
    assert result.shape == (3,)
    np.testing.assert_array_almost_equal(result, [4.0, 10.0, 18.0])
    
    # Dot product
    result = viscopuppy.dot_product(a, b)
    assert isinstance(result, float)
    assert abs(result - 32.0) < 1e-10  # 1*4 + 2*5 + 3*6 = 32


def test_ranges_operations():
    """Test C++23 ranges operations."""
    numbers = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
    
    # Filter even numbers
    result = viscopuppy.filter_even(numbers)
    assert isinstance(result, np.ndarray)
    assert result.shape == (5,)
    np.testing.assert_array_equal(result, [2, 4, 6, 8, 10])
    
    # Transform range (square each element)
    result = viscopuppy.transform_range([1.0, 2.0, 3.0, 4.0, 5.0])
    assert isinstance(result, np.ndarray)
    assert result.shape == (5,)
    np.testing.assert_array_almost_equal(result, [1.0, 4.0, 9.0, 16.0, 25.0])


def test_coroutine_fibonacci_generator():
    """Test C++23 coroutine Fibonacci generator."""
    # Create generator
    gen = viscopuppy.fibonacci_generator(10)
    
    # Test iteration
    sequence = list(gen)
    assert len(sequence) == 10
    assert sequence == [0, 1, 1, 2, 3, 5, 8, 13, 21, 34]
    
    # Test empty generator
    gen = viscopuppy.fibonacci_generator(0)
    assert list(gen) == []
    
    # Test single element generator
    gen = viscopuppy.fibonacci_generator(1)
    assert list(gen) == [0]


def test_matrix_operations():
    """Test matrix operations."""
    # Create matrices
    a = viscopuppy.Matrix2D(2, 3)
    a[0, 0] = 1.0
    a[0, 1] = 2.0
    a[0, 2] = 3.0
    a[1, 0] = 4.0
    a[1, 1] = 5.0
    a[1, 2] = 6.0
    
    b = viscopuppy.Matrix2D(3, 2)
    b[0, 0] = 7.0
    b[0, 1] = 8.0
    b[1, 0] = 9.0
    b[1, 1] = 10.0
    b[2, 0] = 11.0
    b[2, 1] = 12.0
    
    # Test matrix properties
    assert a.rows == 2
    assert a.cols == 3
    assert b.rows == 3
    assert b.cols == 2
    
    # Test element access
    assert a[0, 0] == 1.0
    assert a[1, 2] == 6.0
    assert b[2, 1] == 12.0
    
    # Test matrix multiplication
    c = viscopuppy.matrix_multiply(a, b)
    assert c.rows == 2
    assert c.cols == 2
    
    # Check results
    # 1*7 + 2*9 + 3*11 = 58
    assert abs(c[0, 0] - 58.0) < 1e-10
    # 1*8 + 2*10 + 3*12 = 64
    assert abs(c[0, 1] - 64.0) < 1e-10
    # 4*7 + 5*9 + 6*11 = 139
    assert abs(c[1, 0] - 139.0) < 1e-10
    # 4*8 + 5*10 + 6*12 = 154
    assert abs(c[1, 1] - 154.0) < 1e-10
    
    # Test to_numpy conversion
    numpy_array = a.to_numpy()
    assert isinstance(numpy_array, np.ndarray)
    assert numpy_array.shape == (2, 3)
    np.testing.assert_array_almost_equal(
        numpy_array,
        [[1.0, 2.0, 3.0], [4.0, 5.0, 6.0]]
    )
    
    # Test incompatible matrix multiplication
    d = viscopuppy.Matrix2D(2, 3)  # Wrong dimensions
    with pytest.raises(RuntimeError, match="Matrix dimensions incompatible"):
        viscopuppy.matrix_multiply(a, d)


def test_concept_example():
    """Test C++23 concept example."""
    result = viscopuppy.concept_example()
    assert isinstance(result, str)
    assert "C++23 concepts are working correctly" in result


def test_range_example():
    """Test C++23 range example."""
    result = viscopuppy.range_example()
    assert isinstance(result, str)
    assert "C++23 ranges example" in result
    assert "sum of squares = 55" in result  # 1² + 2² + 3² + 4² + 5² = 55


def test_coroutine_example():
    """Test C++23 coroutine example."""
    result = viscopuppy.coroutine_example(10)
    assert isinstance(result, str)
    assert "C++23 coroutines" in result
    assert "10" in result  # Should mention generating 10 numbers


def test_large_array_performance():
    """Test performance with large arrays."""
    size = 10000
    a = np.random.randn(size).astype(np.float64)
    b = np.random.randn(size).astype(np.float64)
    
    # Test vector addition
    result = viscopuppy.vector_add(a, b)
    assert result.shape == (size,)
    
    # Verify correctness
    expected = a + b
    np.testing.assert_array_almost_equal(result, expected, decimal=10)
    
    # Test dot product
    result = viscopuppy.dot_product(a, b)
    expected = np.dot(a, b)
    assert abs(result - expected) < 1e-10


def test_edge_cases():
    """Test edge cases."""
    # Empty arrays
    a = np.array([], dtype=np.float64)
    b = np.array([], dtype=np.float64)
    
    result = viscopuppy.vector_add(a, b)
    assert result.shape == (0,)
    
    result = viscopuppy.dot_product(a, b)
    assert result == 0.0
    
    # Single element arrays
    a = np.array([42.0], dtype=np.float64)
    b = np.array([2.0], dtype=np.float64)
    
    result = viscopuppy.vector_add(a, b)
    np.testing.assert_array_almost_equal(result, [44.0])
    
    result = viscopuppy.dot_product(a, b)
    assert result == 84.0
    
    # Negative values
    a = np.array([-1.0, -2.0, -3.0], dtype=np.float64)
    b = np.array([1.0, 2.0, 3.0], dtype=np.float64)
    
    result = viscopuppy.vector_add(a, b)
    np.testing.assert_array_almost_equal(result, [0.0, 0.0, 0.0])
    
    result = viscopuppy.vector_multiply(a, b)
    np.testing.assert_array_almost_equal(result, [-1.0, -4.0, -9.0])


if __name__ == "__main__":
    pytest.main([__file__, "-v"])