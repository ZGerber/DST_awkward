# DST_awkward
A utility for converting DST files to awkward record arrays and back.

## DST Reading encoding
The way that DST files read from files into COMMON blocks/golobal C structs is encoded in a YAML file. The file should contain four keys, e.g.:
```
bank_id: 13101
name: "rusdraw"
endian: "<"
layout:
```
The `layout` is where the reading proceedure is enccoded. It is a YAML list with a number of possible items. 

### Scalars and 1D arrays
The simplest things are scalars and planar arrays, where the keys in the list shold include (field) `name`, (data) `type`, and optionally `shape`. No shape defaults to `shape: [1]`. `shape` is a list, of which we only consider 1D for the moment. The shape can be a string which is a previously defined *name*. It can also be an integer literal. More (partial) example from `rusdraw`:
```
layout:
  - { name: "event_num", type: "int32" }
  - { name: "trig_id",   type: "int32", shape: [3] }
  - { name: "nofwf",     type: "int32" }
  - { name: "xxyy",      type: "int32", shape: ["nofwf"] }
```
This corresponds to the C code 
```
nobj = 1;
rcode += dst_unpacki4_ (&rusdraw_.event_num, &nobj, bank, &rusdraw_blen, &rusdraw_maxlen);
rcode += dst_unpacki4_ (&rusdraw_.trig_id[0], &nobj, bank, &rusdraw_blen,
		   &rusdraw_maxlen);
nobj=1;
rcode +=  dst_unpacki4_ (&rusdraw_.errcode, &nobj, bank, &rusdraw_blen, &rusdraw_maxlen);
nobj=3;
rcode += dst_unpacki4_ (&rusdraw_.yymmdd, &nobj, bank, &rusdraw_blen, &rusdraw_maxlen);
nobj=1;
rcode += dst_unpacki4_ (&rusdraw_.nofwf, &nobj, bank, &rusdraw_blen, &rusdraw_maxlen);
nobj = rusdraw_.nofwf;
rcode += dst_unpacki4_ (&rusdraw_.nretry[0], &nobj, bank, &rusdraw_blen, &rusdraw_maxlen);
``` 
This describes a byte buffer where the first four bytes (32 bits) code (little endian integer) for `event_num`, the next 3$\times$4 bytes (3$\timets$32 bits) code for `trig_id`, the next 4 bytes code for `nofwf`, and finally the next `nofwf`$\times$4 bytes code for the `xxyy` array. (N.B. `rusdraw` is actually significantly more complicated than this.)

### Interleaved sequences
A common pattern for 2D arrays in DST bank is to have several different attributes with the same size on the first axis stored in an interleaved fashion. This is denoted by the type `interleaved_sequence` (which doesn't have `name` or `shape`) and which requires a `count` and a list of `items`. The `items` have the same expectations as with the scalars and 1D arrays above. More example from `rusdraw`:
```
  - type: "interleaved_sequence"
    count: "nofwf"
    items:
      - { name: "fadcti",    type: "int32", shape: [2] }
      - { name: "fadcav",    type: "int32", shape: [2] }
      - { name: "fadc",      type: "int32", shape: [2, 128] }
```
corresponding to the following C loop:
```
for(i = 0;  i< rusdraw_.nofwf; i++) {
  nobj = 2;
  rcode += dst_unpacki4_ (&rusdraw_.fadcti[i][0], &nobj, bank, &rusdraw_blen, &rusdraw_maxlen);
  rcode += dst_unpacki4_ (&rusdraw_.fadcav[i][0], &nobj, bank, &rusdraw_blen, &rusdraw_maxlen);
  nobj = 128;
  for (j = 0; j < 2; j++) {
	  rcode += dst_unpacki4_ (&rusdraw_.fadc[i][j][0], &nobj, bank, &rusdraw_blen, &rusdraw_maxlen);
	}
}
This describes reading from the byte buffer 2 4-byte integers into `fadcti[0]` that go into `fadcti[0,0]` and `fadcti[0,1]`, then 2 4-byte integers that go into `fadcav[0]`, the 256 (2$times$128) integers into `fadc[0]`, *then* 2 4-byte integers into `fadcti[1]`, then `fadcav[1]` then `fadc[1]` and so on up to `nofwf`. The python code knows how to reshape the 2D array which must therefore have standard flattening.

Interleaved sequences can also have jagged arrays, which handle situations like FD data where the number of PMTs in a camera vary by camera (unlike the SD data above where there are always 2 layers per counter). This requires the `size_from` key to which the value must be a previously defined item (which will have been stored as context). An examnple from `fraw1`:
```
  - { name: "num_mir",    type: "int16" }
  - { name: "num_chan",   type: "int16", shape: ["num_mir"] }
  - type: "interleaved_sequence"
    count: "num_mir"    # Loop N times
    size_ref: "num_chan" # Each iteration reads num_chan[i] items
    items:
      - { name: "channel",  type: "int16" }
      - { name: "it0_chan", type: "int16" }
      - { name: "nt_chan",  type: "int16" }
```
corresponding to the C code:
```
nobj=1;
rcode += dst_packi2_(&fraw1_.num_mir, &nobj, fraw1_bank, &fraw1_blen, &fraw1_maxlen);
nobj=fraw1_.num_mir;
rcode += dst_packi2_(&fraw1_.num_chan[0], &nobj, fraw1_bank, &fraw1_blen, &fraw1_maxlen);
for(i=0;i<fraw1_.num_mir;i++) {
  nobj=fraw1_.num_chan[i];
  rcode += dst_packi2_(&fraw1_.channel[i][0], &nobj, fraw1_bank, &fraw1_blen, &fraw1_maxlen);
  rcode += dst_packi2_(&fraw1_.it0_chan[i][0], &nobj, fraw1_bank, &fraw1_blen, &fraw1_maxlen);
  rcode += dst_packi2_(&fraw1_.nt_chan[i][0], &nobj, fraw1_bank, &fraw1_blen, &fraw1_maxlen);
}  
```

### Double Jagged Arrays: bulk_jagged
For rank three arrays, usually waveforms in FDs, there is the `bulk_jagged` type. Here there are no sub-items (there is no interleaving), and the `count` is replaced by `outer_counts` and `inner_counts`. An example from `fraw1`:
```
  - type: "bulk_jagged"
    name: "m_fadc"
    dtype: "int8" # integer1
    outer_counts: "num_chan" # 1st unflatten: separate mirrors
    inner_counts: "nt_chan"  # 2nd unflatten: separate channels
```
which corresponds to (the code for `num_mir` and `nt_chan` is assumed from above)
```
for(i=0;i<fraw1_.num_mir;i++) {
  for(j=0;j<fraw1_.num_chan[i];j++) {
    nobj=fraw1_.nt_chan[i][j];
    rcode += dst_packi1_(&fraw1_.m_fadc[i][j][0], &nobj, fraw1_bank, &fraw1_blen, &fraw1_maxlen);
  }
}  
```

### Interleaved_mixed