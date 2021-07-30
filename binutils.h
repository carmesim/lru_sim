#ifndef BINUTILS_H
#define BINUTILS_H

#include <stdint.h>
#include <stdbool.h>

bool get_bit_at(uint8_t, uint8_t);
uint8_t count_zeroes(uint8_t byte);
uint32_t int_to_binary(uint32_t);

#endif // BINUTILS_H
