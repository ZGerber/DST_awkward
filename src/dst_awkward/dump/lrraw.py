from .fdraw import dump_fdraw

def dump_lrraw(data, short=False):
    dump_fdraw(data, short, "LRRAW")