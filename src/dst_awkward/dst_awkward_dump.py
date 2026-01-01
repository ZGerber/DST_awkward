#!/usr/bin/env python3
import sys
import os
import awkward as ak
from dst_awkward.dump import get_dump

def main():
    args = sys.argv[1:]
    
    if not args:
        print_usage()
        sys.exit(2)

    # --- Configuration State ---
    # Maps bank_name -> mode (True=Long, False=Short)
    # If a bank is not in this map, it is not dumped (unless dump_all is set)
    want_banks = {} 
    dump_all = False
    all_mode_long = True # Default mode for 'all'
    
    skip_events = 0
    input_files = []

    # --- 1. Parse Arguments (Manual Loop to mimic dstdump.c) ---
    for arg in args:
        if arg.startswith('#'):
            # Event Skip: #NNN
            try:
                skip_events = int(arg[1:])
                print(f" skipping first {skip_events} events...")
            except ValueError:
                print(f"Invalid skip count: {arg}")
                sys.exit(1)
        
        elif arg.startswith('+') or arg.startswith('-'):
            mode_long = (arg.startswith('+'))
            name = arg[1:]
            
            if name == "all":
                dump_all = True
                all_mode_long = mode_long
                # If -all or +all is specified, we clear specific requests 
                # (Logic matches dstdump.c: "clr_bank_list_" or setting flags)
                # But typically +all overrides previous specific settings.
                # For simplicity: dump_all overrides the want_banks list logic below.
            else:
                want_banks[name] = mode_long
        
        else:
            # Assume filename
            input_files.append(arg)

    if not input_files:
        print("No input files specified.")
        sys.exit(1)

    # --- 2. Process Files ---
    for fpath in input_files:
        process_file(fpath, want_banks, dump_all, all_mode_long, skip_events)

def process_file(fpath, want_banks, dump_all, all_mode_long, skip_events):
    print(f"Reading Parquet file: {fpath}")
    
    try:
        # Load the array. 
        # Note: ak.from_parquet is lazy, but iterating Python-side pulls data.
        events = ak.from_parquet(fpath)
    except Exception as e:
        print(f"Error opening {fpath}: {e}")
        return

    num_events = len(events)
    
    # Check if we have anything to do
    if skip_events >= num_events:
        print(f"Skipping all {num_events} events in file.")
        return

    # Slice the array to skip events
    # (Awkward slicing is efficient)
    events_to_process = events[skip_events:]
    
    # Iterate
    # We enumerate to keep track of global event count if needed, 
    # but here we just process.
    for i, event in enumerate(events_to_process):
        
        # Original dstdump output format
        print("START OF EVENT " + "*" * 59)
        
        # 1. Identify which fields actually exist in this specific event (not None)
        # event.fields gives the Schema (all possible columns).
        # We must check which ones have data for THIS event.
        present_banks = []
        for field in event.fields:
            # Check if data is present (not None)
            if event[field] is None:
                continue
            present_banks.append(field)
            
        # 2. Filter based on user request (+all or specific banks)
        banks_to_dump = []
        
        if dump_all:
            # Dump everything that is actually present
            for name in present_banks:
                banks_to_dump.append((name, all_mode_long))
        else:
            # Dump only what was requested, provided it exists
            for name, mode in want_banks.items():
                if name in present_banks:
                    banks_to_dump.append((name, mode))
                    
        # Sort by name for consistent output
        banks_to_dump.sort(key=lambda x: x[0])
        
        # 3. Dump
        for name, is_long in banks_to_dump:
            bank_data = event[name]
            
            # Double check (though present_banks logic handles this)
            if bank_data is None:
                continue

            # Dispatch
            try:
                dump_func = get_dump(name)
                dump_func(bank_data, short=(not is_long))
            except Exception as e:
                print(f"  [ERROR] Failed to dump bank '{name}': {e}")

        print("END OF EVENT " + "*" * 61)

def print_usage():
    cmd = os.path.basename(sys.argv[0])
    print(f"\nUsage: {cmd} [flags] parquet_file ...\n")
    print("Flags:")
    print("  -name: Show short form of bank 'name'")
    print("  +name: Show long form of bank 'name'")
    print("  -all:  Show short form of all banks")
    print("  +all:  Show long form of all banks")
    print("  #NNN:  Skip the first NNN events")
    print("\nDefault: No banks dumped unless specified.")
    print("\nExamples:")
    print(f"  {cmd} +all file.parquet")
    print(f"  {cmd} -rusdraw +rusdgeom file.parquet")

if __name__ == "__main__":
    main()