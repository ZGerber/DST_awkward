#!/usr/bin/env python3
"""
Build an Awkward Array from PRFC banks and dump a human-readable summary.

This does NOT integrate PRFC into the generic BankReader yet; it uses the standalone
PRFC parser (`dst_awkward.prfc_reader.parse_prfc_bank`) and then wraps its output
into an event dict suitable for `ak.Array`.
"""

from __future__ import annotations

import argparse
import pprint
from typing import Any

import awkward as ak

from dst_awkward.dst_io import DSTFile
from dst_awkward.prfc_reader import parse_prfc_bank


PRFC_BANKID = 30002


def _present_fit_indices(prfc: dict[str, Any]) -> list[int]:
    present = set()
    for key in ("pflinfo", "bininfo", "mtxinfo"):
        present.update(i for i, x in enumerate(prfc[key]) if x)
    return sorted(present)


def dump_event(event: dict[str, Any], max_fits: int = 16) -> None:
    prfc = event.get("prfc")
    if prfc is None:
        print("event has no prfc")
        return

    fits = _present_fit_indices(prfc)
    print("PRFC summary:")
    print(f"  version: {prfc.get('_version')}")
    print(f"  masks: pfl={prfc['pflinfo_mask']} bin={prfc['bininfo_mask']} mtx={prfc['mtxinfo_mask']}")
    print(f"  present fits: {fits}")

    for i in fits[:max_fits]:
        print(f"  fit[{i}]: failmode={prfc['failmode'][i]} nbin={prfc['nbin'][i]} nel={prfc['nel'][i]} chi2={prfc['chi2'][i]}")


def main() -> None:
    p = argparse.ArgumentParser(description="Parse PRFC from DST and dump as an Awkward Array.")
    p.add_argument("dst_file", help="Path to .dst, .dst.gz, or .dst.bz2")
    p.add_argument("--limit", type=int, default=None, help="Max events to build/dump")
    p.add_argument("--dump-events", type=int, default=1, help="How many events to dump to stdout")
    p.add_argument(
        "--full",
        action="store_true",
        help="Print the full event record (all fields, all array contents).",
    )
    args = p.parse_args()

    event_list: list[dict[str, Any]] = []
    current_event: dict[str, Any] = {}
    event_count = 0

    with DSTFile(args.dst_file) as dst:
        for bank_id, ver, raw_bytes in dst.banks():
            if bank_id != PRFC_BANKID:
                continue

            # Event boundary heuristic (same as other scripts in this repo):
            # if we see the same bank name again, start a new event.
            if "prfc" in current_event:
                event_list.append(current_event)
                current_event = {}
                event_count += 1
                if args.limit is not None and event_count >= args.limit:
                    break

            res = parse_prfc_bank(raw_bytes)
            prfc = res.data
            prfc["_version"] = ver
            prfc["_parsed_bytes"] = res.cursor
            prfc["_raw_bytes"] = len(raw_bytes)
            current_event["prfc"] = prfc

        if current_event and (args.limit is None or event_count < args.limit):
            event_list.append(current_event)

    if not event_list:
        raise SystemExit("No PRFC banks found; no events built.")

    events_ak = ak.Array(event_list)
    print(f"events: {len(events_ak)}")
    print(f"type: {events_ak.type}")

    for i in range(min(args.dump_events, len(events_ak))):
        print(f"\n--- EVENT {i} ---")
        if args.full:
            pprint.pprint(ak.to_list(events_ak[i]), width=120, sort_dicts=False)
        else:
            dump_event(ak.to_list(events_ak[i]), max_fits=16)


if __name__ == "__main__":
    main()


