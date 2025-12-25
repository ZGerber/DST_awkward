import awkward as ak
import numpy as np
import os
import argparse
from dst_awkward.dst_io import DSTFile
from dst_awkward.dst_reader import BankReader

BANK_MAP = {
    13109: "sdtrgbk",
}

# Mapping for eventNameFromId simulation
RAW_BANK_NAMES = {
    13101: "rusdraw",
    13115: "tasdcalibev", 
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

def get_bank_name(bank_id):
    """Mimics eventNameFromId C function."""
    return RAW_BANK_NAMES.get(bank_id, f"Unknown Bank {bank_id}")

def sdtrgbk_dump(sdtrgbk: ak.Record, long_output=1):
    print("sdtrgbk :")
    print()
    
    # C: eventNameFromId(sdtrgbk_.raw_bankid,bname,bname_len);
    bname = get_bank_name(sdtrgbk.raw_bankid)
    print(f"raw waveform bank used: {bname}")
    
    # C: fprintf(fp, "igevent %d trigp %d ...", ...)
    print(f"igevent {sdtrgbk.igevent} trigp {sdtrgbk.trigp} "
          f"dec_ped {sdtrgbk.dec_ped} inc_ped {sdtrgbk.inc_ped} "
          f"nsd {sdtrgbk.nsd} n_bad_ped {sdtrgbk.n_bad_ped} "
          f"n_isol {sdtrgbk.n_isol} n_spat_cont {sdtrgbk.n_spat_cont} "
          f"n_pot_st_cont {sdtrgbk.n_pot_st_cont} n_l1_tg {sdtrgbk.n_l1_tg}", end="")

    # Trigger Info
    # C: if (sdtrgbk_.igevent>=1)
    if sdtrgbk.igevent >= 1:
        print(" L2TRIG:", end="")
        for i in range(3):
            isd = sdtrgbk.il2sd[i]
            isig = sdtrgbk.il2sd_sig[i]
            
            # Access jagged arrays
            # In C: sdtrgbk_.secf[isd][isig]
            # In Awkward: sdtrgbk.secf[isd][isig]
            secf_val = sdtrgbk.secf[isd][isig]
            ql_val = sdtrgbk.q[isd][isig][0]
            qu_val = sdtrgbk.q[isd][isig][1]
            xxyy_val = sdtrgbk.xxyy[isd]
            
            print(f" xxyy {xxyy_val:04d} secf {secf_val:.6f} Ql {ql_val} Qu {qu_val}", end="")
    
    print("\n\n", end="") # Matching C's double newline

    # Long Output Table
    if long_output == 1:
        # C: fprintf(fp, "%3s \t %4s \t %2s \t %6s \n", "isd", "xxyy", "ig", "nl1sig");
        print(f"{'isd':>3} \t {'xxyy':>4} \t {'ig':>2} \t {'nl1sig':>6} ")
        
        for i in range(sdtrgbk.nsd):
            # C: fprintf(fp, "%3d \t %04d \t %2d \t %4d \n", i, sdtrgbk_.xxyy[i], (int) sdtrgbk_.ig[i], sdtrgbk_.nl1[i]);
            print(f"{i:3d} \t {sdtrgbk.xxyy[i]:04d} \t {sdtrgbk.ig[i]:2d} \t {sdtrgbk.nl1[i]:4d} ")

def main():
    parser = argparse.ArgumentParser(description="Process a DST file and print sdtrgbk dump.")
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
    print("\n--- Sdtrgbk Dump (from Awkward Array) ---")
    for i, event in enumerate(events_ak):
        if 'sdtrgbk' not in event.fields:
            print(f"Event {i}: No sdtrgbk bank found.")
            continue
        
        sdtrgbk_bank = event['sdtrgbk']
        print(f"\nEvent {i}:")
        sdtrgbk_dump(sdtrgbk_bank)

if __name__ == "__main__":
    main()