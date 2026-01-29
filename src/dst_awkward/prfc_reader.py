"""
Parser for PRFC (Profile Constraint) DST bank.

This module parses the PRFC bank (bank_id=30002) which contains profile
information from the group profile program. The bank uses bitmask-gated
sections with conditional data based on failmode checks.
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

# Backward compatibility alias
PRFCParseResult = ConditionalBankResult


def parse_prfc_bank(
    buffer: bytes, start_offset: int = 8, endian: str = "<"
) -> ConditionalBankResult:
    """
    Parse a PRFC bank (bank_id=30002) from `buffer`.

    This parser follows `prfc_bank_to_common_` in `prfc_dst.c`:
    masks (int16) → 3 gated loops over 16 fits → per-fit variable-length sections.

    Values are stored in a *dense* 16-fit representation:
    arrays/lists are length 16; missing fits are None.

    Args:
        buffer: Full bank bytes, including the 8-byte [bank_id, bank_version] header.
        start_offset: Byte offset where payload begins (default 8).
        endian: Endianness character for numpy dtypes ('<' little, '>' big).

    Returns:
        ConditionalBankResult(data=dict, cursor=int)
    """
    reader = BufferReader(buffer, start_offset, endian)

    MAXMEL = 10  # Maximum matrix elements

    # --- 1) Masks (consumed as int16 on disk) ---
    pflinfo_mask = reader.read_i2()
    bininfo_mask = reader.read_i2()
    mtxinfo_mask = reader.read_i2()

    pflinfo = decode_mask_msb_first(pflinfo_mask, bits=16)
    bininfo = decode_mask_msb_first(bininfo_mask, bits=16)
    mtxinfo = decode_mask_msb_first(mtxinfo_mask, bits=16)

    data: dict[str, Any] = {
        "pflinfo_mask": pflinfo_mask,
        "bininfo_mask": bininfo_mask,
        "mtxinfo_mask": mtxinfo_mask,
        "pflinfo": pflinfo,
        "bininfo": bininfo,
        "mtxinfo": mtxinfo,
    }

    # --- 2) Profile section (gated by pflinfo[i]) ---
    failmode = fit_list()

    # Profile parameters (only if failmode==SUCCESS)
    szmx = fit_list()
    dszmx = fit_list()
    rszmx = fit_list()
    lszmx = fit_list()
    tszmx = fit_list()
    xm = fit_list()
    dxm = fit_list()
    rxm = fit_list()
    lxm = fit_list()
    txm = fit_list()
    x0 = fit_list()
    dx0 = fit_list()
    rx0 = fit_list()
    lx0 = fit_list()
    tx0 = fit_list()
    lamb = fit_list()
    dlamb = fit_list()
    rlamb = fit_list()
    llamb = fit_list()
    tlamb = fit_list()
    eng = fit_list()
    deng = fit_list()
    reng = fit_list()
    leng = fit_list()
    teng = fit_list()

    traj_source = fit_list()
    errstat = fit_list()
    ndf = fit_list()
    chi2 = fit_list()

    SUCCESS = 0

    for i in range(MAXFIT):
        if not pflinfo[i]:
            continue

        fm = reader.read_i4()
        failmode[i] = fm
        if fm != SUCCESS:
            continue

        # Each group is value/stat/right/left/geom, all float64
        szmx[i] = reader.read_f8()
        dszmx[i] = reader.read_f8()
        rszmx[i] = reader.read_f8()
        lszmx[i] = reader.read_f8()
        tszmx[i] = reader.read_f8()

        xm[i] = reader.read_f8()
        dxm[i] = reader.read_f8()
        rxm[i] = reader.read_f8()
        lxm[i] = reader.read_f8()
        txm[i] = reader.read_f8()

        x0[i] = reader.read_f8()
        dx0[i] = reader.read_f8()
        rx0[i] = reader.read_f8()
        lx0[i] = reader.read_f8()
        tx0[i] = reader.read_f8()

        lamb[i] = reader.read_f8()
        dlamb[i] = reader.read_f8()
        rlamb[i] = reader.read_f8()
        llamb[i] = reader.read_f8()
        tlamb[i] = reader.read_f8()

        eng[i] = reader.read_f8()
        deng[i] = reader.read_f8()
        reng[i] = reader.read_f8()
        leng[i] = reader.read_f8()
        teng[i] = reader.read_f8()

        traj_source[i] = reader.read_i4()
        errstat[i] = reader.read_i4()
        ndf[i] = reader.read_i4()
        chi2[i] = reader.read_f8()

    data.update(
        {
            "failmode": failmode,
            "szmx": szmx,
            "dszmx": dszmx,
            "rszmx": rszmx,
            "lszmx": lszmx,
            "tszmx": tszmx,
            "xm": xm,
            "dxm": dxm,
            "rxm": rxm,
            "lxm": lxm,
            "txm": txm,
            "x0": x0,
            "dx0": dx0,
            "rx0": rx0,
            "lx0": lx0,
            "tx0": tx0,
            "lambda": lamb,
            "dlambda": dlamb,
            "rlambda": rlamb,
            "llambda": llamb,
            "tlambda": tlamb,
            "eng": eng,
            "deng": deng,
            "reng": reng,
            "leng": leng,
            "teng": teng,
            "traj_source": traj_source,
            "errstat": errstat,
            "ndf": ndf,
            "chi2": chi2,
        }
    )

    # --- 3) Bin section (gated by bininfo[i]) ---
    nbin = fit_zeros()
    dep = fit_empty_arrays(reader.f8)
    gm = fit_empty_arrays(reader.f8)
    scin = fit_empty_arrays(reader.f8)
    rayl = fit_empty_arrays(reader.f8)
    aero = fit_empty_arrays(reader.f8)
    crnk = fit_empty_arrays(reader.f8)
    sigmc = fit_empty_arrays(reader.f8)
    sig = fit_empty_arrays(reader.f8)
    ig = fit_empty_arrays(reader.i2)

    for i in range(MAXFIT):
        if not bininfo[i]:
            continue

        nb = reader.read_i2()
        nbin[i] = nb

        dep[i] = reader.read_f8_array(nb)
        gm[i] = reader.read_f8_array(nb)

        scin[i] = reader.read_f8_array(nb)
        rayl[i] = reader.read_f8_array(nb)
        aero[i] = reader.read_f8_array(nb)
        crnk[i] = reader.read_f8_array(nb)
        sigmc[i] = reader.read_f8_array(nb)
        sig[i] = reader.read_f8_array(nb)

        ig[i] = reader.read_i2_array(nb)

    data.update(
        {
            "nbin": nbin,
            "dep": dep,
            "gm": gm,
            "scin": scin,
            "rayl": rayl,
            "aero": aero,
            "crnk": crnk,
            "sigmc": sigmc,
            "sig": sig,
            "ig": ig,
        }
    )

    # --- 4) Matrix section (gated by mtxinfo[i]) ---
    nel = fit_zeros()
    mor = fit_zeros()
    mxel = fit_empty_arrays(reader.f8)

    for i in range(MAXFIT):
        if not mtxinfo[i]:
            continue

        ne = reader.read_i2()
        mo = reader.read_i2()
        nel[i] = ne
        mor[i] = mo

        ne_clamped = min(int(ne), MAXMEL)
        mxel[i] = reader.read_f8_array(ne_clamped)

        # If the bank actually contained more than MAXMEL elements, the C code clamps
        # and only unpacks MAXMEL. We mirror that behavior (no skipping of remaining
        # elements), which is consistent with the C implementation.

    data.update({"nel": nel, "mor": mor, "mxel": mxel})

    return ConditionalBankResult(data=data, cursor=reader.cursor)
