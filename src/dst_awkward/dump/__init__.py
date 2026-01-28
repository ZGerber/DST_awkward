# src/dst_awkward/dump/__init__.py
import importlib
# import sys
# from matplotlib.pylab import short

# Registry of dump functions: { 'bank_name': function }
_DUMP_REGISTRY = {}

def default_dump(name, data, short=False):
    """Fallback dump if no specific function is found."""
    
    # 1. Access version directly (Awkward Records use data['field'], not .get())
    # We catch KeyError just in case, but assume it exists.
    try:
        ver = data['_version']
    except (KeyError, AttributeError, TypeError):
        ver = '?'
        
    if short:
        # Mimic short dump: just existence or header info
        print(f"Bank: {name} (ver: {ver}) - [Generic Short Dump]")
    else:
        # Mimic long dump: print structure
        print(f"Bank: {name} (ver: {ver})")

        # 2. Convert Awkward Record to Python Dict to iterate fields
        # (Awkward Records do not support .items() natively)
        try:
            content = data.to_list()
        except AttributeError:
            content = data

        if isinstance(content, dict):
            for key, val in content.items():
                if key == '_version': continue
                print(f"  {key}: {val}")
        else:
            print(f"  {content}")

def get_dump(bank_name):
    """
    Tries to import a module named dst_awkward.dump.<bank_name>
    and retrieve a function named 'dump_<bank_name>'.
    Returns default_dump if not found.
    """
    if bank_name in _DUMP_REGISTRY:
        return _DUMP_REGISTRY[bank_name]

    # Dynamic import attempt
    try:
        module_name = f"dst_awkward.dump.{bank_name}"
        mod = importlib.import_module(module_name)
        func_name = f"dump_{bank_name}"
        
        if hasattr(mod, func_name):
            func = getattr(mod, func_name)
            _DUMP_REGISTRY[bank_name] = func
            return func
    except ImportError:
        pass

    return lambda d, short=False: default_dump(bank_name, d, short)