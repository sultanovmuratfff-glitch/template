#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <viscopuppy/mathlib.hpp>

using namespace viscopuppy::mathlib;
using Catch::Matchers::WithinRel;

TEST_CASE("Basic arithmetic operations", "[mathlib][basic]") {
    SECTION("Addition") {
        REQUIRE(add(2, 3) == 5);
        REQUIRE(add(2.5, 3.5) == 6.0);
        REQUIRE(add(-5, 10) == 5);
    }
    
    SECTION("Multiplication") {
        REQUIRE(multiply(2, 3) == 6);
        REQUIRE(multiply(2.5, 4.0) == 10.0);
        REQUIRE(multiply(-5, 3) == -15);
    }
    
    SECTION("Safe division") {
        SECTION("Successful division") {
            auto result = safe_divide(10.0, 2.0);
            REQUIRE(result.has_value());
            REQUIRE_THAT(result.value(), WithinRel(5.0, 1e-10));
        }
        
        SECTION("Division by zero") {
            auto result = safe_divide(10.0, 0.0);
            REQUIRE_FALSE(result.has_value());
            REQUIRE(result.error() == "Division by zero");
        }
    }
    
    SECTION("Factorial") {
        REQUIRE(factorial(0) == 1);
        REQUIRE(factorial(1) == 1);
        REQUIRE(factorial(5) == 120);
        REQUIRE(factorial(10) == 3628800);
    }
    
    SECTION("Fibonacci") {
        REQUIRE(fibonacci(0) == 0);
        REQUIRE(fibonacci(1) == 1);
        REQUIRE(fibonacci(5) == 5);
        REQUIRE(fibonacci(10) == 55);
    }
}

TEST_CASE("Vector operations", "[mathlib][vector]") {
    std::vector<double> a = {1.0, 2.0, 3.0};
    std::vector<double> b = {4.0, 5.0, 6.0};
    std::vector<double> c = {7.0, 8.0}; // Different size
    
    SECTION("Vector addition") {
        auto result = vector_add(a, b);
        REQUIRE(result.has_value());
        
        auto& vec = result.value();
        REQUIRE(vec.size() == 3);
        REQUIRE_THAT(vec[0], WithinRel(5.0, 1e-10));
        REQUIRE_THAT(vec[1], WithinRel(7.0, 1e-10));
        REQUIRE_THAT(vec[2], WithinRel(9.0, 1e-10));
    }
    
    SECTION("Vector addition with size mismatch") {
        auto result = vector_add(a, c);
        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "Container sizes must match");
    }
    
    SECTION("Vector multiplication") {
        auto result = vector_multiply(a, b);
        REQUIRE(result.has_value());
        
        auto& vec = result.value();
        REQUIRE(vec.size() == 3);
        REQUIRE_THAT(vec[0], WithinRel(4.0, 1e-10));
        REQUIRE_THAT(vec[1], WithinRel(10.0, 1e-10));
        REQUIRE_THAT(vec[2], WithinRel(18.0, 1e-10));
    }
    
    SECTION("Dot product") {
        auto result = dot_product(a, b);
        REQUIRE(result.has_value());
        REQUIRE_THAT(result.value(), WithinRel(32.0, 1e-10)); // 1*4 + 2*5 + 3*6 = 32
    }
    
    SECTION("Dot product with size mismatch") {
        auto result = dot_product(a, c);
        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "Container sizes must match");
    }
}

TEST_CASE("C++23 Ranges operations", "[mathlib][ranges]") {
    std::vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    SECTION("Filter even numbers") {
        auto evens = filter_even(numbers);
        REQUIRE(evens.size() == 5);
        REQUIRE(evens == std::vector<int>{2, 4, 6, 8, 10});
    }
    
    SECTION("Transform range") {
        auto squares = transform_range(numbers, [](int x) { return x * x; });
        REQUIRE(squares.size() == 10);
        REQUIRE(squares[0] == 1);
        REQUIRE(squares[4] == 25);
        REQUIRE(squares[9] == 100);
    }
}

TEST_CASE("C++23 Coroutine Fibonacci generator", "[mathlib][coroutine]") {
    SECTION("Generate Fibonacci sequence") {
        auto gen = fibonacci_generator<int>(10);
        
        std::vector<int> sequence;
        while (auto value = gen.next()) {
            sequence.push_back(*value);
        }
        
        REQUIRE(sequence.size() == 10);
        REQUIRE(sequence[0] == 0);
        REQUIRE(sequence[1] == 1);
        REQUIRE(sequence[2] == 1);
        REQUIRE(sequence[3] == 2);
        REQUIRE(sequence[4] == 3);
        REQUIRE(sequence[5] == 5);
        REQUIRE(sequence[6] == 8);
        REQUIRE(sequence[7] == 13);
        REQUIRE(sequence[8] == 21);
        REQUIRE(sequence[9] == 34);
    }
    
    SECTION("Empty generator") {
        auto gen = fibonacci_generator<int>(0);
        REQUIRE_FALSE(gen.next().has_value());
    }
    
    SECTION("Single element generator") {
        auto gen = fibonacci_generator<int>(1);
        
        auto value = gen.next();
        REQUIRE(value.has_value());
        REQUIRE(*value == 0);
        
        REQUIRE_FALSE(gen.next().has_value());
    }
}

TEST_CASE("Matrix operations", "[mathlib][matrix]") {
    SECTION("Matrix creation and access") {
        Matrix2D<double> m(2, 3);
        REQUIRE(m.rows() == 2);
        REQUIRE(m.cols() == 3);
        
        m(0, 0) = 1.0;
        m(0, 1) = 2.0;
        m(0, 2) = 3.0;
        m(1, 0) = 4.0;
        m(1, 1) = 5.0;
        m(1, 2) = 6.0;
        
        REQUIRE(m(0, 0) == 1.0);
        REQUIRE(m(1, 2) == 6.0);
    }
    
    SECTION("Matrix multiplication") {
        Matrix2D<double> a(2, 3);
        a(0, 0) = 1; a(0, 1) = 2; a(0, 2) = 3;
        a(1, 0) = 4; a(1, 1) = 5; a(1, 2) = 6;
        
        Matrix2D<double> b(3, 2);
        b(0, 0) = 7; b(0, 1) = 8;
        b(1, 0) = 9; b(1, 1) = 10;
        b(2, 0) = 11; b(2, 1) = 12;
        
        auto result = matrix_multiply(a, b);
        REQUIRE(result.has_value());
        
        auto& c = result.value();
        REQUIRE(c.rows() == 2);
        REQUIRE(c.cols() == 2);
        
        // 1*7 + 2*9 + 3*11 = 58
        REQUIRE_THAT(c(0, 0), WithinRel(58.0, 1e-10));
        // 1*8 + 2*10 + 3*12 = 64
        REQUIRE_THAT(c(0, 1), WithinRel(64.0, 1e-10));
        // 4*7 + 5*9 + 6*11 = 139
        REQUIRE_THAT(c(1, 0), WithinRel(139.0, 1e-10));
        // 4*8 + 5*10 + 6*12 = 154
        REQUIRE_THAT(c(1, 1), WithinRel(154.0, 1e-10));
    }
    
    SECTION("Matrix multiplication with incompatible dimensions") {
        Matrix2D<double> a(2, 3);
        Matrix2D<double> b(2, 3); // Wrong dimensions
        
        auto result = matrix_multiply(a, b);
        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "Matrix dimensions incompatible for multiplication");
    }
}

TEST_CASE("Concepts compile-time checks", "[mathlib][concepts]") {
    // These are compile-time tests
    STATIC_REQUIRE(Arithmetic<int>);
    STATIC_REQUIRE(Arithmetic<double>);
    STATIC_REQUIRE(Integral<int>);
    STATIC_REQUIRE(FloatingPoint<double>);
    STATIC_REQUIRE(!Integral<double>);
    STATIC_REQUIRE(!FloatingPoint<int>);
    
    // ContainerLike concept
    STATIC_REQUIRE(ContainerLike<std::vector<int>>);
    STATIC_REQUIRE(ContainerLike<std::vector<double>>);
}

TEST_CASE("Performance and edge cases", "[mathlib][performance]") {
    SECTION("Large vector operations") {
        const size_t N = 10000;
        std::vector<double> a(N), b(N);
        
        // Fill with test data
        for (size_t i = 0; i < N; ++i) {
            a[i] = static_cast<double>(i);
            b[i] = static_cast<double>(N - i);
        }
        
        auto result = vector_add(a, b);
        REQUIRE(result.has_value());
        
        auto& vec = result.value();
        REQUIRE(vec.size() == N);
        
        // Check a few values
        REQUIRE_THAT(vec[0], WithinRel(static_cast<double>(N), 1e-10));
        REQUIRE_THAT(vec[N/2], WithinRel(static_cast<double>(N), 1e-10));
        REQUIRE_THAT(vec[N-1], WithinRel(static_cast<double>(N), 1e-10));
    }
    
    SECTION("Zero and negative values") {
        std::vector<double> a = {0.0, -1.0, -2.0};
        std::vector<double> b = {0.0, 1.0, 2.0};
        
        auto add_result = vector_add(a, b);
        REQUIRE(add_result.has_value());
        REQUIRE(add_result.value() == std::vector<double>{0.0, 0.0, 0.0});
        
        auto mul_result = vector_multiply(a, b);
        REQUIRE(mul_result.has_value());
        REQUIRE(mul_result.value() == std::vector<double>{0.0, -1.0, -4.0});
    }
}