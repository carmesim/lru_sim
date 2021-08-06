#include <unistd.h> // For sysconf, _SC_PAGE_SIZE
#include <stdio.h>
#include <stdlib.h>   // For srand
#include <unistd.h>   // For sleep
#include <stdint.h>   // For uint8_t, uint32_t
#include <stdbool.h>
#include <assert.h>  // For assert
#include <time.h>    // For time

#include "definitions.h"
#include "binutils.h"

long page_size_in_bytes() {
    return sysconf(_SC_PAGE_SIZE);
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

void sanity_check_set_bit() {
    fprintf(stderr, "[self-test] Sanity checking set_bit\n");
    // 5 is 0b00000101
    uint8_t val = 5;
    assert(int_to_binary(val) == 101);

    set_bit(&val, 3, true);
    // val is now 0b00001101 = 13
    assert(int_to_binary(val) == 1101);

    set_bit(&val, 4, true);
    // val is now 0b00011101 = 29
    assert(int_to_binary(val) == 11101);

    set_bit(&val, 4, false);
    // val is now 0b00001101 = 13
    assert(int_to_binary(val) == 1101);

    set_bit(&val, 3, false);
    // val is now 0b00000101 = 5
    assert(int_to_binary(val) == 101);

    set_bit(&val, 2, false);
    // val is now 0b00000001 = 1
    assert(int_to_binary(val) == 1);



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
    printf("Tests\n");
    sanity_check_get_bit_at();
    sanity_check_count_zeroes();
    sanity_check_set_bit();
    printf("\nProgram\n");
    srand(4);

    int i, n_pages = 0;
    init_pages_as_free();
    while(n_pages < 100){

        printf("\n\n=======================(Ciclo %d)==========================\n", n_pages);
        unreference_all_pages();

        for (i = 0; i < 2; i++){
            int v_addr = rand()%N_SLOTS_VM;
            printf("\nReferencing page in virtual addr %d...\n", v_addr);
            int rv = reference_page(v_addr);
            if(rv == -1){
                printf("Invalid virtual address\n");
                return 1;
            }
        }
        update_counters();



        // Visualization of the memoy contents
        for(i = 0; i < N_SLOTS_VM; i++){
            if(i < N_SLOTS_RM){
                printf("VM[%d] = (end=%d, ismapped=%d)", i, page_table[i].real_addr, page_table[i].is_mapped);
                printf(" | RM[%d] = (counter=%s, content=%d)", i,converte_n_bin(real_memory[i].page.referenced_counter, 8),  real_memory[i].page.content);
                printf(" | SW[%d] = (content=%d, old_addr =%d, is_free=%d)\n", i, swap[i].page.content, swap[i].old_addr, swap[i].page.is_free );
            }else{
                printf("VM[%d] = (end=%d, ismapped=%d)\n", i, page_table[i].real_addr, page_table[i].is_mapped);
            }
        }


        int n;
        scanf("%d", &n);
        n_pages++;
        printf("\n\n");
    }
    // mem_free_pages(); // free all memory pages

    return 0;
}
