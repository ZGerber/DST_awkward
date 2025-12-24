import awkward as ak
import numpy as np
import os
import time
import argparse
from dst_awkward.dst_io import DSTFile
from dst_awkward.dst_reader import BankReader

BANK_MAP = {
    13101: "rusdraw",
}

RUSDRAW_SITEMAP = {0: "BR",
                   1: "LR",
                   2: "SK",
                   3: "BRLR",
                   4: "BRSK",
                   5: "LRSK",
                   6: "BRLRSK",}

class DSTReader:
    def __init__(self):
        self.readers = {}
        self.bank_names = {}
        
        print("Loading schemas...")
        for bank_id, name in BANK_MAP.items():
            try:
                self.readers[bank_id] = BankReader(name)
                self.bank_names[bank_id] = name
                print(f"  [OK] Loaded {name} ({bank_id})")
            except Exception as e:
                print(f"  [FAIL] Could not load {name}: {e}")

    def process_file(self, filename, limit=None):
        """Reads DST file and yields Events (dicts of banks)."""
        current_event = {}
        event_count = 0
        
        with DSTFile(filename) as dst:
            for bank_id, ver, raw_bytes in dst.banks():
                if bank_id not in self.readers:
                    continue
                
                name = self.bank_names[bank_id]
                reader = self.readers[bank_id]
                
                # Boundary Check: New Event?
                if name in current_event:
                    yield current_event
                    current_event = {}
                    event_count += 1
                    if limit and event_count >= limit:
                        return

                # Parse
                try:
                    data, _ = reader.parse_buffer(raw_bytes)
                    data['_version'] = ver 
                    current_event[name] = data
                except Exception as e:
                    print(f"Error parsing bank {name} (ID {bank_id}, ver {ver}): {e}")
                    print(f"Buffer length: {len(raw_bytes)}")

            # Yield last event
            if current_event:
                yield current_event

def rusdraw_dump(rusdraw: ak.Record):
    print("rusdraw :")
    
    # Mimic C dump format (adjust based on rusdraw_dump.txt example)
    yr = rusdraw.yymmdd // 10000
    mo = (rusdraw.yymmdd // 100) % 100
    day = rusdraw.yymmdd % 100
    hr = rusdraw.hhmmss // 10000    # Assuming hhmmss is in the format HHMMSS
    mn = (rusdraw.hhmmss // 100) % 100
    sec = rusdraw.hhmmss % 100
    usec = rusdraw.usec

    print(f"event_num {rusdraw.event_num:d} event_code {rusdraw.event_code:d} site",end="")
    print(f" {RUSDRAW_SITEMAP[rusdraw.site]}",end="")
    print(f" run_id: BR={rusdraw.run_id[0]:d} LR={rusdraw.run_id[1]:d} SK={rusdraw.run_id[2]:d}", end="")
    print(f" trig_id: BR={rusdraw.trig_id[0]:d} LR={rusdraw.trig_id[1]:d} SK={rusdraw.trig_id[2]:d}")
    print(f"errcode {rusdraw.errcode} date {mo:02d}/{day:02d}/{yr:02d} {hr:02d}:{mn:02d}:{sec:02d}.{usec:06d}",end="")
    print(f" nofwf {rusdraw.nofwf:d} monyymmdd {rusdraw.monyymmdd:06d} monhhmmss {rusdraw.monhhmmss:06d}")

    print("wf# wf_id  X   Y    clkcnt     mclkcnt   fadcti(lower,upper)  fadcav      pchmip        pchped      nfadcpermip     mftchi2      mftndof")
    for j in range(rusdraw.nofwf):
        print(f"{j:02d} {f'{rusdraw.wf_id[j]:02d}':>5} {rusdraw.xxyy[j]//100:4d} {rusdraw.xxyy[j]%100:3d}",end="")
        print(f" {rusdraw.clkcnt[j]:10d} {rusdraw.mclkcnt[j]:10d}",end="")
        print(f" {rusdraw.fadcti[j][0]:8d} {rusdraw.fadcti[j][1]:8d}",end="")
        print(f" {rusdraw.fadcav[j][0]:5d} {rusdraw.fadcav[j][1]:4d}",end="")
        print(f" {rusdraw.pchmip[j][0]:6d} {rusdraw.pchmip[j][1]:7d}",end="")
        print(f" {rusdraw.pchped[j][0]:5d} {rusdraw.pchped[j][1]:5d}",end="")
        print(f" {rusdraw.mip[j][0]:8.1f} {rusdraw.mip[j][1]:6.1f}",end="")
        print(f" {rusdraw.mftchi2[j][0]:6.1f} {rusdraw.mftchi2[j][1]:6.1f}",end="")
        print(f" {rusdraw.mftndof[j][0]:5d} {rusdraw.mftndof[j][1]:4d}")

        print("lower fadc")
        for row_start in range(0, 128, 12):
            row_end = min(row_start + 12, 128)
            values = [rusdraw.fadc[j][0][k] for k in range(row_start, row_end)]
            print(' '.join(f"{v:6d}" for v in values))
        print("upper fadc")
        for row_start in range(0, 128, 12):
            row_end = min(row_start + 12, 128)
            values = [rusdraw.fadc[j][1][k] for k in range(row_start, row_end)]
            print(' '.join(f"{v:6d}" for v in values))

def main():
    parser = argparse.ArgumentParser(description="Process a DST file and print rusdraw dump.")
    parser.add_argument("input_file", help="Path to the input DST file (.dst or .dst.gz)")
    args = parser.parse_args()
    
    input_file = args.input_file
    
    if not os.path.exists(input_file):
        print(f"Error: Input file '{input_file}' not found.")
        return

    # 1. Initialize
    reader = DSTReader()
    
    # 2. Read Events
    event_list = []
    for ev in reader.process_file(input_file, limit=None):
        event_list.append(ev)        

    # 3. Create Awkward Array
    events_ak = ak.Array(event_list)

    # 4. Print Rusdraw Dump (Mimicking rusdraw_dst.c from rusdraw_dump.txt)
    print("\n--- Rusdraw Dump (from Awkward Array) ---")
    for i, event in enumerate(events_ak):
        if 'rusdraw' not in event.fields:
            print(f"Event {i}: No rusdraw bank found.")
            continue
        
        rusdraw = event['rusdraw']
        print(f"\nEvent {i}:")
        rusdraw_dump(rusdraw)
        
if __name__ == "__main__":
    main()