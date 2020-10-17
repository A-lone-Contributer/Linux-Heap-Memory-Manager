#ifndef __UAPI_MM__
#define __UAPI_MM__

#include <stdint.h>

void *
xcalloc(char *struct_name, int units);

void
xfree(void *app_ptr);

void
mm_instantiate_new_page_family(
        char *struct_name,
        uint32_t struct_size);


/*
 * Public APIs Exposed to the Application using Memory Manager
 */

/*Printing Functions*/
void mm_print_memory_usage(char *struct_name);
void mm_print_block_usage();

/*Initialization Functions*/
void
mm_init();

/*Registration function*/
#define MM_REG_STRUCT(struct_name)  \
    (mm_instantiate_new_page_family(#struct_name, sizeof(struct_name)))

/*Allocators and De-Allocators*/
#define XCALLOC(units, struct_name) \
    (xcalloc(#struct_name, units))

#define XFREE(ptr)  \
    xfree(ptr)

#endif /* __UAPI_MM__ */
