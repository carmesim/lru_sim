#include <stdio.h>
#include <stdlib.h>
#include "definitions.h"
#include "binutils.h"

mem_slot_t real_memory[N_SLOTS_RM];
sw_mem_slot_t swap[N_SLOTS_SWAP];

// process table stores the mapping between VM and RM
page_table_entry_t page_table[N_SLOTS_VM];

char* converte_n_bin(int x, int n){
    // 30 é maior do que qualquer campo
    char str[30] = {'\0'};
    char* inv = calloc(30, sizeof(char));
    int tam = 0;
    while(x >= 2){
        if(x%2 == 0)
            str[tam++] = '0';
        else
            str[tam++] = '1';
        x /= 2;
    }
    if(x == 0)
        str[tam] = '0';
    else
        str[tam] = '1';
    int i, extra = n - tam - 1;
    for(i = 0; i < extra; i ++){
        inv[i] = '0';
    }
    for(i = 0; i <= tam; i ++){
        inv[extra + i] = str[tam - i];
    }
    return inv;
}

static int8_t get_free_real_address() {
    for(int8_t i = 0; i < N_SLOTS_RM; i++) {
        if(real_memory[i].page.is_free) {
            //printf("Page %d is free\n", i);
            return i;
        }
        //printf("Page %d isn't free\n", i);
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

// set referenced bit of all pages as 0 
void unreference_all_pages() {
    for(int8_t i = 0; i < N_SLOTS_RM; i++) {
        real_memory[i].page.R = 0;
    }
}

// set the virtual address that was mapped on the lru as unmapped
void unmap_address(int8_t real_addr){
    int i;
    for(i = 0; i < N_SLOTS_VM; i++){
        if(page_table[i].real_addr == real_addr){
            page_table[i].is_mapped = false;
            break;
        }
    }
}

// returns the address on swap of an old real memory address
int8_t get_swap_address(int8_t end_real){
    int i;
    for (i = 0; i < N_SLOTS_SWAP; i++){
        if(swap[i].old_addr == end_real){
            return i;
        }
    }
    return -1;
}

// remove a page from swap and returns it
page_t remove_from_swap(int8_t swap_addr){
    swap[swap_addr].page.is_free = 0;
    return swap[swap_addr].page;
}

// update counters based on the referenced bit (R)
void update_counters(){
    int i;
    for (i = 0; i < N_SLOTS_RM; i++){
        // update counter
        printf("RM[%d].page.R = %d\n", i, real_memory[i].page.R);
        if(real_memory[i].page.R == 1){
            if(real_memory[i].page.referenced_counter != 0){
                real_memory[i].page.referenced_counter /=2 ; // update counter;
            }else{
                real_memory[i].page.referenced_counter = 0 ; // update counter;
            }
            real_memory[i].page.referenced_counter += 128; // update counter;
        }else{
            if(real_memory[i].page.referenced_counter != 0){
                real_memory[i].page.referenced_counter /=2 ; // update counter;
            }else{
                real_memory[i].page.referenced_counter = 0 ; // update counter;
            }
        }
    }
}


int reference_page(int8_t addr) {
    if(addr >= N_SLOTS_VM) {
        // invalid virtual address
        return -1;
    }
    
    // lookup page table entry of given address
    page_table_entry_t pte = page_table[addr];

    if(pte.is_mapped){
        // reference page
        // assuming it's on the real memory
        //printf("Page in real address %d was referenced\n", pte.real_addr);

        real_memory[pte.real_addr].page.R = 1; // was recently referenced ! (in the last cycle)

    }else{
        // page miss
        printf("Page Miss !\n");
        int8_t real_addr = get_free_real_address();// -1 se não achar

        if(real_addr == -1){//real memory is full

            int8_t swap_addr = get_swap_address(pte.real_addr);
            page_t swap_page;
            bool from_swap = 0;

            if(swap_addr != -1){ // está na swap
                swap_page = remove_from_swap(swap_addr);
                from_swap = 1;
            }else{
                // get a new swap address to recieve the oldest page
                swap_addr = get_free_swap_address();// tem que estar entre N_SLOTS_RM
            }

            // find lru page and puts it into swap
            int8_t liberated_adrr; //liberated address
            swap[swap_addr].page = lru_page(&liberated_adrr);
            printf("Page mapped to %d\n", liberated_adrr);
            printf("obs:The removed page went to swap[%d]\n", swap_addr);

            // set the virtual address that was mapped on the lru as unmapped
            unmap_address(liberated_adrr);

            // the freed address will recieve the new page
            real_memory[liberated_adrr].page.R = 1; // was recently referenced ! (in the last cycle)

            if(from_swap == 1){
                real_memory[liberated_adrr].page.content = swap_page.content;
            }else{
                real_memory[liberated_adrr].page.content = rand()%MAX_CONTENT_VAL;
            }

            pte.real_addr = liberated_adrr; // store it in the page table
            pte.is_mapped = 1;  
            page_table[addr] = pte;
        }else{
            pte.real_addr = real_addr;
            pte.is_mapped = 1;

            real_memory[pte.real_addr].page.content = rand()%MAX_CONTENT_VAL;
            real_memory[pte.real_addr].page.R = 1; // was recently referenced ! (in the last cycle)
            real_memory[pte.real_addr].page.is_free = false; // set the page as linked to a virtual address

            page_table[addr] = pte;//update page table entry
            printf("Page mapped to %d\n", real_addr);
        }
    }
    return 0;
}


// returns the last recently used page 
// and changes lib_addr to the freed address
// on the real memory
page_t lru_page(int8_t *lib_addr){
    int i, pi_counter = 0, n_zeros_page = 0, i_lru = 0, greatest;
    page_t lru;
    for (i = 0; i < N_SLOTS_RM; i++){
        pi_counter = real_memory[i].page.referenced_counter;
        n_zeros_page = count_zeroes(pi_counter);
        if(i == 0){
            greatest = n_zeros_page;
        }else{
            if(n_zeros_page > greatest){
                greatest = n_zeros_page;
                i_lru = i;
            }
        }
    }
    *lib_addr = i_lru;
    lru = real_memory[i_lru].page;
    return lru;
}
