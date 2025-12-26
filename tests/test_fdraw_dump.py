import awkward as ak
import numpy as np
import os
import argparse
from dst_awkward.dst_io import DSTFile
from dst_awkward.dst_reader import BankReader

# --- Constants ---
BANK_MAP = {
    12092: "fdraw",
    12102: "brraw",
    12201: "lrraw",
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

def fdraw_dump(fdraw: ak.Record, bank_name="fdraw"):
    print(f"\n{bank_name} :")
    
    # --- Header ---
    print(f"\n\n event_code {fdraw.event_code} part {fdraw.part} num_mir {fdraw.num_mir} event_num {fdraw.event_num}")
    print(f" julian {fdraw.julian} jsecond {fdraw.jsecond} gps1pps_tick {fdraw.gps1pps_tick}")
    print(f" ctdclock {fdraw.ctdclock} ctd_version {fdraw.ctd_version} tf_version {fdraw.tf_version} sdf_version {fdraw.sdf_version}")

    # --- Mirror Loop ---
    for i in range(fdraw.num_mir):
        print(f"\n mirror index {i}: trig_code {fdraw.trig_code[i]} second {fdraw.second[i]} "
              f"microsec {fdraw.microsec[i]} clkcnt {fdraw.clkcnt[i]}")
        
        print(f"   mir_num {fdraw.mir_num[i]} num_chan {fdraw.num_chan[i]} "
              f"tf_mode {fdraw.tf_mode[i]} tf_mode2 {fdraw.tf_mode2[i]}")

        # hit_pt (Pattern) - formatting 16 shorts per line usually, or just a list
        # Using a simplified print for the array here
        print("   hit_pt:", end="")
        for k in range(256): # Usually 256 items for hit pattern
             if k % 16 == 0: print("\n     ", end="")
             print(f" {fdraw.hit_pt[i][k]:04X}", end="")
        print()

        # --- Channel Loop ---
        # Get channel count for this mirror
        n_chan = fdraw.num_chan[i]
        
        for j in range(n_chan):
            print(f"      channel: {fdraw.channel[i][j]}  sdf_peak: {fdraw.sdf_peak[i][j]}  "
                  f"sdf_tmphit: {fdraw.sdf_tmphit[i][j]}")
            
            print(f"      sdf_mode: {fdraw.sdf_mode[i][j]}  sdf_ctrl: {fdraw.sdf_ctrl[i][j]}  "
                  f"sdf_thre: {fdraw.sdf_thre[i][j]}")

            # Mean & Disp (Arrays of size 4)
            mean = fdraw.mean[i][j]
            disp = fdraw.disp[i][j]
            print(f"      mean: {mean[0]:5d} {mean[1]:5d} {mean[2]:5d} {mean[3]:5d}  "
                  f"disp: {disp[0]:5d} {disp[1]:5d} {disp[2]:5d} {disp[3]:5d}")

            # --- Waveform Data (FADC) ---
            print("      waveform data:")
            # FADC is usually 512 samples
            wf = fdraw.m_fadc[i][j]
            
            # Print loop matching C formatting (12 items per line, hex)
            for k in range(len(wf)):
                if k % 12 == 0:
                    print("       ", end="")
                
                # Mask to 16-bit hex to match C %04X behavior for signed shorts
                val = wf[k] & 0xFFFF
                print(f" {val:04X}", end="")
                
                if (k + 1) % 12 == 0:
                    print()
            
            # Handle trailing newline if not exactly multiple of 12
            if len(wf) % 12 != 0:
                print()

def main():
    parser = argparse.ArgumentParser(description="Process a DST file and print fdraw/brraw/lrraw dump.")
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
    print("\n--- Raw FD Dump (from Awkward Array) ---")
    for i, event in enumerate(events_ak):
        print(f"\nEvent {i}:")
        
        # Check for any of the 3 raw banks
        for name in ['fdraw', 'brraw', 'lrraw']:
            if name in event.fields:
                fdraw_dump(event[name], bank_name=name)

if __name__ == "__main__":
    main()