import awkward as ak
import numpy as np
import os
import time
import argparse
import glob
import yaml
from pathlib import Path
from dst_awkward.dst_io import DSTFile
from dst_awkward.dst_reader import BankReader

# --- Constants ---
# Hardcoded Marker IDs (no schema needed)
START_BANKID = 1400000023
STOP_BANKID  = 1400000101

class DSTProcessor:
    def __init__(self, get_banks=None, all_banks=True, verbose=True):
        """
        Args:
            get_banks (list): List of bank names (strings) to retrieve.
            all_banks (bool): If True, ignores get_banks and retrieves all known schemas.
            verbose (bool): Print setup info.
        """
        self.verbose = verbose
        self.all_banks = all_banks
        # Convert get_banks to a set for faster lookup
        self.get_banks = set(get_banks) if get_banks else set()
        
        self.readers = {}     # Map: bank_id -> BankReader object
        self.bank_names = {}  # Map: bank_id -> bank_name (str)
        self.got_banks = set() # Track what we actually find in the file

        # 1. Load Schemas Dynamically from 'schemas/*.yml'
        self._discover_schemas()

        # 2. Register Special Marker Banks (No Schema required)
        self._register_marker(START_BANKID, "start")
        self._register_marker(STOP_BANKID, "stop")

    def _discover_schemas(self):
        """Scans the 'schemas' subdirectory for YAML schema files."""
        current_dir = Path(__file__).parent
        schemas_dir = current_dir / "schemas"
        
        if self.verbose:
            print(f"Searching for schemas in: {schemas_dir}")

        # Look for both .yml and .yaml
        schema_files = list(schemas_dir.glob("*.yml")) + list(schemas_dir.glob("*.yaml"))
        
        for schema_path in schema_files:
            try:
                stem_name = schema_path.stem
                
                # Filter: If we only want specific banks, skip unrequested ones
                if not self.all_banks and stem_name not in self.get_banks:
                    continue

                # Parse YAML to get bankId
                with open(schema_path, 'r') as f:
                    schema_data = yaml.safe_load(f)
                    bank_id = schema_data.get('bank_id')
                
                if bank_id:
                    # Initialize Reader (assuming BankReader accepts the name stem)
                    # NOTE: BankReader needs to know to look in 'schemas' and parse yaml now too.
                    # If BankReader is hardcoded for JSON, you might need to pass the dict directly 
                    # or update BankReader. Assuming BankReader can handle the name lookup:
                    self.readers[bank_id] = BankReader(stem_name) 
                    self.bank_names[bank_id] = stem_name
                    
                    if self.verbose:
                        print(f"  [+] Loaded schema: {stem_name} (ID: {bank_id})")
                else:
                    if self.verbose:
                        print(f"  [!] Skipping {stem_name}: No 'bankId' found in YAML.")
                
            except Exception as e:
                print(f"  [!] Failed to load schema {schema_path}: {e}")

    def _register_marker(self, bank_id, name):
        """Registers a bank that has no payload/schema (Header only)."""
        if self.all_banks or name in self.get_banks:
            self.bank_names[bank_id] = name
            self.readers[bank_id] = None # Marker -> No Reader needed
            if self.verbose:
                print(f"  [+] Registered marker: {name} (ID: {bank_id})")

    def process_file(self, filename, limit=None):
        """Reads DST file and yields Events (dicts of banks)."""
        current_event = {}
        event_count = 0
        
        # Open the DST file
        with DSTFile(filename) as dst:
            for bank_id, ver, raw_bytes in dst.banks():
                
                # 1. Filter: Do we know/want this bank?
                if bank_id not in self.bank_names:
                    continue

                name = self.bank_names[bank_id]
                reader = self.readers[bank_id]
                
                self.got_banks.add(name)

                # 2. Event Boundary Check
                # DST files are sequential. If we see a bank that is already 
                # in the current buffer, the previous event is complete.
                if name in current_event:
                    yield current_event
                    current_event = {}
                    event_count += 1
                    if limit and event_count >= limit:
                        return

                # 3. Parse Data
                try:
                    if reader is None:
                        # Marker Bank (Start/Stop)
                        data = {"active": True, "_version": ver}
                    else:
                        # Standard Bank
                        data, _ = reader.parse_buffer(raw_bytes)
                        data['_version'] = ver 

                    current_event[name] = data
                    
                except Exception as e:
                    print(f"Error parsing bank {name} (ID {bank_id}): {e}")

            # Yield the final event sitting in the buffer
            if current_event:
                yield current_event

def main():
    parser = argparse.ArgumentParser(description="Convert DST file to Parquet/Awkward.")
    parser.add_argument("input_file", help="Path to input .dst or .dst.gz file")
    parser.add_argument("--limit", type=int, default=None, help="Max events to process")
    parser.add_argument("--banks", type=str, default=None, 
                        help="Comma-separated list of banks to read. If omitted, reads ALL.")
    
    args = parser.parse_args()

    # Determine Output Filename
    input_path = Path(args.input_file)
    output_parquet = input_path.with_name(
        input_path.name.replace('.dst', '').replace('.gz', '') + '.parquet'
    )

    # Configure Bank Selection
    get_banks_list = []
    all_banks_flag = True
    
    if args.banks:
        get_banks_list = [b.strip() for b in args.banks.split(',')]
        all_banks_flag = False
        print(f"Configuration: Reading specific banks: {get_banks_list}")
    else:
        print("Configuration: Reading ALL available banks.")

    # Initialize Processor
    processor = DSTProcessor(get_banks=get_banks_list, all_banks=all_banks_flag)
    
    # Run Processing
    print(f"\nProcessing {input_path}...")
    t0 = time.time()
    
    event_list = []
    try:
        for ev in processor.process_file(str(input_path), limit=args.limit):
            event_list.append(ev)
    except KeyboardInterrupt:
        print("\nInterrupted! Saving current buffer...")
    
    t1 = time.time()
    
    print(f"\nExtraction complete.")
    print(f"Events processed: {len(event_list)}")
    print(f"Time elapsed: {t1-t0:.2f}s")
    print(f"Banks found (got_banks): {sorted(list(processor.got_banks))}")

    if not event_list:
        print("No events found or selected.")
        return

    # Convert to Awkward and Save
    print("Building Awkward Array...")
    events_ak = ak.Array(event_list)
    print(f"Array Type: {events_ak.type}")
    
    print(f"Saving to {output_parquet}...")
    ak.to_parquet(events_ak, str(output_parquet))
    print("Success.")

if __name__ == "__main__":
    main()