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

//! Converts an integer to binary
unsigned int_to_binary(unsigned k) {
    if (k == 0) { return 0; }
    if (k == 1) { return 1; }
    return (k % 2) + 10 * int_to_binary(k / 2);
}

//! Makes sure any functions relating to binary logic work as they should
void sanity_check_binary_logic() {
    fprintf(stderr, "[self-test] Sanity checking binary manipulation logic.\n");
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

int main()
{
    sanity_check_binary_logic();
    return 0;
}
