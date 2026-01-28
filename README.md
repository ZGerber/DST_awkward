# DST_awkward

A Python utility for converting Telescope Array DST (Data Summary Tape) files to Awkward Arrays and Parquet format, enabling efficient analysis of cosmic ray detector data.

## Overview

DST files are binary data files containing detector events. This package provides:

- **Conversion**: Transform DST files into modern data formats (Awkward Arrays, Parquet)
- **Inspection**: Human-readable dumping of bank data (similar to `dstdump`)
- **Schema-driven parsing**: Bank layouts defined in YAML files
- **Conditional bank support**: Handles complex banks like PRFC and HCBIN with bitmask-gated sections

## Installation

```bash
pip install -e .
```

This installs the package in editable mode and creates CLI entry points.

## Quick Start

### Convert DST to Parquet

```bash
dst-convert run123.dst
# Output: run123.parquet

# Convert specific banks only
dst-convert run123.dst --banks rusdraw,prfc,hcbin

# Limit number of events
dst-convert run123.dst --limit 1000
```

### Inspect Parquet Files

```bash
# Dump all banks (long format)
dst-dump +all run123.parquet

# Dump specific banks
dst-dump +prfc -rusdraw run123.parquet

# Process multiple files
dst-dump +all run1.parquet run2.parquet run3.parquet
```

## Architecture

### Data Flow

```
DST File (.dst)
    ↓
[dst-convert]
    ↓
Parquet File (.parquet)
    ↓
[dst-dump]
    ↓
Human-readable output
```

### Components

1. **`dst-convert`** (`dst_events_to_awkward.py`)
   - Reads DST files sequentially
   - Discovers bank schemas from `schemas/*.yaml`
   - Parses banks using `BankReader` (dispatches to custom parsers for PRFC/HCBIN)
   - Groups banks into events (event boundaries detected by bank name repetition)
   - Outputs Awkward Array → Parquet file

2. **`dst-dump`** (`dst_awkward_dump.py`)
   - Reads Parquet files (output of `dst-convert`)
   - Formats bank data as human-readable text
   - Supports short (`-`) and long (`+`) output formats

3. **`BankReader`** (`dst_reader.py`)
   - Generic YAML-driven parser for most banks
   - Dispatches to custom parsers for conditional banks:
     - `prfc_reader.py` - PRFC bank (3 masks, 3 gated sections)
     - `hcbin_reader.py` - HCBIN bank (1 mask, failmode-gated)

## Bank Schema Format

Bank layouts are defined in YAML files in `src/dst_awkward/schemas/`. Each schema file describes how to parse a specific bank type.

### Basic Structure

```yaml
bank_id: 13101
name: "rusdraw"
endian: "<"
layout:
  # Field definitions...
```

- `bank_id`: Unique identifier for this bank type
- `name`: Bank name (used for event grouping and CLI)
- `endian`: Byte order (`"<"` little-endian, `">"` big-endian)
- `layout`: List of field definitions (see below)

### Field Types

#### Scalars and 1D Arrays

```yaml
layout:
  - { name: "event_num", type: "int32" }           # Scalar
  - { name: "trig_id",   type: "int32", shape: [3] }  # Fixed-size array
  - { name: "nofwf",     type: "int32" }           # Scalar used as size
  - { name: "xxyy",      type: "int32", shape: ["nofwf"] }  # Variable-size array
```

**Types**: `int8`, `int16`, `int32`, `float32`, `float64`

**Shape**: 
- Integer literal: fixed size (e.g., `[3]`)
- String reference: variable size from previously read field (e.g., `["nofwf"]`)

#### Interleaved Sequences

For 2D arrays where multiple fields are interleaved:

```yaml
layout:
  - { name: "nofwf", type: "int32" }
  - type: "interleaved_sequence"
    count: "nofwf"  # Loop this many times
    items:
      - { name: "fadcti", type: "int32", shape: [2] }
      - { name: "fadcav", type: "int32", shape: [2] }
      - { name: "fadc",   type: "int32", shape: [2, 128] }
```

This reads: `fadcti[0]`, `fadcav[0]`, `fadc[0]`, then `fadcti[1]`, `fadcav[1]`, `fadc[1]`, etc.

#### Jagged Arrays in Interleaved Sequences

When array sizes vary per iteration:

```yaml
layout:
  - { name: "num_mir",  type: "int16" }
  - { name: "num_chan", type: "int16", shape: ["num_mir"] }
  - type: "interleaved_sequence"
    count: "num_mir"
    size_ref: "num_chan"  # Use num_chan[i] as size for iteration i
    items:
      - { name: "channel", type: "int16" }
```

#### Bulk Jagged Arrays

For rank-3 arrays (e.g., waveforms: mirror → channel → samples):

```yaml
layout:
  - type: "bulk_jagged"
    name: "m_fadc"
    dtype: "int8"
    outer_counts: "num_chan"  # First dimension
    inner_counts: "nt_chan"   # Second dimension
```

#### Interleaved Mixed

For loops with both fixed-size and variable-size items:

```yaml
layout:
  - type: "interleaved_mixed"
    count: "nsds"
    items:
      - { name: "xyzclf", type: "float64", shape: [3] }  # Fixed size
      - { name: "sdsigq", type: "float64", size_from: "nsig" }  # Variable size
```

## Conditional Banks

Some banks (PRFC, HCBIN) have complex conditional layouts that can't be expressed in pure YAML. These use custom Python parsers.

### PRFC Bank

- **3 masks**: `pflinfo`, `bininfo`, `mtxinfo` (16-bit each, MSB-first)
- **3 sections**: Profile parameters, bin data, matrix data
- **Failmode checks**: Profile section skips data if `failmode != SUCCESS`

### HCBIN Bank

- **1 mask**: `bininfo` (16-bit, MSB-first)
- **1 section**: Bin data with nested failmode check
- **Failmode check**: If `failmode != SUCCESS`, bin arrays are not present

Both parsers use shared utilities in `conditional_bank_utils.py`:
- `BufferReader`: Stateful binary reader with cursor tracking
- `decode_mask_msb_first()`: Decode packed bitmasks
- Per-fit storage helpers: `fit_list()`, `fit_empty_arrays()`, `fit_zeros()`

## Output Format

### Parquet Structure

The output Parquet file contains an Awkward Array of **events**. Each event is a record with fields for each bank type:

```python
import awkward as ak
events = ak.from_parquet("run123.parquet")

# Access event 0
event = events[0]

# Access banks
rusdraw_data = event["rusdraw"]
prfc_data = event["prfc"]
hcbin_data = event["hcbin"]

# Banks may be None if not present in that event
if event["prfc"] is not None:
    print(event["prfc"]["nbin"])
```

### Event Boundaries

Events are detected when a bank name repeats (e.g., seeing `start` or `rusdraw` again indicates a new event). The previous event is finalized and yielded.

## Adding New Banks

1. **Create schema file**: `src/dst_awkward/schemas/mybank.yaml`
   ```yaml
   bank_id: 12345
   name: "mybank"
   endian: "<"
   layout:
     - { name: "field1", type: "int32" }
     # ... more fields
   ```

2. **If conditional**: Create `src/dst_awkward/mybank_reader.py` with `parse_mybank_bank()` function

3. **Add dispatch**: Update `dst_reader.py` to route to your parser:
   ```python
   if self.schema.get("name") == "mybank":
       from dst_awkward.mybank_reader import parse_mybank_bank
       # ...
   ```

4. **Add dump function** (optional): `src/dst_awkward/dump/mybank.py` with `dump_mybank()` function

## Examples

### Convert and Analyze

```bash
# Convert DST to Parquet
dst-convert data.dst --banks prfc,hcbin

# Inspect results
dst-dump +prfc data.parquet | head -100

# Use in Python
python -c "
import awkward as ak
events = ak.from_parquet('data.parquet')
print(f'Events: {len(events)}')
print(f'PRFC present in: {sum(events.prfc is not None)} events')
"
```

### Multiple Files

```bash
# Convert multiple runs
for f in run*.dst; do
    dst-convert "$f"
done

# Dump all results
dst-dump +all run*.parquet
```

## Project Structure

```
DST_awkward/
├── src/dst_awkward/
│   ├── dst_io.py              # DST file reading
│   ├── dst_reader.py          # Generic YAML-driven parser
│   ├── dst_events_to_awkward.py  # Convert tool
│   ├── dst_awkward_dump.py    # Dump tool
│   ├── conditional_bank_utils.py  # Shared utilities for PRFC/HCBIN
│   ├── prfc_reader.py          # PRFC custom parser
│   ├── hcbin_reader.py         # HCBIN custom parser
│   ├── schemas/                # YAML bank schemas
│   └── dump/                   # Bank dump formatters
├── legacy/                     # Original C bank code (reference)
└── tests/                      # Test scripts
```

## Requirements

- Python >= 3.12
- awkward >= 2.8.11
- numpy >= 2.4.0
- pyarrow >= 22.0.0
- pyyaml >= 6.0.3

## License

See LICENSE file.
