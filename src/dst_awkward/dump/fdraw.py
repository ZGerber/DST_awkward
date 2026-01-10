import datetime

def caldat(julian):
    # Convert Julian day to Gregorian date
    # Julian day 0 is -4712-01-01, but adjust for the C code
    # The C code uses caldat from Fortran, which is 1-based Julian
    date = datetime.date.fromordinal(julian - 1721425)
    return date.month, date.day, date.year

def dump_fdraw(data, short=False, bank_name="FDRAW"):
    """
    Replicates the exact output format of fdraw_dst.c
    
    Args:
        data: awkward Record or dictionary containing the fdraw bank data.
        short (bool): If True, suppresses detailed tube and waveform output.
        bank_name (str): "FDRAW", "BRRAW", or "LRRAW" to set the header.
    """
    
    # Determine siteid from bank_name
    if bank_name == "BRRAW":
        siteid = 0
        site_str = "BLACK_ROCK"
    elif bank_name == "LRRAW":
        siteid = 1
        site_str = "LONG_RIDGE"
    else:
        siteid = 2
        site_str = "UNDEFINED"
    
    print(f"{bank_name} :")
    
    # Date/time calculation
    hr = data['jsecond'] // 3600 + 12
    if hr >= 24:
        mo, day, yr = caldat(data['julian'] + 1)
        hr -= 24
    else:
        mo, day, yr = caldat(data['julian'])
    
    min_ = (data['jsecond'] // 60) % 60
    sec = data['jsecond'] % 60
    
    if (data['ctd_version'] >> 30) & 0x1 == 1:
        nano = int(1e9 * data['ctdclock'] / data['gps1pps_tick'])
    else:
        nano = (data['ctdclock'] - data['gps1pps_tick']) * 25
    
    print(f"  {site_str} site:  part {data['part']:02d}  event_code: {data['event_code']}")
    print(f"  firmware: CTD ver {data['ctd_version']}  TF ver {data['tf_version']}  SDF ver {data['sdf_version']}")
    print(f"  trigger {data['event_num']:6d}  {mo}/{day:02d}/{yr:4d}  {hr:02d}:{min_:02d}:{sec:02d}.{nano:09d}")
    print(f"  gps tick: {data['gps1pps_tick']:9d}  ctdclock: {data['ctdclock']:9d}")
    print(f"  number of participating cameras: {data['num_mir']:2d}")
    
    num_mir = data['num_mir']
    for i in range(num_mir):
        hr_i = data['second'][i] // 3600
        min_i = (data['second'][i] // 60) % 60
        sec_i = data['second'][i] % 60
        
        print(f"  camera {data['mir_num'][i]:2d}  code: {data['trig_code'][i]:1d}  store time: {hr_i:02d}:{min_i:02d}:{sec_i:02d}.{data['microsec'][i]:06d}  clkcnt: {data['clkcnt'][i]:9d}")
        print(f"    tf mode: {data['tf_mode'][i]}  mode2: {data['tf_mode2'][i]}")
        print(f"    have waveform data for {data['num_chan'][i]:3d} tubes")
        print(f"      hit_pt: (last entry={data['hit_pt'][i][256]})")
        
        # Print hit_pt in 16 columns
        for j in range(257):  # fdraw_nchan_mir + 1 = 256 + 1 = 257?
            if j % 16 == 0:
                print("        ", end='')
            print(f" {data['hit_pt'][i][j]}", end='')
            if (j + 1) % 16 == 0:
                print()
        
        if not short:
            num_chan_i = data['num_chan'][i]
            for j in range(num_chan_i):
                # Waveform sums for visualization
                wf = [0] * 64
                for k in range(512):  # fdraw_nt_chan_max = 512?
                    wf[k // 8] += data['m_fadc'][i][j][k]
                
                minwf = min(wf)
                maxwf = max(wf)
                
                print(f"    cam {data['mir_num'][i]:2d} tube {data['channel'][i][j]:3d}:  peak: {data['sdf_peak'][i][j]}  tmphit: {data['sdf_tmphit'][i][j]}")
                print(f"      mode: {data['sdf_mode'][i][j]}  ctrl: {data['sdf_ctrl'][i][j]}  thre: {data['sdf_thre'][i][j]}")
                mean = data['mean'][i][j]
                disp = data['disp'][i][j]
                print(f"      mean: {mean[0]:5d} {mean[1]:5d} {mean[2]:5d} {mean[3]:5d}  disp: {disp[0]:5d} {disp[1]:5d} {disp[2]:5d} {disp[3]:5d}")
                
                print("      waveform data:")
                for k in range(512):
                    if k % 12 == 0:
                        print("       ", end='')
                    print(f" {data['m_fadc'][i][j][k]:04X}", end='')
                    if (k + 1) % 12 == 0:
                        print()
                if 512 % 12 != 0:
                    print()
                
                print(f"visualization (8-bin sums): c {data['mir_num'][i]:02d} t {data['channel'][i][j]:03d}: min {minwf:6d} max {maxwf:6d}")
                for row in range(3, -1, -1):
                    thresh1 = int(minwf + (row / 4.0) * (maxwf - minwf))
                    thresh2 = int(minwf + ((row + 0.5) / 4.0) * (maxwf - minwf))
                    print("   ", end='')
                    for k in range(64):
                        if wf[k] >= thresh2:
                            print(":", end='')
                        elif wf[k] >= thresh1:
                            print(".", end='')
                        else:
                            print(" ", end='')
                    print()