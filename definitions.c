#include <stdio.h>
#include <stdlib.h>
#include "definitions.h"

mem_slot_t real_memory[N_SLOTS_RM];
mem_slot_t swap[N_SLOTS_SWAP];

// process table stores the mapping between VM and RM
page_table_entry_t page_table[N_SLOTS_VM];

static int8_t get_free_real_address() {
    for(int8_t i = 0; i < N_SLOTS_RM; i++) {
        if(real_memory[i].page.is_free) {
            printf("Page %d is free\n", i);
            return i;
        }
        printf("Page %d isn't free\n", i);
    }
    return -1;
}

static int8_t get_free_swap_address() {
    for(int8_t i = 0; i < N_SLOTS_RM; i++) {
        if(swap[i].page.is_free) {
            return i;
        }
    }
    return -1;
}

// set all pages as free
void init_pages_as_free() {
    for(int8_t i = 0; i < N_SLOTS_RM; i++) {
        real_memory[i].page.is_free = true;
        swap[i].page.is_free = true;
    }
}

int reference_page(int8_t addr) {
    if(addr >= N_SLOTS_VM) {
        // invalid virtual address
        return -1;
    }
    
    // lookup page table entry of given address
    page_table_entry_t pte = page_table[addr];

    if(pte.is_mapped == true){
        // reference page
        // assuming it's on the real memory
        real_memory[pte.real_addr].page.referenced_counter++; // update counter;
        real_memory[pte.real_addr].page.R = 1; // was recently referenced ! (in the last cycle)
    }else{
        // page fault
        int8_t real_addr = get_free_real_address();// -1 se não achar

        if(real_addr == -1){//real memory is full
            // page miss

            printf("Page Miss !\n");
            
            // get a new swap address to recieve the oldest page
            int8_t swap_addr = get_free_swap_address();// tem que estar entre N_SLOTS_RM
            if(swap_addr == -1){
                // :(
            }

            // find lru page and puts it into swap
            int8_t liberated_adrr; //liberated address
            swap[swap_addr].page = lru_page(&liberated_adrr);
            printf("The removed page went to swap[%d]\n", swap_addr);
            printf("The liberated addr on the real memory was %d\n", liberated_adrr);

            // the freed address will recieve the new page
            real_memory[liberated_adrr].page.referenced_counter = 1;
            real_memory[liberated_adrr].page.R = 1; // was recently referenced ! (in the last cycle)

            pte.real_addr = liberated_adrr; // store it in the page table
            pte.is_mapped = true;  
        }else{
            pte.real_addr = real_addr;
            real_memory[pte.real_addr].page.referenced_counter++; // update counter
            real_memory[pte.real_addr].page.R = 1; // was recently referenced ! (in the last cycle)
            real_memory[pte.real_addr].page.is_free = false; // set the page as linked to a virtual address
        }
    }
    return 0;
}


// returns the last recently used page 
// and changes lib_addr to the freed address
// on the real memory
page_t lru_page(int8_t *lib_addr){
    int i, pi_counter = 0, lowest, i_lru = 0;
    page_t lru;
    for (i = 0; i < N_SLOTS_RM; i++){
        pi_counter = real_memory[i].page.referenced_counter;
        if(i == 0){
            lowest = pi_counter;
        }else{
            if(pi_counter < lowest){
                lowest = pi_counter;
                i_lru = i;
            }
        }
    }
    *lib_addr = i_lru;
    lru = real_memory[i_lru].page;
    return lru;
}
