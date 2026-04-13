"""
Turbomachinery Solver Package

This module contains the neuro-symbolic math engine (SymPy)
and the coordinate generation engine for the velocity triangles.
"""

# Expose the core classes at the package level for cleaner imports
from .equation_graph import EquationGraph
from .coordinate_engine import CoordinateEngine

__all__ = ["EquationGraph", "CoordinateEngine"]