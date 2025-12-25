import awkward as ak
import numpy as np
import os
import argparse
from dst_awkward.dst_io import DSTFile
from dst_awkward.dst_reader import BankReader

BANK_MAP = {
    13104: "rusdgeom",
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

def rusdgeom_dump(rusdgeom: ak.Record, long_output=1):
    print("rusdgeom :")
    print(f"nsds={rusdgeom.nsds} tearliest={rusdgeom.tearliest:.2f}")

    # --- Plane Fit [0] ---
    print(f"Plane fit  xcore={rusdgeom.xcore[0]:.2f}+/-{rusdgeom.dxcore[0]:.2f} "
          f"ycore={rusdgeom.ycore[0]:.2f}+/-{rusdgeom.dycore[0]:.2f} "
          f"t0={rusdgeom.t0[0]:.2f}+/-{rusdgeom.dt0[0]:.2f} "
          f"theta={rusdgeom.theta[0]:.2f}+/-{rusdgeom.dtheta[0]:.2f} "
          f"phi={rusdgeom.phi[0]:.2f}+/-{rusdgeom.dphi[0]:.2f} "
          f"chi2={rusdgeom.chi2[0]:.2f} ndof={rusdgeom.ndof[0]}")

    # --- Modified Linsley Fit [1] ---
    print(f"Modified Linsley fit  xcore={rusdgeom.xcore[1]:.2f}+/-{rusdgeom.dxcore[1]:.2f} "
          f"ycore={rusdgeom.ycore[1]:.2f}+/-{rusdgeom.dycore[1]:.2f} "
          f"t0={rusdgeom.t0[1]:.2f}+/-{rusdgeom.dt0[1]:.2f} "
          f"theta={rusdgeom.theta[1]:.2f}+/-{rusdgeom.dtheta[1]:.2f} "
          f"phi={rusdgeom.phi[1]:.2f}+/-{rusdgeom.dphi[1]:.2f} "
          f"chi2={rusdgeom.chi2[1]:.2f} ndof={rusdgeom.ndof[1]}")

    # --- Final Fit w/ Curvature [2] ---
    print(f"Mod. Lin. fit w curv.  xcore={rusdgeom.xcore[2]:.2f}+/-{rusdgeom.dxcore[2]:.2f} "
          f"ycore={rusdgeom.ycore[2]:.2f}+/-{rusdgeom.dycore[2]:.2f} "
          f"t0={rusdgeom.t0[2]:.2f}+/-{rusdgeom.dt0[2]:.2f} "
          f"theta={rusdgeom.theta[2]:.2f}+/-{rusdgeom.dtheta[2]:.2f} "
          f"phi={rusdgeom.phi[2]:.2f}+/-{rusdgeom.dphi[2]:.2f} "
          f"a={rusdgeom.a:.2f}+/-{rusdgeom.da:.2f} "
          f"chi2={rusdgeom.chi2[2]:.2f} ndof={rusdgeom.ndof[2]}")

    # --- SD Summary Table ---
    # C Header: "index", "xxyy", "pulsa,[VEM]", "sdtime,[1200m]", "sdterr,[1200m]", "sdirufptn", "igsd"
    # Format: %s%8s%18s%17s%16s%10s%8s
    print(f"{'index':s}{'xxyy':>8s}{'pulsa,[VEM]':>18s}{'sdtime,[1200m]':>17s}"
          f"{'sdterr,[1200m]':>16s}{'sdirufptn':>10s}{'igsd':>8s}")

    for i in range(rusdgeom.nsds):
        # C Format: %3d%10.04d%15f%15f%15f%11d%12d
        xxyy_fmt = f"{rusdgeom.xxyy[i]:04d}".rjust(10)
        print(f"{i:3d}{xxyy_fmt}"
              f"{rusdgeom.pulsa[i]:15f}"
              f"{rusdgeom.sdtime[i]:15f}"
              f"{rusdgeom.sdterr[i]:15f}"
              f"{rusdgeom.sdirufptn[i]:11d}"
              f"{rusdgeom.igsd[i]:12d}")

    # --- Detailed Signal Table (Long Output) ---
    if long_output == 1:
        print()
        # C Header: "index", "xxyy", "sdsigq,[VEM]", "sdsigt,[1200m]", "sdsigte,[1200m]", "sdirufptn", "igsig"
        # Note: C header column 6 says "sdirufptn" but variable printed is irufptn[i][j]
        print(f"{'index':s}{'xxyy':>8s}{'sdsigq,[VEM]':>18s}{'sdsigt,[1200m]':>17s}"
              f"{'sdsigte,[1200m]':>16s}{'sdirufptn':>10s}{'igsig':>8s}")

        for i in range(rusdgeom.nsds):
            # nsig is available per SD
            nsig_count = rusdgeom.nsig[i]
            
            # Access jagged arrays: rusdgeom.sdsigq[i][j]
            for j in range(nsig_count):
                xxyy_fmt = f"{rusdgeom.xxyy[i]:04d}".rjust(10)
                print(f"{i:3d}{xxyy_fmt}"
                      f"{rusdgeom.sdsigq[i][j]:15f}"
                      f"{rusdgeom.sdsigt[i][j]:15f}"
                      f"{rusdgeom.sdsigte[i][j]:15f}"
                      f"{rusdgeom.irufptn[i][j]:11d}"
                      f"{rusdgeom.igsig[i][j]:12d}")

def main():
    parser = argparse.ArgumentParser(description="Process a DST file and print rusdgeom dump.")
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
    print("\n--- Rusdgeom Dump (from Awkward Array) ---")
    for i, event in enumerate(events_ak):
        if 'rusdgeom' not in event.fields:
            print(f"Event {i}: No rusdgeom bank found.")
            continue
        
        rusdgeom_bank = event['rusdgeom']
        print(f"\nEvent {i}:")
        rusdgeom_dump(rusdgeom_bank)

if __name__ == "__main__":
    main()