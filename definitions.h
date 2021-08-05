#ifndef DEFINITIONS_H
#define DEFINITIONS_H
#include <stdint.h>
#include <stdbool.h>

#define N_SLOTS_RM 8
#define N_SLOTS_VM 16
#define N_SLOTS_SWAP 8 // |SW| >= |VM| - |RM|

#define MAX_CONTENT_VAL 1000

typedef struct{
    uint8_t referenced_counter;
    int R;// referenced bit
    bool is_free;// true when the memory page is unused
    int content;
}page_t;

typedef struct {
    page_t page;
}mem_slot_t;


typedef struct {
    page_t page;
    int8_t old_addr;
}sw_mem_slot_t;

extern mem_slot_t real_memory[N_SLOTS_RM];
//mem_slot_t virtual_memory[N_SLOTS_VM];
extern sw_mem_slot_t swap[N_SLOTS_SWAP];

typedef struct{
    bool is_mapped;
    int8_t real_addr; // between 0 and N_SLOTS_RM - 1
}page_table_entry_t;

// process table stores the mapping between VM and RM
extern page_table_entry_t page_table[N_SLOTS_VM];

int reference_page(int8_t addr);

// returns the last recently used page 
// and changes lib_addr to the freed address
// on the real memory
page_t lru_page(int8_t *lib_addr);
void init_pages_as_free();
void unreference_all_pages();
int8_t get_swap_address(int8_t end_real);
page_t remove_from_swap(int8_t swap_addr);
void update_counters();

//impostor --- must be removed
char* converte_n_bin(int x, int n);

#endif // DEFINITIONS_H
