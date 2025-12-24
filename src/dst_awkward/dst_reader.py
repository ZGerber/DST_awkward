import numpy as np
import awkward as ak
from importlib import resources
import yaml
import struct

def load_schema(bank_name: str):
    with resources.files('dst_awkward.schemas').joinpath(f'{bank_name}.yaml').open('r') as f:
        return yaml.safe_load(f)

class BankReader:
    def __init__(self, bank_name: str):
        # Load the schema using the bank name (e.g., "fraw1" -> loads fraw1.yaml)
        self.schema = load_schema(bank_name)
        
        # Map YAML types to Numpy dtypes (assuming Little Endian '<')
        endian = self.schema.get('endian', '<')
        self.dtypes = {
            'int8':  np.dtype(f'{endian}i1'),
            'int16': np.dtype(f'{endian}i2'),
            'int32': np.dtype(f'{endian}i4'),
            'float32': np.dtype(f'{endian}f4'),
            'float64': np.dtype(f'{endian}f8'),
        }

    def parse_buffer(self, buffer, start_offset=8):
        """Parses a bytes object (a single bank) into a Dictionary of Awkward Arrays."""
        # Start at 8 to skip [BankID (4b), BankVersion (4b)]
        # unless overridden by the user.
        cursor = start_offset

        ctx = {} 
        results = {}

        for field in self.schema['layout']:
            f_type = field.get('type')

            # --- Case 1: Standard Primitive Field ---
            if f_type not in ['interleaved_sequence', 'bulk_jagged', 'interleaved_jagged', 'interleaved_mixed']:
                dtype = self.dtypes[f_type]
                name = field['name']
                
                count = 1
                if 'shape' in field:
                    for dim in field['shape']:
                        if isinstance(dim, str):
                            count *= ctx[dim]
                        else:
                            count *= dim
                
                n_bytes = count * dtype.itemsize
                data = np.frombuffer(buffer, dtype=dtype, count=count, offset=cursor)
                cursor += n_bytes

                if 'shape' not in field:
                    val = data[0]
                    ctx[name] = val
                    results[name] = val
                else:
                    results[name] = ak.Array(data)
                    ctx[name] = data 

            # --- Case 2: Interleaved Sequence (Fixed or Global Dynamic) ---
            elif f_type == 'interleaved_sequence':
                if isinstance(field['count'], str):
                    loop_count = ctx[field['count']]
                else:
                    loop_count = field['count']
                
                # Determine sizes per iteration
                if 'size_ref' in field:
                    # FRAW1 Style: All items depend on a global size array
                    sizes = ctx[field['size_ref']]
                else:
                    # RUFPTN/RUSDRAW Style: Items have fixed shapes
                    sizes = None 

                temp_storage = {sub['name']: [] for sub in field['items']}
                
                for i in range(loop_count):
                    # If sizes is None, we default to 1, but item['shape'] overrides it below
                    current_size_base = sizes[i] if sizes is not None else 1
                    
                    for sub_field in field['items']:
                        dtype = self.dtypes[sub_field['type']]
                        
                        # Calculate specific item count
                        item_count = current_size_base
                        fixed_dims = [] # Keep track of dimensions

                        if 'shape' in sub_field:
                            for dim in sub_field['shape']:
                                fixed_dims.append(dim)
                                item_count *= dim 
                        
                        n_bytes = item_count * dtype.itemsize
                        data = np.frombuffer(buffer, dtype=dtype, count=item_count, offset=cursor)
                        cursor += n_bytes

                        if fixed_dims:
                            # If we have a dynamic base size (like 'nl1'), that is the first dimension
                            shape = tuple(fixed_dims)
                            if current_size_base > 1:
                                shape = (current_size_base,) + shape
                            
                            # Reshape in place
                            data = data.reshape(shape)

                        temp_storage[sub_field['name']].append(data)

                for k, v in temp_storage.items():
                    results[k] = ak.Array(v)

            # --- Case 3: Bulk Jagged ---
            elif f_type == 'bulk_jagged':
                dtype = self.dtypes[field['dtype']]
                outer_counts = ctx[field['outer_counts']]
                
                item_shape = field.get('item_shape', [])
                items_per_row = 1
                for dim in item_shape: items_per_row *= dim

                if 'inner_counts' in field:
                    inner_counts = ctx[field['inner_counts']]
                    flat_counts = ak.flatten(inner_counts)
                    total_elements = np.sum(flat_counts)
                    n_bytes = int(total_elements * items_per_row * dtype.itemsize)
                    raw = np.frombuffer(buffer, dtype=dtype, count=total_elements * items_per_row, offset=cursor)
                    cursor += n_bytes
                    level1 = ak.unflatten(raw, flat_counts)
                    level2 = ak.unflatten(level1, outer_counts)
                    results[field['name']] = level2
                else:
                    total_elements = np.sum(outer_counts)
                    n_bytes = int(total_elements * items_per_row * dtype.itemsize)
                    raw = np.frombuffer(buffer, dtype=dtype, count=total_elements * items_per_row, offset=cursor)
                    cursor += n_bytes
                    if item_shape: raw = raw.reshape(-1, *item_shape)
                    results[field['name']] = ak.unflatten(raw, outer_counts)

            # --- Case 4: Interleaved Mixed (New for RUSDGEOM) ---
            # Handles loops where some items are dynamic (array-driven) and some are fixed
            elif f_type == 'interleaved_mixed':
                if isinstance(field['count'], str):
                    loop_count = ctx[field['count']]
                else:
                    loop_count = field['count']
                temp_storage = {sub['name']: [] for sub in field['items']}

                for i in range(loop_count):
                    for sub_field in field['items']:
                        dtype = self.dtypes[sub_field['type']]
                        
                        # Calculate Count
                        count = 1
                        # Dynamic sizing from a previously read array (e.g., nsig[i])
                        if 'size_from' in sub_field:
                            count *= ctx[sub_field['size_from']][i]
                        # Fixed shape multiplication
                        if 'shape' in sub_field:
                            for dim in sub_field['shape']: count *= dim
                        
                        n_bytes = count * dtype.itemsize
                        data = np.frombuffer(buffer, dtype=dtype, count=count, offset=cursor)
                        cursor += n_bytes
                        temp_storage[sub_field['name']].append(data)

                for k, v in temp_storage.items():
                    results[k] = ak.Array(v)
                    # We don't necessarily add mixed jagged arrays to ctx for sizing others

        return results, cursor
    
# --- Helper to Simulate Reading from a File ---
def read_dst_file(filename, schema_path):
    reader = BankReader(schema_path)
    
    with open(filename, 'rb') as f:
        # Skip DST File Headers/Block logic for this snippet
        # In a real app, you would read the "Block" header here (Start of Block)
        # and then iterate over Banks.
        
        # Simulating finding a bank in the stream:
        # In reality, you'd read the 4-byte length and bank ID here.
        
        # Example: reading the raw bytes of ONE bank (after the header)
        # This is where you would pass the buffer from f.read(bank_length)
        pass
