"""
Parser for HCTIM (Time Geometry) DST bank.

This module parses the HCTIM bank (bank_id=15006) which contains track geometry
from profile constraint geometry fit, used for analysis of BigH data.

The bank uses a bitmask-gated section with conditional data based on failmode checks.
This bank is designed to work in conjunction with PRFC bank.
"""

from __future__ import annotations

from typing import Any

import numpy as np

from .conditional_bank_utils import (
    MAXFIT,
    BufferReader,
    ConditionalBankResult,
    decode_mask_msb_first,
    fit_empty_arrays,
    fit_list,
    fit_zeros,
)


def parse_hctim_bank(
    buffer: bytes, start_offset: int = 8, endian: str = "<"
) -> ConditionalBankResult:
    """
    Parse an HCTIM bank (bank_id=15006) from `buffer`.

    This parser follows `hctim_bank_to_common_` in `hctim_dst.c`:
    - Single timinfo mask (int16) decoded MSB-first
    - For each active fit: timestamp fields, failmode check
    - If failmode==SUCCESS: geometry parameters and tube data

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

    # --- 1) Read timinfo mask (int16) ---
    timinfo_mask = reader.read_i2()
    timinfo = decode_mask_msb_first(timinfo_mask, bits=16)

    # --- 2) Initialize per-fit storage ---
    # Timestamp fields (present for all active fits)
    jday = fit_list()
    jsec = fit_list()
    msec = fit_list()

    # Failmode (present for all active fits)
    failmode = fit_list()

    # Chi2 values (only if failmode==SUCCESS)
    mchi2 = fit_list()
    rchi2 = fit_list()
    lchi2 = fit_list()

    # Rp values (only if failmode==SUCCESS)
    mrp = fit_list()
    rrp = fit_list()
    lrp = fit_list()

    # Psi values (only if failmode==SUCCESS)
    mpsi = fit_list()
    rpsi = fit_list()
    lpsi = fit_list()

    # Theta values (only if failmode==SUCCESS)
    mthe = fit_list()
    rthe = fit_list()
    lthe = fit_list()

    # Phi values (only if failmode==SUCCESS)
    mphi = fit_list()
    rphi = fit_list()
    lphi = fit_list()

    # Direction vectors [3] (only if failmode==SUCCESS)
    mtkv = fit_list()
    rtkv = fit_list()
    ltkv = fit_list()

    # Rp vectors [3] (only if failmode==SUCCESS)
    mrpv = fit_list()
    rrpv = fit_list()
    lrpv = fit_list()

    # Rp unit vectors [3] (only if failmode==SUCCESS)
    mrpuv = fit_list()
    rrpuv = fit_list()
    lrpuv = fit_list()

    # Shower normal vectors [3] (only if failmode==SUCCESS)
    mshwn = fit_list()
    rshwn = fit_list()
    lshwn = fit_list()

    # Core location vectors [3] (only if failmode==SUCCESS)
    mcore = fit_list()
    rcore = fit_list()
    lcore = fit_list()

    # Mirror info (only if failmode==SUCCESS)
    nmir = fit_zeros()
    mir = fit_empty_arrays(reader.i4)
    mirntube = fit_empty_arrays(reader.i4)

    # Tube info (only if failmode==SUCCESS)
    ntube = fit_zeros()
    tube = fit_empty_arrays(reader.i4)
    tubemir = fit_empty_arrays(reader.i4)
    ig = fit_empty_arrays(reader.i4)

    # Per-tube data arrays (only if failmode==SUCCESS)
    time = fit_empty_arrays(reader.f8)
    timefit = fit_empty_arrays(reader.f8)
    thetb = fit_empty_arrays(reader.f8)
    sgmt = fit_empty_arrays(reader.f8)
    asx = fit_empty_arrays(reader.f8)
    asy = fit_empty_arrays(reader.f8)
    asz = fit_empty_arrays(reader.f8)

    # --- 3) Parse each active fit ---
    for i in range(MAXFIT):
        if not timinfo[i]:
            continue

        # Timestamp fields (always present for active fits)
        jday[i] = reader.read_i4()
        jsec[i] = reader.read_i4()
        msec[i] = reader.read_i4()

        # Failmode (always present for active fits)
        failmode[i] = reader.read_i4()

        if failmode[i] != SUCCESS:
            continue

        # Chi2 values
        mchi2[i] = reader.read_f8()
        rchi2[i] = reader.read_f8()
        lchi2[i] = reader.read_f8()

        # Rp values
        mrp[i] = reader.read_f8()
        rrp[i] = reader.read_f8()
        lrp[i] = reader.read_f8()

        # Psi values
        mpsi[i] = reader.read_f8()
        rpsi[i] = reader.read_f8()
        lpsi[i] = reader.read_f8()

        # Theta values
        mthe[i] = reader.read_f8()
        rthe[i] = reader.read_f8()
        lthe[i] = reader.read_f8()

        # Phi values
        mphi[i] = reader.read_f8()
        rphi[i] = reader.read_f8()
        lphi[i] = reader.read_f8()

        # Direction vectors [3]
        mtkv[i] = reader.read_f8_array(3)
        rtkv[i] = reader.read_f8_array(3)
        ltkv[i] = reader.read_f8_array(3)

        # Rp vectors [3]
        mrpv[i] = reader.read_f8_array(3)
        rrpv[i] = reader.read_f8_array(3)
        lrpv[i] = reader.read_f8_array(3)

        # Rp unit vectors [3]
        mrpuv[i] = reader.read_f8_array(3)
        rrpuv[i] = reader.read_f8_array(3)
        lrpuv[i] = reader.read_f8_array(3)

        # Shower normal vectors [3]
        mshwn[i] = reader.read_f8_array(3)
        rshwn[i] = reader.read_f8_array(3)
        lshwn[i] = reader.read_f8_array(3)

        # Core location vectors [3]
        mcore[i] = reader.read_f8_array(3)
        rcore[i] = reader.read_f8_array(3)
        lcore[i] = reader.read_f8_array(3)

        # Mirror info
        nmir_i = reader.read_i2()
        nmir[i] = nmir_i
        mir[i] = reader.read_i2_array(nmir_i).astype(np.int32)
        mirntube[i] = reader.read_i2_array(nmir_i).astype(np.int32)

        # Tube info
        ntube_i = reader.read_i2()
        ntube[i] = ntube_i
        tube[i] = reader.read_i2_array(ntube_i).astype(np.int32)
        tubemir[i] = reader.read_i2_array(ntube_i).astype(np.int32)
        ig[i] = reader.read_i2_array(ntube_i).astype(np.int32)

        # Per-tube data arrays
        time[i] = reader.read_f8_array(ntube_i)
        timefit[i] = reader.read_f8_array(ntube_i)
        thetb[i] = reader.read_f8_array(ntube_i)
        sgmt[i] = reader.read_f8_array(ntube_i)
        asx[i] = reader.read_f8_array(ntube_i)
        asy[i] = reader.read_f8_array(ntube_i)
        asz[i] = reader.read_f8_array(ntube_i)

    # --- 4) Build result dictionary ---
    data: dict[str, Any] = {
        "timinfo_mask": timinfo_mask,
        "timinfo": timinfo,
        "jday": jday,
        "jsec": jsec,
        "msec": msec,
        "failmode": failmode,
        "mchi2": mchi2,
        "rchi2": rchi2,
        "lchi2": lchi2,
        "mrp": mrp,
        "rrp": rrp,
        "lrp": lrp,
        "mpsi": mpsi,
        "rpsi": rpsi,
        "lpsi": lpsi,
        "mthe": mthe,
        "rthe": rthe,
        "lthe": lthe,
        "mphi": mphi,
        "rphi": rphi,
        "lphi": lphi,
        "mtkv": mtkv,
        "rtkv": rtkv,
        "ltkv": ltkv,
        "mrpv": mrpv,
        "rrpv": rrpv,
        "lrpv": lrpv,
        "mrpuv": mrpuv,
        "rrpuv": rrpuv,
        "lrpuv": lrpuv,
        "mshwn": mshwn,
        "rshwn": rshwn,
        "lshwn": lshwn,
        "mcore": mcore,
        "rcore": rcore,
        "lcore": lcore,
        "nmir": nmir,
        "mir": mir,
        "mirntube": mirntube,
        "ntube": ntube,
        "tube": tube,
        "tubemir": tubemir,
        "ig": ig,
        "time": time,
        "timefit": timefit,
        "thetb": thetb,
        "sgmt": sgmt,
        "asx": asx,
        "asy": asy,
        "asz": asz,
    }

    return ConditionalBankResult(data=data, cursor=reader.cursor)
