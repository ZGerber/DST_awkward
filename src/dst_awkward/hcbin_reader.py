"""
Parser for HCBIN (Light Flux Bin) DST bank.

This module parses the HCBIN bank (bank_id=15007) which contains light flux
bin information for BigH data analysis. The bank uses a bitmask-gated section
with conditional data based on failmode checks.

This bank is designed to work in conjunction with PRFC bank in the context
of the "profile constraint geometry fit".
"""

from __future__ import annotations

from typing import Any

from .conditional_bank_utils import (
    MAXFIT,
    BufferReader,
    ConditionalBankResult,
    decode_mask_msb_first,
    fit_empty_arrays,
    fit_list,
    fit_zeros,
)


def parse_hcbin_bank(
    buffer: bytes, start_offset: int = 8, endian: str = "<"
) -> ConditionalBankResult:
    """
    Parse an HCBIN bank (bank_id=15007) from `buffer`.

    This parser follows `hcbin_bank_to_common_` in `hcbin_dst.c`:
    - Single bininfo mask (int16) decoded MSB-first
    - For each active fit: timestamp fields, failmode check
    - If failmode==SUCCESS: nbin and variable-length arrays

    Values are stored in a *dense* 16-fit representation:
    arrays/lists are length 16; missing fits are None or empty arrays.

    Args:
        buffer: Full bank bytes, including the 8-byte [bank_id, bank_version] header.
        start_offset: Byte offset where payload begins (default 8).
        endian: Endianness character for numpy dtypes ('<' little, '>' big).

    Returns:
        ConditionalBankResult(data=dict, cursor=int)
    """
    reader = BufferReader(buffer, start_offset, endian)

    SUCCESS = 0

    # --- 1) Read bininfo mask (int16) ---
    bininfo_mask = reader.read_i2()
    bininfo = decode_mask_msb_first(bininfo_mask, bits=16)

    # --- 2) Initialize per-fit storage ---
    # Timestamp fields (present for all active fits)
    jday = fit_list()
    jsec = fit_list()
    msec = fit_list()

    # Failmode (present for all active fits)
    failmode = fit_list()

    # Bin count (only if failmode==SUCCESS)
    nbin = fit_zeros()

    # Bin direction vectors (only if failmode==SUCCESS)
    bvx = fit_empty_arrays(reader.f8)
    bvy = fit_empty_arrays(reader.f8)
    bvz = fit_empty_arrays(reader.f8)

    # Bin size in degrees (only if failmode==SUCCESS)
    bsz = fit_empty_arrays(reader.f8)

    # Signal in pe/degree/m^2 and error (only if failmode==SUCCESS)
    sig = fit_empty_arrays(reader.f8)
    sigerr = fit_empty_arrays(reader.f8)

    # Correction factor / exposure (only if failmode==SUCCESS)
    cfc = fit_empty_arrays(reader.f8)

    # Good bin indicator (only if failmode==SUCCESS)
    ig = fit_empty_arrays(reader.i4)

    # --- 3) Loop over 16 fits ---
    for i in range(MAXFIT):
        if not bininfo[i]:
            continue

        # Read timestamp fields (always present for active fits)
        jday[i] = reader.read_i4()
        jsec[i] = reader.read_i4()
        msec[i] = reader.read_i4()

        # Read failmode
        failmode[i] = reader.read_i4()

        if failmode[i] != SUCCESS:
            continue

        # Read nbin and arrays (only if failmode==SUCCESS)
        nb = reader.read_i2()
        nbin[i] = nb

        # Read bin direction vectors
        bvx[i] = reader.read_f8_array(nb)
        bvy[i] = reader.read_f8_array(nb)
        bvz[i] = reader.read_f8_array(nb)

        # Read bin size
        bsz[i] = reader.read_f8_array(nb)

        # Read signal and error
        sig[i] = reader.read_f8_array(nb)
        sigerr[i] = reader.read_f8_array(nb)

        # Read correction factor
        cfc[i] = reader.read_f8_array(nb)

        # Read good bin indicator
        ig[i] = reader.read_i4_array(nb)

    # --- 4) Build result dictionary ---
    data: dict[str, Any] = {
        # Mask
        "bininfo_mask": bininfo_mask,
        "bininfo": bininfo,
        # Timestamp fields
        "jday": jday,
        "jsec": jsec,
        "msec": msec,
        # Status
        "failmode": failmode,
        "nbin": nbin,
        # Bin direction
        "bvx": bvx,
        "bvy": bvy,
        "bvz": bvz,
        # Bin properties
        "bsz": bsz,
        "sig": sig,
        "sigerr": sigerr,
        "cfc": cfc,
        "ig": ig,
    }

    return ConditionalBankResult(data=data, cursor=reader.cursor)
