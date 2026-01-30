"""
Replicates the exact output format of mdweat_dst.c
"""

import awkward as ak


def dump_mdweat(data, short=False):
    """
    Replicates the exact output format of mdweat_dst.c

    Args:
        data: Awkward Record or dictionary containing the mdweat bank data.
        short (bool): If True, show compact output (long_output=0).
                      If False, show detailed explanation (long_output=1).
    """
    data = ak.to_list(data) if hasattr(data, "to_list") else data

    print("mdweat :")

    if short:
        print(f"part_num {data['part_num']:02d} code {data['code']:07d}")
    else:
        print(f"Part number: {data['part_num']:02d}")
        print(f"Part Weather code: {data['code']:07d}")
        print("n e s w o t h 7-digit weather code recorder by runners")
        print("n = 1,  0 Clouds North?")
        print("e = 1,  0 Clouds East?")
        print("s = 1,  0 Clouds South?")
        print("w = 1,  0 Clouds West?")
        print("o = 0 - 4 Overhead cloud thickness? 5 - weat code invalid")
        print("t = 1,  0 Stars visible?")
        print("h = 1,  0 Was it hazy? 2 - can't tell")
