#!/usr/bin/env python3
"""
Basic usage examples for Viscopuppy library.

This example demonstrates the core functionality of the Viscopuppy library
including arithmetic, vector operations, matrix operations, and C++23 features.
"""

import numpy as np
import viscopuppy as vp


def arithmetic_operations():
    """Demonstrate basic arithmetic operations."""
    print("=" * 60)
    print("Arithmetic Operations")
    print("=" * 60)

    # Integer addition
    result = vp.add(10, 20)
    print(f"add(10, 20) = {result}")

    # Float addition
    result = vp.add(3.14, 2.71)
    print(f"add(3.14, 2.71) = {result}")

    # Integer multiplication
    result = vp.multiply(6, 7)
    print(f"multiply(6, 7) = {result}")

    # Float multiplication
    result = vp.multiply(2.5, 4.0)
    print(f"multiply(2.5, 4.0) = {result}")

    # Factorial
    result = vp.factorial(10)
    print(f"factorial(10) = {result}")

    # Fibonacci
    result = vp.fibonacci(20)
    print(f"fibonacci(20) = {result}")

    # Safe division
    result = vp.safe_divide(10.0, 3.0)
    print(f"safe_divide(10.0, 3.0) = {result}")

    # Safe division by zero (demonstrates error handling)
    try:
        result = vp.safe_divide(10.0, 0.0)
        print(f"safe_divide(10.0, 0.0) = {result}")
    except RuntimeError as e:
        print(f"safe_divide(10.0, 0.0) raised RuntimeError: {e}")


def vector_operations():
    """Demonstrate vector/array operations."""
    print("\n" + "=" * 60)
    print("Vector Operations")
    print("=" * 60)

    # Create vectors as NumPy arrays
    a = np.array([1.0, 2.0, 3.0], dtype=np.float64)
    b = np.array([4.0, 5.0, 6.0], dtype=np.float64)

    print(f"Vector a: {a}")
    print(f"Vector b: {b}")
    print()

    # Vector addition (element-wise)
    sum_vec = vp.vector_add(a, b)
    print(f"vector_add(a, b): {sum_vec}")

    # Vector multiplication (element-wise)
    prod_vec = vp.vector_multiply(a, b)
    print(f"vector_multiply(a, b): {prod_vec}")

    # Dot product
    dot_prod = vp.dot_product(a, b)
    print(f"dot_product(a, b): {dot_prod}")

    # Shape mismatch error handling
    try:
        c = np.array([1.0, 2.0], dtype=np.float64)
        vp.vector_add(a, c)
    except RuntimeError as e:
        print(f"vector_add with mismatched shapes raised RuntimeError: {e}")


def matrix_operations():
    """Demonstrate matrix operations."""
    print("\n" + "=" * 60)
    print("Matrix Operations")
    print("=" * 60)

    # Create matrices using the Matrix2D class
    A = vp.Matrix2D(3, 3)
    B = vp.Matrix2D(3, 3)

    # Fill matrix A with values
    values_A = [[1.0, 2.0, 3.0], [4.0, 5.0, 6.0], [7.0, 8.0, 9.0]]
    for i in range(3):
        for j in range(3):
            A[i, j] = values_A[i][j]

    # Fill matrix B with values
    values_B = [[9.0, 8.0, 7.0], [6.0, 5.0, 4.0], [3.0, 2.0, 1.0]]
    for i in range(3):
        for j in range(3):
            B[i, j] = values_B[i][j]

    print(f"Matrix A ({A.rows}x{A.cols}):")
    print(A.to_numpy())
    print(f"\nMatrix B ({B.rows}x{B.cols}):")
    print(B.to_numpy())

    # Matrix multiplication
    try:
        C = vp.matrix_multiply(A, B)
        print(f"\nmatrix_multiply(A, B) ({C.rows}x{C.cols}):")
        print(C.to_numpy())
    except RuntimeError as e:
        print(f"matrix_multiply failed: {e}")


def range_operations():
    """Demonstrate C++23 range operations."""
    print("\n" + "=" * 60)
    print("C++23 Range Operations")
    print("=" * 60)

    numbers = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]

    # Filter even numbers
    evens = vp.filter_even(numbers)
    print(f"Original list: {numbers}")
    print(f"filter_even: {evens}")

    # Transform range (square each element)
    doubles = [1.0, 2.0, 3.0, 4.0, 5.0]
    squared = vp.transform_range(doubles)
    print(f"\nOriginal list: {doubles}")
    print(f"transform_range (x^2): {squared}")


def coroutine_example():
    """Demonstrate C++23 coroutine features."""
    print("\n" + "=" * 60)
    print("C++23 Coroutine Features")
    print("=" * 60)

    # Fibonacci generator
    gen = vp.fibonacci_generator(10)
    fib_numbers = list(gen)
    print(f"Fibonacci generator (limit=10): {fib_numbers}")

    # Coroutine example function
    result = vp.coroutine_example(15)
    print(f"coroutine_example(15): {result}")


def cpp_features():
    """Demonstrate C++23 concept and range features."""
    print("\n" + "=" * 60)
    print("C++23 Modern Features")
    print("=" * 60)

    # Concept example
    result = vp.concept_example()
    print(f"concept_example: {result}")

    # Range example
    result = vp.range_example()
    print(f"range_example: {result}")


def performance_demo():
    """Demonstrate performance with larger arrays."""
    print("\n" + "=" * 60)
    print("Performance Demo")
    print("=" * 60)

    n = 1000000
    print(f"Creating two {n}-element vectors...")

    # Large random vectors
    a_large = np.random.randn(n).astype(np.float64)
    b_large = np.random.randn(n).astype(np.float64)

    # Dot product of large vectors
    import time
    start = time.time()
    dot_result = vp.dot_product(a_large, b_large)
    elapsed = time.time() - start

    print(f"dot_product result: {dot_result:.6f}")
    print(f"Time for {n}-element dot product: {elapsed:.6f} seconds")
    print(f"Performance: {(2 * n) / (elapsed * 1e9):.2f} GFLOP/s")

    # Vector addition of large vectors
    start = time.time()
    sum_result = vp.vector_add(a_large, b_large)
    elapsed = time.time() - start

    print(f"\nTime for {n}-element vector_add: {elapsed:.6f} seconds")
    print(f"Performance: {n / (elapsed * 1e9):.2f} GFLOP/s")


def main():
    """Run all examples."""
    print("Viscopuppy Library Examples")
    print("=" * 60)
    print()

    # Run examples
    arithmetic_operations()
    vector_operations()
    matrix_operations()
    range_operations()
    coroutine_example()
    cpp_features()
    performance_demo()

    print("\n" + "=" * 60)
    print("All examples completed successfully!")
    print("=" * 60)


if __name__ == "__main__":
    main()