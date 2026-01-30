#!/usr/bin/env python3
"""
Validate dst-dump output against dstdump.run for a given bank.

Tests both short (-bank) and long (+bank) output forms.

Usage:
    python validate_dump.py <bank> <dst_file> <parquet_file>

Example:
    python validate_dump.py talex00 test.dst.gz test.parquet
"""

import argparse
import difflib
import subprocess
import sys


def run_dstdump(bank: str, dst_file: str, long_output: bool) -> list[str]:
    """Run C dstdump.run and return output lines (excluding header)."""
    prefix = "+" if long_output else "-"
    result = subprocess.run(
        ["dstdump.run", f"{prefix}{bank}", dst_file],
        capture_output=True,
        text=True,
    )
    # Strip "Reading DST file:" header
    return [l for l in result.stdout.splitlines() if not l.startswith("Reading ")]


def run_dst_dump(bank: str, parquet_file: str, long_output: bool) -> list[str]:
    """Run Python dst-dump and return output lines (excluding header)."""
    prefix = "+" if long_output else "-"
    result = subprocess.run(
        ["dst-dump", f"{prefix}{bank}", parquet_file],
        capture_output=True,
        text=True,
    )
    # Strip "Reading Parquet file:" header
    return [l for l in result.stdout.splitlines() if not l.startswith("Reading ")]


def compare_outputs(c_lines: list[str], py_lines: list[str], label: str) -> bool:
    """Compare outputs and show diff if mismatch. Returns True if match."""
    if c_lines == py_lines:
        print(f"  {label}: PASS")
        return True
    else:
        print(f"  {label}: FAIL")
        # Show unified diff (limited context for readability)
        diff = list(
            difflib.unified_diff(
                c_lines,
                py_lines,
                fromfile="dstdump.run",
                tofile="dst-dump",
                lineterm="",
                n=3,
            )
        )
        # Limit diff output to first 50 lines
        for line in diff[:50]:
            print(f"    {line}")
        if len(diff) > 50:
            print(f"    ... ({len(diff) - 50} more diff lines)")
        return False


def validate_bank(bank: str, dst_file: str, parquet_file: str) -> bool:
    """Validate both short and long output forms for a bank."""
    print(f"Validating bank: {bank}")

    all_pass = True

    # Test short form (-bank)
    c_short = run_dstdump(bank, dst_file, long_output=False)
    py_short = run_dst_dump(bank, parquet_file, long_output=False)
    if not compare_outputs(c_short, py_short, f"-{bank} (short)"):
        all_pass = False

    # Test long form (+bank)
    c_long = run_dstdump(bank, dst_file, long_output=True)
    py_long = run_dst_dump(bank, parquet_file, long_output=True)
    if not compare_outputs(c_long, py_long, f"+{bank} (long)"):
        all_pass = False

    return all_pass


def main():
    parser = argparse.ArgumentParser(
        description="Validate dst-dump against dstdump.run for a given bank"
    )
    parser.add_argument("bank", help="Bank name to validate (e.g., talex00, prfc)")
    parser.add_argument("dst_file", help="Path to DST file (.dst.gz)")
    parser.add_argument("parquet_file", help="Path to Parquet file (.parquet)")
    args = parser.parse_args()

    success = validate_bank(args.bank, args.dst_file, args.parquet_file)
    sys.exit(0 if success else 1)


if __name__ == "__main__":
    main()
