#include "definitions.h"

mem_slot_t real_memory[N_SLOTS_RM];
mem_slot_t swap[N_SLOTS_SWAP];

// process table stores the mapping between VM and RM
page_table_entry_t page_table[N_SLOTS_VM];

int reference_page(uint8_t addr){
    if(addr >= N_SLOTS_VM){
        return -1;//invalid virtual address
    }
    // lookup page table entry of given address
    page_table_entry_t pte = page_table[addr];

    if(pte.is_mapped){
        // reference page
        real_memory[pte.real_addr].page.referenced_counter++;
    }else{
        // page miss 

    }

    return 0;
}
