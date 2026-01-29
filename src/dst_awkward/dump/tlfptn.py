import awkward as ak

TLFPTN_ORIGIN_X_CLF = 0.0
TLFPTN_ORIGIN_Y_CLF = 0.0


def dump_tlfptn(data, short=False):
    """
    Roughly matches `tlfptn_common_to_dumpf_` in `legacy/tlfptn_dst.c`.

    Args:
        data: Awkward Record or dictionary containing the tlfptn bank data.
        short: If True, suppress the per-hit table (matches C long_output=0).
    """
    data = ak.to_list(data) if hasattr(data, "to_list") else data
    nhits = int(data["nhits"])
    nsclust = int(data["nsclust"])
    nstclust = int(data["nstclust"])
    nborder = int(data["nborder"])

    # core estimate is from tyro_xymoments[2][0:2]
    core_x = float(data["tyro_xymoments"][2][0]) + TLFPTN_ORIGIN_X_CLF
    core_y = float(data["tyro_xymoments"][2][1]) + TLFPTN_ORIGIN_Y_CLF

    # t0 definition in the C dump:
    # 0.5*(tearliest[0]+tearliest[1]) + tyro_tfitpars[2][0] * 1e-6
    t0 = 0.5 * (float(data["tearliest"][0]) + float(data["tearliest"][1])) + float(
        data["tyro_tfitpars"][2][0]
    ) * 1.0e-6

    print("tlfptn :")
    print(
        f"nhits {nhits} nsclust {nsclust} nstclust {nstclust} nborder {nborder} "
        f"core_x {core_x:.6f} core_y {core_y:.6f} t0 {t0:.9f} "
    )

    if short:
        return

    # Per-hit table (C prints: Q upper/lower, T upper/lower, isgood)
    print("#    XXYY       Q upper        Q lower        T upper           T lower            isgood")

    # In schema, pulsa/reltime are shape (nhits, 2), indexed [i][0/1]
    for i in range(nhits):
        xxyy = int(data["xxyy"][i])
        # Match C's %7.4d formatting: 4-digit zero pad, width 7 right-justified.
        xxyy_fmt = f"{xxyy:04d}".rjust(7)
        q_upper = float(data["pulsa"][i][0])
        q_lower = float(data["pulsa"][i][1])

        t_upper = float(data["tearliest"][0]) + 1e-6 * float(data["reltime"][i][0])
        t_lower = float(data["tearliest"][1]) + 1e-6 * float(data["reltime"][i][1])

        isgood = int(data["isgood"][i])

        print(f"{i:02d}{xxyy_fmt}{q_upper:15f}{q_lower:15f}{t_upper:22.9f}{t_lower:18.9f}{isgood:7d}")


