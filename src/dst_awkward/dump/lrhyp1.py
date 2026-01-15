from .hyp1 import dump_hyp1

def dump_lrhyp1(data, short=True):
    """Wrapper for dump_hyp1 with LRHYP1 bank name."""
    dump_hyp1(data, short, bank_name="LRHYP1")
