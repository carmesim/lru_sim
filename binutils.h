#ifndef BINUTILS_H
#define BINUTILS_H

#include <stdint.h>
#include <stdbool.h>

//! Gets the bit in position `idx` within `byte`
bool get_bit_at(uint8_t, uint8_t);
//! Will count how many bits are unset in `byte`
uint8_t count_zeroes(uint8_t byte);

//! Converts an integer to binary
uint32_t int_to_binary(uint32_t);

//! Sets a bit on or off
void set_bit(uint8_t *, const uint8_t, const bool);

#endif // BINUTILS_H
