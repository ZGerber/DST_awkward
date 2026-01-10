def dump_rusdmc1(data, short=False):
    """
    Replicates the exact output format of rusdmc1_dst.c
    
    Args:
        data: awkward Record or dictionary containing the rusdmc1 bank data.
        short (bool): If True, short form (omits tdistbr, tdistlr, tdistsk).
                      If False, long form (includes all fields).
    """
    
    print("rusdmc1 :")
    if short:
        print(f"xcore {data['xcore']} ycore {data['ycore']} t0 {data['t0']} bdist {data['bdist']} tdist {data['tdist']}")
    else:
        print(f"xcore {data['xcore']} ycore {data['ycore']} t0 {data['t0']} bdist {data['bdist']} tdistbr {data['tdistbr']} tdistlr {data['tdistlr']} tdistsk {data['tdistsk']} tdist {data['tdist']}")