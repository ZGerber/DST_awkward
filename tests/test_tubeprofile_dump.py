import awkward as ak
import numpy as np
import os
import argparse
from dst_awkward.dst_io import DSTFile
from dst_awkward.dst_reader import BankReader

# --- Constants ---
R2D = 180.0 / np.pi

BANK_MAP = {
    12096: "fdtubeprofile",
    12106: "brtubeprofile",
    12206: "lrtubeprofile",
    13313: "hytubeprofile",
    20002: "sttubeprofile",
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
                
                if name in current_event:
                    yield current_event
                    current_event = {}
                    event_count += 1
                    if limit and event_count >= limit:
                        return

                try:
                    data, _ = reader.parse_buffer(raw_bytes)
                    data['_version'] = ver 
                    current_event[name] = data
                except Exception as e:
                    print(f"Error parsing bank {name} (ID {bank_id}, ver {ver}): {e}")

            if current_event:
                yield current_event

def dump_fdtubeprofile(bank_data, bank_name):
    """Dumps fdtubeprofile and its clones (br/lr/hy)."""
    print(f"{bank_name.upper()} :")
    
    # 3 Fits
    for i in range(3):
        # Header info for this fit
        # Note: status, rp, psi, t0 are arrays of size 3
        print(f"Fit {i}: psi {bank_data.psi[i]*R2D:6.2f}  rp {bank_data.rp[i]:7.1f}  t0 {bank_data.t0[i]:7.1f}")
        
        print(" idx                            gram      npe     enpe   simnpe      flux     "
              "eflux       nfl     cvdir     cvmie     cvray   simflux      tres  tchi2        "
              "Ne       eNe   qual  simtime  simtrms  timeres timechi2")
        
        # Sort indices based on X (grammage) for display, matching standard dump behavior
        # bank_data.x[i] is the grammage array for this fit
        # Using argsort on the specific fit's x array
        sorted_indices = np.argsort(bank_data.x[i])
        
        for idx in sorted_indices:
            # Common arrays (camera, tube) are 1D
            cam = bank_data.camera[idx]
            tube = bank_data.tube[idx]
            qual = bank_data.tube_qual[idx]
            
            # Fit specific arrays are 2D [fit_idx][tube_idx]
            g = bank_data.x[i][idx]
            npe = bank_data.npe[i][idx]
            enpe = bank_data.enpe[i][idx]
            simnpe = bank_data.simnpe[i][idx]
            flux = bank_data.flux[i][idx]
            eflux = bank_data.eflux[i][idx]
            simflux = bank_data.simflux[i][idx]
            ne = bank_data.ne[i][idx]
            ene = bank_data.ene[i][idx]
            tres = bank_data.tres[i][idx]
            tchi2 = bank_data.tchi2[i][idx]
            simtime = bank_data.simtime[i][idx]
            simtrms = bank_data.simtrms[i][idx]
            simtres = bank_data.simtres[i][idx]
            timechi2 = bank_data.timechi2[i][idx]

            # Dummy values for flux components not in DST (cvdir, cvmie, cvray) -> 0.0
            print(f"{idx:4d} [ cam {cam:2d} tube {tube:3d} ]   {g:9.4f} {npe:8.2f} {enpe:8.2f} {simnpe:8.2f} "
                  f"{flux/R2D:9.3e} {eflux/R2D:9.3e} {0.0:9.3e} {0.0:9.3e} {0.0:9.3e} {simflux/R2D:9.2e} "
                  f"{tres:6.2f} {tchi2:9.3e} {ne:9.3e} {ene:9.3e} {qual:6d} "
                  f"{simtime:8.2f} {simtrms:8.2f} {simtres:8.2f} {timechi2:.3e}")

    # Footer info
    print(f"X0 {bank_data.X0:.2f} +/- {bank_data.eX0:.2f}  "
          f"Lambda {bank_data.Lambda:.2f} +/- {bank_data.eLambda:.2f}")
    print()

def dump_sttubeprofile(bank_data):
    """Dumps sttubeprofile (stereo structure)."""
    print("STTUBEPROFILE :")
    
    # Iterate over 2 sites (BR=0, LR=1)
    site_names = ["BLACK ROCK", "LONG RIDGE"]
    
    for i in range(2):
        if i < len(site_names):
            print(f"\nTube information for {site_names[i]}")
        else:
            print(f"\nTube information for Site {i}")

        # Sort indices based on X (grammage) for this site
        # In sttubeprofile, x is Jagged: bank_data.x[i] is an array of size ntube[i]
        sorted_indices = np.argsort(bank_data.x[i])
        
        print(" idx                            gram      npe     enpe   simnpe      flux     "
              "eflux       nfl     cvdir     cvmie     cvray   simflux      tres  tchi2        "
              "Ne       eNe  qual")

        for idx in sorted_indices:
            # Access jagged arrays: array[site_index][tube_index]
            cam = bank_data.camera[i][idx]
            tube = bank_data.tube[i][idx]
            qual = bank_data.tube_qual[i][idx]
            g = bank_data.x[i][idx]
            npe = bank_data.npe[i][idx]
            enpe = bank_data.enpe[i][idx]
            simnpe = bank_data.simnpe[i][idx]
            flux = bank_data.flux[i][idx]
            eflux = bank_data.eflux[i][idx]
            simflux = bank_data.simflux[i][idx]
            ne = bank_data.ne[i][idx]
            ene = bank_data.ene[i][idx]
            tres = bank_data.tres[i][idx]
            tchi2 = bank_data.tchi2[i][idx]

            print(f"{idx:4d} [ cam {cam:2d} tube {tube:3d} ]   {g:9.4f} {npe:8.2f} {enpe:8.2f} {simnpe:8.2f} "
                  f"{flux:9.3e} {eflux:9.3e} {0.0:9.3e} {0.0:9.3e} {0.0:9.3e} {simflux:9.2e} "
                  f"{tres:6.2f} {tchi2:9.3e} {ne:9.3e}     {qual:d}")

    # Footer
    print(f"X0 {bank_data.X0:.2f} +/- {bank_data.eX0:.2f}  "
          f"Lambda {bank_data.Lambda:.2f} +/- {bank_data.eLambda:.2f}")
    print()

def main():
    parser = argparse.ArgumentParser(description="Process a DST file and print tubeprofile dumps.")
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

    # 4. Print Dumps
    print("\n--- Tube Profile Dump (from Awkward Array) ---")
    for i, event in enumerate(events_ak):
        print(f"\nEvent {i}:")
        
        # FD/BR/LR/HY Profiles (Same Structure)
        for bank_name in ["fdtubeprofile", "brtubeprofile", "lrtubeprofile", "hytubeprofile"]:
            if bank_name in event.fields:
                dump_fdtubeprofile(event[bank_name], bank_name)
        
        # Stereo Profile (Different Structure)
        if "sttubeprofile" in event.fields:
            dump_sttubeprofile(event["sttubeprofile"])

if __name__ == "__main__":
    main()