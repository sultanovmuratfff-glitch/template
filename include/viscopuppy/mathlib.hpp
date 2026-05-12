#pragma once

#include <concepts>
#include <expected>
#include <string>
#include <type_traits>
#include <vector>
#include <span>
#include <ranges>
#include <coroutine>

namespace viscopuppy::mathlib {

// ============================================================================
// Concepts for type constraints (C++20/23)
// ============================================================================

/// Concept for arithmetic types
template<typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

/// Concept for floating point types
template<typename T>
concept FloatingPoint = std::floating_point<T>;

/// Concept for integral types
template<typename T>
concept Integral = std::integral<T>;

/// Concept for container types with data() and size() methods
template<typename Container>
concept ContainerLike = requires(Container c) {
    { c.data() } -> std::convertible_to<const typename Container::value_type*>;
    { c.size() } -> std::convertible_to<std::size_t>;
};

// ============================================================================
// Basic arithmetic operations
// ============================================================================

/// Add two numbers with concept constraints
template<Arithmetic T>
[[nodiscard]] constexpr auto add(T a, T b) noexcept -> T {
    return a + b;
}

/// Multiply two numbers with concept constraints
template<Arithmetic T>
[[nodiscard]] constexpr auto multiply(T a, T b) noexcept -> T {
    return a * b;
}

/// Safe division with error handling using std::expected (C++23)
template<FloatingPoint T>
[[nodiscard]] auto safe_divide(T numerator, T denominator) 
    -> std::expected<T, std::string> {
    
    if (denominator == T{0}) {
        return std::unexpected("Division by zero");
    }
    return numerator / denominator;
}

/// Compute factorial with constexpr and concept constraints
template<Integral T>
[[nodiscard]] constexpr auto factorial(T n) -> T {
    if (n < 0) return 0;
    T result = 1;
    for (T i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;
}

/// Compute Fibonacci number with constexpr
template<Integral T>
[[nodiscard]] constexpr auto fibonacci(T n) -> T {
    if (n <= 1) return n;
    
    T a = 0, b = 1;
    for (T i = 2; i <= n; ++i) {
        T temp = a + b;
        a = b;
        b = temp;
    }
    return b;
}

// ============================================================================
// Vector/Array operations with NumPy-like interface
// ============================================================================

/// Element-wise addition of two containers
template<ContainerLike Container>
[[nodiscard]] auto vector_add(const Container& a, const Container& b)
    -> std::expected<Container, std::string> {
    
    if (a.size() != b.size()) {
        return std::unexpected("Container sizes must match");
    }
    
    Container result;
    result.reserve(a.size());
    
    for (std::size_t i = 0; i < a.size(); ++i) {
        result.push_back(a.data()[i] + b.data()[i]);
    }
    
    return result;
}

/// Element-wise multiplication of two containers
template<ContainerLike Container>
[[nodiscard]] auto vector_multiply(const Container& a, const Container& b)
    -> std::expected<Container, std::string> {
    
    if (a.size() != b.size()) {
        return std::unexpected("Container sizes must match");
    }
    
    Container result;
    result.reserve(a.size());
    
    for (std::size_t i = 0; i < a.size(); ++i) {
        result.push_back(a.data()[i] * b.data()[i]);
    }
    
    return result;
}

/// Dot product of two containers
template<ContainerLike Container>
[[nodiscard]] auto dot_product(const Container& a, const Container& b)
    -> std::expected<typename Container::value_type, std::string> {
    
    if (a.size() != b.size()) {
        return std::unexpected("Container sizes must match");
    }
    
    using ValueType = typename Container::value_type;
    ValueType result{};
    
    for (std::size_t i = 0; i < a.size(); ++i) {
        result += a.data()[i] * b.data()[i];
    }
    
    return result;
}

// ============================================================================
// C++23 Ranges examples
// ============================================================================

/// Filter even numbers using C++23 ranges
template<std::ranges::range Range>
[[nodiscard]] auto filter_even(const Range& range) {
    auto even_filter = range | std::views::filter([](auto x) {
        return x % 2 == 0;
    });
    
    // Convert to vector for return
    return std::vector(std::ranges::begin(even_filter), 
                       std::ranges::end(even_filter));
}

/// Transform range using C++23 ranges
template<std::ranges::range Range, typename Func>
[[nodiscard]] auto transform_range(const Range& range, Func func) {
    auto transformed = range | std::views::transform(func);
    
    // Convert to vector for return
    using ResultType = std::invoke_result_t<Func, 
        typename std::ranges::range_value_t<Range>>;
    return std::vector<ResultType>(std::ranges::begin(transformed),
                                   std::ranges::end(transformed));
}

// ============================================================================
// C++23 Coroutine example
// ============================================================================

/// Simple coroutine generator for Fibonacci sequence
template<Integral T>
class FibonacciGenerator {
public:
    struct promise_type {
        T current_value{};
        
        auto get_return_object() {
            return FibonacciGenerator{handle_type::from_promise(*this)};
        }
        
        auto initial_suspend() noexcept { return std::suspend_always{}; }
        auto final_suspend() noexcept { return std::suspend_always{}; }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
        
        auto yield_value(T value) {
            current_value = value;
            return std::suspend_always{};
        }
    };
    
    using handle_type = std::coroutine_handle<promise_type>;
    
    explicit FibonacciGenerator(handle_type h) : handle(h) {}
    ~FibonacciGenerator() { if (handle) handle.destroy(); }
    
    FibonacciGenerator(const FibonacciGenerator&) = delete;
    FibonacciGenerator& operator=(const FibonacciGenerator&) = delete;
    
    FibonacciGenerator(FibonacciGenerator&& other) noexcept 
        : handle(other.handle) {
        other.handle = nullptr;
    }
    
    FibonacciGenerator& operator=(FibonacciGenerator&& other) noexcept {
        if (this != &other) {
            if (handle) handle.destroy();
            handle = other.handle;
            other.handle = nullptr;
        }
        return *this;
    }
    
    /// Get next value from generator
    [[nodiscard]] auto next() -> std::optional<T> {
        if (!handle || handle.done()) {
            return std::nullopt;
        }
        handle.resume();
        if (handle.done()) {
            return std::nullopt;
        }
        return handle.promise().current_value;
    }
    
private:
    handle_type handle;
};

/// Create Fibonacci sequence generator
template<Integral T>
[[nodiscard]] auto fibonacci_generator(T limit) -> FibonacciGenerator<T> {
    T a = 0, b = 1;
    
    for (T i = 0; i < limit; ++i) {
        if (i == 0) {
            co_yield a;
        } else if (i == 1) {
            co_yield b;
        } else {
            T next = a + b;
            a = b;
            b = next;
            co_yield next;
        }
    }
}

// ============================================================================
// Matrix operations (simplified 2D)
// ============================================================================

/// Simple 2D matrix type
template<Arithmetic T>
class Matrix2D {
public:
    Matrix2D(std::size_t rows, std::size_t cols) 
        : rows_(rows), cols_(cols), data_(rows * cols) {}
    
    [[nodiscard]] auto rows() const noexcept -> std::size_t { return rows_; }
    [[nodiscard]] auto cols() const noexcept -> std::size_t { return cols_; }
    
    [[nodiscard]] auto operator()(std::size_t row, std::size_t col) -> T& {
        return data_[row * cols_ + col];
    }
    
    [[nodiscard]] auto operator()(std::size_t row, std::size_t col) const -> const T& {
        return data_[row * cols_ + col];
    }
    
    [[nodiscard]] auto data() noexcept -> T* { return data_.data(); }
    [[nodiscard]] auto data() const noexcept -> const T* { return data_.data(); }
    
private:
    std::size_t rows_;
    std::size_t cols_;
    std::vector<T> data_;
};

/// Matrix multiplication
template<Arithmetic T>
[[nodiscard]] auto matrix_multiply(const Matrix2D<T>& a, const Matrix2D<T>& b)
    -> std::expected<Matrix2D<T>, std::string> {
    
    if (a.cols() != b.rows()) {
        return std::unexpected("Matrix dimensions incompatible for multiplication");
    }
    
    Matrix2D<T> result(a.rows(), b.cols());
    
    for (std::size_t i = 0; i < a.rows(); ++i) {
        for (std::size_t j = 0; j < b.cols(); ++j) {
            T sum{};
            for (std::size_t k = 0; k < a.cols(); ++k) {
                sum += a(i, k) * b(k, j);
            }
            result(i, j) = sum;
        }
    }
    
    return result;
}

} // namespace viscopuppy::mathlib