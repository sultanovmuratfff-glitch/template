#include <nanobind/nanobind.h>
#include <nanobind/ndarray.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>
#include <nanobind/stl/optional.h>

#include <memory>
#include <numeric>
#include <ranges>

#include <viscopuppy/mathlib.hpp>

namespace nb = nanobind;
using namespace nb::literals;

namespace viscopuppy::mathlib {

// Helper to convert std::expected to Python result
template<typename T>
auto expected_to_python(const std::expected<T, std::string>& result) {
    if (result.has_value()) {
        return nb::cast(result.value());
    } else {
        throw std::runtime_error(result.error());
    }
}

// Helper for NumPy array conversion — takes ownership of the vector
// so the returned ndarray owns its backing memory
template<typename T>
auto vector_to_ndarray(std::vector<T>&& vec) {
    // Move vector to heap so its lifetime is managed by the capsule
    auto* owned = new std::vector<T>(std::move(vec));
    size_t shape[1] = {owned->size()};

    nb::capsule owner(owned, [](void* p) noexcept {
        delete static_cast<std::vector<T>*>(p);
    });

    return nb::ndarray<T, nb::numpy>(
        owned->data(),   // data
        (size_t) 1,      // ndim
        shape,           // shape
        owner            // keep-alive handle
    );
}

} // namespace viscopuppy::mathlib

NB_MODULE(viscopuppy, m) {
    m.doc() = "A high-performance Python library built from modern C++23 code";
    
    // ========================================================================
    // Basic arithmetic operations
    // ========================================================================
    
    m.def("add", 
        nb::overload_cast<int, int>(&viscopuppy::mathlib::add<int>),
        "a"_a, "b"_a,
        "Add two integers"
    );
    
    m.def("add", 
        nb::overload_cast<double, double>(&viscopuppy::mathlib::add<double>),
        "a"_a, "b"_a,
        "Add two floating-point numbers"
    );
    
    m.def("multiply", 
        nb::overload_cast<int, int>(&viscopuppy::mathlib::multiply<int>),
        "a"_a, "b"_a,
        "Multiply two integers"
    );
    
    m.def("multiply", 
        nb::overload_cast<double, double>(&viscopuppy::mathlib::multiply<double>),
        "a"_a, "b"_a,
        "Multiply two floating-point numbers"
    );
    
    m.def("safe_divide", 
        [](double numerator, double denominator) {
            auto result = viscopuppy::mathlib::safe_divide(numerator, denominator);
            return viscopuppy::mathlib::expected_to_python(result);
        },
        "numerator"_a, "denominator"_a,
        "Safe division with error handling"
    );
    
    m.def("factorial",
        &viscopuppy::mathlib::factorial<int>,
        "n"_a,
        "Compute factorial of an integer"
    );
    
    m.def("fibonacci",
        &viscopuppy::mathlib::fibonacci<int>,
        "n"_a,
        "Compute Fibonacci number"
    );
    
    // ========================================================================
    // NumPy array operations
    // ========================================================================
    
    m.def("vector_add",
        [](const nb::ndarray<double, nb::ndim<1>>& a,
           const nb::ndarray<double, nb::ndim<1>>& b) {
            
            // Check shapes
            if (a.shape(0) != b.shape(0)) {
                throw std::runtime_error("Array shapes must match");
            }
            
            // Create result array
            std::vector<double> result(a.shape(0));
            
            // Perform element-wise addition
            for (size_t i = 0; i < a.shape(0); ++i) {
                result[i] = a(i) + b(i);
            }
            
            return viscopuppy::mathlib::vector_to_ndarray(std::move(result));
        },
        "a"_a, "b"_a,
        "Element-wise addition of two NumPy arrays"
    );
    
    m.def("vector_multiply",
        [](const nb::ndarray<double, nb::ndim<1>>& a,
           const nb::ndarray<double, nb::ndim<1>>& b) {
            
            // Check shapes
            if (a.shape(0) != b.shape(0)) {
                throw std::runtime_error("Array shapes must match");
            }
            
            // Create result array
            std::vector<double> result(a.shape(0));
            
            // Perform element-wise multiplication
            for (size_t i = 0; i < a.shape(0); ++i) {
                result[i] = a(i) * b(i);
            }
            
            return viscopuppy::mathlib::vector_to_ndarray(std::move(result));
        },
        "a"_a, "b"_a,
        "Element-wise multiplication of two NumPy arrays"
    );
    
    m.def("dot_product",
        [](const nb::ndarray<double, nb::ndim<1>>& a,
           const nb::ndarray<double, nb::ndim<1>>& b) -> double {
            
            // Check shapes
            if (a.shape(0) != b.shape(0)) {
                throw std::runtime_error("Array shapes must match");
            }
            
            // Compute dot product
            double result = 0.0;
            for (size_t i = 0; i < a.shape(0); ++i) {
                result += a(i) * b(i);
            }
            
            return result;
        },
        "a"_a, "b"_a,
        "Dot product of two NumPy arrays"
    );
    
    // ========================================================================
    // C++23 Ranges examples
    // ========================================================================
    
    m.def("filter_even",
        [](const std::vector<int>& numbers) {
            auto result = viscopuppy::mathlib::filter_even(numbers);
            return viscopuppy::mathlib::vector_to_ndarray(std::move(result));
        },
        "numbers"_a,
        "Filter even numbers from a list using C++23 ranges"
    );
    
    m.def("transform_range",
        [](const std::vector<double>& numbers) {
            auto result = viscopuppy::mathlib::transform_range(
                numbers,
                [](double x) { return x * x; }  // Square each element
            );
            return viscopuppy::mathlib::vector_to_ndarray(std::move(result));
        },
        "numbers"_a,
        "Transform range using C++23 ranges (squares each element)"
    );
    
    // ========================================================================
    // C++23 Coroutine example wrapper
    // ========================================================================
    
    // Python generator wrapper for Fibonacci sequence
    class FibonacciGeneratorPy {
    public:
        FibonacciGeneratorPy(int limit) 
            : gen_(std::make_shared<viscopuppy::mathlib::FibonacciGenerator<int>>(
                viscopuppy::mathlib::fibonacci_generator<int>(limit))) {}
        
        nb::object next() {
            auto value = gen_->next();
            if (value.has_value()) {
                return nb::cast(value.value());
            } else {
                throw nb::stop_iteration();
            }
        }
        
        FibonacciGeneratorPy __iter__() { return *this; }
        
    private:
        std::shared_ptr<viscopuppy::mathlib::FibonacciGenerator<int>> gen_;
    };
    
    nb::class_<FibonacciGeneratorPy>(m, "FibonacciGenerator")
        .def(nb::init<int>(), "limit"_a)
        .def("__iter__", &FibonacciGeneratorPy::__iter__)
        .def("__next__", &FibonacciGeneratorPy::next);
    
    m.def("fibonacci_generator",
        [](int limit) {
            return FibonacciGeneratorPy(limit);
        },
        "limit"_a,
        "Create a Fibonacci sequence generator using C++23 coroutines"
    );
    
    // ========================================================================
    // Matrix operations
    // ========================================================================
    
    // Python wrapper for Matrix2D
    nb::class_<viscopuppy::mathlib::Matrix2D<double>>(m, "Matrix2D")
        .def(nb::init<size_t, size_t>(), "rows"_a, "cols"_a)
        .def_prop_ro("rows", &viscopuppy::mathlib::Matrix2D<double>::rows)
        .def_prop_ro("cols", &viscopuppy::mathlib::Matrix2D<double>::cols)
        .def("__getitem__", 
            [](const viscopuppy::mathlib::Matrix2D<double>& m, 
               const nb::tuple& idx) {
                if (nb::len(idx) != 2) {
                    throw std::runtime_error("Matrix2D index must be a tuple of (row, col)");
                }
                size_t row = nb::cast<size_t>(idx[0]);
                size_t col = nb::cast<size_t>(idx[1]);
                return m(row, col);
            })
        .def("__setitem__",
            [](viscopuppy::mathlib::Matrix2D<double>& m,
               const nb::tuple& idx, double value) {
                if (nb::len(idx) != 2) {
                    throw std::runtime_error("Matrix2D index must be a tuple of (row, col)");
                }
                size_t row = nb::cast<size_t>(idx[0]);
                size_t col = nb::cast<size_t>(idx[1]);
                m(row, col) = value;
            })
        .def("to_numpy",
            [](const viscopuppy::mathlib::Matrix2D<double>& m) {
                size_t shape[2] = {m.rows(), m.cols()};
                // m.data() returns const double*, need to cast away const for ndarray
                return nb::ndarray<double, nb::numpy>(
                    const_cast<double*>(m.data()),   // data
                    (size_t) 2,                       // ndim
                    shape                              // shape
                );
            },
            "Convert matrix to NumPy array"
        );
    
    m.def("matrix_multiply",
        [](const viscopuppy::mathlib::Matrix2D<double>& a,
           const viscopuppy::mathlib::Matrix2D<double>& b) {
            auto result = viscopuppy::mathlib::matrix_multiply(a, b);
            return viscopuppy::mathlib::expected_to_python(result);
        },
        "a"_a, "b"_a,
        "Matrix multiplication"
    );
    
    // ========================================================================
    // Concept example (compile-time type checking)
    // ========================================================================
    
    m.def("concept_example",
        []() -> std::string {
            // Demonstrate concept usage
            static_assert(viscopuppy::mathlib::Arithmetic<int>);
            static_assert(viscopuppy::mathlib::Arithmetic<double>);
            static_assert(viscopuppy::mathlib::Integral<int>);
            static_assert(viscopuppy::mathlib::FloatingPoint<double>);
            
            return "C++23 concepts are working correctly!";
        },
        "Demonstrate C++23 concept usage"
    );
    
    // ========================================================================
    // Range example
    // ========================================================================
    
    m.def("range_example",
        []() -> std::string {
            std::vector<int> numbers = {1, 2, 3, 4, 5};
            
            // Use C++23 ranges
            auto squared = numbers | std::views::transform([](int x) { return x * x; });
            auto sum = std::accumulate(squared.begin(), squared.end(), 0);
            
            return "C++23 ranges example: sum of squares = " + std::to_string(sum);
        },
        "Demonstrate C++23 range usage"
    );
    
    // ========================================================================
    // Coroutine example
    // ========================================================================
    
    m.def("coroutine_example",
        [](int n) -> std::string {
            auto gen = viscopuppy::mathlib::fibonacci_generator<int>(n);
            std::vector<int> sequence;
            
            while (auto value = gen.next()) {
                sequence.push_back(*value);
            }
            
            return "Generated " + std::to_string(sequence.size()) + 
                   " Fibonacci numbers using C++23 coroutines";
        },
        "n"_a,
        "Demonstrate C++23 coroutine usage"
    );
}