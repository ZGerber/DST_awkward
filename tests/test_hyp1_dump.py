import awkward as ak
import numpy as np
import os
import argparse
from dst_awkward.dst_io import DSTFile
from dst_awkward.dst_reader import BankReader

# --- Constants ---
M_PI = np.pi
R2D = 180.0 / M_PI

BANK_MAP = {
    13300: "hyp1",
    13301: "brhyp1",
    13302: "lrhyp1",
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
        current_event = None
        event_count = 0
        # Standard DST Markers
        START_BANKID = 1400000023
        STOP_BANKID  = 1400000101
        
        with DSTFile(filename) as dst:
            for bank_id, ver, raw_bytes in dst.banks():
                if bank_id == START_BANKID:
                    current_event = {}
                    continue
                elif bank_id == STOP_BANKID:
                    if current_event is not None:
                        yield current_event
                        event_count += 1
                        current_event = None
                        if limit and event_count >= limit:
                            return
                    continue

                if current_event is not None:
                    if bank_id in self.readers:
                        name = self.bank_names[bank_id]
                        reader = self.readers[bank_id]
                        try:
                            data, _ = reader.parse_buffer(raw_bytes)
                            data['_version'] = ver 
                            current_event[name] = data
                        except Exception as e:
                            print(f"Error parsing bank {name} (ID {bank_id}, ver {ver}): {e}")

def dump_hyp1(hyp1, bank_name, long_output=True):
    # Mimicking the C output structure
    # Original C code reference: hyp1_dst.c
    # Function: dump_hyp1(FILE *fp, HYP1 *hyp1, integer4 *long_output)

    # Print Bank Header
    # Determine bank type based on site ID
    # FD Site IDs (example values, replace with actual if known)
    BR = 1
    LR = 2
    MD = 3

    # Simulate the bank name based on fdsiteid
    if hyp1.fdsiteid == BR:
        bank_str = "BRHYP1"
    elif hyp1.fdsiteid == LR:
        bank_str = "LRHYP1"
    elif hyp1.fdsiteid == MD:
        bank_str = "MDHYP1"
    else:
        bank_str = "HYP1"

    print(f"{bank_str} Bank")
    year = hyp1.yymmdd // 10000
    month = (hyp1.yymmdd // 100) % 100
    day = hyp1.yymmdd % 100
    hour = hyp1.hhmmss // 10000
    minute = (hyp1.hhmmss // 100) % 100
    second = hyp1.hhmmss % 100
    print(f"Timestamp: {hyp1.julian} -- {year:02d}/{month:02d}/{day:02d} -- "
          f"{hour:02d}:{minute:02d}:{second:02d}.{int(hyp1.tref):09d}")
    print(f"FD/SD offset: {hyp1.offset} ns")

    for i in range(hyp1.nfit):
        fit_type_chars = hyp1.fitType[i]
        fit_type_str = "".join([chr(c) for c in fit_type_chars if c != 0])
        print()
        print(f"FIT: {fit_type_str}")
        print(f"x_c, y_c = {hyp1.xcore[i]/1000:7.5g}, {hyp1.ycore[i]/1000:7.5g} [km North/East of CLF]")
        print(f"zen, azm = {np.degrees(hyp1.zen[i]):7.5g}, {np.degrees(hyp1.azm[i]):7.5g} [degrees]")
        print(f"tc = {hyp1.tc[i]/1000:7.5g} [microsec after timestamp]")
        print(f"rp, psi = {hyp1.rp[i]/1000:7.5g} km, {np.degrees(hyp1.psi[i]):7.5g} deg")
        print(f"t0 = {hyp1.t0[i]/1000:7.5g} usec")

        if i==0:
            print(f"chi2 / dof = {hyp1.chi2[i]:7.5g} / ({hyp1.nhits:d} + {hyp1.ngtube:d} - 3)")
            print(f"           = {hyp1.chi2[i]/(hyp1.nhits + hyp1.ngtube - 3):7.5g}")
        else:
            print(f"chi2 / dof = {hyp1.chi2[i]:7.5g} / ({hyp1.nhits:d} + {hyp1.ngtube:d} - 5)")
            print(f"           = {hyp1.chi2[i]/(hyp1.nhits + hyp1.ngtube - 5):7.5g}")

        print("chi2 components:")
        print(f"{'SDP':>11s} {'COC':>11s} {'FDTiming':>11s} {'SDTiming':>11s}")
        print(f"{hyp1.chi2Comp[i][2]:>11.3e} {hyp1.chi2Comp[i][3]:>11.3e} {hyp1.chi2Comp[i][0]:>11.3e} {hyp1.chi2Comp[i][1]:>11.3e}")

    if long_output:
        for i in range(hyp1.nfit):
            fit_type_chars = hyp1.fitType[i]
            fit_type_str = "".join([chr(c) for c in fit_type_chars if c != 0])
            print(f"FIT: {fit_type_str}")
            print(f"sd hits: {hyp1.nhits}")
            print(f"{'sdPlaneAlt':>13s} {'sdPlaneAzm':>13s} {'rho':>13s} "
                  f"{'sdTime':>13s} {'sdTimeSigma':>13s} {'sdResidual':>13s} "
                  f"{'sdpos X':>13s} {'sdpos Y':>13s}")
            for j in range(hyp1.nhits):
                print(f"{np.degrees(hyp1.sdPlaneAlt[i][j]):13.5f} {np.degrees(hyp1.sdPlaneAzm[i][j]):13.5f} {hyp1.rho[j]:13.5f} "
                      f"{hyp1.sdTime[i][j]/1000.:13.5f} {hyp1.sdTimeSigma[i][j]/1000.:13.5f} "
                      f"{hyp1.sdResidual[i][j]:13.5f} {hyp1.xyz[j][0]/1000.:13.5f} {hyp1.xyz[j][1]/1000.:13.5f}")

            print()
            print(f"fd tubes: {hyp1.ngtube}")
            print(f"{'planeAlt':>13s} {'planeAzm':>13s} {'npe':>13s} "
                  f"{'fdTime':>13s} {'fdTimeRMS':>13s} {'fdResidual':>13s} "
                  f"{'tubeVector X':>13s} {'tubeVector Y':>13s} {'tubeVector Z':>13s}")
            for j in range(hyp1.ngtube):
                print(f"{np.degrees(hyp1.planeAlt[i][j]):13.5f} {np.degrees(hyp1.planeAzm[i][j]):13.5f} {hyp1.npe[j]:13.5f} "
                      f"{hyp1.fdTime[j]/1000.:13.5f} {hyp1.fdTimeRMS[j]/1000.:13.5f} "
                      f"{hyp1.fdResidual[i][j]:13.5f} {hyp1.tubeVector[j][0]:13.5f} {hyp1.tubeVector[j][1]:13.5f} {hyp1.tubeVector[j][2]:13.5f}")

def main():
    parser = argparse.ArgumentParser(description="Process a DST file and print hyp1/brhyp1/lrhyp1 dump.")
    parser.add_argument("input_file", help="Path to the input DST file (.dst or .dst.gz)")
    args = parser.parse_args()
    
    input_file = args.input_file
    
    if not os.path.exists(input_file):
        print(f"Error: Input file '{input_file}' not found.")
        return

    reader = DSTReader()
    
    event_list = []
    for ev in reader.process_file(input_file, limit=None):
        event_list.append(ev)        

    events_ak = ak.Array(event_list)

    print("\n--- HYP1 Dump (from Awkward Array) ---")
    for i, event in enumerate(events_ak):
        print(f"\nEvent {i}:")
        for bank_name in ["hyp1", "brhyp1", "lrhyp1"]:
            if bank_name in event.fields:
                bank_data = event[bank_name]
                if bank_data is not None:
                    dump_hyp1(bank_data, bank_name)

if __name__ == "__main__":
    main()