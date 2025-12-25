import awkward as ak
import numpy as np
import os
import argparse
import sys
from dst_awkward.dst_io import DSTFile
from dst_awkward.dst_reader import BankReader

RUFPTN_ORIGIN_X_CLF = -12.2435
RUFPTN_ORIGIN_Y_CLF = -16.4406
RUFPTN_TIMDIST      = 0.249827048333

BANK_MAP = {
    13103: "rufptn",
}

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

            # Yield last event
            if current_event:
                yield current_event

def rufptn_dump(rufptn: ak.Record):
    print("rufptn :")
    
    # --- Header Calculation ---
    # C: rufptn_.tyro_xymoments[2][0] + RUFPTN_ORIGIN_X_CLF
    core_x = rufptn.tyro_xymoments[2][0] + RUFPTN_ORIGIN_X_CLF
    
    # C: rufptn_.tyro_xymoments[2][1] + RUFPTN_ORIGIN_Y_CLF
    core_y = rufptn.tyro_xymoments[2][1] + RUFPTN_ORIGIN_Y_CLF
    
    # C: 0.5*(rufptn_.tearliest[0]+rufptn_.tearliest[1]) + rufptn_.tyro_tfitpars[2][0]/RUFPTN_TIMDIST*1e-6
    # Note: tyro_tfitpars is shape (3, 2). C uses index [2][0].
    t0 = 0.5 * (rufptn.tearliest[0] + rufptn.tearliest[1]) + \
         (rufptn.tyro_tfitpars[2][0] / RUFPTN_TIMDIST * 1e-6)

    # --- Header Print ---
    # C: "nhits %d nsclust %d nstclust %d nborder %d core_x %f core_y %f t0 %.9f \n"
    print(f"nhits {rufptn.nhits} nsclust {rufptn.nsclust} nstclust {rufptn.nstclust} "
          f"nborder {rufptn.nborder} core_x {core_x:f} core_y {core_y:f} t0 {t0:.9f} ")

    # --- Table Header ---
    # C: fprintf(fp, "%s%8s%14s%15s%15s%18s%18s\n", "#","XXYY","Q upper","Q lower","T upper","T lower","isgood");
    print(f"{'#':s}{'XXYY':>8s}{'Q upper':>14s}{'Q lower':>15s}{'T upper':>15s}{'T lower':>18s}{'isgood':>18s}")

    # --- Table Loop ---
    for i in range(rufptn.nhits):
        # C Calculations inside loop
        # rufptn_.tearliest[0]+(4.0028e-6)*rufptn_.reltime[i][0]
        t_upper = rufptn.tearliest[0] + (4.0028e-6) * rufptn.reltime[i][0]
        
        # rufptn_.tearliest[1]+(4.0028e-6)*rufptn_.reltime[i][1]
        t_lower = rufptn.tearliest[1] + (4.0028e-6) * rufptn.reltime[i][1]
        
        # C Format: "%02d%7.4d%15f%15f%22.9f%18.9f%7d\n"
        # Note: %7.4d in C means min width 7, min digits 4 (0-padded). Python {val:7.4d} is invalid.
        # We emulate it with f"{val:04d}".rjust(7)
        xxyy_fmt = f"{rufptn.xxyy[i]:04d}".rjust(7)
        
        print(f"{i:02d}{xxyy_fmt}"
              f"{rufptn.pulsa[i][0]:15f}"
              f"{rufptn.pulsa[i][1]:15f}"
              f"{t_upper:22.9f}"
              f"{t_lower:18.9f}"
              f"{rufptn.isgood[i]:7d}")

def main():
    parser = argparse.ArgumentParser(description="Process a DST file and print rufptn dump.")
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

    # 4. Print Dump
    print("\n--- Rufptn Dump (from Awkward Array) ---")
    for i, event in enumerate(events_ak):
        if 'rufptn' not in event.fields:
            print(f"Event {i}: No rufptn bank found.")
            continue
        
        rufptn_bank = event['rufptn']
        print(f"\nEvent {i}:")
        rufptn_dump(rufptn_bank)

if __name__ == "__main__":
    main()