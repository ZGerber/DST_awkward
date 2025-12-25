import awkward as ak
import numpy as np
import os
import argparse
from dst_awkward.dst_io import DSTFile
from dst_awkward.dst_reader import BankReader

# --- CONSTANTS ---
RADDEG = 57.2957795131

BANK_MAP = {
    13105: "rusdmc",
    13106: "rusdmc1",
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

def rusdmc_dump(rusdmc: ak.Record):
    print("rusdmc :")
    print(f"Event Number: {rusdmc.event_num}")
    print(f"Corsika Particle ID: {rusdmc.parttype}")
    print(f"Total Energy of Primary Particle: {rusdmc.energy:g} EeV")
    print(f"Height of First Interaction: {rusdmc.height/1.e5:g} km")
    print(f"Zenith Angle of Primary Particle Direction: {rusdmc.theta*RADDEG:g} Degrees")
    print(f"Azimuth Angle of Primary Particle Direction: {rusdmc.phi*RADDEG:g} Degrees (N of E)")
    print(f"Counter ID Number for Counter Closest to Core: {rusdmc.corecounter}")
    
    # Core XYZ conversion cm -> m
    cx = rusdmc.corexyz[0] / 100.0
    cy = rusdmc.corexyz[1] / 100.0
    cz = rusdmc.corexyz[2] / 100.0
    print(f"Position of the core in CLF reference frame: ({cx:g},{cy:g},{cz:g}) m")
    
    print(f"Time of shower front passing through core position: {rusdmc.tc} x 20 nsec")

def rusdmc1_dump(rusdmc1: ak.Record, long_output=1):
    print("rusdmc1 :")
    
    if long_output == 0:
        print(f"xcore {rusdmc1.xcore:f} ycore {rusdmc1.ycore:f} t0 {rusdmc1.t0:f} "
              f"bdist {rusdmc1.bdist:f} tdist {rusdmc1.tdist:f}")
    else:
        print(f"xcore {rusdmc1.xcore:f} ycore {rusdmc1.ycore:f} t0 {rusdmc1.t0:f} "
              f"bdist {rusdmc1.bdist:f} tdistbr {rusdmc1.tdistbr:f} "
              f"tdistlr {rusdmc1.tdistlr:f} tdistsk {rusdmc1.tdistsk:f} "
              f"tdist {rusdmc1.tdist:f}")

def main():
    parser = argparse.ArgumentParser(description="Process a DST file and print rusdmc/rusdmc1 dumps.")
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
    print("\n--- Rusdmc/Rusdmc1 Dump (from Awkward Array) ---")
    for i, event in enumerate(events_ak):
        print(f"\nEvent {i}:")
        
        # Check and dump rusdmc
        if 'rusdmc' in event.fields:
            rusdmc_dump(event['rusdmc'])
        else:
            print("  (No rusdmc bank found)")

        # Check and dump rusdmc1
        if 'rusdmc1' in event.fields:
            rusdmc1_dump(event['rusdmc1'])
        else:
            print("  (No rusdmc1 bank found)")

if __name__ == "__main__":
    main()