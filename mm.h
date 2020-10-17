#ifndef __MM__
#define __MM__

#include <stdint.h>
#include "gluethread/glthread.h"
#include <stddef.h> /*for size_t*/


typedef enum{

    MM_FALSE,
    MM_TRUE
} vm_bool_t;

typedef struct block_meta_data_{

    vm_bool_t is_free;
    uint32_t block_size;
    uint32_t offset;    /*offset from the start of the page*/
    glthread_t priority_thread_glue;
    struct block_meta_data_ *prev_block;
    struct block_meta_data_ *next_block;
} block_meta_data_t;
GLTHREAD_TO_STRUCT(glthread_to_block_meta_data, 
    block_meta_data_t, priority_thread_glue, glthread_ptr);

#define offset_of(container_structure, field_name)  \
    ((size_t)&(((container_structure *)0)->field_name))

/*Forward Declaration*/
struct vm_page_family_;

typedef struct vm_page_{
    struct vm_page_ *next;
    struct vm_page_ *prev;
    struct vm_page_family_ *pg_family; /*back pointer*/
    uint32_t page_index;
    block_meta_data_t block_meta_data;
    char page_memory[0];
} vm_page_t;

#define MM_GET_PAGE_FROM_META_BLOCK(block_meta_data_ptr)    \
    ((vm_page_t *)((char *)block_meta_data_ptr - block_meta_data_ptr->offset))

#define NEXT_META_BLOCK(block_meta_data_ptr)    \
    (block_meta_data_ptr->next_block)

#define NEXT_META_BLOCK_BY_SIZE(block_meta_data_ptr)    \
    (block_meta_data_t *)((char *)(block_meta_data_ptr + 1) \
        + block_meta_data_ptr->block_size)

#define PREV_META_BLOCK(block_meta_data_ptr)    \
    (block_meta_data_ptr->prev_block)

#define mm_bind_blocks_for_allocation(allocated_meta_block, free_meta_block)  \
    free_meta_block->prev_block = allocated_meta_block;        \
    free_meta_block->next_block = allocated_meta_block->next_block;    \
    allocated_meta_block->next_block = free_meta_block;                \
    if (free_meta_block->next_block)\
    free_meta_block->next_block->prev_block = free_meta_block

#define mm_bind_blocks_for_deallocation(freed_meta_block_top, freed_meta_block_down)    \
    freed_meta_block_top->next_block = freed_meta_block_down->next_block;               \
    if(freed_meta_block_down->next_block)                                               \
    freed_meta_block_down->next_block->prev_block = freed_meta_block_top

vm_bool_t
mm_is_vm_page_empty(vm_page_t *vm_page);

#define MM_MAX_STRUCT_NAME 32
typedef struct vm_page_family_{

    char struct_name[MM_MAX_STRUCT_NAME];
    uint32_t struct_size;
    vm_page_t *first_page;
    glthread_t free_block_priority_list_head;
    
    /*Statistics*/
    uint32_t total_memory_in_use_by_app;
    uint32_t no_of_system_calls_to_alloc_dealloc_vm_pages;
} vm_page_family_t;

static inline block_meta_data_t *
mm_get_biggest_free_block_page_family(
        vm_page_family_t *vm_page_family){

    glthread_t *biggest_free_block_glue = 
        vm_page_family->free_block_priority_list_head.right;
    
    if(biggest_free_block_glue)
        return glthread_to_block_meta_data(biggest_free_block_glue);

    return NULL;
}

vm_page_t *
allocate_vm_page();

#define MARK_VM_PAGE_EMPTY(vm_page_t_ptr)                                 \
    vm_page_t_ptr->block_meta_data.next_block = NULL;                     \
    vm_page_t_ptr->block_meta_data.prev_block = NULL;                     \
    vm_page_t_ptr->block_meta_data.is_free = MM_TRUE

#define MM_GET_NEXT_PAGE_IN_HEAP_SEGMENT(vm_page_t_ptr, incr)   \
    ((incr == '+') ? ((vm_page_t *)((char *)vm_page_t_ptr + GB_SYSTEM_PAGE_SIZE)): \
                     ((vm_page_t *)((char *)vm_page_t_ptr - GB_SYSTEM_PAGE_SIZE)))

void
mm_init();

#define N_PAGE_FAMILY_PER_VM_PAGE   \
    (GB_SYSTEM_PAGE_SIZE/sizeof(vm_page_family_t))

#define VM_PAGE_FAMILY_RESIDUAL_SPACE   \
    (GB_SYSTEM_PAGE_SIZE - (N_PAGE_FAMILY_PER_VM_PAGE * sizeof(vm_page_family_t)))

#define ITERATE_PAGE_FAMILIES_BEGIN(first_vm_page_family_ptr, curr)   \
{                                                                     \
    uint32_t count = 0; curr = NULL;                                  \
    for(curr = (vm_page_family_t *)first_vm_page_family_ptr;          \
        count != gb_no_of_vm_families_registered;                     \
        count++, curr++){                                             \
        if(count == N_PAGE_FAMILY_PER_VM_PAGE){                       \
            curr = (vm_page_family_t *)((char *)curr +                \
                VM_PAGE_FAMILY_RESIDUAL_SPACE);                       \
            count = 0;                                                \
        }

#define ITERATE_PAGE_FAMILIES_END(first_vm_page_family_ptr, curr) \
    }}

vm_page_family_t *
lookup_page_family_by_name(char *struct_name);

#define ITERATE_VM_PAGE_PER_FAMILY_BEGIN(vm_page_family_ptr, curr)   \
{                                             \
    curr = vm_page_family_ptr->first_page;    \
    vm_page_t *next = NULL;                   \
    for(; curr; curr = next){           \
        next = curr->next;

#define ITERATE_VM_PAGE_PER_FAMILY_END(vm_page_family_ptr, curr)   \
    }}

#define ITERATE_VM_PAGE_ALL_BLOCKS_BEGIN(vm_page_ptr, curr)    \
{\
    curr = &vm_page_ptr->block_meta_data;\
    block_meta_data_t *next = NULL;\
    for( ; curr; curr = next){\
        next = NEXT_META_BLOCK(curr);

#define ITERATE_VM_PAGE_ALL_BLOCKS_END(vm_page_ptr, curr)   \
    }}

#define ITERATE_HEAP_SEGMENT_PAGE_WISE_BEGIN(vm_page_begin_ptr, curr)   \
{                                                               \
    void *heap_segment_end = sbrk(0);                           \
    for(curr = (vm_page_t *)vm_page_begin_ptr;                  \
        (void *)curr != heap_segment_end;                       \
        curr = MM_GET_NEXT_PAGE_IN_HEAP_SEGMENT(curr, '+')){    \

#define ITERATE_HEAP_SEGMENT_PAGE_WISE_END(vm_page_begin_ptr, curr) \
    }}   

void mm_vm_page_delete_and_free(vm_page_t *vm_page);
#endif /**/
