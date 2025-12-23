import awkward as ak
import numpy as np
import os
import time
from dst_io import DSTFile
from dst_reader import BankReader

# --- Configuration ---
# Complete mapping of Bank IDs to Schema Files
BANK_MAP = {
    # SD Raw & MC
    13101: ("rusdraw",  "rusdraw.yaml"),
    13105: ("rusdmc",   "rusdmc.yaml"),
    13106: ("rusdmc1",  "rusdmc1.yaml"),
    
    # SD Reconstruction
    13103: ("rufptn",   "rufptn.yaml"),
    13104: ("rusdgeom", "rusdgeom.yaml"),
    13107: ("rufldf",   "rufldf.yaml"),
    13109: ("sdtrgbk",  "sdtrgbk.yaml"),
    13112: ("bsdinfo",  "bsdinfo.yaml"),

    # FD Data (Geometries & Raw)
    12091: ("geofd",    "geofd.yaml"),
    12101: ("geobr",    "geobr.yaml"),
    12202: ("geolr",    "geolr.yaml"),
    12001: ("fraw1",    "fraw1.yaml"),
    
    # Calibration / Library
    12811: ("showlib",  "showlib.yaml"),
}

class DSTMultiReader:
    def __init__(self, schema_dir="./"):
        self.readers = {}
        self.bank_names = {}
        
        print(f"Loading schemas from {schema_dir}...")
        for bank_id, (name, yaml_file) in BANK_MAP.items():
            path = os.path.join(schema_dir, yaml_file)
            if os.path.exists(path):
                try:
                    self.readers[bank_id] = BankReader(path)
                    self.bank_names[bank_id] = name
                    print(f"  [OK] Loaded {name} ({bank_id})")
                except Exception as e:
                    print(f"  [FAIL] Could not load {name}: {e}")
            else:
                # print(f"  [SKIP] Schema {yaml_file} not found.")
                pass

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
    # --- FILE SETTINGS ---
    input_file = "DAT000017.spctrflat.showlib.sdrec.dst.gz"
    output_parquet = "DAT000017.spctrflat.showlib.sdrec.parquet"
    
    if not os.path.exists(input_file):
        print(f"Error: Input file '{input_file}' not found.")
        return

    # 1. Initialize
    multi_reader = DSTMultiReader(".")
    
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
