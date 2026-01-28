"""
Shared utilities for parsing mask-gated conditional DST banks.

Banks like PRFC and HCBIN use bitmasks to indicate which of 16 "fits" are present,
with conditional sections that may or may not be present based on mask bits and
field values (like failmode checks).

This module provides common helpers:
- BufferReader: Stateful binary reader with cursor tracking
- decode_mask_msb_first: Decode packed bitmasks using MSB-first ordering
- fit_list, fit_empty_arrays, fit_zeros: Per-fit storage initialization
"""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any

import numpy as np

MAXFIT = 16  # Maximum number of fits in PRFC/HCBIN banks


@dataclass(frozen=True)
class ConditionalBankResult:
    """Result of parsing a conditional bank payload."""

    data: dict[str, Any]
    cursor: int


def decode_mask_msb_first(mask_i16: int, bits: int = 16) -> list[bool]:
    """
    Decode a packed mask using MSB-first bit ordering.

    This matches the C code pattern:
        if (mask & 0x8000) used; mask <<= 1; repeated MAXFIT times

    Args:
        mask_i16: The packed mask value (typically read as int16).
        bits: Number of bits to decode (default 16 for MAXFIT).

    Returns:
        List of booleans, one per fit, True if that fit is present.
    """
    m = int(mask_i16) & ((1 << bits) - 1)
    return [((m >> (bits - 1 - i)) & 1) == 1 for i in range(bits)]


def fit_list(default: Any = None) -> list[Any]:
    """Create a list of MAXFIT elements initialized to default value."""
    return [default] * MAXFIT


def fit_empty_arrays(dtype: np.dtype) -> list[np.ndarray]:
    """Create a list of MAXFIT empty arrays with the given dtype."""
    return [np.empty(0, dtype=dtype) for _ in range(MAXFIT)]


def fit_zeros() -> list[int]:
    """Create a list of MAXFIT zeros (for counters like nbin)."""
    return [0] * MAXFIT


class BufferReader:
    """
    Stateful binary buffer reader with cursor tracking.

    Provides convenient methods for reading scalars and arrays from a bytes
    buffer while automatically advancing the cursor position.
    """

    def __init__(self, buffer: bytes, cursor: int, endian: str = "<"):
        """
        Initialize the buffer reader.

        Args:
            buffer: The bytes buffer to read from.
            cursor: Starting byte offset in the buffer.
            endian: Endianness character ('<' little, '>' big).
        """
        self.buffer = buffer
        self.cursor = int(cursor)
        self.endian = endian

        # Pre-create dtype objects for efficiency
        self.i2 = np.dtype(f"{endian}i2")
        self.i4 = np.dtype(f"{endian}i4")
        self.f4 = np.dtype(f"{endian}f4")
        self.f8 = np.dtype(f"{endian}f8")

    def read_scalar(self, dtype: np.dtype) -> int | float:
        """Read a single scalar value and advance cursor."""
        v = np.frombuffer(self.buffer, dtype=dtype, count=1, offset=self.cursor)[0]
        self.cursor += int(dtype.itemsize)
        return v.item()

    def read_array(self, dtype: np.dtype, n: int) -> np.ndarray:
        """Read an array of n elements and advance cursor."""
        n = int(n)
        a = np.frombuffer(self.buffer, dtype=dtype, count=n, offset=self.cursor)
        self.cursor += int(n * dtype.itemsize)
        return a

    # Convenience methods for common types
    def read_i2(self) -> int:
        """Read a single int16."""
        return self.read_scalar(self.i2)

    def read_i4(self) -> int:
        """Read a single int32."""
        return self.read_scalar(self.i4)

    def read_f4(self) -> float:
        """Read a single float32."""
        return self.read_scalar(self.f4)

    def read_f8(self) -> float:
        """Read a single float64."""
        return self.read_scalar(self.f8)

    def read_i2_array(self, n: int) -> np.ndarray:
        """Read an array of int16."""
        return self.read_array(self.i2, n)

    def read_i4_array(self, n: int) -> np.ndarray:
        """Read an array of int32."""
        return self.read_array(self.i4, n)

    def read_f4_array(self, n: int) -> np.ndarray:
        """Read an array of float32."""
        return self.read_array(self.f4, n)

    def read_f8_array(self, n: int) -> np.ndarray:
        """Read an array of float64."""
        return self.read_array(self.f8, n)
