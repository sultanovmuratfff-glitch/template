#pragma once

/// @file soe.hpp
/// @brief Sum-of-Exponentials (SOE) approximation of t^{-alpha}.
///
/// Using the identity
///     t^{-alpha} = 1 / Gamma(alpha) * ∫_0^∞ s^{alpha-1} e^{-t s} ds
/// and a generalized Gauss-Laguerre quadrature rule with weight function
/// s^{alpha-1} e^{-s}, we obtain the approximation
///     t^{-alpha} ≈ Σ_{k=1}^N  c_k * exp(-λ_k * t)
/// where
///     x_k  = Gauss-Laguerre nodes  (for weight x^{alpha-1} e^{-x})
///     w_k  = Gauss-Laguerre weights
///     c_k  = w_k * exp(x_k) / Gamma(alpha)
///     λ_k  = x_k

#include "gausslaguerre.hpp"

#include <algorithm>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <limits>
#include <numbers>
#include <ranges>
#include <span>
#include <stdexcept>
#include <vector>

namespace viscopuppy::soe {

// ============================================================================
// SOEApproximation — Sum-of-Exponentials approximation of t^{-alpha}
// ============================================================================

/// @brief Sum-of-Exponentials approximation for the kernel  t ↦ t^{-α}.
///
/// @tparam Real  Floating-point type (float, double, long double).
///
/// ### Mathematical background
///
/// The Laplace transform identity:
///   t^{-α} = 1/Γ(α) ∫₀^∞ s^{α-1} e^{-ts}  ds     (α > 0, t > 0)
///
/// After a change of variable s = x/t, the integral becomes a
/// generalized Gauss-Laguerre integral with weight x^{α-1} e^{-x}.
/// Applying an N-point quadrature rule yields the SOE representation.
///
/// ### Usage
///
/// @code
///   viscopuppy::soe::SOEApproximation<double> approx(32, 0.5);
///   double val = approx.evaluate(2.0);          // t^{-0.5} ≈ …
/// @endcode
template <quadrature::RealType Real>
class SOEApproximation {
public:
    /// @brief Build the SOE approximation with N exponential terms.
    ///
    /// @param n     Number of quadrature points (number of exponentials).
    /// @param alpha Exponent parameter α > 0 for t^{-α}.
    ///              Must satisfy alpha > Real{0}.
    SOEApproximation(std::size_t n, Real alpha)
        : c_(n), lambda_(n), n_{n}, alpha_{alpha} {

        if (alpha <= Real{0})
            throw std::invalid_argument("SOEApproximation: alpha must be > 0");
        if (n == 0)
            throw std::invalid_argument("SOEApproximation: n must be > 0");

        // Generalized Gauss-Laguerre with weight x^{alpha-1} e^{-x}
        quadrature::GaussLaguerre<Real> gl(n, alpha - Real{1});

        auto nodes   = gl.nodes();
        auto weights = gl.weights();

        // c_k = w_k * exp(x_k) / Gamma(alpha)
        // λ_k = x_k
        Real inv_gamma = Real{1} / std::tgamma(alpha);
        for (std::size_t k = 0; k < n; ++k) {
            lambda_[k] = nodes[k];
            c_[k]      = weights[k] * std::exp(nodes[k]) * inv_gamma;
        }
    }

    // --- Accessors ---

    [[nodiscard]] auto coefficients() const noexcept -> std::span<const Real> {
        return c_;
    }
    [[nodiscard]] auto exponents() const noexcept -> std::span<const Real> {
        return lambda_;
    }
    [[nodiscard]] auto size() const noexcept -> std::size_t { return n_; }
    [[nodiscard]] auto alpha() const noexcept -> Real { return alpha_; }

    // --- Single-point evaluation ---

    /// @brief Evaluate the SOE approximation at a single point t > 0.
    [[nodiscard]] auto evaluate(Real t) const -> Real {
        Real result = Real{0};
        for (std::size_t k = 0; k < n_; ++k) {
            result += c_[k] * std::exp(-lambda_[k] * t);
        }
        return result;
    }

private:
    std::vector<Real> c_;       ///< Coefficients c_k
    std::vector<Real> lambda_;  ///< Exponents λ_k
    std::size_t n_{0};
    Real alpha_{};
};

} // namespace viscopuppy::soe