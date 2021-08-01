#include <stdlib.h>
#include "definitions.h"

mem_slot_t real_memory[N_SLOTS_RM];
mem_slot_t swap[N_SLOTS_SWAP];

// process table stores the mapping between VM and RM
page_table_entry_t page_table[N_SLOTS_VM];

static uint8_t get_free_real_address() {
    for(uint8_t i = 0; i < N_SLOTS_RM; i++) {
        if(real_memory[i].page.is_free) {
            return i;
        }
    }
    return -1;
}

static uint8_t get_free_swap_address() {
    for(uint8_t i = 0; i < N_SLOTS_RM; i++) {
        if(swap[i].page.is_free) {
            return i;
        }
    }
    return -1;
}

int reference_page(uint8_t addr) {
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
        uint8_t real_addr = get_free_real_address();// -1 se não achar

        if(real_addr == -1){//real memory is full
            // page miss
            
            // get a new swap address to recieve the oldest page
            uint8_t swap_addr = get_free_swap_address();// tem que estar entre N_SLOTS_RM
            if(swap_addr == -1){
                // :(
            }

            // find lru page and puts it into swap
            uint8_t liberated_adrr; //liberated address
            swap[swap_addr].page = lru_page(&liberated_adrr);

            // the freed address will recieve the new page
            real_memory[liberated_adrr].page.referenced_counter = 1;
            real_memory[liberated_adrr].page.R = 1; // was recently referenced ! (in the last cycle)

            pte.real_addr = liberated_adrr; // store it in the page table
            pte.is_mapped = true;  
        }else{
            pte.real_addr = real_addr;
            real_memory[pte.real_addr].page.referenced_counter++; // update counter
            real_memory[pte.real_addr].page.R = 1; // was recently referenced ! (in the last cycle)
        }
    }
    return 0;
}


// returns the last recently used page 
// and changes lib_addr to the freed address
// on the real memory
page_t lru_page(uint8_t *lib_addr){
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
