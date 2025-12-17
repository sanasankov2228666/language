#ifndef ASSEMBLER_H_
#define ASSEMBLER_H_
#include <stdio.h>
#include <math.h>

#define NUM_LBL_NUMBER 10
#define STR_LBL_NUMBER 10
#define MAX_NAME_LBL 15

enum
{
    ERROR = 1,
    SUCCSES = 0,
};

typedef int asm_err_t;


#define LABLE_MISSTAKE_OUT printf("problems with label name str: %zu\n", *str_c);\
                           return ERROR;


#define BYTE_CODE_ERROR if (check_ret == 1)\
                        {\
                            asm_deleter(&assembler, buffer_commands);\
                            return ERROR;\
                        }

struct str_label
{
    int num = -1;
    char name[MAX_NAME_LBL] = {};
};

struct labels
{
    struct str_label str_labels[STR_LBL_NUMBER] = {};
    int num_labels[NUM_LBL_NUMBER] = {};
    int current_ptr = -1;
};

struct main_str
{
    struct labels lables = {};
    int* buffer_out = NULL;
    char** mas_str = NULL;
    size_t len = 0;
    FILE* stream_out = NULL;
};


int* signature_maker(int* code);


//-----------------------функции-хэша----------------------

size_t hash_func(const char* str);

int hash_cmp(const void* elem_1,const void* elem_2);

void hash_sort();

int hash_search_cmp(const void* key, const void* element);

//---------------------------------------------------------


//----------------------------------------------------ФУНКЦИИ-БАЙТКОДА------------------------------------------------------------

size_t byte_code_maker(struct main_str* assembler, int asm_number);

asm_err_t proccesing_label(struct main_str* assembler, size_t* pc_code, size_t* str_c, int asm_number);

asm_err_t str_label_processing (struct main_str* assembler, size_t* pc_code, size_t* str_c);

asm_err_t proccesing_witharg(struct main_str* assembler, size_t* pc_code, size_t* str_c, int asm_number);

asm_err_t equal_label_check(struct main_str* assembler, char* name_label, size_t* str_c);

asm_err_t proccesing_arg_label(struct main_str* assembler, char* curr_str, size_t* pc_code, size_t* str_c, int asm_number );

asm_err_t check_mem_arg(struct main_str* assembler, char* curr_str, size_t* pc_code, size_t* str_c);

asm_err_t check_reg_syntax(char* curr_str, size_t* str_c);

struct commands* recognizing_command(struct main_str* assembler, size_t* pc_code, size_t* str_c);

//-------------------------------------------------------------------------------------------------------------------------------

int check_byte_code(int* mass, size_t len);

int asm_deleter(struct main_str* assembler, char* buffer);

#endif
