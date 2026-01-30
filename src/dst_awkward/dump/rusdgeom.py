import awkward as ak


def dump_rusdgeom(data, short=False):
    """
    Replicates the exact output format of rusdgeom_dst.c
    
    Args:
        data: awkward Record or dictionary containing the rusdgeom bank data.
        short (bool): If True, short form (omits signal-level details).
                      If False, long form (includes signal-level details).
    """
    data = ak.to_list(data) if hasattr(data, "to_list") else data
    
    print("rusdgeom :")
    print(f"nsds={data['nsds']} tearliest={data['tearliest']:.2f}")
    
    # Plane fit (index 0)
    print(f"Plane fit  xcore={data['xcore'][0]:.2f}+/-{data['dxcore'][0]:.2f} ycore={data['ycore'][0]:.2f}+/-{data['dycore'][0]:.2f} t0={data['t0'][0]:.2f}+/-{data['dt0'][0]:.2f} theta={data['theta'][0]:.2f}+/-{data['dtheta'][0]:.2f} phi={data['phi'][0]:.2f}+/-{data['dphi'][0]:.2f} chi2={data['chi2'][0]:.2f} ndof={data['ndof'][0]}")
    
    # Modified Linsley fit (index 1)
    print(f"Modified Linsley fit  xcore={data['xcore'][1]:.2f}+/-{data['dxcore'][1]:.2f} ycore={data['ycore'][1]:.2f}+/-{data['dycore'][1]:.2f} t0={data['t0'][1]:.2f}+/-{data['dt0'][1]:.2f} theta={data['theta'][1]:.2f}+/-{data['dtheta'][1]:.2f} phi={data['phi'][1]:.2f}+/-{data['dphi'][1]:.2f} chi2={data['chi2'][1]:.2f} ndof={data['ndof'][1]}")
    
    # Mod. Lin. fit w curv. (index 2)
    print(f"Mod. Lin. fit w curv.  xcore={data['xcore'][2]:.2f}+/-{data['dxcore'][2]:.2f} ycore={data['ycore'][2]:.2f}+/-{data['dycore'][2]:.2f} t0={data['t0'][2]:.2f}+/-{data['dt0'][2]:.2f} theta={data['theta'][2]:.2f}+/-{data['dtheta'][2]:.2f} phi={data['phi'][2]:.2f}+/-{data['dphi'][2]:.2f} a={data['a']:.2f}+/-{data['da']:.2f} chi2={data['chi2'][2]:.2f} ndof={data['ndof'][2]}")
    
    # Short table header
    print(f"{"index":s}{"xxyy":>8s}{"pulsa,[VEM]":>18s}{"sdtime,[1200m]":>17s}{"sdterr,[1200m]":>16s}{"sdirufptn":>10s}{"igsd":>8s}")
    
    nsds = data['nsds']
    for i in range(nsds):
        # Short table - xxyy uses %10.04d format (zero-padded 4 digits in width 10)
        xxyy = f"{data['xxyy'][i]:04d}"
        print(f"{i:3d}{xxyy:>10}{data['pulsa'][i]:15f}{data['sdtime'][i]:15f}{data['sdterr'][i]:15f}{data['sdirufptn'][i]:11d}{data['igsd'][i]:12d}")
    
    if not short:
        print()
        # Long table header
        print(f"{"index":s}{"xxyy":>8s}{"sdsigq,[VEM]":>18s}{"sdsigt,[1200m]":>17s}{"sdsigte,[1200m]":>16s}{"sdirufptn":>10s}{"igsig":>8s}")
        for i in range(nsds):
            nsig_i = data['nsig'][i]
            xxyy = f"{data['xxyy'][i]:04d}"
            for j in range(nsig_i):
                # Long table - xxyy uses %10.04d format (zero-padded 4 digits in width 10)
                print(f"{i:3d}{xxyy:>10}{data['sdsigq'][i][j]:15f}{data['sdsigt'][i][j]:15f}{data['sdsigte'][i][j]:15f}{data['irufptn'][i][j]:11d}{data['igsig'][i][j]:12d}")