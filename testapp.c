#include <stdio.h>
#include "uapi_mm.h"
#include <assert.h>

typedef struct emp_ {

    char name[32];
    uint32_t emp_id;
} emp_t;

typedef struct student_ {

    char name[32];
    uint32_t rollno;
    uint32_t marks_phys;
    uint32_t marks_chem;
    uint32_t marks_maths;
    struct student_ *next;
} student_t;

int
main(int argc, char **argv){

    mm_init();
    MM_REG_STRUCT(emp_t);
    MM_REG_STRUCT(student_t);
#if 0
    emp_t *emp1 = xcalloc("emp_t", 1);
    emp_t *emp2 = xcalloc("emp_t", 1);
    emp_t *emp3 = xcalloc("emp_t", 1);
    emp_t *emp4 = xcalloc("emp_t", 3);
    student_t *stud1 = xcalloc("student_t", 1);
    student_t *stud2 = xcalloc("student_t", 2);
    student_t *stud3 = xcalloc("student_t", 1);
    mm_print_memory_usage();
    xfree(emp1);
    mm_print_memory_usage();
    xfree(emp2);
    mm_print_memory_usage();
    xfree(emp3);
    mm_print_memory_usage();
    xfree(emp4);
    mm_print_memory_usage();
    xfree(stud1);
    mm_print_memory_usage();
    xfree(stud2);
    mm_print_memory_usage();
    xfree(stud3);
    mm_print_memory_usage();
#endif
    int i = 0;
    student_t *stud = NULL, *prev = NULL;
    student_t *first = NULL;
    for( ; i < 120; i++){
        stud = xcalloc("student_t", 1);
        if(i == 0)
            first = stud;
        assert(stud);
        if(prev){
            prev->next = stud;
        }
        prev = stud;
    }
    mm_print_memory_usage(0);
    mm_print_block_usage();
    #if 1
    i = 0;
    student_t *next = NULL;
    for( ; first; first = next){
        next = first->next;
        if(1 || i%4 == 0)
        xfree(first);
        i++;
    }
    mm_print_memory_usage(0);
    mm_print_block_usage();
    #endif
    return 0;
}
