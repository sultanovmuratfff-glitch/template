#!/usr/bin/env python3
"""Legacy setup.py for compatibility with older build tools."""

from setuptools import setup

# This file exists only for compatibility with older build tools
# that don't support pyproject.toml. The actual build configuration
# is in pyproject.toml using scikit-build-core.

if __name__ == "__main__":
    setup()