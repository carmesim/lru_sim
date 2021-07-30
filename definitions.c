#include "definitions.h"

mem_slot_t real_memory[N_SLOTS_RM];
mem_slot_t swap[N_SLOTS_SWAP];

// process table stores the mapping between VM and RM
page_table_entry_t page_table[N_SLOTS_VM];

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
        //[TODO] update the referenced bit
    }else{
        // page fault
        uint8_t real_addr = get_free_real_address();
        if(real_addr == -1){//real memory is full
            // page miss
            
            // generate addr form N_SLOTS_RM to N_SLOTS_VM
            uint8_t swap_addr = rand()%N_SLOTS_VM + N_SLOTS_RM;

            // find lru page and puts it into swap
            uint8_t liberated_adrr; //liberated address
            swap[swap_addr].page = lru_page(&liberated_adrr);

            // the freed address will recieve the new page
            real_memory[liberated_adrr].page.referenced_counter = 1;

            pte.real_addr = lib_addr; // store it in the page table
            pte.is_mapped = true;  
        }else{
            real_memory[pte.real_addr].page.referenced_counter++;
        }
    }
    return 0;
}


page_t lru_page(uint8_t *lib_addr){
    // [TODO]
}
