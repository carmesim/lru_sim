#include "binutils.h"

//!
//! Gets the bit in position `idx` within `byte`
//!
bool get_bit_at(uint8_t byte, uint8_t idx) {
    return ((byte >> (idx))  & 0x01);
}

//! Set a bit on or off.
//! The bit in the position `idx` will be set on if `set_on` is true
//! or set off if `set_on` is false
void set_bit(uint8_t * byte, const uint8_t idx, const bool set_on) {
    if(set_on && !get_bit_at(*byte, idx)) {
        // Set a bit on
        *byte |= 0x01 << idx;
        return;
    }

    // Set a bit off
    *byte &= ~(0x01 << idx);
}

//! Will count how many bits are unset in `byte`
uint8_t count_zeroes(uint8_t byte) {
    uint8_t count = 0;
    for (uint8_t i = 0; i < 8; i++) {
        if (!get_bit_at(byte, i)) {
            count++;
        }
    }
    return count;
}

//! Converts an integer to binary
uint32_t int_to_binary(uint32_t k) {
    if (k == 0) { return 0; }
    if (k == 1) { return 1; }
    return (k % 2) + 10 * int_to_binary(k / 2);
}
