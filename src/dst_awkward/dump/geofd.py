import numpy as np

def dump_geofd(data, short=True, bank_name="GEOFD"):
    """
    Replicates the exact output format of geofd_dst.c
    
    Args:
        data: awkward Record or dictionary containing the geofd bank data.
        short (bool): If True, shows only core geometry data.
                If False, includes detailed V3 extension information.
        bank_name (str): "GEOFD", "GEOBR", "GEOLR", "GEOMD", or "GEOTL" to set the header.
    """
    
    # Site ID mapping (from C code)
    SITE_MAP = {
        0: "GEOBR",
        1: "GEOLR",
        2: "GEOMD",
        3: "GEOTL"
    }
    
    siteid = int(data['siteid'])
    header_name = SITE_MAP.get(siteid, "GEOFD")
    
    print(f"\n{header_name} bank (geometry information for FD)\n")
    
    # Print uniqID with date if available
    uniqID = int(data['uniqID'])
    print(f"uniq ID: {uniqID}")
    if uniqID != 0:
        # TODO: Would need convertSec2DateLine function for date conversion
        # For now just print the ID
        pass
    print()
    
    # Print site location
    print(f"Central Laser Facility lat, lon, alt      : {np.degrees(data['latitude']):12.8f} {np.degrees(data['longitude']):12.8f} {data['altitude']:12.8f}")
    print(f"Site origin lat, lon, WGS84alt                 : {np.degrees(data['latitude']):12.8f} {np.degrees(data['longitude']):12.8f} {data['altitude']:12.8f}\n")
    
    # Print site vectors (Earth-centered)
    print(f"CLF location (rel. to center of earth in meters)         : {data['vclf'][0]:9.1f} {data['vclf'][1]:9.1f} {data['vclf'][2]:9.1f}")
    print(f"Site origin location (rel. to center of earth in meters) : {data['vsite'][0]:9.1f} {data['vsite'][1]:9.1f} {data['vsite'][2]:9.1f}")
    print(f"Position of site relative to CLF (meters)                : {data['local_vsite'][0]:9.3f} {data['local_vsite'][1]:9.3f} {data['local_vsite'][2]:9.3f}\n")
    
    # Print rotation matrices
    print("Site to Earth rotation matrix")
    for i in range(3):
        print(f"  {data['site2earth'][i][0]:9.6f} {data['site2earth'][i][1]:9.6f} {data['site2earth'][i][2]:9.6f}")
    print()
    
    print("Site to CLF rotation matrix")
    for i in range(3):
        print(f"  {data['site2clf'][i][0]:9.6f} {data['site2clf'][i][1]:9.6f} {data['site2clf'][i][2]:9.6f}")
    print()
    
    # Print camera dimensions
    print(f"Camera dimensions (meters)                      : {data['cam_width']:8.4f} {data['cam_height']:8.4f} {data['cam_depth']:8.4f}")
    print(f"PMT flat-to-flat distance (meters)              : {data['pmt_flat2flat']:8.4f}")
    print(f"PMT point-to-point distance (meters)            : {data['pmt_point2point']:8.4f}")
    print(f"Mirror segment flat-to-flat distance (meters)   : {data['seg_flat2flat']:8.4f}")
    print(f"Mirror segment point-to-point distance (meters) : {data['seg_point2point']:8.4f}")
    print(f"Mirror diameter (meters)                        : {data['diameter_old']:8.4f}\n")
    
    # Print mirror segments (legacy 18 segments)
    print("Unit vectors to mirror segments (from center of curvature, z-axis along mirror axis)")
    for i in range(18):
        print(f"  Segment {i:2d} : {data['vseg_old'][i][0]:9.6f} {data['vseg_old'][i][1]:9.6f} {data['vseg_old'][i][2]:9.6f}")
    
    # Print mirror locations (legacy 12 mirrors)
    print("\nMirror locations relative to site origin (meters)")
    for i in range(12):
        print(f"  Mirror {i:2d} : {data['local_vmir_old'][i][0]:8.4f} {data['local_vmir_old'][i][1]:8.4f} {data['local_vmir_old'][i][2]:8.4f}")
    
    print("\nCamera locations relative to site origin: not used")
    for i in range(12):
        print(f"  Mirror {i:2d} : {data['local_vcam_old'][i][0]:8.4f} {data['local_vcam_old'][i][1]:8.4f} {data['local_vcam_old'][i][2]:8.4f}")
    
    # Print mirror GPS coordinates
    print("\nMirror locations (lat, lon, alt)")
    for i in range(12):
        print(f"  Mirror {i:2d} : {np.degrees(data['mir_lat_old'][i]):12.8f} {np.degrees(data['mir_lon_old'][i]):12.8f} {data['mir_alt_old'][i]:8.4f}")
    
    # Print camera properties
    print("\nCamera radii of curvature, mirror-camera separation")
    for i in range(12):
        print(f"  Mirror {i:2d} : R {data['rcurve_old'][i]:8.4f} S {data['sep_old'][i]:8.4f}")
    
    # Print mirror pointing directions
    print("\nMirror pointing directions (site coordinate system)")
    for i in range(12):
        zen = np.degrees(data['mir_the_old'][i])
        azm = np.degrees(data['mir_phi_old'][i])
        ring = int(data['ring_old'][i])
        print(f"  Mirror {i:2d} : {data['vmir_old'][i][0]:12.9f} {data['vmir_old'][i][1]:12.9f} {data['vmir_old'][i][2]:12.9f} [ zen {zen:9.6f} azm {azm:11.6f} : ring {ring}]")
    
    # Print site-to-camera rotation matrices
    print("\nSite to Camera rotation matrices")
    for i in range(12):
        print(f"  Site to Camera {i:2d} :")
        for j in range(3):
            print(f"    {data['site2cam_old'][i][j][0]:9.6f} {data['site2cam_old'][i][j][1]:9.6f} {data['site2cam_old'][i][j][2]:9.6f}")
        print()
    
    # Print tube locations
    print("Tube locations on camera / pointing directions")
    for i in range(12):
        for j in range(256):
            print(f"  Camera {i:2d} Tube {j:3d} : x {data['xtube_old'][j]:8.4f} y {data['ytube_old'][j]:8.4f}   {data['vtube_old'][i][j][0]:9.6f} {data['vtube_old'][i][j][1]:9.6f} {data['vtube_old'][i][j][2]:9.6f}")
    
    if not short:
        print("\n\nNow for GEOFD v3 extensions.\n")
        
        nmir = int(data['nmir'])
        print(f"Number of mirrors: {nmir}")
        print("Camera type, diameter (m), mir-cam separation, number of segments per mirror:")
        for i in range(nmir):
            camtype = int(data['camtype'][i])
            diameter = data['diameters'][i]
            nseg = int(data['nseg'][i])
            print(f"  Camera {i:2d} : {camtype} {diameter:f} {nseg}")
        print()
        
        # Mirror GPS locations (V3)
        print("Mirror GPS lat/lon/alt(WGS84 m) (axis+sphere intersection):")
        for i in range(nmir):
            print(f"  Mirror {i:2d} : {np.degrees(data['mir_lat'][i]):12.8f} {np.degrees(data['mir_lon'][i]):12.8f} {data['mir_alt'][i]:8.3f}")
        print()
        
        # V3 Mirror locations
        print("Mirror locations relative to site origin (m):")
        for i in range(nmir):
            print(f"  Mirror {i:2d} : {data['local_vmir'][i][0]:8.4f} {data['local_vmir'][i][1]:8.4f} {data['local_vmir'][i][2]:8.4f}")
        
        print("\nCamera locations relative to site origin: not used")
        for i in range(nmir):
            print(f"  Mirror {i:2d} : {data['local_vcam'][i][0]:8.4f} {data['local_vcam'][i][1]:8.4f} {data['local_vcam'][i][2]:8.4f}")
        
        # V3 Mirror segments (jagged)
        print("\nMirror segments: curvature center offsets (x y z), radius, spot size (deg):")
        for i in range(nmir):
            nseg_i = int(data['nseg'][i])
            for j in range(nseg_i):
                print(f"  M {i:2d} S {j:2d} : {data['vseg'][i][j][0]:f} {data['vseg'][i][j][1]:f} {data['vseg'][i][j][2]:f}")
        print()
        
        # V3 Tube locations
        print("Tube3 locations on camera / pointing directions")
        for i in range(nmir):
            for j in range(256):
                elev = np.degrees(np.arcsin(data['vtube'][i][j][2]))
                azm = np.degrees(np.arctan2(data['vtube'][i][j][1], data['vtube'][i][j][0]))
                print(f"  Camera {i:2d} Tube {j:3d} : x {data['xtube_old'][j]:8.4f} y {data['ytube_old'][j]:8.4f}   {data['vtube'][i][j][0]:9.6f} {data['vtube'][i][j][1]:9.6f} {data['vtube'][i][j][2]:9.6f} (elev {elev:.2f}  az {azm:.2f} ccwe)")
        
        print("\n\nFurther quantities defined in GEOFD version 3 will be added here soon.\n\n")
