/// @file module.cpp
/// @brief Python bindings for viscopuppy — Gauss-Laguerre quadrature and SOE approximation.
///
/// Exposes:
///   - GaussLaguerre(n, alpha)  → generalized Gauss-Laguerre rule
///   - SOEApproximation(n, alpha) → sum-of-exponentials for t^{-alpha}
///   - soe_approximate(n, alpha, t) → quick one-shot evaluation

#include <nanobind/nanobind.h>
#include <nanobind/ndarray.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>
#include <nanobind/stl/optional.h>

#include <memory>
#include <span>

#include <viscopuppy/gausslaguerre.hpp>
#include <viscopuppy/soe.hpp>

namespace nb = nanobind;
using namespace nb::literals;
using namespace viscopuppy;

namespace viscopuppy::python {

// ============================================================================
// Helpers
// ============================================================================

/// Move a std::vector<T> into a NumPy array that owns its memory.
template <typename T>
auto vector_to_ndarray(std::vector<T>&& vec) {
    using Vec = std::vector<T>;
    auto* owned = new Vec(std::move(vec));
    std::size_t shape[1] = {owned->size()};

    nb::capsule owner(owned, [](void* p) noexcept {
        delete static_cast<Vec*>(p);
    });

    return nb::ndarray<T, nb::numpy>(owned->data(), static_cast<std::size_t>(1), shape, owner);
}

} // namespace viscopuppy::python

// ============================================================================
// Module registration
// ============================================================================

NB_MODULE(viscopuppy, m) {
    using Real = double; // Primary precision exposed to Python

    m.doc() = R"pbdoc(
        viscopuppy — Sum-of-Exponentials approximation for t^{-alpha}.

        Based on generalized Gauss-Laguerre quadrature via the Golub-Welsch
        algorithm (FastGaussQuadrature.jl).

        Classes:
          GaussLaguerre(n, alpha)   Generalized Gauss-Laguerre quadrature rule.
          SOEApproximation(n, alpha) Sum-of-Exponentials approximation.
    )pbdoc";

    // ========================================================================
    // GaussLaguerre — Generalized Gauss-Laguerre quadrature
    // ========================================================================

    nb::class_<quadrature::GaussLaguerre<Real>>(m, "GaussLaguerre",
        R"pbdoc(
            Generalized Gauss-Laguerre quadrature rule.

            Computes N-point quadrature for:
                ∫_0^∞  x^{alpha} e^{-x}  f(x)  dx

            Uses the Golub-Welsch algorithm (eigenvalues of the Jacobi matrix).

            Parameters
            ----------
            n : int
                Number of quadrature points.
            alpha : float
                Exponent in the weight function (alpha > -1).
        )pbdoc"
    )
        .def(nb::init<std::size_t, Real>(), "n"_a, "alpha"_a)
        .def_prop_ro("nodes", [](const quadrature::GaussLaguerre<Real>& self) {
            std::vector<Real> vec(self.nodes().begin(), self.nodes().end());
            return viscopuppy::python::vector_to_ndarray(std::move(vec));
        }, "Quadrature nodes x_k (eigenvalues)")
        .def_prop_ro("weights", [](const quadrature::GaussLaguerre<Real>& self) {
            std::vector<Real> vec(self.weights().begin(), self.weights().end());
            return viscopuppy::python::vector_to_ndarray(std::move(vec));
        }, "Quadrature weights w_k")
        .def_prop_ro("size", &quadrature::GaussLaguerre<Real>::size,
            "Number of quadrature points");


    // ========================================================================
    // SOEApproximation — Sum-of-Exponentials for t^{-alpha}
    // ========================================================================

    nb::class_<soe::SOEApproximation<Real>>(m, "SOEApproximation",
        R"pbdoc(
            Sum-of-Exponentials approximation for the kernel t ↦ t^{-alpha}.

            t^{-alpha} = 1/Gamma(alpha) * ∫_0^∞ s^{alpha-1} * e^{-t*s} ds

            Approximated via an N-term sum:
                t^{-alpha} ≈ Σ_{k=0}^{N-1} c_k * exp(-λ_k * t)

            Parameters
            ----------
            n : int
                Number of exponential terms (quadrature points).
            alpha : float
                Exponent of t (alpha > 0).
        )pbdoc"
    )
        .def(nb::init<std::size_t, Real>(), "n"_a, "alpha"_a)
        .def("evaluate", &soe::SOEApproximation<Real>::evaluate,
            "t"_a,
            "Evaluate the SOE approximation at a single point t > 0.")
        .def_prop_ro("coefficients", [](const soe::SOEApproximation<Real>& self) {
            std::vector<Real> vec(self.coefficients().begin(), self.coefficients().end());
            return viscopuppy::python::vector_to_ndarray(std::move(vec));
        }, "Coefficients c_k of the sum of exponentials")
        .def_prop_ro("exponents", [](const soe::SOEApproximation<Real>& self) {
            std::vector<Real> vec(self.exponents().begin(), self.exponents().end());
            return viscopuppy::python::vector_to_ndarray(std::move(vec));
        }, "Exponents λ_k of the sum of exponentials")
        .def_prop_ro("size", &soe::SOEApproximation<Real>::size,
            "Number of exponential terms");

    // ========================================================================
    // Convenience: one-shot SOE evaluation
    // ========================================================================

    m.def("soe_approximate",
        [](std::size_t n, Real alpha, Real t) -> Real {
            soe::SOEApproximation<Real> approx(n, alpha);
            return approx.evaluate(t);
        },
        "n"_a, "alpha"_a, "t"_a,
        R"pbdoc(
            One-shot SOE approximation of t^{-alpha}.

            Parameters
            ----------
            n : int
                Number of exponential terms.
            alpha : float
                Exponent (alpha > 0).
            t : float
                Point at which to evaluate (t > 0).

            Returns
            -------
            float
                Approximate value of t^{-alpha}.
        )pbdoc"
    );
}