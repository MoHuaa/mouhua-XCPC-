"""Build the printable ICPC reference from the repository's real sources."""

from .catalog import Catalog, Entry, Section, load_catalog

__all__ = ["Catalog", "Entry", "Section", "load_catalog"]
