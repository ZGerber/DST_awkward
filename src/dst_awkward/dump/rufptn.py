def dump_rufptn(data, short=False):
    """
    Replicates the exact output format of rufptn_dst.c
    
    Args:
        data: awkward Record or dictionary containing the rufptn bank data.
        short (bool): Ignored, as rufptn dump is the same for short/long.
    """
    
    # Constants from rufptn_dst.h
    RUFPTN_ORIGIN_X_CLF = -12.2435
    RUFPTN_ORIGIN_Y_CLF = -16.4406
    RUFPTN_TIMDIST = 0.249827048333
    
    print("rufptn :")
    
    # Compute core_x, core_y, t0
    core_x = data['tyro_xymoments'][2][0] + RUFPTN_ORIGIN_X_CLF
    core_y = data['tyro_xymoments'][2][1] + RUFPTN_ORIGIN_Y_CLF
    t0 = 0.5 * (data['tearliest'][0] + data['tearliest'][1]) + data['tyro_tfitpars'][2][0] / RUFPTN_TIMDIST * 1e-6
    
    print(f"nhits {data['nhits']} nsclust {data['nsclust']} nstclust {data['nstclust']} nborder {data['nborder']} core_x {core_x} core_y {core_y} t0 {t0:.9f} ")
    
    print("#      XXYY       Q upper        Q lower          T upper            T lower isgood")
    
    nhits = data['nhits']
    for i in range(nhits):
        xxyy = data['xxyy'][i]
        pulsa_upper = data['pulsa'][i][0]
        pulsa_lower = data['pulsa'][i][1]
        t_upper = data['tearliest'][0] + (4.0028e-6) * data['reltime'][i][0]
        t_lower = data['tearliest'][1] + (4.0028e-6) * data['reltime'][i][1]
        isgood = data['isgood'][i]
        
        print(f"{i:02d}{xxyy:7d}{pulsa_upper:15f}{pulsa_lower:15f}{t_upper:22.9f}{t_lower:18.9f}{isgood:7d}")