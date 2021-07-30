#include <stdlib.h>
#include "definitions.h"

mem_slot_t real_memory[N_SLOTS_RM];
mem_slot_t swap[N_SLOTS_SWAP];

// process table stores the mapping between VM and RM
page_table_entry_t page_table[N_SLOTS_VM];

static int8_t get_free_real_address() {
    for(int8_t i = 0; i < N_SLOTS_RM; i++) {
        if(real_memory[i].page.is_free) {
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

    if(pte.is_mapped){
        // reference page

        // assuming it's on the real memory
        real_memory[pte.real_addr].page.referenced_counter++;

    }else{
        // page fault
        int8_t real_addr = get_free_real_address();
        if(real_addr == -1){//real memory is full
            // page miss
            
            // generate addr form N_SLOTS_RM to N_SLOTS_VM
            uint8_t swap_addr = rand()%N_SLOTS_VM + N_SLOTS_RM;

            // find lru page and puts it into swap
            uint8_t lib_adrr;
            swap[swap_addr].page = lru_page(&lib_adrr);

            // the freed address will recieve the new page
            real_memory[lib_adrr].page.referenced_counter++;
            page_t new_page = malloc(sizeof(page_t));
            new_page.referenced_counter = 1;
            real_memory[lib_adrr].page = new_page;

            pte.real_addr = lib_addr;
        }else{

        }

        if(addr >= N_SLOTS_RM){ 
            // page miss
            uint8_t lib_adrr;
            swap[pte.real_addr].page = lru_page(&lib_adrr);
            real_memory[lib_adrr].page.referenced_counter++;
            page_t new_page = malloc(sizeof(page_t));
            new_page.referenced_counter = 1;
            real_memory[lib_adrr].page = new_page;
        }else{
            uint8_t real_addr = rand()%N_SLOTS_RM;
            page_t new_page = malloc(sizeof(page_t));
            new_page.referenced_counter = 1;
            real_memory[real_addr].page = new_page;
        }
    }
    return 0;
}


page_t lru_page(uint8_t *lib_addr){
    // [TODO]
}
