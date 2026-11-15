#!/usr/bin/env python3

#!/usr/bin/env python3
import sys

MASK_32 = 0xFFFFFFFF          # keeps values within 32 bits (emulates uint32 overflow)
CRC_START = 0x1000            # checksum range start (after header + IPL3)
CRC_LENGTH = 0x100000         # 1 MiB checksummed
CIC_6102_SEED = 0xF8CA4DDC
HEADER_CRC_OFFSET = 0x10

def rotate_left(value, amount):
    amount &= 0x1F
    if amount == 0:
        return value
    return ((value << amount) | (value >> (32 - amount))) & MASK_32

with open(sys.argv[1], "rb+") as rom_file:
    rom = rom_file.read(CRC_START + CRC_LENGTH).ljust(CRC_START + CRC_LENGTH, b"\0")

    sum_all      = CIC_6102_SEED  # running sum of words
    carry_count  = CIC_6102_SEED  # counts overflows of sum_all
    xor_all      = CIC_6102_SEED  # running XOR of words
    sum_rotated  = CIC_6102_SEED  # running sum of rotated words
    mixed_xor    = CIC_6102_SEED  # conditional XOR accumulator
    mixed_sum    = CIC_6102_SEED  # running sum of word ^ sum_rotated

    for offset in range(CRC_START, CRC_START + CRC_LENGTH, 4):
        word = int.from_bytes(rom[offset:offset + 4], "big")

        if ((sum_all + word) & MASK_32) < sum_all:
            carry_count = (carry_count + 1) & MASK_32
        sum_all = (sum_all + word) & MASK_32

        xor_all ^= word

        rotated = rotate_left(word, word)  # rotate by the word's low 5 bits
        sum_rotated = (sum_rotated + rotated) & MASK_32

        mixed_xor ^= rotated if mixed_xor > word else (sum_all ^ word)
        mixed_sum = (mixed_sum + (word ^ sum_rotated)) & MASK_32

    crc1 = sum_all ^ carry_count ^ xor_all
    crc2 = sum_rotated ^ mixed_xor ^ mixed_sum

    rom_file.seek(HEADER_CRC_OFFSET)
    rom_file.write(crc1.to_bytes(4, "big") + crc2.to_bytes(4, "big"))
