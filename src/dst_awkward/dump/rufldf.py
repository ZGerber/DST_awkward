def dump_rufldf(data, short=False):
    """
    Replicates the exact output format of rufldf_dst.c
    
    Args:
        data: awkward Record or dictionary containing the rufldf bank data.
        short (bool): Ignored, as rufldf dump is the same for short/long.
    """
    
    print("rufldf :")
    
    # First line for index 0
    print(f"xcore[0] {data['xcore'][0]:.2f} dxcore[0] {data['dxcore'][0]:.2f} ycore[0] {data['ycore'][0]:.2f} dycore[0] {data['dycore'][0]:.2f} s800[0] {data['s800'][0]:.2f} energy[0] {data['energy'][0]:.2f} atmcor[0]: {data['atmcor'][0]:.2f} chi2[0] {data['chi2'][0]:.2f} ndof[0] {data['ndof'][0]}")
    
    # Second line for index 1
    print(f"xcore[1] {data['xcore'][1]:.2f} dxcore[1] {data['dxcore'][1]:.2f} ycore[1] {data['ycore'][1]:.2f} dycore[1] {data['dycore'][1]:.2f} s800[1] {data['s800'][1]:.2f} energy[1] {data['energy'][1]:.2f} atmcor[1]: {data['atmcor'][1]:.2f} chi2[1] {data['chi2'][1]:.2f} ndof[1] {data['ndof'][1]}")
    
    # Third line for scalars
    print(f"theta {data['theta']:.2f} dtheta {data['dtheta']:.2f} phi {data['phi']:.2f} dphi {data['dphi']:.2f} t0 {data['t0']:.2f} dt0 {data['dt0']:.2f}")