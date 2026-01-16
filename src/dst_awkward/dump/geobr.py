from .geofd import dump_geofd

def dump_geobr(data, short=True):
    """Wrapper for dump_geofd with GEOBR bank name."""
    dump_geofd(data, short, bank_name="GEOBR")
