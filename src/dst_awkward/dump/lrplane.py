from .fdplane import dump_fdplane

def dump_lrplane(data, short=False):
    dump_fdplane(data, short, bank_name="LRPLANE")