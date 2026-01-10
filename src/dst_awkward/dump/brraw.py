from .fdraw import dump_fdraw

def dump_brraw(data, short=False):
    dump_fdraw(data, short, "BRRAW")