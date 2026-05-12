"""
viscopuppy - A high-performance Python library built from modern C++23 code.
"""

from ._version import __version__
from .viscopuppy import *

__all__ = [
    "__version__",
    "add",
    "multiply",
    "factorial",
    "fibonacci",
    "vector_add",
    "vector_multiply",
    "dot_product",
    "safe_divide",
    "coroutine_example",
    "concept_example",
    "range_example",
]