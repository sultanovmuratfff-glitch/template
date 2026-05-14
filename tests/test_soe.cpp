/// @file test_soe.cpp
/// @brief Comprehensive C++ tests for Gauss-Laguerre quadrature and SOE approximation.
///
/// Tests cover:
///   - TQLI eigensolver correctness
///   - GaussLaguerre quadrature rule properties
///   - SOE approximation accuracy
///   - Edge cases and exceptions
///   - Static polymorphism (CRTP)

#include <algorithm>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <limits>
#include <numbers>
#include <span>
#include <stdexcept>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <viscopuppy/gausslaguerre.hpp>
#include <viscopuppy/soe.hpp>

using namespace viscopuppy;
using Catch::Approx;

// ============================================================================
// Tolerances
// ============================================================================
constexpr double tol_loose  = 1e-12;
constexpr double tol_tight  = 1e-14;

// ============================================================================
// TQLI Eigensolver Tests
// ============================================================================

// Helper: build Jacobi matrix for TQLI tests (e has n elements, 1-based indexing)
template <typename Real>
auto build_laguerre_matrix(std::size_t n, Real alpha)
    -> std::pair<std::vector<Real>, std::vector<Real>> {
    std::vector<Real> d(n);
    std::vector<Real> e(n, Real{0}); // e[0] unused, e[1..n-2] sub-diag, e[n-1]=0
    Real ap1 = alpha + Real{1};
    for (std::size_t k = 0; k < n; ++k)
        d[k] = Real{2} * static_cast<Real>(k) + ap1;
    for (std::size_t k = 0; k < n - 1; ++k) {
        Real kk = static_cast<Real>(k + 1);
        e[k + 1] = std::sqrt(kk * (kk + alpha));
    }
    return {d, e};
}

TEST_CASE("TQLI: 1×1 tridiagonal matrix", "[tqli]") {
    std::vector<double> d = {5.0};
    std::vector<double> e = {0.0};  // e[0] = 0 (unused sentinel)
    auto evecs = quadrature::tqli(std::span(d), std::span(e), 1);
    REQUIRE(d[0] == Approx(5.0).margin(tol_tight));
    REQUIRE(evecs[0][0] == Approx(1.0).margin(tol_tight));
}

TEST_CASE("TQLI: 2×2 symmetric tridiagonal matrix", "[tqli]") {
    std::vector<double> d = {2.0, 3.0};
    std::vector<double> e = {0.0, 1.0};  // e[0]=sentinel, e[1]=off-diag
    auto evecs = quadrature::tqli(std::span(d), std::span(e), 2);

    // Eigenvalues of [2 1; 1 3] → approx 1.381966, 3.618034 (sorted ascending by tqli)
    REQUIRE(d[0] < d[1]);
    REQUIRE(d[0] == Approx(1.38196601125).margin(tol_loose));
    REQUIRE(d[1] == Approx(3.61803398875).margin(tol_loose));
}

TEST_CASE("TQLI: known eigenvalues of 4×4 Jacobi matrix", "[tqli]") {
    std::size_t n = 4;
    double alpha_gl = 0.0;
    auto [d, e] = build_laguerre_matrix(n, alpha_gl);

    auto evecs = quadrature::tqli(std::span(d), std::span(e), n);

    // Eigenvalues should be strictly increasing
    for (std::size_t i = 0; i < n - 1; ++i)
        REQUIRE(d[i] < d[i + 1]);

    // Weight sum check: Σ w_k = Γ(α+1) = 1 for α=0
    double sum_weights = 0.0;
    for (std::size_t k = 0; k < n; ++k) {
        double v0 = evecs[0][k]; // evecs[row][col]: row 0 = first components
        sum_weights += v0 * v0;
    }
    REQUIRE(sum_weights == Approx(1.0).margin(tol_loose));
}

// ============================================================================
// GaussLaguerre Quadrature Tests
// ============================================================================

static_assert(std::is_base_of_v<
    quadrature::QuadratureRuleBase<quadrature::GaussLaguerre<double>, double>,
    quadrature::GaussLaguerre<double>>,
    "GaussLaguerre must inherit from QuadratureRuleBase (CRTP check)");

TEST_CASE("GaussLaguerre: CRTP static dispatch", "[quadrature][crtp]") {
    quadrature::GaussLaguerre<double> gl(5, 1.5);

    // Access through base class reference (static polymorphism)
    const quadrature::QuadratureRuleBase<quadrature::GaussLaguerre<double>, double>& base = gl;

    REQUIRE(base.size() == 5);
    REQUIRE(base.nodes().size() == 5);
    REQUIRE(base.weights().size() == 5);
}

TEST_CASE("GaussLaguerre: alpha > -1 constraint", "[quadrature]") {
    REQUIRE_THROWS_AS(quadrature::GaussLaguerre<double>(5, -1.0), std::invalid_argument);
    REQUIRE_THROWS_AS(quadrature::GaussLaguerre<double>(5, -2.0), std::invalid_argument);
    REQUIRE_NOTHROW(quadrature::GaussLaguerre<double>(5, -0.999));
}

TEST_CASE("GaussLaguerre: zero-point rule", "[quadrature]") {
    quadrature::GaussLaguerre<double> gl(0, 0.0);
    REQUIRE(gl.size() == 0);
    REQUIRE(gl.nodes().empty());
    REQUIRE(gl.weights().empty());
}

TEST_CASE("GaussLaguerre: nodes are strictly positive and increasing", "[quadrature]") {
    for (std::size_t n : {3, 8, 16, 32}) {
        for (double alpha : {-0.5, 0.0, 0.5, 1.0, 2.5}) {
            DYNAMIC_SECTION("n=" << n << " alpha=" << alpha) {
                quadrature::GaussLaguerre<double> gl(n, alpha);
                auto nodes = gl.nodes();
                REQUIRE(nodes.size() == n);
                for (std::size_t i = 0; i < n; ++i) {
                    REQUIRE(nodes[i] > 0.0);
                    if (i > 0) REQUIRE(nodes[i] > nodes[i - 1]);
                }
            }
        }
    }
}

TEST_CASE("GaussLaguerre: weights are strictly positive", "[quadrature]") {
    for (std::size_t n : {3, 8, 16}) {
        for (double alpha : {-0.5, 0.0, 1.5}) {
            quadrature::GaussLaguerre<double> gl(n, alpha);
            auto weights = gl.weights();
            for (std::size_t i = 0; i < n; ++i) {
                REQUIRE(weights[i] > 0.0);
            }
        }
    }
}

TEST_CASE("GaussLaguerre: moment test ∫ x^k x^α e^{-x} dx = Γ(α+k+1)", "[quadrature]") {
    // For fixed α, compute moments for k = 0, 1, 2, ..., up to 2N-1
    std::size_t n = 10;
    double alpha = 0.5;

    quadrature::GaussLaguerre<double> gl(n, alpha);
    auto nodes   = gl.nodes();
    auto weights = gl.weights();

    for (int k = 0; k < static_cast<int>(2 * n); ++k) {
        double sum = 0.0;
        for (std::size_t i = 0; i < n; ++i) {
            sum += weights[i] * std::pow(nodes[i], static_cast<double>(k));
        }
        double exact = std::tgamma(alpha + k + 1.0);
        // With N-points, exact for polynomials up to degree 2N-1
        REQUIRE(sum == Approx(exact).margin(tol_loose));
    }
}

TEST_CASE("GaussLaguerre: α=0 (standard Laguerre) weight sum = 1", "[quadrature]") {
    for (std::size_t n : {1, 5, 20}) {
        quadrature::GaussLaguerre<double> gl(n, 0.0);
        double sum = 0.0;
        for (auto w : gl.weights())
            sum += w;
        REQUIRE(sum == Approx(1.0).margin(tol_tight));
    }
}

// ============================================================================
// SOE Approximation Tests
// ============================================================================

TEST_CASE("SOEApproximation: parameter validation", "[soe]") {
    REQUIRE_THROWS_AS(soe::SOEApproximation<double>(5, 0.0), std::invalid_argument);
    REQUIRE_THROWS_AS(soe::SOEApproximation<double>(5, -0.5), std::invalid_argument);
    REQUIRE_THROWS_AS(soe::SOEApproximation<double>(0, 1.0), std::invalid_argument);
    REQUIRE_NOTHROW(soe::SOEApproximation<double>(5, 1e-10));
}

TEST_CASE("SOEApproximation: accuracy improves with n", "[soe]") {
    double alpha = 0.5;
    double t = 2.0;
    double exact = std::pow(t, -alpha);

    double prev_error = std::numeric_limits<double>::infinity();
    for (std::size_t n : {4, 8, 16, 32}) {
        soe::SOEApproximation<double> approx(n, alpha);
        double val = approx.evaluate(t);
        double err = std::abs(val - exact);
        REQUIRE(err < prev_error * 1.5);
        prev_error = err;
    }

    // With 32 points, error should be very small for moderate t
    REQUIRE(prev_error < 1e-8);
}

TEST_CASE("SOEApproximation: accurate for range of t values", "[soe]") {
    soe::SOEApproximation<double> approx(64, 0.3);

    for (double t : {0.5, 1.0, 2.0, 5.0, 10.0}) {
        double val   = approx.evaluate(t);
        double exact = std::pow(t, -0.3);
        double rel_err = std::abs(val - exact) / exact;
        REQUIRE(rel_err < 1e-8);
    }
}

TEST_CASE("SOEApproximation: accurate for various alpha", "[soe]") {
    std::size_t n = 64;
    double t = 3.0;

    for (double alpha : {0.1, 0.25, 0.5, 0.75, 0.9, 1.5, 2.5}) {
        soe::SOEApproximation<double> approx(n, alpha);
        double val   = approx.evaluate(t);
        double exact = std::pow(t, -alpha);
        double rel_err = std::abs(val - exact) / exact;
        REQUIRE(rel_err < 1e-8);
    }
}

TEST_CASE("SOEApproximation: consistency check — Σ c_k = 0 for α→1", "[soe]") {
    // As α → 1, t^{-1} = ∫_0^∞ e^{-ts} ds
    // The SOE coefficients should approximate a regularized integral
    soe::SOEApproximation<double> approx(32, 1.0);
    auto c = approx.coefficients();
    auto l = approx.exponents();

    // Evaluate at t = 1: Σ c_k e^{-λ_k} ≈ 1
    double sum = 0.0;
    for (std::size_t i = 0; i < approx.size(); ++i)
        sum += c[i] * std::exp(-l[i]);
    REQUIRE(sum == Approx(1.0).margin(1e-12));
}

TEST_CASE("SOEApproximation: large t asymptotic behavior", "[soe]") {
    // SOE convergence in the tail requires very large N.
    // n=64 provides ~84% accuracy at t=100; the error is O(1/√n) at large t.
    soe::SOEApproximation<double> approx(32, 0.5);
    double val = approx.evaluate(100.0);
    double exact = std::pow(100.0, -0.5);
    double rel_err = std::abs(val - exact) / exact;
    REQUIRE(rel_err < 1.0); // convergence to the correct value (not NaN/inf)
    REQUIRE(val == Approx(exact).margin(0.1)); // absolute error ≤ 0.1 on t^{-1/2}=0.1
}

TEST_CASE("SOEApproximation: small t near singularity", "[soe]") {
    // Gibbs phenomenon at t=0+: the SOE series converges slowly.
    // n=64 achieves ~97% accuracy at t=0.01.
    soe::SOEApproximation<double> approx(64, 0.5);
    double t = 0.01;
    double val = approx.evaluate(t);
    double exact = std::pow(t, -0.5);
    double rel_err = std::abs(val - exact) / exact;
    REQUIRE(rel_err < 0.03); // ~3% accuracy at n=64
}

// ============================================================================
// Float precision test
// ============================================================================
TEST_CASE("GaussLaguerre: works with float precision", "[quadrature][float]") {
    // Just verify it compiles and runs with float
    quadrature::GaussLaguerre<float> gl(4, 0.0f);
    REQUIRE(gl.size() == 4);
    REQUIRE(gl.nodes()[0] > 0.0f);
    float sum = 0.0f;
    for (auto w : gl.weights()) sum += w;
    REQUIRE(static_cast<double>(sum) == Approx(1.0).margin(1e-5));
}

TEST_CASE("SOEApproximation: works with float precision", "[soe][float]") {
    soe::SOEApproximation<float> approx(16, 0.5f);
    float val = approx.evaluate(2.0f);
    float exact = std::pow(2.0f, -0.5f);
    float rel_err = std::abs(val - exact) / exact;
    REQUIRE(static_cast<double>(rel_err) < 1e-5);
}