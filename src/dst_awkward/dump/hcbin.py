"""
Replicates the exact output format of hcbin_dst.c (rusdgeom-style: flat, direct indexing).
"""

import awkward as ak

MAXFIT = 16
SUCCESS = 0

# Bin ig codes -> message (C trans1)
_IG = {
    -2: "cherenkov cut",
    -1: "sick plane fit",
    0: "too much corr",
    1: "",
}

# Failmode codes -> message (C trans2)
_FAILMODE = {
    1: "Fit not requested",
    2: "Fit not implemented",
    3: "Bank(s) required for fit are missing or have failed",
    4: "Bank(s) required for desired trajectory source are missing/failed",
    10: "Upward going track",
    11: "Too few good bins",
    12: "Fitter failed",
    13: "Trajectory (direction and/or core) unreasonable",
}


def _ig_message(code):
    return _IG.get(int(code), "unknown ig code")


def _failmode_message(code):
    return _FAILMODE.get(int(code), "Unknown failmode")


def dump_hcbin(data, short=False):
    """
    Replicates the exact output format of hcbin_dst.c

    Args:
        data: Awkward Record or dictionary containing the hcbin bank data.
        short (bool): Ignored; HCBIN C dump uses same output for short/long.
    """
    _ = short
    data = ak.to_list(data) if hasattr(data, "to_list") else data

    bininfo = data["bininfo"]
    nbin = data["nbin"]
    print("\nHCBIN bank. bins: ", end="")
    for i in range(MAXFIT):
        if bininfo[i]:
            print(f" {nbin[i]:03d}", end="")
        else:
            print(" -- ", end="")
    print("\n")

    jday = data["jday"]
    jsec = data["jsec"]
    msec = data["msec"]
    failmode = data["failmode"]
    bvx = data["bvx"]
    bvy = data["bvy"]
    bvz = data["bvz"]
    bsz = data["bsz"]
    sig = data["sig"]
    sigerr = data["sigerr"]
    cfc = data["cfc"]
    ig = data["ig"]

    for i in range(MAXFIT):
        if not bininfo[i]:
            continue
        ms = msec[i]
        h = ms // 3_600_000
        m = (ms // 60_000) % 60
        s = (ms // 1_000) % 60
        frac = ms % 1_000
        print(f"    -> Fit {i + 1} jDay/Sec: {jday[i]}/{jsec[i]:05d} {h:02d}:{m:02d}:{s:02d}.{frac:03d}")
        if failmode[i] != SUCCESS:
            print(f"    {_failmode_message(failmode[i])}")
            continue
        print("             bin direction     bin size  signal   signal   cfc")
        print("  bin      nx      ny      nz    (deg) pe/deg/m^2  error   m^2 ig")
        nb = nbin[i]
        for j in range(nb):
            msg = _ig_message(ig[i][j])
            print(
                " {:4d} {:8.5f} {:8.5f} {:8.5f} {:4.2f} {:8.1f} {:8.1f} {:6.2f} {:2d}  {}".format(
                    j, bvx[i][j], bvy[i][j], bvz[i][j], bsz[i][j],
                    sig[i][j], sigerr[i][j], cfc[i][j], ig[i][j], msg
                )
            )
        print()
