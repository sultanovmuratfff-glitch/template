// Basic usage examples for Viscopuppy C++ library
//
// This example demonstrates the core functionality of the Viscopuppy C++ library
// including vector operations, matrix operations, and linear algebra.

#include <viscopuppy/mathlib.hpp>
#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>

namespace vp = viscopuppy;

void vector_operations() {
    std::cout << std::string(60, '=') << std::endl;
    std::cout << "Vector Operations" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    // Create vectors
    std::vector<double> a = {1.0, 2.0, 3.0};
    std::vector<double> b = {4.0, 5.0, 6.0};
    
    std::cout << "Vector a: ";
    for (auto val : a) std::cout << val << " ";
    std::cout << std::endl;
    
    std::cout << "Vector b: ";
    for (auto val : b) std::cout << val << " ";
    std::cout << std::endl << std::endl;
    
    // Dot product
    double dot_product = vp::math::dot(a, b);
    std::cout << "Dot product (a · b): " << dot_product << std::endl;
    
    // Cross product
    auto cross_product = vp::math::cross(a, b);
    std::cout << "Cross product (a × b): ";
    for (auto val : cross_product) std::cout << val << " ";
    std::cout << std::endl;
    
    // Vector norm
    double norm_a = vp::math::norm(a);
    double norm_b = vp::math::norm(b);
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "Norm of a: " << norm_a << std::endl;
    std::cout << "Norm of b: " << norm_b << std::endl;
    
    // Vector addition and subtraction
    auto sum_vec = vp::math::add(a, b);
    auto diff_vec = vp::math::subtract(a, b);
    std::cout << "a + b: ";
    for (auto val : sum_vec) std::cout << val << " ";
    std::cout << std::endl;
    
    std::cout << "a - b: ";
    for (auto val : diff_vec) std::cout << val << " ";
    std::cout << std::endl;
    
    // Scalar multiplication
    auto scaled = vp::math::scale(a, 2.5);
    std::cout << "2.5 * a: ";
    for (auto val : scaled) std::cout << val << " ";
    std::cout << std::endl;
}

void matrix_operations() {
    std::cout << std::endl << std::string(60, '=') << std::endl;
    std::cout << "Matrix Operations" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    // Create matrices
    vp::math::Matrix<double> A = {{1.0, 2.0, 3.0},
                                  {4.0, 5.0, 6.0},
                                  {7.0, 8.0, 9.0}};
    
    vp::math::Matrix<double> B = {{9.0, 8.0, 7.0},
                                  {6.0, 5.0, 4.0},
                                  {3.0, 2.0, 1.0}};
    
    std::cout << "Matrix A:" << std::endl;
    for (const auto& row : A) {
        for (auto val : row) std::cout << val << " ";
        std::cout << std::endl;
    }
    
    std::cout << std::endl << "Matrix B:" << std::endl;
    for (const auto& row : B) {
        for (auto val : row) std::cout << val << " ";
        std::cout << std::endl;
    }
    std::cout << std::endl;
    
    // Matrix addition
    auto C = vp::math::add(A, B);
    std::cout << "Matrix addition (A + B):" << std::endl;
    for (const auto& row : C) {
        for (auto val : row) std::cout << val << " ";
        std::cout << std::endl;
    }
    
    // Matrix multiplication
    auto D = vp::math::matmul(A, B);
    std::cout << std::endl << "Matrix multiplication (A × B):" << std::endl;
    for (const auto& row : D) {
        for (auto val : row) std::cout << val << " ";
        std::cout << std::endl;
    }
    
    // Matrix transpose
    auto A_transpose = vp::math::transpose(A);
    std::cout << std::endl << "Transpose of A:" << std::endl;
    for (const auto& row : A_transpose) {
        for (auto val : row) std::cout << val << " ";
        std::cout << std::endl;
    }
    
    // Matrix determinant
    double det_A = vp::math::det(A);
    std::cout << std::fixed << std::setprecision(4);
    std::cout << std::endl << "Determinant of A: " << det_A << std::endl;
    
    // Matrix trace
    double trace_A = vp::math::trace(A);
    std::cout << "Trace of A: " << trace_A << std::endl;
}

void linear_algebra() {
    std::cout << std::endl << std::string(60, '=') << std::endl;
    std::cout << "Linear Algebra" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    // Create a well-conditioned matrix for solving
    vp::math::Matrix<double> A = {{4.0, 1.0, 2.0},
                                  {1.0, 5.0, 3.0},
                                  {2.0, 3.0, 6.0}};
    
    std::vector<double> b = {1.0, 2.0, 3.0};
    
    std::cout << "Matrix A:" << std::endl;
    for (const auto& row : A) {
        for (auto val : row) std::cout << val << " ";
        std::cout << std::endl;
    }
    
    std::cout << std::endl << "Vector b: ";
    for (auto val : b) std::cout << val << " ";
    std::cout << std::endl << std::endl;
    
    // Solve linear system Ax = b
    try {
        auto x = vp::math::solve(A, b);
        std::cout << "Solution x to Ax = b: ";
        for (auto val : x) std::cout << val << " ";
        std::cout << std::endl;
        
        // Verify solution
        auto Ax = vp::math::matvec(A, x);
        std::cout << "Ax (should equal b): ";
        for (auto val : Ax) std::cout << val << " ";
        std::cout << std::endl;
        
        // Compute residual
        double residual = 0.0;
        for (size_t i = 0; i < b.size(); ++i) {
            residual += (Ax[i] - b[i]) * (Ax[i] - b[i]);
        }
        residual = std::sqrt(residual);
        std::cout << std::scientific << std::setprecision(2);
        std::cout << "Residual norm: " << residual << std::endl;
        std::cout << std::fixed;
    } catch (const std::exception& e) {
        std::cout << "Could not solve linear system: " << e.what() << std::endl;
    }
    
    // Eigenvalue decomposition
    std::cout << std::endl << "Eigenvalue decomposition:" << std::endl;
    auto [eigenvalues, eigenvectors] = vp::math::eig(A);
    
    std::cout << "Eigenvalues: ";
    for (auto val : eigenvalues) std::cout << val << " ";
    std::cout << std::endl;
    
    std::cout << "Eigenvectors (columns):" << std::endl;
    for (size_t i = 0; i < eigenvectors.size(); ++i) {
        for (size_t j = 0; j < eigenvectors[i].size(); ++j) {
            std::cout << eigenvectors[i][j] << " ";
        }
        std::cout << std::endl;
    }
    
    // Verify eigenvalue decomposition
    std::cout << std::scientific << std::setprecision(2);
    for (size_t i = 0; i < eigenvalues.size(); ++i) {
        // Extract eigenvector
        std::vector<double> v(eigenvectors.size());
        for (size_t j = 0; j < eigenvectors.size(); ++j) {
            v[j] = eigenvectors[j][i];
        }
        
        // Compute Av and λv
        auto Av = vp::math::matvec(A, v);
        std::vector<double> lambda_v(v.size());
        for (size_t j = 0; j < v.size(); ++j) {
            lambda_v[j] = eigenvalues[i] * v[j];
        }
        
        // Compute residual
        double residual = 0.0;
        for (size_t j = 0; j < v.size(); ++j) {
            residual += (Av[j] - lambda_v[j]) * (Av[j] - lambda_v[j]);
        }
        residual = std::sqrt(residual);
        
        std::cout << std::fixed << std::setprecision(4);
        std::cout << "Eigenvalue " << i << ": λ = " << eigenvalues[i];
        std::cout << std::scientific << std::setprecision(2);
        std::cout << ", residual = " << residual << std::endl;
    }
}

void performance_demo() {
    std::cout << std::endl << std::string(60, '=') << std::endl;
    std::cout << "Performance Demo" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    // Create larger arrays
    const size_t n = 1000;
    std::cout << "Creating " << n << "x" << n << " matrix and " 
              << n << "-element vector..." << std::endl;
    
    // Large random matrix
    vp::math::Matrix<double> A_large(n, std::vector<double>(n));
    std::vector<double> x_large(n);
    
    // Fill with random values
    std::srand(42); // Seed for reproducibility
    for (size_t i = 0; i < n; ++i) {
        x_large[i] = static_cast<double>(std::rand()) / RAND_MAX;
        for (size_t j = 0; j < n; ++j) {
            A_large[i][j] = static_cast<double>(std::rand()) / RAND_MAX;
        }
    }
    
    // Matrix-vector multiplication
    std::cout << "Performing matrix-vector multiplication..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    auto b_large = vp::math::matvec(A_large, x_large);
    auto end = std::chrono::high_resolution_clock::now();
    
    auto elapsed = std::chrono::duration<double>(end - start).count();
    double gflops = (2.0 * n * n) / (elapsed * 1e9);
    
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "Time for " << n << "x" << n << " matvec: " 
              << elapsed << " seconds" << std::endl;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Performance: " << gflops << " GFLOP/s" << std::endl;
    
    // Dot product of large vectors
    std::vector<double> y_large(n);
    for (size_t i = 0; i < n; ++i) {
        y_large[i] = static_cast<double>(std::rand()) / RAND_MAX;
    }
    
    start = std::chrono::high_resolution_clock::now();
    double dot_large = vp::math::dot(x_large, y_large);
    end = std::chrono::high_resolution_clock::now();
    
    elapsed = std::chrono::duration<double>(end - start).count();
    gflops = (2.0 * n) / (elapsed * 1e9);
    
    std::cout << std::fixed << std::setprecision(6);
    std::cout << std::endl << "Time for " << n << "-element dot product: " 
              << elapsed << " seconds" << std::endl;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Performance: " << gflops << " GFLOP/s" << std::endl;
}

int main() {
    std::cout << "Viscopuppy C++ Library Examples" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    std::cout << std::endl;
    
    // Run examples
    vector_operations();
    matrix_operations();
    linear_algebra();
    performance_demo();
    
    std::cout << std::endl << std::string(60, '=') << std::endl;
    std::cout << "All examples completed successfully!" << std::endl;
    std::cout << std::string(60, '=') << std::endl;
    
    return 0;
}