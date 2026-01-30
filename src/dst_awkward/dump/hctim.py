"""
Replicates the exact output format of hctim_dst.c
"""

import awkward as ak

MAXFIT = 16
SUCCESS = 0
DEGRAD = 57.2958

# Failmode codes -> message
_FAILMODE = {
    1: "Fit not requested",
    2: "Fit not implemented",
    3: "Bank(s) required for fit are missing or have failed",
    4: "Bank(s) required for desired trajectory source are missing/failed",
    10: "Upward going track",
    11: "Too few good tubes",
    12: "Fitter failed",
    13: "Trajectory (direction and/or core) unreasonable",
}


def _failmode_message(code):
    return _FAILMODE.get(int(code), "Unknown failmode")


def dump_hctim(data, short=False):
    """
    Replicates the exact output format of hctim_dst.c

    Args:
        data: Awkward Record or dictionary containing the hctim bank data.
        short (bool): If True, omit tube details (long_output=0).
                      If False, include tube details (long_output=1).
    """
    data = ak.to_list(data) if hasattr(data, "to_list") else data

    timinfo = data["timinfo"]
    ntube = data["ntube"]

    # Header with tube counts
    print("\nHCTIM bank. tubes: ", end="")
    for i in range(MAXFIT):
        if timinfo[i]:
            print(f" {ntube[i]:03d}", end="")
        else:
            print(" -- ", end="")
    print()

    # Per-fit geometry info
    for i in range(MAXFIT):
        if not timinfo[i]:
            continue

        jday_i = data["jday"][i]
        jsec_i = data["jsec"][i]
        msec_i = data["msec"][i]

        hr = msec_i // 3600000
        min_ = (msec_i // 60000) % 60
        sec = (msec_i // 1000) % 60
        ms = msec_i % 1000

        print(f"\n    -> Fit {i + 1:2d} jDay/Sec: {jday_i}/{jsec_i:05d} {hr:02d}:{min_:02d}:{sec:02d}.{ms:03d}")

        failmode_i = data["failmode"][i]
        if failmode_i != SUCCESS:
            print(f"    {_failmode_message(failmode_i)}")
            continue

        # Chi2 and geometry parameters
        print(
            f"chi2 :{data['mchi2'][i]:10.2f}   range= [{data['rchi2'][i]:10.2f}, {data['lchi2'][i]:10.2f}]"
        )
        print(
            f"rp   :{data['mrp'][i]:10.2f}   range= [{data['rrp'][i]:10.2f}, {data['lrp'][i]:10.2f}]  meters"
        )
        print(
            f"psi  :{data['mpsi'][i] * DEGRAD:10.2f}   range= [{data['rpsi'][i] * DEGRAD:10.2f}, {data['lpsi'][i] * DEGRAD:10.2f}]  degrees"
        )
        print(
            f"theta:{data['mthe'][i] * DEGRAD:10.2f}   range= [{data['rthe'][i] * DEGRAD:10.2f}, {data['lthe'][i] * DEGRAD:10.2f}]  degrees"
        )
        print(
            f"phi  :{data['mphi'][i] * DEGRAD:10.2f}   range= [{data['rphi'][i] * DEGRAD:10.2f}, {data['lphi'][i] * DEGRAD:10.2f}]  degrees"
        )
        print()

        # Core location
        mcore = data["mcore"][i]
        rcore = data["rcore"][i]
        lcore = data["lcore"][i]
        print(
            f"core location x: {mcore[0]:10.2f},  range = [ {rcore[0]:10.2f}, {lcore[0]:10.2f}]  meters"
        )
        print(
            f"core location y: {mcore[1]:10.2f},  range = [ {rcore[1]:10.2f}, {lcore[1]:10.2f}]  meters"
        )
        print()

        # Shower direction vectors
        mtkv = data["mtkv"][i]
        rtkv = data["rtkv"][i]
        ltkv = data["ltkv"][i]
        print(f"shower direction: ( {mtkv[0]:7.4f}, {mtkv[1]:7.4f}, {mtkv[2]:7.4f} )")
        print(f"      dir bound1: ( {rtkv[0]:7.4f}, {rtkv[1]:7.4f}, {rtkv[2]:7.4f} )")
        print(f"      dir bound2: ( {ltkv[0]:7.4f}, {ltkv[1]:7.4f}, {ltkv[2]:7.4f} )")

    # Long output: tube details
    if not short:
        for i in range(MAXFIT):
            if not timinfo[i]:
                continue
            if data["failmode"][i] != SUCCESS:
                continue

            print(f"\n    -> Fit {i + 1:2d} Tubes")
            print(" mir tube     time (ns)  timefit    sgmt view_ang     asx     asy     asz   ig")

            ntube_i = ntube[i]
            for j in range(ntube_i):
                print(
                    f"  {data['tubemir'][i][j]:2d}  {data['tube'][i][j]:03d} "
                    f"{data['time'][i][j]:10.1f} {data['timefit'][i][j]:10.1f} "
                    f"{data['sgmt'][i][j]:8.1f}  {data['thetb'][i][j] * DEGRAD:7.2f} "
                    f"{data['asx'][i][j]:7.4f} {data['asy'][i][j]:7.4f} {data['asz'][i][j]:7.4f} "
                    f"{data['ig'][i][j]:3d}"
                )
