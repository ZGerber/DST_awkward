import awkward as ak
import numpy as np
import os
import time
import argparse  # Added for command-line argument parsing
from dst_awkward.dst_io import DSTFile
from dst_awkward.dst_reader import BankReader

# --- Configuration ---
# Complete mapping of Bank IDs to Schema Files
BANK_MAP = {
    # SD Raw & MC
    13101: "rusdraw",
    13105: "rusdmc",
    13106: "rusdmc1",
    
    # SD Reconstruction
    13103: "rufptn",
    13104: "rusdgeom",
    13107: "rufldf",
    13109: "sdtrgbk",
    13112: "bsdinfo",

    # FD Data (Geometries & Raw)
    12091: "geofd",
    12101: "geobr",
    12202: "geolr",
    12001: "fraw1",
    
    # Calibration / Library
    12811: "showlib",
}

class DSTMultiReader:
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
        
        # DST logic: Banks are sequential. If we see a bank type that 
        # is already in our current event buffer, it means the previous 
        # event is finished.
        
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
                    # NOTE: Ensure dst_reader.py has the start_offset=8 default!
                    data, _ = reader.parse_buffer(raw_bytes)
                    data['_version'] = ver 
                    current_event[name] = data
                except Exception as e:
                    print(f"Error parsing bank {name} (ID {bank_id}, ver {ver}): {e}")
                    print(f"Buffer length: {len(raw_bytes)}")

            # Yield last event
            if current_event:
                yield current_event

def main():
    # --- Parse Command-Line Arguments ---
    parser = argparse.ArgumentParser(description="Process a DST file and convert to Parquet format.")
    parser.add_argument("input_file", help="Path to the input DST file (.dst or .dst.gz)")
    args = parser.parse_args()
    
    input_file = args.input_file
    
    # --- Derive Output Parquet Filename ---
    if input_file.endswith('.dst.gz'):
        output_parquet = input_file.replace('.dst.gz', '.parquet')
    elif input_file.endswith('.dst'):
        output_parquet = input_file.replace('.dst', '.parquet')
    else:
        # Fallback: Append .parquet if no matching extension
        output_parquet = input_file + '.parquet'
    
    if not os.path.exists(input_file):
        print(f"Error: Input file '{input_file}' not found.")
        return

    # 1. Initialize
    multi_reader = DSTMultiReader()
    
    # 2. Read Events
    print(f"\nProcessing {input_file}...")
    t0 = time.time()
    
    event_list = []
    # Set limit=None to process the whole file
    for ev in multi_reader.process_file(input_file, limit=None):
        event_list.append(ev)
        
    t1 = time.time()
    print(f"Done. Extracted {len(event_list)} events in {t1-t0:.2f}s.")

    # 3. Create Awkward Array
    print("Building Awkward Array...")
    events_ak = ak.Array(event_list)
    print(f"Array Type: {events_ak.type}")

    # 4. Save to Parquet
    print(f"Saving to {output_parquet}...")
    try:
        ak.to_parquet(events_ak, output_parquet)
        print("Success!")
    except Exception as e:
        print(f"Failed to write Parquet: {e}")
        print("(Do you have pyarrow installed? 'pip install pyarrow')")

if __name__ == "__main__":
    main()
