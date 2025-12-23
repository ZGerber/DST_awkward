import gzip
import bz2
import struct
import io
import os

# --- DST Protocol Constants (from dst_bank.c) ---
BLOCK_LEN = 32000
OPCODE = 96             # 0x60
START_BLOCK = 97        # 0x61
END_BLOCK_LOGICAL = 98  # 0x62
END_BLOCK_PHYSICAL = 99 # 0x63
FILLER = 100            # 0x64

START_BANK = 7          # 0x07
CONTINUE = 8            # 0x08
END_BANK = 14           # 0x0E
TO_BE_CONTD = 15        # 0x0F

class DSTFile:
    def __init__(self, filename):
        self.filename = filename
        self.f = self._open_file(filename)
        self.block_buffer = b""
        self.cursor = 0
        self.eof = False

    def _open_file(self, filename):
        """Transparently opens .dst, .dst.gz, or .dst.bz2"""
        if filename.endswith(".gz"):
            return gzip.open(filename, 'rb')
        elif filename.endswith(".bz2"):
            return bz2.open(filename, 'rb')
        else:
            return open(filename, 'rb')

    def close(self):
        if self.f:
            self.f.close()

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()

    def _read_next_block(self):
        """Reads exactly one 32KB block from the file."""
        chunk = self.f.read(BLOCK_LEN)
        if len(chunk) < BLOCK_LEN:
            self.eof = True
            if len(chunk) > 0:
                print(f"Warning: Last block incomplete ({len(chunk)} bytes)")
            return False
        
        self.block_buffer = chunk
        self.cursor = 0
        return True

    def banks(self):
        """Generator that yields (bank_id, bank_version, raw_data_bytes)"""
        
        # State for reassembling split banks
        current_bank_data = bytearray()
        building_bank = False

        while not self.eof:
            # If buffer exhausted, get next block
            if self.cursor >= BLOCK_LEN:
                if not self._read_next_block():
                    break
            
            # If we haven't read any blocks yet
            if not self.block_buffer:
                if not self._read_next_block():
                    break

            # --- PARSE BYTE STREAM ---
            byte = self.block_buffer[self.cursor]
            
            # We expect an OPCODE (96) to start any command
            if byte != OPCODE:
                # Skip filler or garbage
                self.cursor += 1
                continue
            
            # Look ahead at the command byte
            if self.cursor + 1 >= BLOCK_LEN:
                # Opcode at very end of block? Edge case, but unlikely in valid DST.
                self.cursor += 1 
                continue

            cmd = self.block_buffer[self.cursor + 1]
            self.cursor += 2 # Consumed OPCODE + CMD

            # --- COMMAND HANDLERS ---

            if cmd == START_BLOCK:
                # Structure: [OPCODE] [START_BLOCK] [BlockNum (4 bytes)]
                # We skip the block number (4 bytes)
                self.cursor += 4 
            
            elif cmd == END_BLOCK_LOGICAL or cmd == END_BLOCK_PHYSICAL:
                # End of meaningful data in this block. Skip to next block.
                self.cursor = BLOCK_LEN 

            elif cmd == START_BANK:
                if building_bank:
                    print("Warning: unexpected START_BANK while building previous bank. Resetting.")
                    current_bank_data = bytearray()
                
                # Read Segment Length (4 bytes)
                seg_len = self._read_int4()
                # Read Segment Data
                self._read_into_buffer(current_bank_data, seg_len)
                building_bank = True

            elif cmd == CONTINUE:
                if not building_bank:
                    print("Warning: unexpected CONTINUE without START_BANK. Skipping.")
                    # Skip length bytes just to be safe
                    seg_len = self._read_int4()
                    self.cursor += seg_len
                else:
                    # Append this segment to our existing buffer
                    seg_len = self._read_int4()
                    self._read_into_buffer(current_bank_data, seg_len)

            elif cmd == END_BANK:
                # Bank finished. 
                # Structure: [OPCODE] [END_BANK] [CRC (4 bytes)]
                crc = self._read_int4() # We ignore CRC check for speed
                
                if building_bank:
                    # The Bank ID and Version are usually the FIRST 8 bytes of the data payload
                    # (based on fraw1_bank_to_common_ in your C code)
                    if len(current_bank_data) >= 8:
                        # Unpack Header
                        bank_id = struct.unpack('<i', current_bank_data[0:4])[0]
                        bank_ver = struct.unpack('<i', current_bank_data[4:8])[0]
                        
                        # Yield the full payload (including the header bytes, 
                        # because your schema parser will likely expect them to consume the ID/Ver fields)
                        yield bank_id, bank_ver, bytes(current_bank_data)
                    
                    # Reset
                    current_bank_data = bytearray()
                    building_bank = False

            elif cmd == TO_BE_CONTD:
                # Just a marker saying "don't process yet, wait for CONTINUE"
                # Checksum (4 bytes) usually follows TBC marker too
                crc = self._read_int4()
                # We stay in `building_bank = True` state

            else:
                # Unknown marker or Filler (100)
                pass

    def _read_int4(self):
        """Reads a 4-byte little-endian integer from current cursor."""
        # Note: This does not handle the edge case where a 4-byte int 
        # is split across the 32KB block boundary. 
        # DST format avoids splitting control words across blocks.
        val = struct.unpack_from('<i', self.block_buffer, self.cursor)[0]
        self.cursor += 4
        return val

    def _read_into_buffer(self, target_bytearray, length):
        """Copies `length` bytes from block to target, advancing cursor."""
        end = self.cursor + length
        target_bytearray.extend(self.block_buffer[self.cursor : end])
        self.cursor = end
