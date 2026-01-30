"""
Parser for STPLN (Plane Fit) DST bank.

This module parses the STPLN bank (bank_id=15043) which contains plane fit
information for stereo events. The bank has conditional packing based on
the if_eye flags for each telescope site.
"""

from __future__ import annotations

from typing import Any

import numpy as np

from .conditional_bank_utils import BufferReader, ConditionalBankResult


def parse_stpln_bank(
    buffer: bytes, start_offset: int = 0, endian: str = "<"
) -> ConditionalBankResult:
    """
    Parse an STPLN bank (bank_id=15043) from `buffer`.

    This parser follows `stpln_bank_to_common_` in `stpln_dst.c`:
    - Header: jday, jsec, msec, neye, nmir, ntube, maxeye, if_eye
    - Per-eye data (conditional on if_eye)
    - Mirror arrays (nmir elements)
    - Tube arrays (ntube elements)
    - Version 2+: saturated, mir_tube_id arrays

    Args:
        buffer: Full bank bytes, including the 8-byte [bank_id, bank_version] header.
        start_offset: Byte offset where bank starts (default 0, reads bank_id/version).
        endian: Endianness character for numpy dtypes ('<' little, '>' big).

    Returns:
        ConditionalBankResult(data=dict, cursor=int)
    """
    reader = BufferReader(buffer, start_offset, endian)

    # Read bank_id and bank_version
    bank_id = reader.read_i4()
    bank_version = reader.read_i4()

    # --- 1) Header ---
    # jday, jsec, msec (3 x int32)
    jday = reader.read_i4()
    jsec = reader.read_i4()
    msec = reader.read_i4()

    # neye, nmir, ntube (3 x int16)
    neye = reader.read_i2()
    nmir = reader.read_i2()
    ntube = reader.read_i2()

    # maxeye, if_eye
    maxeye = reader.read_i4()
    if_eye = reader.read_i4_array(maxeye)

    # --- 2) Per-eye data (conditional) ---
    eyeid = [None] * maxeye
    eye_nmir = [None] * maxeye
    eye_ngmir = [None] * maxeye
    eye_ntube = [None] * maxeye
    eye_ngtube = [None] * maxeye
    rmsdevpln = [None] * maxeye
    rmsdevtim = [None] * maxeye
    tracklength = [None] * maxeye
    crossingtime = [None] * maxeye
    ph_per_gtube = [None] * maxeye
    n_ampwt = [None] * maxeye
    errn_ampwt = [None] * maxeye

    for ieye in range(maxeye):
        if if_eye[ieye] != 1:
            continue

        eyeid[ieye] = reader.read_i2()
        eye_nmir[ieye] = reader.read_i2()
        eye_ngmir[ieye] = reader.read_i2()
        eye_ntube[ieye] = reader.read_i2()
        eye_ngtube[ieye] = reader.read_i2()

        rmsdevpln[ieye] = reader.read_f4()
        rmsdevtim[ieye] = reader.read_f4()
        tracklength[ieye] = reader.read_f4()
        crossingtime[ieye] = reader.read_f4()
        ph_per_gtube[ieye] = reader.read_f4()

        n_ampwt[ieye] = reader.read_f4_array(3).tolist()
        errn_ampwt[ieye] = reader.read_f4_array(6).tolist()

    # --- 3) Mirror arrays (nmir elements) ---
    mirid = reader.read_i2_array(nmir).astype(np.int32).tolist()
    mir_eye = reader.read_i2_array(nmir).astype(np.int32).tolist()
    mir_type = reader.read_i2_array(nmir).astype(np.int32).tolist()
    mir_ngtube = reader.read_i4_array(nmir).tolist()
    mirtime_ns = reader.read_i4_array(nmir).tolist()

    # --- 4) Tube arrays (ntube elements) ---
    ig = reader.read_i2_array(ntube).astype(np.int32).tolist()
    tube_eye = reader.read_i2_array(ntube).astype(np.int32).tolist()

    # --- 5) Version 2+ fields ---
    if bank_version >= 2:
        saturated = reader.read_i4_array(ntube).tolist()
        mir_tube_id = reader.read_i4_array(ntube).tolist()
    else:
        saturated = [0] * ntube
        mir_tube_id = [0] * ntube

    # --- 6) Build result dictionary ---
    data: dict[str, Any] = {
        "bank_version": bank_version,
        "jday": jday,
        "jsec": jsec,
        "msec": msec,
        "neye": neye,
        "nmir": nmir,
        "ntube": ntube,
        "maxeye": maxeye,
        "if_eye": if_eye.tolist(),
        "eyeid": eyeid,
        "eye_nmir": eye_nmir,
        "eye_ngmir": eye_ngmir,
        "eye_ntube": eye_ntube,
        "eye_ngtube": eye_ngtube,
        "rmsdevpln": rmsdevpln,
        "rmsdevtim": rmsdevtim,
        "tracklength": tracklength,
        "crossingtime": crossingtime,
        "ph_per_gtube": ph_per_gtube,
        "n_ampwt": n_ampwt,
        "errn_ampwt": errn_ampwt,
        "mirid": mirid,
        "mir_eye": mir_eye,
        "mir_type": mir_type,
        "mir_ngtube": mir_ngtube,
        "mirtime_ns": mirtime_ns,
        "ig": ig,
        "tube_eye": tube_eye,
        "saturated": saturated,
        "mir_tube_id": mir_tube_id,
    }

    return ConditionalBankResult(data=data, cursor=reader.cursor)
