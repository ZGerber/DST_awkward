import numpy as np

def dump_showlib(data, short=False):
    """
    Replicates the exact output format of showlib_dst.c
    
    Args:
        data: awkward Record or dictionary containing the showlib bank data.
        short (bool): Ignored, as showlib dump is the same for short/long.
    """
    
    # Determine particle string
    particle_code = data['particle']
    if particle_code == 14:
        particle = "prot"
    elif particle_code == 5626:
        particle = "iron"
    else:
        particle = f"{particle_code:4d}"
    
    # Determine model string
    code_mod10 = data['code'] % 10
    if code_mod10 == 0:
        model = " QGSJet 01"
    elif code_mod10 == 1:
        model = "SIBYLL 1.6"
    elif code_mod10 == 2:
        model = "SIBYLL 2.1"
    else:
        model = f"   MODEL {code_mod10}"
    
    # Convert angle to degrees
    angle_deg = np.degrees(data['angle'])
    
    print(f"AZShower {data['code']:6d} {data['number']:3d}: {particle:4s} {model:9s} {angle_deg:2.0f}")
    print(f"         energy: {data['energy']/1e9:6.2f} EeV, first int: {data['first']:6.1f} g/cm2")
    print(f"         nMx: {data['nmax']:10.3e} x0: {data['x0']:6.1f} g/cm2 xMx: {data['xmax']:6.1f} g/cm2 lam: {data['lambda']:4.1f} g/cm2")