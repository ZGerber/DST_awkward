import awkward as ak
import numpy as np
import os
import argparse
import datetime
from dst_awkward.dst_io import DSTFile
from dst_awkward.dst_reader import BankReader

# --- Constants ---
R2D = 180.0 / np.pi
D2R = np.pi / 180.0
BR = 0
LR = 1
TL = 2

BANK_MAP = {
    12093: "fdplane",
    12103: "brplane",
    12203: "lrplane",
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
    """Simple converter for Julian date to YMD (approximate or using datetime)."""
    # Julian Day 2440000.5 is 1968-05-23 00:00:00
    # Unix Epoch (1970-01-01) is Julian 2440587.5
    try:
        dt = datetime.datetime(2000, 1, 1) # dummy, handled by relative delta usually
        # Standard astro julian date conversion
        # J = julian + 0.5
        # This function matches the typical C 'caldat' behavior expected in DST
        # Note: This is a placeholder. If precise historical dates are needed, 
        # use an astronomy library. For "dump" consistency, this is usually sufficient.
        
        # Simplified algorithm:
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

def fdplane_dump(fdplane: ak.Record, bank_name="fdplane", long_output=1):
    # Determine Site Name for header
    site_str = "FDPLANE"
    if fdplane.siteid == BR:
        site_str = "BRPLANE"
    elif fdplane.siteid == LR:
        site_str = "LRPLANE"
    elif fdplane.siteid == TL:
        site_str = "TLPLANE"
    elif bank_name == "brplane":
        site_str = "BRPLANE"
    elif bank_name == "lrplane":
        site_str = "LRPLANE"

    print(f"\n\n{site_str} bank (TA plane- and time-fit data for {site_str.replace('PLANE','')} FD)")
    
    if fdplane.uniqID != -1:
        print(f"Processed with GEOFD UniqID {fdplane.uniqID} and filter mode {fdplane.fmode}.\n")

    # Time Calculation
    hr = fdplane.jsecond // 3600 + 12
    jd = fdplane.julian
    if hr >= 24:
        jd += 1
        hr -= 24
    
    yr, mo, day = caldat(jd)
    minute = (fdplane.jsecond // 60) % 60
    sec = fdplane.jsecond % 60
    nano = fdplane.jsecfrac

    print(f"{yr:4d}/{mo:02d}/{day:02d} {hr:02d}:{minute:02d}:{sec:02d}.{nano:09d} | Part {fdplane.part:6d} Event {fdplane.event_num:6d}\t", end="")

    if fdplane.type == 2:
        print("downward-going event")
    elif fdplane.type == 3:
        print("  upward-going event")
    elif fdplane.type == 4:
        print("       in-time event")
    elif fdplane.type == 5:
        print("         noise event")
    else:
        print(f"         type {fdplane.type}")

    print(f"Run start     : {fdplane.julian:9d} {fdplane.jsecond:5d}.{fdplane.jsecfrac:09d}")
    print(f"Event Start   : {fdplane.second:5d}.{fdplane.secfrac:09d}")
    print(f"Number of tubes                   : {fdplane.ntube:4d}")
    print(f"Number of tubes in fit            : {fdplane.ngtube:4d}")
    
    seed_idx = fdplane.seed
    # Handle bounds check for seed printing
    if 0 <= seed_idx < len(fdplane.camera):
        print(f"Seed for track                    : {fdplane.seed:4d} [ cam {fdplane.camera[seed_idx]:2d} tube {fdplane.tube[seed_idx]:3d} ]\n")
    else:
        print(f"Seed for track                    : {fdplane.seed:4d} [ cam ?? tube ?? ]\n")

    print(f"Norm vector to SDP ( chi2 = {fdplane.sdp_chi2:7.4f} )")
    print(f"  nx        {fdplane.sdp_n[0]:9.6f} +/- {fdplane.sdp_en[0]:9.6f}")
    print(f"  ny        {fdplane.sdp_n[1]:9.6f} +/- {fdplane.sdp_en[1]:9.6f}")
    print(f"  nz        {fdplane.sdp_n[2]:9.6f} +/- {fdplane.sdp_en[2]:9.6f}")
    
    print(f"covariance: {fdplane.sdp_n_cov[0][0]:9.6f} {fdplane.sdp_n_cov[0][1]:9.6f} {fdplane.sdp_n_cov[0][2]:9.6f}")
    print(f"            {fdplane.sdp_n_cov[1][0]:9.6f} {fdplane.sdp_n_cov[1][1]:9.6f} {fdplane.sdp_n_cov[1][2]:9.6f}")
    print(f"            {fdplane.sdp_n_cov[2][0]:9.6f} {fdplane.sdp_n_cov[2][1]:9.6f} {fdplane.sdp_n_cov[2][2]:9.6f}\n")
    
    print(f"  SDP theta, phi: {fdplane.sdp_the * R2D:f} {fdplane.sdp_phi * R2D:f}")
    print(f"Angular extent (degrees)           : {R2D * fdplane.azm_extent:7.4f}")
    print(f"Duration (ns)                      : {fdplane.time_extent:7.4f}")
    print(f"Shower zenith, azimuth             : {R2D * fdplane.shower_zen:7.4f} {R2D * fdplane.shower_azm:7.4f}")
    print(f"Shower axis vector                 : {fdplane.shower_axis[0]:9.6f} {fdplane.shower_axis[1]:9.6f} {fdplane.shower_axis[2]:9.6f}")
    print(f"Rp unit vector                     : {fdplane.rpuv[0]:9.6f} {fdplane.rpuv[1]:9.6f} {fdplane.rpuv[2]:9.6f}")
    print(f"Shower core location (site coords) : {fdplane.core[0]:9.2f} {fdplane.core[1]:9.2f} {fdplane.core[2]:9.2f}\n")

    print(f"time-fit status [linear][pseudotangent][tangent] = {fdplane.status:03d}\n")

    # Linear Fit
    print(f"Linear fit : {'GOOD' if (fdplane.status // 100) else 'BAD'}")
    print(f"Linear fit         ( chi2 = {fdplane.linefit_chi2:7.4f} )")
    print(f"int   : {fdplane.linefit_int:10.3f} +/- {fdplane.linefit_eint:10.3f} (ns)")
    print(f"slope : {D2R * fdplane.linefit_slope:10.3f} +/- {D2R * fdplane.linefit_eslope:10.3f} (ns/deg)")
    print(f"covariance: {fdplane.linefit_cov[0][0]:10.3f} {fdplane.linefit_cov[0][1]:10.3f}")
    print(f"            {fdplane.linefit_cov[1][0]:10.3f} {fdplane.linefit_cov[1][1]:10.3f}\n")

    # Pseudo-Tangent Fit
    print(f"Pseudo-tangent fit : {'GOOD' if ((fdplane.status % 100) // 10) else 'BAD'}")
    print(f"Pseudo-tangent fit ( chi2 = {fdplane.ptanfit_chi2:7.4f} )")
    print(f"Rp    : {fdplane.ptanfit_rp:10.3f} +/- {fdplane.ptanfit_erp:10.3f} (m)")
    print(f"T0    : {fdplane.ptanfit_t0:10.3f} +/- {fdplane.ptanfit_et0:10.3f} (ns)")
    print(f"covariance: {fdplane.ptanfit_cov[0][0]:10.3f} {fdplane.ptanfit_cov[0][1]:10.3f}")
    print(f"            {fdplane.ptanfit_cov[1][0]:10.3f} {fdplane.ptanfit_cov[1][1]:10.3f}\n")

    # Tangent Fit
    print(f"Tangent fit : {'GOOD' if (fdplane.status % 10) else 'BAD'}")
    print(f"Tangent fit        ( chi2 = {fdplane.tanfit_chi2:7.4f} )")
    print(f"Rp    : {fdplane.rp:10.3f} +/- {fdplane.erp:10.3f} (m)")
    print(f"Psi   : {R2D * fdplane.psi:10.3f} +/- {R2D * fdplane.epsi:10.3f} (degrees)")
    print(f"T0    : {fdplane.t0:10.3f} +/- {fdplane.et0:10.3f} (ns)")
    print(f"covariance: {fdplane.tanfit_cov[0][0]:10.3f} {fdplane.tanfit_cov[0][1]:10.3f} {fdplane.tanfit_cov[0][2]:10.3f}")
    print(f"            {fdplane.tanfit_cov[1][0]:10.3f} {fdplane.tanfit_cov[1][1]:10.3f} {fdplane.tanfit_cov[1][2]:10.3f}")
    print(f"            {fdplane.tanfit_cov[2][0]:10.3f} {fdplane.tanfit_cov[2][1]:10.3f} {fdplane.tanfit_cov[2][2]:10.3f}\n")

    # Tube Info
    if long_output == 1:
        print("Time-ordered tube information:")
        print("indx                            npe       time       trms      alt      azm     palt     pazm        res         chi2   sigma knex    qual  it0 it1")
        
        # Sort by time
        sorted_indices = np.argsort(fdplane.time)
        
        for idx in sorted_indices:
            tube_qual = fdplane.tube_qual[idx]
            qual_str = f"{tube_qual:6d}  " if tube_qual == 1 else f"-{-tube_qual:05d}  "
            
            print(f"{idx:4d} [ cam {fdplane.camera[idx]:2d} tube {fdplane.tube[idx]:3d} ] "
                  f"{fdplane.npe[idx]:10.3f} {fdplane.time[idx]:10.3f} {fdplane.time_rms[idx]:10.3f} "
                  f"{R2D * fdplane.alt[idx]:8.3f} {R2D * fdplane.azm[idx]:8.3f} "
                  f"{R2D * fdplane.plane_alt[idx]:8.3f} {R2D * fdplane.plane_azm[idx]:8.3f} "
                  f"{fdplane.tanfit_res[idx]:10.3f} {fdplane.tanfit_tchi2[idx]:12.3f} "
                  f"{fdplane.sigma[idx]:7.3f}    {fdplane.knex_qual[idx]:d}  "
                  f"{qual_str}{fdplane.it0[idx]:3d} {fdplane.it1[idx]:3d}")
    else:
        print("Tube information not displayed in short output")

    print("\n\n")

def main():
    parser = argparse.ArgumentParser(description="Process a DST file and print fdplane/brplane/lrplane dump.")
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
    print("\n--- FD/BR/LR Plane Dump (from Awkward Array) ---")
    for i, event in enumerate(events_ak):
        print(f"\nEvent {i}:")
        
        for bank_name in ['fdplane', 'brplane', 'lrplane']:
            if bank_name in event.fields:
                fdplane_dump(event[bank_name], bank_name=bank_name)

if __name__ == "__main__":
    main()