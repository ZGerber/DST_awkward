import datetime
import numpy as np
import awkward as ak

def caldat(julian):
    # Convert Julian day to Gregorian date
    # Julian day 0 is -4712-01-01, but adjust for the C code
    # The C code uses caldat from Fortran, which is 1-based Julian
    date = datetime.date.fromordinal(julian - 1721425)
    return date.month, date.day, date.year

def asstring(awkward_int_array):
    # Convert byte array to string, stripping null bytes
    return ak.to_numpy(awkward_int_array).astype(np.uint8).tobytes().decode('ascii').rstrip('\x00')

def dump_hyp1(data, short=True, bank_name="HYP1"):
    """
    Replicates the exact output format of hyp1_dst.c
    
    Args:
        data: awkward Record or dictionary containing the hyp1 bank data.
        short (bool): If True, shows only summary fit data.
                If False, includes detailed SD and FD hit information.
        bank_name (str): "HYP1", "BRHYP1", "LRHYP1", or "MDHYP1" to set the header.
    """
    
    print(f"{bank_name} Bank")
    
    # Extract date/time components
    year = data['yymmdd'] // int(1e4)
    month = (data['yymmdd'] // 100) % 100
    day = data['yymmdd'] % 100
    hour = data['hhmmss'] // int(1e4)
    minute = (data['hhmmss'] // 100) % 100
    second = data['hhmmss'] % 100
    
    print(f"Timestamp: {data['julian']} -- {month:02d}/{day:02d}/{year:02d} -- {hour:02d}:{minute:02d}:{second:02d}.{int(data['tref']):09d}")
    print(f"FD/SD offset: {data['offset']:f} ns")
    
    # Print fit information
    nfit = data['nfit']
    for i in range(nfit):
        print(f"\nFIT: {asstring(data['fitType'][i])}")
        print(f"x_c, y_c = {data['xcore'][i]/1000:7.5g}, {data['ycore'][i]/1000:7.5g} [km North/East of CLF]")
        print(f"zen, azm = {np.degrees(data['zen'][i]):7.5g}, {np.degrees(data['azm'][i]):7.5g} [degrees]")
        print(f"tc = {data['tc'][i]/1000:7.5g} [microsec after timestamp]")
        print(f"rp, psi = {data['rp'][i]/1000:7.5g} km, {np.degrees(data['psi'][i]):7.5g} deg")
        print(f"t0 = {data['t0'][i]/1000:7.5g} usec")
        
        nhits = data['nhits']
        ngtube = data['ngtube']
        
        if i == 0:
            dof = nhits + ngtube - 3
        else:
            dof = nhits + ngtube - 5
        
        print(f"chi2 / dof = {data['chi2'][i]:7.5g} / ({nhits} + {ngtube} - {3 if i==0 else 5})")
        print(f"           = {data['chi2'][i]/dof:7.5g}")
        
        print("chi2 components:")
        print(f"{'SDP':>11s} {'COC':>11s} {'FDTiming':>11s} {'SDTiming':>11s}")
        
        print(f"{data['chi2Comp'][i][2]:11.3e} {data['chi2Comp'][i][3]:11.3e} {data['chi2Comp'][i][0]:11.3e} {data['chi2Comp'][i][1]:11.3e}")
    
    if not short:
        for i in range(nfit):
            print(f"FIT: {asstring(data['fitType'][i])}")
            print(f"sd hits: {data['nhits']}")
            
            print(f"{'sdPlaneAlt':>13s} {'sdPlaneAzm':>13s} {'rho':>13s} {'sdTime':>13s} {'sdTimeSigma':>13s} {'sdResidual':>13s} {'sdpos X':>13s} {'sdpos Y':>13s}")
            
            for j in range(data['nhits']):
                print(f"{np.degrees(data['sdPlaneAlt'][i][j]):13.5f} "
                      f"{np.degrees(data['sdPlaneAzm'][i][j]):13.5f} "
                      f"{data['rho'][j]:13.5f} "
                      f"{data['sdTime'][i][j]/1000:13.5f} "
                      f"{data['sdTimeSigma'][i][j]/1000:13.5f} "
                      f"{data['sdResidual'][i][j]:13.5f} "
                      f"{data['xyz'][j][0]/1000:13.5f} "
                      f"{data['xyz'][j][1]/1000:13.5f}")
            
            print(f"\nfd tubes: {data['ngtube']}")
            print(f"{'planeAlt':>13s} {'planeAzm':>13s} {'npe':>13s} {'fdTime':>13s} {'fdTimeRMS':>13s} {'fdResidual':>13s} {'tubeVector X':>13s} {'tubeVector Y':>13s} {'tubeVector Z':>13s}")
            
            for j in range(data['ngtube']):
                print(f"{np.degrees(data['planeAlt'][i][j]):13.5f} "
                      f"{np.degrees(data['planeAzm'][i][j]):13.5f} "
                      f"{data['npe'][j]:13.5f} "
                      f"{data['fdTime'][j]/1000:13.5f} "
                      f"{data['fdTimeRMS'][j]/1000:13.5f} "
                      f"{data['fdResidual'][i][j]:13.5f} "
                      f"{data['tubeVector'][j][0]:13.5f} "
                      f"{data['tubeVector'][j][1]:13.5f} "
                      f"{data['tubeVector'][j][2]:13.5f}")
