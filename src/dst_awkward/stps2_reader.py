"""
Parser for STPS2 (Filter) DST bank.

This module parses the STPS2 bank (bank_id=15042) which contains filter
values computed during the stps2 filter. The bank has conditional packing
based on the if_eye flags for each telescope site.
"""

from __future__ import annotations

from typing import Any

from .conditional_bank_utils import BufferReader, ConditionalBankResult


def parse_stps2_bank(
    buffer: bytes, start_offset: int = 8, endian: str = "<"
) -> ConditionalBankResult:
    """
    Parse an STPS2 bank (bank_id=15042) from `buffer`.

    This parser follows `stps2_bank_to_common_` in `stps2_dst.c`:
    - maxeye (int32) tells how many eyes
    - if_eye[maxeye] (int32 array) flags which eyes are active
    - For each active eye: per-eye float32/int32/int8 values

    Args:
        buffer: Full bank bytes, including the 8-byte [bank_id, bank_version] header.
        start_offset: Byte offset where payload begins (default 8).
        endian: Endianness character for numpy dtypes ('<' little, '>' big).

    Returns:
        ConditionalBankResult(data=dict, cursor=int)
    """
    reader = BufferReader(buffer, start_offset, endian)

    # --- 1) Read maxeye and if_eye ---
    maxeye = reader.read_i4()
    if_eye = reader.read_i4_array(maxeye)

    # --- 2) Initialize per-eye storage ---
    plog = [None] * maxeye
    rvec = [None] * maxeye
    rwalk = [None] * maxeye
    ang = [None] * maxeye
    aveTime = [None] * maxeye
    sigmaTime = [None] * maxeye
    avePhot = [None] * maxeye
    sigmaPhot = [None] * maxeye
    lifetime = [None] * maxeye
    totalLifetime = [None] * maxeye
    inTimeTubes = [None] * maxeye
    upward = [None] * maxeye

    # --- 3) Parse each active eye ---
    for ieye in range(maxeye):
        if if_eye[ieye] != 1:
            continue

        plog[ieye] = reader.read_f4()
        rvec[ieye] = reader.read_f4()
        rwalk[ieye] = reader.read_f4()
        ang[ieye] = reader.read_f4()
        aveTime[ieye] = reader.read_f4()
        sigmaTime[ieye] = reader.read_f4()
        avePhot[ieye] = reader.read_f4()
        sigmaPhot[ieye] = reader.read_f4()
        lifetime[ieye] = reader.read_f4()
        totalLifetime[ieye] = reader.read_f4()
        inTimeTubes[ieye] = reader.read_i4()
        upward[ieye] = reader.read_i1()

    # --- 4) Build result dictionary ---
    data: dict[str, Any] = {
        "maxeye": maxeye,
        "if_eye": if_eye.tolist(),
        "plog": plog,
        "rvec": rvec,
        "rwalk": rwalk,
        "ang": ang,
        "aveTime": aveTime,
        "sigmaTime": sigmaTime,
        "avePhot": avePhot,
        "sigmaPhot": sigmaPhot,
        "lifetime": lifetime,
        "totalLifetime": totalLifetime,
        "inTimeTubes": inTimeTubes,
        "upward": upward,
    }

    return ConditionalBankResult(data=data, cursor=reader.cursor)
