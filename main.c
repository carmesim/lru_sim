#include <unistd.h> // For sysconf, _SC_PAGE_SIZE
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

long page_size_in_bytes() {
    return sysconf(_SC_PAGE_SIZE);
}

//!
//! Gets the bit in position `idx` within `byte`
//!
bool get_bit_at(uint8_t byte, uint8_t idx) {
    return ((byte >> (idx))  & 0x01);
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
unsigned int_to_binary(unsigned k) {
    if (k == 0) { return 0; }
    if (k == 1) { return 1; }
    return (k % 2) + 10 * int_to_binary(k / 2);
}

//! Makes sure any functions relating to binary logic work as they should
void sanity_check_get_bit_at() {
    fprintf(stderr, "[self-test] Sanity checking get_bit_at\n");
    // 25 is 0b11001
    uint8_t twenty_five = 25;

    uint8_t twenty_five_binary[] = { 1, 0, 0, 1, 1 };

    for(int i = 0; i < 5; i++) {
        assert(
            get_bit_at(twenty_five, i) == twenty_five_binary[i]
        );
    }
    fprintf(stderr, "[self-test] OK\n");
}

void sanity_check_count_zeroes() {
    fprintf(stderr, "[self-test] Sanity checking count_zeroes\n");
    // 65 is 0b10000010
    // 6 zeroes
    uint8_t sixty_five = 65;

    // 0 is 0b0
    // 8 zeroes
    uint8_t min_byte = 0;

    // 255 is 0b11111111
    // No zeroes
    uint8_t max_byte = 255;

    assert(
        count_zeroes(sixty_five) == 6
    );

    assert(
        count_zeroes(min_byte) == 8
    );

    assert(
        count_zeroes(max_byte) == 0
    );

    fprintf(stderr, "[self-test] OK\n");
}

int main()
{
    sanity_check_get_bit_at();
    sanity_check_count_zeroes();
    return 0;
}
