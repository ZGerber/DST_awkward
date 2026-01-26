from __future__ import annotations

from dataclasses import dataclass
from typing import Any

import numpy as np


@dataclass(frozen=True)
class PRFCParseResult:
    """
    Result of parsing a single PRFC bank payload.

    Notes:
    - This parser follows `prfc_bank_to_common_` in `prfc_dst.c`:
      masks (int16) → 3 gated loops over 16 fits → per-fit variable-length sections.
    - Values are stored in a *dense* 16-fit representation:
      arrays/lists are length 16; missing fits are None.
    """

    data: dict[str, Any]
    cursor: int


def _decode_mask_msb_first(mask_i16: int, bits: int = 16) -> list[bool]:
    """
    Match PRFC's C logic:
      if (mask & 0x8000) used; mask <<= 1; repeated PRFC_MAXFIT times
    """
    m = int(mask_i16) & ((1 << bits) - 1)
    return [((m >> (bits - 1 - i)) & 1) == 1 for i in range(bits)]


def parse_prfc_bank(buffer: bytes, start_offset: int = 8, endian: str = "<") -> PRFCParseResult:
    """
    Parse a PRFC bank (bank_id=30002) from `buffer`.

    Args:
      buffer: Full bank bytes, including the 8-byte [bank_id, bank_version] header.
      start_offset: Byte offset where payload begins (default 8).
      endian: Endianness character for numpy dtypes ('<' little, '>' big). PRFC is '<'.

    Returns:
      PRFCParseResult(data=dict, cursor=int)
    """
    cursor = int(start_offset)

    i2 = np.dtype(f"{endian}i2")
    i4 = np.dtype(f"{endian}i4")
    f8 = np.dtype(f"{endian}f8")

    def read_scalar(dtype: np.dtype) -> int | float:
        nonlocal cursor
        v = np.frombuffer(buffer, dtype=dtype, count=1, offset=cursor)[0]
        cursor += int(dtype.itemsize)
        return v.item()

    def read_array(dtype: np.dtype, n: int) -> np.ndarray:
        nonlocal cursor
        n = int(n)
        a = np.frombuffer(buffer, dtype=dtype, count=n, offset=cursor)
        cursor += int(n * dtype.itemsize)
        return a

    # --- 1) Masks (consumed as int16 on disk) ---
    pflinfo_mask = read_scalar(i2)
    bininfo_mask = read_scalar(i2)
    mtxinfo_mask = read_scalar(i2)

    pflinfo = _decode_mask_msb_first(pflinfo_mask, bits=16)
    bininfo = _decode_mask_msb_first(bininfo_mask, bits=16)
    mtxinfo = _decode_mask_msb_first(mtxinfo_mask, bits=16)

    # Dense 16-fit storage
    MAXFIT = 16
    MAXMEL = 10

    def fit_list() -> list[Any]:
        return [None] * MAXFIT

    def fit_empty_arrays(dtype: np.dtype) -> list[np.ndarray]:
        """Dense per-fit jagged payloads: default to empty arrays when absent."""
        return [np.empty(0, dtype=dtype) for _ in range(MAXFIT)]

    def fit_zeros() -> list[int]:
        """Dense per-fit counters: default to 0 when absent."""
        return [0] * MAXFIT

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
    szmx = fit_list(); dszmx = fit_list(); rszmx = fit_list(); lszmx = fit_list(); tszmx = fit_list()
    xm = fit_list(); dxm = fit_list(); rxm = fit_list(); lxm = fit_list(); txm = fit_list()
    x0 = fit_list(); dx0 = fit_list(); rx0 = fit_list(); lx0 = fit_list(); tx0 = fit_list()
    lamb = fit_list(); dlamb = fit_list(); rlamb = fit_list(); llamb = fit_list(); tlamb = fit_list()
    eng = fit_list(); deng = fit_list(); reng = fit_list(); leng = fit_list(); teng = fit_list()

    traj_source = fit_list()
    errstat = fit_list()
    ndf = fit_list()
    chi2 = fit_list()

    SUCCESS = 0

    for i in range(MAXFIT):
        if not pflinfo[i]:
            continue

        fm = read_scalar(i4)
        failmode[i] = fm
        if fm != SUCCESS:
            continue

        # Each group is value/stat/right/left/geom, all float64
        szmx[i], dszmx[i], rszmx[i], lszmx[i], tszmx[i] = (read_scalar(f8), read_scalar(f8), read_scalar(f8), read_scalar(f8), read_scalar(f8))
        xm[i], dxm[i], rxm[i], lxm[i], txm[i] = (read_scalar(f8), read_scalar(f8), read_scalar(f8), read_scalar(f8), read_scalar(f8))
        x0[i], dx0[i], rx0[i], lx0[i], tx0[i] = (read_scalar(f8), read_scalar(f8), read_scalar(f8), read_scalar(f8), read_scalar(f8))
        lamb[i], dlamb[i], rlamb[i], llamb[i], tlamb[i] = (read_scalar(f8), read_scalar(f8), read_scalar(f8), read_scalar(f8), read_scalar(f8))
        eng[i], deng[i], reng[i], leng[i], teng[i] = (read_scalar(f8), read_scalar(f8), read_scalar(f8), read_scalar(f8), read_scalar(f8))

        traj_source[i] = read_scalar(i4)
        errstat[i] = read_scalar(i4)
        ndf[i] = read_scalar(i4)
        chi2[i] = read_scalar(f8)

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
    dep = fit_empty_arrays(f8)
    gm = fit_empty_arrays(f8)
    scin = fit_empty_arrays(f8)
    rayl = fit_empty_arrays(f8)
    aero = fit_empty_arrays(f8)
    crnk = fit_empty_arrays(f8)
    sigmc = fit_empty_arrays(f8)
    sig = fit_empty_arrays(f8)
    ig = fit_empty_arrays(i2)

    for i in range(MAXFIT):
        if not bininfo[i]:
            continue

        nb = read_scalar(i2)
        nbin[i] = nb

        dep[i] = read_array(f8, nb)
        gm[i] = read_array(f8, nb)

        scin[i] = read_array(f8, nb)
        rayl[i] = read_array(f8, nb)
        aero[i] = read_array(f8, nb)
        crnk[i] = read_array(f8, nb)
        sigmc[i] = read_array(f8, nb)
        sig[i] = read_array(f8, nb)

        ig[i] = read_array(i2, nb)

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
    mxel = fit_empty_arrays(f8)

    for i in range(MAXFIT):
        if not mtxinfo[i]:
            continue

        ne = read_scalar(i2)
        mo = read_scalar(i2)
        nel[i] = ne
        mor[i] = mo

        ne_clamped = min(int(ne), MAXMEL)
        mxel[i] = read_array(f8, ne_clamped)

        # If the bank actually contained more than MAXMEL elements, the C code clamps
        # and only unpacks MAXMEL. We mirror that behavior (no skipping of remaining
        # elements), which is consistent with the C implementation.

    data.update({"nel": nel, "mor": mor, "mxel": mxel})

    return PRFCParseResult(data=data, cursor=cursor)


