import awkward as ak
import numpy as np
import os
import argparse
import datetime
from dst_awkward.dst_io import DSTFile
from dst_awkward.dst_reader import BankReader

# --- Constants ---
R2D = 180.0 / np.pi
M_PI = np.pi
STPLANE_MAXSITES = 4

BANK_MAP = {
    20001: "stplane",
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

def caldat(julian):
    """Simple converter for Julian date to YMD."""
    try:
        # Simplified algorithm for Gregorian calendar
        j = julian + 32044
        g = j // 146097
        dg = j % 146097
        c = (dg // 36524 + 1) * 3 // 4
        dc = dg - c * 36524
        b = dc // 1461
        db = dc % 1461
        a = (db // 365 + 1) * 3 // 4
        da = db - a * 365
        y = g * 400 + c * 100 + b * 4 + a
        m = (da * 5 + 308) // 153 - 2
        d = da - (m + 4) * 153 // 5 + 122
        Y = y - 4800 + (m + 2) // 12
        M = (m + 2) % 12 + 1
        D = d + 1
        return Y, M, D
    except:
        return 0, 0, 0

def stplane_dump(stplane: ak.Record):
    print("STPLANE :")
    
    # Impact Point
    print(f" Impact Point (m): ( {stplane.impactPoint[0]:12.3f}  {stplane.impactPoint[1]:12.3f}  {stplane.impactPoint[2]:12.3f} )")
    
    # Shower Vector
    print(f" Shower Vector:    ({stplane.showerVector[0]:7.4f},{stplane.showerVector[1]:7.4f},{stplane.showerVector[2]:7.4f})")
    
    # Zenith
    zenith_deg = np.arccos(-stplane.showerVector[2]) * R2D
    print(f"    Zenith: {zenith_deg:7.2f}")
    
    # Azimuth
    z = np.arctan2(-stplane.showerVector[1], -stplane.showerVector[0])
    if z < 0:
        z += 2.0 * M_PI
    print(f"   Azimuth: {z * R2D:7.2f}")

    # Crossing Angles
    # C uses direct array access: [0]=BR/LR, [1]=BR/MD, [3]=LR/MD.
    # Logic in C:
    # if (stplane->sites[0] && stplane->sites[1]) -> index 0 (BR-LR)
    # if (stplane->sites[0] && stplane->sites[2]) -> index 1 (BR-MD)
    # if (stplane->sites[1] && stplane->sites[2]) -> index 3 (LR-MD) (Note: index 2 would be BR-TL presumably, index 3 is LR-MD)
    
    if stplane.sites[0] and stplane.sites[1]:
        print(f" BR-LR plane-crossing angle (deg.): {stplane.sdp_angle[0] * R2D:.3f}")
    
    if stplane.sites[0] and stplane.sites[2]:
        print(f" BR-MD plane-crossing angle (deg.): {stplane.sdp_angle[1] * R2D:.3f}")
        
    if stplane.sites[1] and stplane.sites[2]:
        print(f" LR-MD plane-crossing angle (deg.): {stplane.sdp_angle[3] * R2D:.3f}")

    # Per-Site Info Loop
    for site in range(STPLANE_MAXSITES):
        if not stplane.sites[site]:
            continue
            
        ymd = 0
        hms = 0
        nano = 0
        
        # Time Calculation
        if stplane.nanocore[site] >= 0:
            hr = stplane.jseccore[site] // 3600 + 12
            julian = stplane.juliancore[site]
            
            if hr >= 24:
                julian += 1
                hr -= 24
            
            yr, mo, day = caldat(julian)
            
            minute = (stplane.jseccore[site] // 60) % 60
            sec = stplane.jseccore[site] % 60
            
            ymd = 10000 * yr + 100 * mo + day
            hms = 10000 * hr + 100 * minute + sec
            nano = stplane.nanocore[site]

        print(f"\n Geometry information for site {site}")
        print(f"Event: part {stplane.part[site]:2d}  trigger {stplane.event_num[site]:6d}")
        
        print(f" Core time (CLF) from T0 fit: {ymd:08d} {hms:06d}.{nano:09d}")
        
        print(f" Core position (site):  ( {stplane.core[site][0]:12.3f} {stplane.core[site][1]:12.3f} {stplane.core[site][2]:12.3f} )")
        
        print(f" Rp unit vector (site):  ( {stplane.rpuv[site][0]:10.7f} {stplane.rpuv[site][1]:10.7f} {stplane.rpuv[site][2]:10.7f} )")
        
        print(f" magnitude(Rp) {stplane.rp[site]:12.3f} ; psi {stplane.psi[site] * R2D:8.3f}")
        
        print(f" SDP normal vector (CLF): {stplane.sdp_n[site][0]:.8f} {stplane.sdp_n[site][1]:.8f} {stplane.sdp_n[site][2]:.8f}")
        
        print(f" Track length (deg): {stplane.track_length[site] * R2D:.3f}")
        print(f" Expected duration (ns): {stplane.expected_duration[site]:.3f}")

def main():
    parser = argparse.ArgumentParser(description="Process a DST file and print stplane dump.")
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
    print("\n--- STPLANE Dump (from Awkward Array) ---")
    for i, event in enumerate(events_ak):
        if 'stplane' not in event.fields:
            print(f"Event {i}: No stplane bank found.")
            continue
        
        stplane = event['stplane']
        print(f"\nEvent {i}:")
        stplane_dump(stplane)

if __name__ == "__main__":
    main()