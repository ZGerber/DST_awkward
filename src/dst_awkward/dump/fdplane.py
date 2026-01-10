import datetime
import numpy as np

def caldat(julian):
    # Convert Julian day to Gregorian date
    # Julian day 0 is -4712-01-01, but adjust for the C code
    # The C code uses caldat from Fortran, which is 1-based Julian
    date = datetime.date.fromordinal(julian - 1721425)
    return date.month, date.day, date.year

def dump_fdplane(data, short=False, bank_name="FDPLANE"):
    """
    Replicates the exact output format of fdplane_dst.c
    
    Args:
        data: awkward Record or dictionary containing the fdplane bank data.
        short (bool): If True, excludes detailed tube information.
                If False, includes detailed tube information.
        bank_name (str): "FDPLANE", "BRPLANE", "LRPLANE", or "TLPLANE" to set the header.
    """
    
    print(f"\n\n{bank_name} bank (TA plane- and time-fit data for FD)\n")
    
    if data['uniqID'] != -1:
        print(f"Processed with GEOFD UniqID {data['uniqID']} and filter mode {data['fmode']}.\n\n")
    
    # Date/time calculation
    hr = data['jsecond'] // 3600 + 12
    if hr >= 24:
        mo, day, yr = caldat(data['julian'] + 1)
        hr -= 24
    else:
        mo, day, yr = caldat(data['julian'])
    
    min_ = (data['jsecond'] // 60) % 60
    sec = data['jsecond'] % 60
    nano = data['jsecfrac']
    
    print(f"{yr:4d}/{mo:02d}/{day:02d} {hr:02d}:{min_:02d}:{sec:02d}.{nano:09d} | Part {data['part']:6d} Event {data['event_num']:6d}\t", end='')
    
    if data['type'] == 2:
        print("downward-going event")
    elif data['type'] == 3:
        print("  upward-going event")
    elif data['type'] == 4:
        print("       in-time event")
    elif data['type'] == 5:
        print("         noise event")
    
    print(f"Run start     : {data['julian']:9d} {data['jsecond']:5d}.{data['jsecfrac']:09d}")
    print(f"Event Start   : {data['second']:5d}.{data['secfrac']:09d}")
    print(f"Number of tubes                   : {data['ntube']:4d}")
    print(f"Number of tubes in fit            : {data['ngtube']:4d}")
    print(f"Seed for track                    : {data['seed']:4d} [ cam {data['camera'][data['seed']]:2d} tube {data['tube'][data['seed']]:3d} ]\n\n")
    
    print(f"Norm vector to SDP ( chi2 = {data['sdp_chi2']:7.4f} )")
    print(f"  nx        {data['sdp_n'][0]:9.6f} +/- {data['sdp_en'][0]:9.6f}")
    print(f"  ny        {data['sdp_n'][1]:9.6f} +/- {data['sdp_en'][1]:9.6f}")
    print(f"  nz        {data['sdp_n'][2]:9.6f} +/- {data['sdp_en'][2]:9.6f}")
    print(f"covariance: {data['sdp_n_cov'][0][0]:9.6f} {data['sdp_n_cov'][0][1]:9.6f} {data['sdp_n_cov'][0][2]:9.6f}")
    print(f"            {data['sdp_n_cov'][1][0]:9.6f} {data['sdp_n_cov'][1][1]:9.6f} {data['sdp_n_cov'][1][2]:9.6f}")
    print(f"            {data['sdp_n_cov'][2][0]:9.6f} {data['sdp_n_cov'][2][1]:9.6f} {data['sdp_n_cov'][2][2]:9.6f}\n")
    print(f"  SDP theta, phi: {np.degrees(data['sdp_the']):.6f} {data['sdp_phi'] * 180.0 / np.pi:.6f}")

    print(f"Angular extent (degrees)           : {np.degrees(data['azm_extent']):7.4f}")
    print(f"Duration (ns)                      : {data['time_extent']:7.4f}")
    print(f"Shower zenith, azimuth             : {np.degrees(data['shower_zen']):7.4f} {np.degrees(data['shower_azm']):7.4f}")
    print(f"Shower axis vector                 : {data['shower_axis'][0]:9.6f} {data['shower_axis'][1]:9.6f} {data['shower_axis'][2]:9.6f}")
    print(f"Rp unit vector                     : {data['rpuv'][0]:9.6f} {data['rpuv'][1]:9.6f} {data['rpuv'][2]:9.6f}")
    print(f"Shower core location (site coords) : {data['core'][0]:9.2f} {data['core'][1]:9.2f} {data['core'][2]:9.2f}\n\n")
    
    print(f"time-fit status [linear][pseudotangent][tangent] = {data['status']:03d}\n\n")
    
    print("Linear fit : ", end='')
    if (data['status'] // 100):
        print("GOOD")
    else:
        print("BAD")
    print(f"Linear fit         ( chi2 = {data['linefit_chi2']:7.4f} )")
    print(f"int   : {data['linefit_int']:10.3f} +/- {data['linefit_eint']:10.3f} (ns)")
    print(f"slope : {np.radians(data['linefit_slope']):10.3f} +/- {np.radians(data['linefit_eslope']):10.3f} (ns/deg)")
    print(f"covariance: {data['linefit_cov'][0][0]:10.3f} {data['linefit_cov'][0][1]:10.3f}")
    print(f"            {data['linefit_cov'][1][0]:10.3f} {data['linefit_cov'][1][1]:10.3f}\n\n")
    
    print("Pseudo-tangent fit : ", end='')
    if ((data['status'] % 100) // 10):
        print("GOOD")
    else:
        print("BAD")
    print(f"Pseudo-tangent fit ( chi2 = {data['ptanfit_chi2']:7.4f} )")
    print(f"Rp    : {data['ptanfit_rp']:10.3f} +/- {data['ptanfit_erp']:10.3f} (m)")
    print(f"T0    : {data['ptanfit_t0']:10.3f} +/- {data['ptanfit_et0']:10.3f} (ns)")
    print(f"covariance: {data['ptanfit_cov'][0][0]:10.3f} {data['ptanfit_cov'][0][1]:10.3f}")
    print(f"            {data['ptanfit_cov'][1][0]:10.3f} {data['ptanfit_cov'][1][1]:10.3f}\n\n")
    
    print("Tangent fit : ", end='')
    if (data['status'] % 10):
        print("GOOD")
    else:
        print("BAD")
    print(f"Tangent fit        ( chi2 = {data['tanfit_chi2']:7.4f} )")
    print(f"Rp    : {data['rp']:10.3f} +/- {data['erp']:10.3f} (m)")
    print(f"Psi   : {np.degrees(data['psi']):10.3f} +/- {np.degrees(data['epsi']):10.3f} (degrees)")
    print(f"T0    : {data['t0']:10.3f} +/- {data['et0']:10.3f} (ns)")
    print(f"covariance: {data['tanfit_cov'][0][0]:10.3f} {data['tanfit_cov'][0][1]:10.3f} {data['tanfit_cov'][0][2]:10.3f}")
    print(f"            {data['tanfit_cov'][1][0]:10.3f} {data['tanfit_cov'][1][1]:10.3f} {data['tanfit_cov'][1][2]:10.3f}")
    print(f"            {data['tanfit_cov'][2][0]:10.3f} {data['tanfit_cov'][2][1]:10.3f} {data['tanfit_cov'][2][2]:10.3f}\n\n")
    
    # Tube info
    if not short:
        t = np.argsort(data['time'])
        
        print("Time-ordered tube information:")
        print("indx                            npe       time       trms      alt      azm     palt     pazm        res         chi2   sigma knex    qual  it0 it1")
        for i in range(data['ntube']):
            idx = t[i]
            print(f"{idx:4d} [ cam {data['camera'][idx]:2d} tube {data['tube'][idx]:3d} ] {data['npe'][idx]:10.3f} {data['time'][idx]:10.3f} {data['time_rms'][idx]:10.3f} {np.degrees(data['alt'][idx]):8.3f} {np.degrees(data['azm'][idx]):8.3f} {np.degrees(data['plane_alt'][idx]):8.3f} {np.degrees(data['plane_azm'][idx]):8.3f} {data['tanfit_res'][idx]:10.3f} {data['tanfit_tchi2'][idx]:12.3f} {data['sigma'][idx]:7.3f}    {data['knex_qual'][idx]:d}  ", end='')
            if data['tube_qual'][idx] == 1:
                print(f"{data['tube_qual'][idx]:6d}  ", end='')
            else:
                print(f"-{ -data['tube_qual'][idx]:05d}  ", end='')
            print(f"{data['it0'][idx]:3d} {data['it1'][idx]:3d}")
    else:
        print("Tube information not displayed in short output\n")
    
    print("\n\n")