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
        """
        Reads DST file and yields Events (dicts of banks).
        Uses Event Start (ID 0) and Event End (ID 1) markers to define event boundaries.
        """
        # State tracking: None means we are outside an event.
        current_event = None 
        event_count = 0
        
        # Standard DST Markers
        START_BANKID = 1400000023
        STOP_BANKID  = 1400000101
        
        with DSTFile(filename) as dst:
            for bank_id, ver, raw_bytes in dst.banks():
                
                # --- Case 1: Start of Event ---
                if bank_id == START_BANKID:
                    # Initialize a new dictionary to hold bank data
                    current_event = {}
                    continue

                # --- Case 2: End of Event ---
                elif bank_id == STOP_BANKID:
                    # If we have an open event, yield it now
                    if current_event is not None:
                        # Optional: Only yield if we actually found banks of interest?
                        # For now, we yield if the event structure was valid, 
                        # allowing the main loop to check if keys exist.
                        yield current_event
                        
                        event_count += 1
                        current_event = None # Reset state
                        
                        if limit and event_count >= limit:
                            return
                    continue

                # --- Case 3: Data Banks ---
                # Only process banks if we are currently inside a valid event bracket
                if current_event is not None:
                    if bank_id in self.readers:
                        name = self.bank_names[bank_id]
                        reader = self.readers[bank_id]
                        
                        try:
                            # Parse the bank
                            data, _ = reader.parse_buffer(raw_bytes)
                            data['_version'] = ver 
                            current_event[name] = data
                        except Exception as e:
                            print(f"Error parsing bank {name} (ID {bank_id}, ver {ver}): {e}")

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
            if name in event.fields and event[name] is not None:
                fdraw_dump(event[name], bank_name=name)

if __name__ == "__main__":
    main()