"""
Replicates the exact output format of talex00_dst.c
"""

import awkward as ak

# Tower names (from talex00_dst.h)
_TOWER_NAMES = ["BR", "LR", "SK", "BF", "DM", "KM", "SC", "SN", "SR", "MD"]


def _list_towers(tower_bit_flag):
    """Convert tower bitflag to space-separated string of tower names."""
    towers = []
    for i, name in enumerate(_TOWER_NAMES):
        if tower_bit_flag & (1 << i):
            towers.append(name)
    return "".join(towers) if towers else "??"


def dump_talex00(data, short=False):
    """
    Replicates the exact output format of talex00_dst.c

    Args:
        data: Awkward Record or dictionary containing the talex00 bank data.
        short (bool): If True, show waveform table only (long_output=0).
                      If False, show detailed per-waveform info with FADC traces (long_output=1).
    """
    data = ak.to_list(data) if hasattr(data, "to_list") else data

    print("talex00 :")

    yr = data["yymmdd"] // 10000
    mo = (data["yymmdd"] // 100) % 100
    day = data["yymmdd"] % 100
    hr = data["hhmmss"] // 10000
    min_ = (data["hhmmss"] // 100) % 100
    sec = data["hhmmss"] % 100
    usec = data["usec"]

    site_str = _list_towers(data["site"])
    print(
        f"event_num {data['event_num']} event_code {data['event_code']} site {site_str} "
        f"errcode {data['errcode']} date {mo:02d}/{day:02d}/{yr:02d} {hr:02d}:{min_:02d}:{sec:02d}.{usec:06d} "
        f"nofwf {data['nofwf']} monyymmdd {data['monyymmdd']:06d} monhhmmss {data['monhhmmss']:06d}"
    )

    nofwf = data["nofwf"]

    if short:
        # short form (long_output=0): waveform table
        print(
            "wf# wf_id  X   Y    clkcnt     mclkcnt   fadcti(lower,upper)  fadcav      pchmip        pchped      nfadcpermip     mftchi2      mftndof"
        )
        for i in range(nofwf):
            xy0 = data["xxyy"][i] // 100
            xy1 = data["xxyy"][i] % 100
            wf_id_str = f"{data['wf_id'][i]:02d}"
            print(
                f"{i:02d} {wf_id_str:>5} {xy0:4d} {xy1:3d} {data['clkcnt'][i]:10d} {data['mclkcnt'][i]:10d} "
                f"{data['fadcti'][i][0]:8d} {data['fadcti'][i][1]:8d} "
                f"{data['fadcav'][i][0]:5d} {data['fadcav'][i][1]:4d} "
                f"{data['pchmip'][i][0]:6d} {data['pchmip'][i][1]:7d} "
                f"{data['pchped'][i][0]:5d} {data['pchped'][i][1]:5d} "
                f"{data['mip'][i][0]:8.1f} {data['mip'][i][1]:6.1f} "
                f"{data['mftchi2'][i][0]:6.1f} {data['mftchi2'][i][1]:6.1f} "
                f"{data['mftndof'][i][0]:5d} {data['mftndof'][i][1]:4d}"
            )
    else:
        # long form (long_output=1): per-waveform with FADC traces
        for i in range(nofwf):
            print(
                "wf# wf_id  X   Y    clkcnt     mclkcnt   fadcti(lower,upper)  fadcav      pchmip        pchped      nfadcpermip     mftchi2      mftndof lat_lon_alt xyz_coor_clf"
            )
            xy0 = data["xxyy"][i] // 100
            xy1 = data["xxyy"][i] % 100
            wf_id_str = f"{data['wf_id'][i]:02d}"
            print(
                f"{i:02d} {wf_id_str:>5} {xy0:4d} {xy1:3d} {data['clkcnt'][i]:10d} {data['mclkcnt'][i]:10d} "
                f"{data['fadcti'][i][0]:8d} {data['fadcti'][i][1]:8d} "
                f"{data['fadcav'][i][0]:5d} {data['fadcav'][i][1]:4d} "
                f"{data['pchmip'][i][0]:6d} {data['pchmip'][i][1]:7d} "
                f"{data['pchped'][i][0]:5d} {data['pchped'][i][1]:5d} "
                f"{data['mip'][i][0]:8.1f} {data['mip'][i][1]:6.1f} "
                f"{data['mftchi2'][i][0]:6.1f} {data['mftchi2'][i][1]:6.1f} "
                f"{data['mftndof'][i][0]:5d} {data['mftndof'][i][1]:4d} "
                f"{data['lat_lon_alt'][i][0]:.2f} {data['lat_lon_alt'][i][1]:.2f} {data['lat_lon_alt'][i][2]:.1f} "
                f"{data['xyz_cor_clf'][i][0]:.1f} {data['xyz_cor_clf'][i][1]:.1f} {data['xyz_cor_clf'][i][2]:.1f} "
            )
            print("lower fadc")
            k = 0
            for j in range(128):
                if k == 12:
                    print()
                    k = 0
                print(f"{data['fadc'][i][0][j]:6d} ", end="")
                k += 1
            print()
            print("upper fadc")
            k = 0
            for j in range(128):
                if k == 12:
                    print()
                    k = 0
                print(f"{data['fadc'][i][1][j]:6d} ", end="")
                k += 1
            print()
