"""
Replicates the exact output format of stps2_dst.c
"""

import awkward as ak


def dump_stps2(data, short=False):
    """
    Replicates the exact output format of stps2_dst.c

    Args:
        data: Awkward Record or dictionary containing the stps2 bank data.
        short (bool): If True, show only basic info (long_output=0).
                      If False, show full details (long_output=1).
    """
    data = ak.to_list(data) if hasattr(data, "to_list") else data

    maxeye = data["maxeye"]
    if_eye = data["if_eye"]

    print("\nSTPS2 bank. \n")

    for ieye in range(maxeye):
        if if_eye[ieye] != 1:
            continue

        print(f"eyeid {ieye + 1}  ________________________________")
        print(f"Event based Plog:\t\t{data['plog'][ieye]:7.2f}")
        print(f"Event Rayleigh Vector Mag:\t{data['rvec'][ieye]:7.2f}")
        print(f"Time spread of all tubes:\t{data['totalLifetime'][ieye]:7.2f} us")
        print(f"Time spread of in-time tubes:\t{data['lifetime'][ieye]:7.2f} us")

        if not short:
            print(f"Random Walk Vector Mag:\t\t{data['rwalk'][ieye]:7.2f}")
            print(f"Mean tube trigger time:\t\t{data['aveTime'][ieye]:7.2f} us")
            print(f"Spread of trigger times:\t{data['sigmaTime'][ieye]:7.2f} us")
            print(f"Mean calibrated photons:{data['avePhot'][ieye]:15.2f}")
            print(f"Spread of calibrated photons:{data['sigmaPhot'][ieye]:10.2f}")
            upward_str = "Yes" if data["upward"][ieye] else "No"
            print(f"Upward:\t\t\t\t{upward_str:>7}")
            print(f"Angle: \t\t\t\t{data['ang'][ieye]:7.2f} degrees")

    print()
