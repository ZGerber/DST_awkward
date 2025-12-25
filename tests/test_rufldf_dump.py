import awkward as ak
import numpy as np
import os
import argparse
from dst_awkward.dst_io import DSTFile
from dst_awkward.dst_reader import BankReader

BANK_MAP = {
    13107: "rufldf",
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

def rufldf_dump(rufldf: ak.Record):
    print("rufldf :")
    
    # --- Index 0 (LDF alone fit) ---
    # C: fprintf (fp, "xcore[0] %.2f ... atmcor[0]: %.2f ... ndof[0] %d\n", ...)
    print(f"xcore[0] {rufldf.xcore[0]:.2f} dxcore[0] {rufldf.dxcore[0]:.2f} "
          f"ycore[0] {rufldf.ycore[0]:.2f} dycore[0] {rufldf.dycore[0]:.2f} "
          f"s800[0] {rufldf.s800[0]:.2f} energy[0] {rufldf.energy[0]:.2f} "
          f"atmcor[0]: {rufldf.atmcor[0]:.2f} chi2[0] {rufldf.chi2[0]:.2f} "
          f"ndof[0] {rufldf.ndof[0]}")

    # --- Index 1 (LDF + geom fit) ---
    # C: fprintf (fp, "xcore[1] %.2f ... atmcor[1]: %.2f ... ndof[1] %d\n", ...)
    print(f"xcore[1] {rufldf.xcore[1]:.2f} dxcore[1] {rufldf.dxcore[1]:.2f} "
          f"ycore[1] {rufldf.ycore[1]:.2f} dycore[1] {rufldf.dycore[1]:.2f} "
          f"s800[1] {rufldf.s800[1]:.2f} energy[1] {rufldf.energy[1]:.2f} "
          f"atmcor[1]: {rufldf.atmcor[1]:.2f} chi2[1] {rufldf.chi2[1]:.2f} "
          f"ndof[1] {rufldf.ndof[1]}")

    # --- Geometry scalars ---
    # C: fprintf (fp, "theta %.2f dtheta %.2f phi %.2f dphi %.2f t0 %.2f dt0 %.2f\n", ...)
    print(f"theta {rufldf.theta:.2f} dtheta {rufldf.dtheta:.2f} "
          f"phi {rufldf.phi:.2f} dphi {rufldf.dphi:.2f} "
          f"t0 {rufldf.t0:.2f} dt0 {rufldf.dt0:.2f}")

    # --- Distance scalars ---
    # C: fprintf (fp, "bdist %.2f tdist %.2f\n", ...)
    print(f"bdist {rufldf.bdist:.2f} tdist {rufldf.tdist:.2f}")

def main():
    parser = argparse.ArgumentParser(description="Process a DST file and print rufldf dump.")
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
    print("\n--- Rufldf Dump (from Awkward Array) ---")
    for i, event in enumerate(events_ak):
        if 'rufldf' not in event.fields:
            print(f"Event {i}: No rufldf bank found.")
            continue
        
        rufldf_bank = event['rufldf']
        print(f"\nEvent {i}:")
        rufldf_dump(rufldf_bank)

if __name__ == "__main__":
    main()