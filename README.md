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
This describes a byte buffer where the first four bytes (32 bits) code (little endian integer) for `event_num`, the next 3$\times$4 bytes (3$\timets$32 bits) code for `trig_id`, the next 4 bytes code for `nofwf`, and finally the next `nofwf`$\times$4 bytes code for the `xxyy` array. (N.B. `rusdraw` is actually significantly more complicated than this.)

### Interleaved arrays
A common pattern for 2D arrays in DST bank is to have several different attributes with the same size on the first axis stored in an interleaved fashion. This could be considered column-wise storage, as the different attributes are rows and the item location in the attribute is the column. This is denoted by the type `interleaved-sequence` (which doesn't have `name` or `shape`) and which requires a `count` and a list of `items`. The `items` have the same expectations as with the scalars and 1D arrays above. More example from `rusdraw`:
```
  - type: "interleaved_sequence"
    count: "nofwf"
    items:
      - { name: "fadcti",    type: "int32", shape: [2] }
      - { name: "fadcav",    type: "int32", shape: [2] }
      - { name: "fadc",      type: "int32", shape: [2, 128] }
```
This describes reading from the byte buffer 2 4-byte integers into `fadcti[0]` that go into `fadcti[0,0]` and `fadcti[0,1]`, then 2 4-byte integers that go into `fadcav[0]`, the 256 (2$times$128) integers into `fadc[0]`, *then* 2 4-byte integers into `fadcti[1]`, then `fadcav[1]` then `fadc[1]` and so on up to `nofwf`. The python code knows how to reshape the 2D array which must therefore behave standard flattening.
