import numpy as np

def dump_rusdmc(data, short=False):
    """
    Replicates the exact output format of rusdmc_dst.c
    
    Args:
        data: awkward Record or dictionary containing the rusdmc bank data.
        short (bool): Ignored, as rusdmc dump is the same for short/long.
    """
    
    print("rusdmc :")
    print(f"Event Number: {data['event_num']}")
    print(f"Corsika Particle ID: {data['parttype']}")
    print(f"Total Energy of Primary Particle: {data['energy']} EeV")
    print(f"Height of First Interaction: {data['height'] / 1e5} km")
    print(f"Zenith Angle of Primary Particle Direction: {np.degrees(data['theta'])} Degrees")
    print(f"Azimuth Angle of Primary Particle Direction: {np.degrees(data['phi'])} Degrees (N of E)")
    print(f"Counter ID Number for Counter Closest to Core: {data['corecounter']}")
    corexyz = data['corexyz']
    print(f"Position of the core in CLF reference frame: ({corexyz[0]/100.},{corexyz[1]/100.},{corexyz[2]/100.}) m")
    print(f"Time of shower front passing through core position: {data['tc']} x 20 nsec")