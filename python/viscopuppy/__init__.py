"""
viscopuppy — Sum-of-Exponentials approximation for t^{-alpha}.

Based on generalized Gauss-Laguerre quadrature via the Golub-Welsch
algorithm (FastGaussQuadrature.jl).

Classes:
  GaussLaguerre(n, alpha)     Generalized Gauss-Laguerre quadrature rule.
  SOEApproximation(n, alpha)  Sum-of-Exponentials approximation.

Functions:
  soe_approximate(n, alpha, t)  One-shot SOE evaluation.
"""

from ._version import __version__

# Import native module (will be available after pip install / build)
try:
    from .viscopuppy import (
        GaussLaguerre,
        SOEApproximation,
        soe_approximate,
    )
except ImportError:
    pass  # Native module not yet built

__all__ = [
    "__version__",
    "GaussLaguerre",
    "SOEApproximation",
    "soe_approximate",
]