#!/usr/bin/env python3
"""
Ad-hoc PRFC reader smoke test.

This repo's `tests/` directory already contains runnable scripts (not pytest-style).
This script extracts PRFC (bank_id=30002) from a DST stream and runs the standalone
PRFC parser in `dst_awkward.prfc_reader`.
"""

from __future__ import annotations

import argparse

from dst_awkward.dst_io import DSTFile
from dst_awkward.prfc_reader import parse_prfc_bank


PRFC_BANKID = 30002


def _present_fit_indices(data: dict) -> list[int]:
    present = set()
    for key in ("pflinfo", "bininfo", "mtxinfo"):
        present.update(i for i, x in enumerate(data[key]) if x)
    return sorted(present)


def summarize_prfc(data: dict, fit_indices: list[int] | None) -> None:
    print("PRFC:")
    print(f"  masks: pfl={data['pflinfo_mask']} bin={data['bininfo_mask']} mtx={data['mtxinfo_mask']}")
    print(f"  pflinfo fits: {[i for i, x in enumerate(data['pflinfo']) if x]}")
    print(f"  bininfo fits: {[i for i, x in enumerate(data['bininfo']) if x]}")
    print(f"  mtxinfo fits: {[i for i, x in enumerate(data['mtxinfo']) if x]}")

    if fit_indices is None or len(fit_indices) == 0:
        fit_indices = _present_fit_indices(data)
        if fit_indices:
            print(f"  note: --fit not specified; printing all present fits: {fit_indices}")
        else:
            print("  note: no fits present in any mask; nothing to print.")
            return

    for i in fit_indices:
        if i < 0 or i >= 16:
            print(f"  fit[{i}] (skipped: out of range)")
            continue

        print(f"\n  fit[{i}]:")
        print(f"    pflinfo/bininfo/mtxinfo: {bool(data['pflinfo'][i])}/{bool(data['bininfo'][i])}/{bool(data['mtxinfo'][i])}")
        print(f"    failmode: {data['failmode'][i]}")
        print(f"    traj_source: {data['traj_source'][i]}")
        print(f"    ndf: {data['ndf'][i]}")
        print(f"    chi2: {data['chi2'][i]}")

        print(f"    nbin: {data['nbin'][i]}")
        if data["dep"][i] is not None:
            print(f"    dep len: {len(data['dep'][i])}")

        print(f"    nel: {data['nel'][i]}")
        if data["mxel"][i] is not None:
            print(f"    mxel len: {len(data['mxel'][i])}")


def main() -> None:
    p = argparse.ArgumentParser(description="Smoke-test PRFC parsing on a DST file.")
    p.add_argument("dst_file", help="Path to .dst, .dst.gz, or .dst.bz2")
    p.add_argument(
        "--fit",
        type=int,
        action="append",
        default=None,
        help=(
            "Fit index to print (0-based). Can be provided multiple times. "
            "If omitted, prints all fits present in any of pflinfo/bininfo/mtxinfo."
        ),
    )
    p.add_argument(
        "--all",
        action="store_true",
        help="Parse/print all PRFC banks in the file (default: first only).",
    )
    args = p.parse_args()

    found = 0
    with DSTFile(args.dst_file) as dst:
        for bank_id, ver, raw_bytes in dst.banks():
            if bank_id != PRFC_BANKID:
                continue

            found += 1
            print(f"\n--- PRFC bank #{found} (version {ver}) ---")
            res = parse_prfc_bank(raw_bytes)
            summarize_prfc(res.data, fit_indices=args.fit)
            print(f"  parsed_bytes: {res.cursor} / {len(raw_bytes)}")

            if not args.all:
                break

    if found == 0:
        raise SystemExit(f"No PRFC banks (bank_id={PRFC_BANKID}) found in {args.dst_file!r}")


if __name__ == "__main__":
    main()


