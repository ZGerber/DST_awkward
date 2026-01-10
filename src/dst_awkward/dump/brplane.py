from .fdplane import dump_fdplane

def dump_brplane(data, short=False):
    dump_fdplane(data, short, bank_name="BRPLANE")