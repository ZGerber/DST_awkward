import awkward as ak


def dump_rusdraw(data, short=False):
    """
    Replicates the exact output format of rusdraw_dst.c
    
    Args:
        data: awkward Record or dictionary containing the rusdraw bank data.
        short (bool): If True, suppresses FADC trace output (matches C long_output=0).
                      If False, includes FADC traces (matches C long_output=1).
    """
    data = ak.to_list(data) if hasattr(data, "to_list") else data
    
    # --- Helper: Site String Conversion ---
    site_map = {
        0: "BR",
        1: "LR",
        2: "SK",
        3: "BRLR",
        4: "BRSK",
        5: "LRSK",
        6: "BRLRSK"
    }
    site_val = data['site']
    site_str = site_map.get(site_val, str(site_val))

    # --- Helper: Date/Time Formatting ---
    yymmdd = data['yymmdd']
    hhmmss = data['hhmmss']
    
    yr = yymmdd // 10000
    mo = (yymmdd // 100) % 100
    day = yymmdd % 100
    
    hr = hhmmss // 10000
    min_ = (hhmmss // 100) % 100
    sec = hhmmss % 100
    usec = data['usec']

    # --- Header Output ---
    print("rusdraw :")
    print(f"event_num {data['event_num']} event_code {data['event_code']} site {site_str} ", end='')
    
    # Check if we need to finish the previous line or if the C code puts a newline
    # C code: ...fprintf(fp,"%d ",rusdraw_.site); ... fprintf(fp,"run_id: ...");
    # The C code does NOT put a newline after the site switch statement.
    
    run_id = data['run_id']
    trig_id = data['trig_id']
    
    print(f"run_id: BR={run_id[0]} LR={run_id[1]} SK={run_id[2]} "
          f"trig_id: BR={trig_id[0]} LR={trig_id[1]} SK={trig_id[2]}")
    
    print(f"errcode {data['errcode']} "
          f"date {mo:02d}/{day:02d}/{yr:02d} {hr:02d}:{min_:02d}:{sec:02d}.{usec:06d} "
          f"nofwf {data['nofwf']} monyymmdd {data['monyymmdd']:06d} monhhmmss {data['monhhmmss']:06d}")

    # --- Waveform Table Header ---
    # Both short and long modes print this header
    print("wf# wf_id  X   Y    clkcnt     mclkcnt   fadcti(lower,upper)  fadcav      pchmip        pchped      nfadcpermip     mftchi2      mftndof")

    nofwf = data['nofwf']
    if nofwf == 0:
        return

    # --- Waveform Loop ---
    for i in range(nofwf):
        # Extract fields for this specific waveform index
        wf_id = data['wf_id'][i]
        xxyy = data['xxyy'][i]
        xy0 = xxyy // 100
        xy1 = xxyy % 100
        
        clkcnt = data['clkcnt'][i]
        mclkcnt = data['mclkcnt'][i]
        
        # Arrays with [2] dimensions (lower/upper)
        # In awkward, data['fadcti'][i] returns [val_lower, val_upper]
        fadcti = data['fadcti'][i]
        fadcav = data['fadcav'][i]
        pchmip = data['pchmip'][i]
        pchped = data['pchped'][i]
        mip    = data['mip'][i]
        mftchi2 = data['mftchi2'][i]
        mftndof = data['mftndof'][i]

        # Print the Summary Line
        # Format string from C:
        # "%02d %5.02d %4d %3d %10d %10d %8d %8d %5d %4d %6d %7d %5d %5d %8.1f %6.1f %6.1f %6.1f %5d %4d\n"
        # wf_id uses %5.02d: width 5, zero-padded to 2 digits
        wf_id_str = f"{wf_id:02d}"
        print(f"{i:02d} {wf_id_str:>5} {xy0:4d} {xy1:3d} {clkcnt:10d} {mclkcnt:10d} "
              f"{fadcti[0]:8d} {fadcti[1]:8d} {fadcav[0]:5d} {fadcav[1]:4d} "
              f"{pchmip[0]:6d} {pchmip[1]:7d} {pchped[0]:5d} {pchped[1]:5d} "
              f"{mip[0]:8.1f} {mip[1]:6.1f} {mftchi2[0]:6.1f} {mftchi2[1]:6.1f} "
              f"{mftndof[0]:5d} {mftndof[1]:4d}")

        # If Long Mode (not short), print FADC traces
        if not short:
            # data['fadc'] is shape (nofwf, 2, 128)
            # traces = [lower_trace_array, upper_trace_array]
            traces = data['fadc'][i]
            
            # --- Lower FADC ---
            print("lower fadc")
            _print_fadc_block(traces[0])
            print()  # newline after lower block (C: fprintf "\nupper fadc\n")
            
            # --- Upper FADC ---
            print("upper fadc")
            _print_fadc_block(traces[1])
            
            print() # Trailing newline after the upper block for this WF


def _print_fadc_block(trace):
    """
    Helper to print 128 FADC values with the specific 
    12-column wrapping logic from rusdraw_dst.c
    """
    k = 0
    # C code loops 0 to 127
    for j in range(128):
        if k == 12:
            print() # newline
            k = 0
        
        # "%6d "
        print(f"{trace[j]:6d} ", end='')
        k += 1
    # Note: C code doesn't print a final newline inside the loop logic 
    # unless k hits 12 exactly at the end (which 128 doesn't, 128 = 10*12 + 8).
    # But C code usually has a print("\n") *after* the block loop?
    # Checking C:
    #   ... loop 128 ...
    #   fprintf(fp,"\nupper fadc\n"); 
    # The newline is printed implicitly by the start of the next section label
    # or the explicit fprintf(fp, "\n") at the end of the WF loop.
    # However, inside the block, if the last line isn't full, it leaves the cursor hanging.
    # The next print ("upper fadc") starts with \n.
    # So we don't need to force a newline here, but Python print usually adds one.
    # Since we used end='', we are hanging.
    # The calling function handles the section headers.