from .geofd import dump_geofd

def dump_geolr(data, short=True):
    """Wrapper for dump_geofd with GEOLR bank name."""
    dump_geofd(data, short, bank_name="GEOLR")
