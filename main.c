#include <unistd.h> // For sysconf, _SC_PAGE_SIZE
#include <stdio.h>
#include <sys/param.h> // For MAX
#include <stdlib.h>   // For srand
#include <unistd.h>   // For sleep
#include <stdint.h>   // For uint8_t, uint32_t
#include <stdbool.h>  // For bool
#include <assert.h>   // For assert
#include <time.h>     // For time

#include "definitions.h"
#include "binutils.h"

static int pages_index[N_SLOTS_VM];// used only to print in colors
static int pages_index_size = 0;

bool index_was_drawn(int index){
    int i;
    for(i = 0; i < pages_index_size; i++){
        if(index == pages_index[i]){
            return 1;
        }
    }
    return 0;
}

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

#ifdef OUR_MAX
void sanity_check_max() {
    fprintf(stderr, "[self-test] Sanity checking max\n");
    assert(max(2, 3) == 3);
    assert(max(5, 1) == 5);
    assert(max(9, 0) == 9);
    assert(max(0, 255) == 255);
    assert(max(255, 256) == 256);
    assert(max(2147483647, 4294967295) == 4294967295);
    fprintf(stderr, "[self-test] OK\n");
}
#endif

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

int main(int argc, char** argv){

    printf("Tests\n");
#ifdef OUR_MAX
    sanity_check_max();
#endif
    sanity_check_get_bit_at();
    sanity_check_count_zeroes();
    sanity_check_set_bit();
    printf("\nProgram\n");
    srand(4);
    
    int i, max_n_pages = 3, cycle = 0;

    if(argc == 2){
        max_n_pages = atoi(argv[1]);// max number of pages genereted at each cycle
        if(max_n_pages > N_SLOTS_VM){
            printf("[ERROR] Max number of pages spawn per cycle must be lower than %d !\n", N_SLOTS_VM);
            return 1;
        }
    }

    init_pages_as_free();
    while(1){

        printf("\n\n=======================(Ciclo %d)==========================\n", cycle);
        unreference_all_pages();

        int v_addr;
        int n_pages = rand()%max_n_pages + 1;
        pages_index_size = 0;

        for (i = 0; i < n_pages; i++){
            v_addr = rand()%N_SLOTS_VM;
            pages_index[pages_index_size++] = v_addr;
            printf("\nReferencing page in virtual addr %d...", v_addr);
            int rv = reference_page(v_addr);
            if(rv == -1){
                printf("Invalid virtual address\n");
                return 1;
            }
        }
        update_counters();

        int max_swap_vm = MAX(N_SLOTS_VM, N_SLOTS_SWAP);
        if(N_SLOTS_VM < N_SLOTS_SWAP ){
            max_swap_vm = N_SLOTS_SWAP;
        }

        // Visualization of the memoy contents
        for(i = 0; i < max_swap_vm; i++){
            if(index_was_drawn(i)){
                RED()
            }
            if(i < N_SLOTS_VM){
                printf("VM[%02d] = (end=%d, ismapped=%d)", i, page_table[i].real_addr, page_table[i].is_mapped);

                if(index_was_drawn(i)){
                    BLACK()
                }

                if(i < N_SLOTS_RM){
                    printf(" | RM[%02d] = (counter=%08d, content=%03d)", i, int_to_binary(real_memory[i].page.referenced_counter),  real_memory[i].page.content);
                    if(i < N_SLOTS_SWAP){
                        printf(" | SW[%02d] = (content=%03d, va=%02d, ra=%02d is_free=%d)\n", i, swap[i].page.content, swap[i].old_vm_addr,  swap[i].old_rm_addr,swap[i].page.is_free );
                    }
                }else{
                    if(i < N_SLOTS_SWAP){
                        printf("%44s| SW[%02d] = (content=%03d, va=%02d, ra=%02d is_free=%d)\n", "", i, swap[i].page.content, swap[i].old_vm_addr,  swap[i].old_rm_addr,swap[i].page.is_free );
                    }else{
                        printf("\n");
                    }
                }
            }else{
                printf("%40s| SW[%02d] = (content=%03d, va=%02d, ra=%02d is_free=%d)\n", "", i, swap[i].page.content, swap[i].old_vm_addr,  swap[i].old_rm_addr,swap[i].page.is_free );

            }
        }
        
        getchar();
        cycle++;
        printf("\n\n");
    }
    return 0;
}
