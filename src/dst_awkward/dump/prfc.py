"""
Replicates the exact output format of prfc_dst.c (rusdgeom-style: flat, direct indexing).
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

# errstat flags
_STAT_ERROR = 1
_RIGHT_ERROR = 2
_LEFT_ERROR = 4
_GEOM_ERROR = 8
_GEOM_INCOMPLETE = 16


def _ig_message(code):
    return _IG.get(int(code), "unknown ig code")


def _failmode_message(code):
    return _FAILMODE.get(int(code), "Unknown failmode")


def dump_prfc(data, short=False):
    """
    Replicates the exact output format of prfc_dst.c

    Args:
        data: Awkward Record or dictionary containing the prfc bank data.
        short (bool): If True, omit Profile Bins and Error matrix tables (matches C long_output=0).
    """
    data = ak.to_list(data) if hasattr(data, "to_list") else data

    bininfo = data["bininfo"]
    nbin = data["nbin"]
    print("\nPRFC bank. bins: ", end="")
    for i in range(MAXFIT):
        if bininfo[i]:
            print(f" {nbin[i]:03d}", end="")
        else:
            print(" -- ", end="")
    print("\n")

    pflinfo = data["pflinfo"]
    failmode = data["failmode"]
    lamb = data.get("lambda")
    if lamb is None:
        lamb = data.get("lambda_")
    if lamb is None:
        lamb = [0.0] * MAXFIT

    mark_header = False
    for i in range(MAXFIT):
        if not pflinfo[i]:
            continue
        print(f"    -> Profile Fit {i + 1}")
        if failmode[i] != SUCCESS:
            print(f"    {_failmode_message(failmode[i])}")
            continue
        if not mark_header:
            print("            value   stat error        right       left     geom error")
            mark_header = True
        print(
            "  Szmx: {:9.3e} +- {:9.3e}   ({:9.3e}, {:9.3e})  +- {:9.3e}  particles".format(
                data["szmx"][i], data["dszmx"][i], data["rszmx"][i], data["lszmx"][i], data["tszmx"][i]
            )
        )
        print(
            "  Xmax: {:9.2f} +- {:9.2f}   ({:9.2f}, {:9.2f})  +- {:9.2f}  g/cm^2".format(
                data["xm"][i], data["dxm"][i], data["rxm"][i], data["lxm"][i], data["txm"][i]
            )
        )
        print(
            "    X0: {:9.2f} +- {:9.2f}   ({:9.2f}, {:9.2f})  +- {:9.2f}  g/cm^2".format(
                data["x0"][i], data["dx0"][i], data["rx0"][i], data["lx0"][i], data["tx0"][i]
            )
        )
        print(
            "  Lamb: {:9.2f} +- {:9.2f}   ({:9.2f}, {:9.2f})  +- {:9.2f}  g/cm^2".format(
                lamb[i], data["dlambda"][i], data["rlambda"][i], data["llambda"][i], data["tlambda"][i]
            )
        )
        print(
            "  Engy: {:9.3f} +- {:9.3f}   ({:9.3f}, {:9.3f})  +- {:9.3f}  EeV".format(
                data["eng"][i], data["deng"][i], data["reng"][i], data["leng"][i], data["teng"][i]
            )
        )
        print()
        print(f" chi2/ndf: {data['chi2'][i]:7.3f} / {data['ndf'][i]:3d}")
        traj = data["traj_source"][i]
        errstat_val = data["errstat"][i]
        print(f" trajectory source: {traj:5d} (Unknown Bank)    errstat: {errstat_val}")
        if errstat_val != SUCCESS:
            if errstat_val & _STAT_ERROR:
                print("   STATISTICAL errors failed")
            if errstat_val & _RIGHT_ERROR:
                print("   RIGHT TRAJECTORY errors failed")
            if errstat_val & _LEFT_ERROR:
                print("   LEFT TRAJECTORY errors failed")
            if errstat_val & _GEOM_ERROR:
                print("   GEOMETRICAL errors failed")
            if errstat_val & _GEOM_INCOMPLETE:
                print("   GEOMETRICAL errors incomplete")
        print()

    if not short:
        dep = data.get("dep")
        scin = data.get("scin")
        rayl = data.get("rayl")
        aero = data.get("aero")
        crnk = data.get("crnk")
        sigmc = data.get("sigmc")
        sig = data.get("sig")
        ig = data.get("ig")
        if dep is not None and ig is not None:
            for i in range(MAXFIT):
                if not bininfo[i]:
                    continue
                nb = nbin[i]
                print(f"    -> Profile Bins {i + 1}")
                print("    slant     scin     rayl    aero     crnk   mc_tot  signal  ig")
                for j in range(nb):
                    msg = _ig_message(ig[i][j])
                    print(
                        "  {:8.2f}  {:7.3f} {:7.3f} {:7.3f} {:9.4f} {:7.2f} {:7.2f}  {}".format(
                            dep[i][j], scin[i][j], rayl[i][j], aero[i][j], crnk[i][j],
                            sigmc[i][j], sig[i][j], msg
                        )
                    )
                print()

        mtxinfo = data.get("mtxinfo")
        nel = data.get("nel")
        mor = data.get("mor")
        mxel = data.get("mxel")
        if mtxinfo is not None and mor is not None and mxel is not None and nel is not None:
            for i in range(MAXFIT):
                if not mtxinfo[i]:
                    continue
                mo = mor[i]
                ne = nel[i]
                mx = mxel[i]
                if mo <= 0:
                    continue
                print(f"    -> Error matrix {i + 1}")
                for j in range(mo):
                    if j == 0:
                        print("  / ", end="")
                    elif j == mo - 1:
                        print("  \\ ", end="")
                    else:
                        print("  | ", end="")
                    for k in range(mo):
                        if k >= j:
                            idx = ne - ((mo - j) * (mo - j + 1)) // 2 + (k - j)
                        else:
                            idx = ne - ((mo - k) * (mo - k + 1)) // 2 + (j - k)
                        print(f"{mx[idx]:13.6g}", end="")
                    if j == 0:
                        print("  \\")
                    elif j == mo - 1:
                        print("  /")
                    else:
                        print("  |")
                print()

    print()
