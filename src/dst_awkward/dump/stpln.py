"""
Replicates the exact output format of stpln_dst.c
"""

import math

import awkward as ak


def dump_stpln(data, short=False):
    """
    Replicates the exact output format of stpln_dst.c

    Args:
        data: Awkward Record or dictionary containing the stpln bank data.
        short (bool): If True, omit tube details (long_output=0).
                      If False, include tube details (long_output=1).
    """
    data = ak.to_list(data) if hasattr(data, "to_list") else data

    jday = data["jday"]
    jsec = data["jsec"]
    msec = data["msec"]

    hr = msec // 3600000
    min_ = (msec // 60000) % 60
    sec = (msec // 1000) % 60
    ms = msec % 1000

    print(
        f"\nSTPLN jDay/Sec: {jday}/{jsec:05d} {hr:02d}:{min_:02d}:{sec:02d}.{ms:03d} ",
        end="",
    )
    print(f"eyes: {data['neye']:2d}  mirs: {data['nmir']:2d}  tubes: {data['ntube']:4d} ")

    maxeye = data["maxeye"]
    if_eye = data["if_eye"]

    for ieye in range(maxeye):
        if if_eye[ieye] != 1:
            continue

        print(f"eyeid {ieye + 1}  ________________________________")
        print("      n_ampwt   errn_ampwt")

        n_ampwt = data["n_ampwt"][ieye]
        errn_ampwt = data["errn_ampwt"][ieye]

        print(f"  {n_ampwt[0]:11.8f} {math.sqrt(errn_ampwt[0]):11.8f}")
        print(f"  {n_ampwt[1]:11.8f} {math.sqrt(errn_ampwt[3]):11.8f}")
        print(f"  {n_ampwt[2]:11.8f} {math.sqrt(errn_ampwt[5]):11.8f}")
        print()

        print("  track info:")
        print(f"  tracklength   : {data['tracklength'][ieye]:11.8f}")
        print(
            f"  crossing time : {data['crossingtime'][ieye]:11.8f}\n"
            f"  ph_per_gtube  : {data['ph_per_gtube'][ieye]:11.8f}"
        )
        print(
            f"  rmsdevpln : {data['rmsdevpln'][ieye]:11.8f}\n"
            f"  rmsdevtim : {data['rmsdevtim'][ieye]:11.8f}"
        )

    print("________________________________________")

    # Mirror info
    nmir = data["nmir"]
    for i in range(nmir):
        print(
            f" eye {data['mir_eye'][i]:2d} mir {data['mirid'][i]:2d} "
            f"Rev {data['mir_type'][i]}  gtubes: {data['mir_ngtube'][i]:3d}  ",
            end="",
        )
        print(f"time: {data['mirtime_ns'][i]:10d}nS")

    # Long output: tube info
    if not short:
        ntube = data["ntube"]
        for i in range(ntube):
            print(
                f"itube {i:3d}  eyeid {data['tube_eye'][i]:2d}  "
                f"mir_tube_id {data['mir_tube_id'][i]:5d} "
                f"saturated {data['saturated'][i]:2d} "
                f"ig {data['ig'][i]}"
            )

    print()
