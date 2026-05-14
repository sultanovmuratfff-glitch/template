#pragma once

/// @file gausslaguerre.hpp
/// @brief Generalized Gauss-Laguerre quadrature via Golub-Welsch algorithm.
///
/// Computes nodes x_k and weights w_k for the weight function
///   w(x) = x^{alpha} e^{-x}   on (0, ∞)
/// using the eigenvalues and eigenvectors of the symmetric tridiagonal
/// Jacobi matrix built from the three-term recurrence coefficients.
///
/// Reference:
///   FastGaussQuadrature.jl — https://github.com/JuliaApproximation/FastGaussQuadrature.jl

#include <algorithm>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <limits>
#include <numbers>
#include <span>
#include <stdexcept>
#include <vector>

namespace viscopuppy::quadrature {

// ============================================================================
// Concepts
// ============================================================================

/// Constrain to IEEE floating-point types.
template <typename T>
concept RealType = std::floating_point<T> && requires(T x) {
    { std::isnan(x) } -> std::convertible_to<bool>;
};

// ============================================================================
// TQLI — Symmetric tridiagonal QL implicit eigensolver
// ============================================================================

/// @brief Compute all eigenvalues and first components of eigenvectors of a
///        symmetric tridiagonal matrix using the QL algorithm with implicit
///        shifts.
///
/// The matrix is represented by its diagonal `d[0..n-1]` and sub-diagonal
/// `e[0..n-2]`.  On output `d` holds the eigenvalues in ascending order
/// and `e` is destroyed.
///
/// @tparam Real  Floating-point type (float, double, long double).
/// @param d      Diagonal elements (in/out: eigenvalues on return).
/// @param e      Sub-diagonal elements (destroyed on return).
/// @param n      Dimension of the matrix.
/// @return       Matrix of eigenvectors; evec[k ][0] = first component of the
///               k-th eigenvector (used for quadrature weights).
template <RealType Real>
[[nodiscard]] auto tqli(std::span<Real> d, std::span<Real> e, std::size_t n)
    -> std::vector<std::vector<Real>> {

    auto eps = std::numeric_limits<Real>::epsilon();

    // Allocate eigenvector matrix (n × n, but we only really need first component)
    std::vector<std::vector<Real>> z(n);
    for (std::size_t i = 0; i < n; ++i) {
        z[i].resize(n, Real{0});
        z[i][i] = Real{1};
    }

    // --- Shift sub-diagonals (EISPACK convention) ---
    // Input: e[0] unused workspace, e[1..n-1] = sub-diagonals
    // After shift: e[0..n-3] = sub-diagonals, e[n-1] = 0 (sentinel)
    if (n > 1) {
        for (std::size_t i = 1; i < n; ++i)
            e[i - 1] = e[i];
        e[n - 1] = Real{0};
    }

    // --- QL iteration with implicit shifts (TQLI from EISPACK) ---
    constexpr std::size_t max_iter = 30;

    for (std::size_t l = 0; l < n; ++l) {
        std::size_t iter = 0;

        // Iterate until e[l] is negligible
        while (true) {
            // 1. Find m (l ≤ m < n-1) such that e[m] is negligible.
            //    When the loop exits naturally, m = n-1 (block [l, n-1]
            //    requires QR), otherwise e[m] is negligible.
            std::size_t m = l;
            for (; m < n - 1; ++m) {
                Real tst1 = std::abs(d[m]) + std::abs(d[m + 1]);
                Real tst2 = tst1 + std::abs(e[m]);
                if (std::abs(tst2 - tst1) <= eps * tst1) break;
            }
            if (m == l) break; // e[l] already negligible → eigenvalue found

            if (++iter > max_iter)
                throw std::runtime_error("TQLI did not converge");

            // 2. Wilkinson shift
            Real g = (d[l + 1] - d[l]) / (Real{2} * e[l]);
            Real r = std::sqrt(g * g + Real{1});
            g = d[m] - d[l] + e[l] / (g + (g >= 0 ? r : -r));

            Real s = Real{1};
            Real c = Real{1};
            Real p = Real{0};

            // 3. Givens rotations from i = m-1 down to i = l
            for (int ii = static_cast<int>(m) - 1; ii >= static_cast<int>(l); --ii) {
                auto i = static_cast<std::size_t>(ii);
                Real f = s * e[i];
                Real b = c * e[i];
                r = std::sqrt(f * f + g * g);
                e[i + 1] = r;

                if (r == Real{0}) {
                    d[i + 1] -= p;
                    e[m] = Real{0};
                    break;
                }
                s = f / r;
                c = g / r;
                g = d[i + 1] - p;
                r = (d[i] - g) * s + Real{2} * c * b;
                p = s * r;
                d[i + 1] = g + p;
                g = c * r - b;

                // Accumulate eigenvectors
                for (std::size_t k = 0; k < n; ++k) {
                    f = z[k][i + 1];
                    z[k][i + 1] = s * z[k][i] + c * f;
                    z[k][i]     = c * z[k][i] - s * f;
                }
            }

            if (r == Real{0}) continue;

            d[l] -= p;
            e[l] = g;
            e[m] = Real{0};
        }
    }

    // --- Sort eigenvalues (and corresponding eigenvectors) ---
    for (std::size_t i = 0; i < n - 1; ++i) {
        std::size_t k = i;
        Real p = d[i];
        for (std::size_t j = i + 1; j < n; ++j) {
            if (d[j] < p) {
                k = j;
                p = d[j];
            }
        }
        if (k != i) {
            d[k] = d[i];
            d[i] = p;
            for (std::size_t j = 0; j < n; ++j) {
                p = z[j][i];
                z[j][i] = z[j][k];
                z[j][k] = p;
            }
        }
    }

    return z;
}

// ============================================================================
// CRTP base for quadrature rules
// ============================================================================

template <typename Derived, RealType Real>
class QuadratureRuleBase {
public:
    [[nodiscard]] auto nodes() const noexcept -> std::span<const Real> {
        return static_cast<const Derived*>(this)->nodes_impl();
    }

    [[nodiscard]] auto weights() const noexcept -> std::span<const Real> {
        return static_cast<const Derived*>(this)->weights_impl();
    }

    [[nodiscard]] auto size() const noexcept -> std::size_t {
        return static_cast<const Derived*>(this)->size_impl();
    }
};

// ============================================================================
// Generalized Gauss-Laguerre quadrature
// ============================================================================

/// @brief Generalized Gauss-Laguerre quadrature rule.
///
/// Computes N-point rule for  ∫_0^∞ x^{alpha} e^{-x} f(x) dx.
///
/// The three-term recurrence for the generalized Laguerre polynomials is:
///   a_k = 2k + alpha + 1
///   b_k = -√(k * (k + alpha))
///
/// Nodes = eigenvalues of the Jacobi matrix J (a on diagonal, -b on off-diagonal).
/// Weights = Γ(alpha+1) * (first component of k-th eigenvector)²
///
/// We store the symmetric form so the off-diagonal is positive √b.
template <RealType Real>
class GaussLaguerre final : public QuadratureRuleBase<GaussLaguerre<Real>, Real> {
public:
    /// @param n     Number of quadrature points.
    /// @param alpha Exponent in the weight x^{alpha} e^{-x}  (alpha > -1).
    GaussLaguerre(std::size_t n, Real alpha)
        : nodes_(n), weights_(n), n_{n}, alpha_{alpha} {

        if (n == 0) return;
        if (alpha <= Real{-1})
            throw std::invalid_argument("GaussLaguerre: alpha must be > -1");

        compute(n, alpha);
    }

    // --- Getters (used by CRTP base) ---
    [[nodiscard]] auto nodes_impl() const noexcept -> std::span<const Real> {
        return nodes_;
    }
    [[nodiscard]] auto weights_impl() const noexcept -> std::span<const Real> {
        return weights_;
    }
    [[nodiscard]] auto size_impl() const noexcept -> std::size_t { return n_; }

private:
    void compute(std::size_t n, Real alpha) {
        // Build Jacobi matrix: diagonal a, sub-diagonal √(k*(k+alpha))
        std::vector<Real> a(n);             // diagonal
        std::vector<Real> e_work(n, Real{0}); // sub-diagonal (e[0] unused, e[n-1]=0 sentinel)

        Real ap1 = alpha + Real{1};

        for (std::size_t k = 0; k < n; ++k) {
            a[k] = Real{2} * static_cast<Real>(k) + ap1;
        }
        for (std::size_t k = 0; k < n - 1; ++k) {
            Real kk = static_cast<Real>(k + 1);
            e_work[k + 1] = std::sqrt(kk * (kk + alpha)); // stored at e[1]..e[n-2]
        }

        // TQLI computes eigenvalues (nodes) and eigenvector matrix
        auto evecs = tqli(std::span<Real>(a), std::span<Real>(e_work), n);

        // Nodes = eigenvalues (sorted ascending by tqli)
        std::copy(a.begin(), a.end(), nodes_.begin());

        // Weights = Γ(alpha+1) * (first component of k-th eigenvector)²
        // evecs[col][row]: column = eigenvector index, row = component
        // First component of k-th eigenvector = evecs[0][k]
        Real gamma_scale = std::tgamma(ap1);
        for (std::size_t k = 0; k < n; ++k) {
            Real v0 = evecs[0][k];
            weights_[k] = gamma_scale * v0 * v0;
        }
    }

    std::vector<Real> nodes_;
    std::vector<Real> weights_;
    std::size_t n_{0};
    Real alpha_{};
};

} // namespace viscopuppy::quadrature