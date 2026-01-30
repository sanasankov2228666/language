#include <stdio.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include "../../stack/headers/stack1.h"
#include "enum.h"
#include "str_func.h"
#include "assembler.h"

//!версия ассемблера
#define VERSION 1

struct commands
{
    const char* name;
    const int number;
    const int arg;
    size_t hash;
};

//!массив комманд
static struct commands commands[NUMBER_COMMANDS] =
{
    {"PUSH",   PUSH,   1, hash_func("PUSH")    },
    {"ADD",    ADD,    0, hash_func("ADD")     },
    {"SUB",    SUB,    0, hash_func("SUB")     },
    {"DIV",    DIV,    0, hash_func("DIV")     },
    {"MULT",   MULT,   0, hash_func("MULT")    },
    {"SQRT",   SQvRT,   0, hash_func("SQRT")    },
    {"IN",     IN,     0, hash_func("IN")      },
    {"HLT",    HLT,    0, hash_func("HLT")     },
    {"OUT",    OUT,    0, hash_func("OUT")     },
    {"PUSHREG",PUSHREG,1, hash_func("PUSHREG") },
    {"POPREG", POPREG, 1, hash_func("POPREG")  },
    {"PUSHM",  PUSHM,  1, hash_func("PUSHM")   },
    {"POPM",   POPM,   1, hash_func("POPM")    },
    {"JUMP",   JUMP,   1, hash_func("JUMP")    },
    {"JB",     JB,     1, hash_func("JB")      },
    {"JBE",    JBE,    1, hash_func("JBE")     },
    {"JA",     JA,     1, hash_func("JA")      },
    {"JAE",    JAE,    1, hash_func("JAE")     },
    {"JE",     JE,     1, hash_func("JE")      },
    {"JNE",    JNE,    1, hash_func("JNE")     },
    {"CALL",   CALL,   1, hash_func("CALL")    },
    {"RET",    RET,    0, hash_func("RET")     },
    {"DRAW",   DRAW,   0, hash_func("DRAW")    }
};

size_t hash_func(const char* str)
{
    size_t index = 0;
    size_t hash = 0;
    size_t shift = 3;

    while (str[index] != '\0' && index < 10)  // Ограничиваем длину для безопасности
    {
        hash += (size_t) str[index] * (1 << (shift * index));
        index++;
    }

    return hash;
}

int hash_cmp(const void* elem_1,const void* elem_2)
{
    const struct commands* frst = (const struct commands*) elem_1;
    const struct commands* scnd = (const struct commands*) elem_2;

    if (frst->hash < scnd->hash) return -1;
    if (frst->hash > scnd->hash) return 1;

    return 0;
}

void hash_sort()
{
    qsort(commands, NUMBER_COMMANDS, sizeof(struct commands), hash_cmp);
}

int hash_search_cmp(const void* key, const void* element)
{
    size_t key_hash = *(size_t*)key;
    const struct commands* cmd = (const struct commands*)element;
    
    if (key_hash < cmd->hash) return -1;
    if (key_hash > cmd->hash) return 1;

    return 0;
}

//---------------------------------------------СОЗДАНИЕ-БАЙТ-КОДА-----------------------------------------------------------------------------

//!функция для создания байткода
size_t byte_code_maker(struct main_str* assembler, int asm_number)
{
    size_t pc_code = 1;
    size_t str_c = 0;
    struct commands* ptr_command = NULL;

    for (; (pc_code < assembler->len && str_c < assembler->len) ; pc_code++, str_c++)
    {
        if ((assembler->mas_str[str_c])[0] == '\0') return pc_code;
        while ((assembler->mas_str[str_c])[0] == '\n') str_c++;

        if ((assembler->mas_str[str_c])[0] == ':')
        {
            check_comments (assembler->mas_str[str_c]);

            if (proccesing_label(assembler, &pc_code, &str_c, asm_number)) return ERROR;
            pc_code--;
        }

        else if ((assembler->mas_str[str_c])[0] == ';') pc_code--;

        else
        {
            check_comments (assembler->mas_str[str_c]);

            ptr_command = recognizing_command(assembler, &pc_code, &str_c);
            if (ptr_command == NULL) return ERROR;

            if (ptr_command->arg == 1)
            {
                pc_code++;
                str_c++;

                if (proccesing_witharg(assembler, &pc_code, &str_c, asm_number)) return ERROR;
            }
        }
    }

    return pc_code;
}

//!распознавание комманды
struct commands* recognizing_command(struct main_str* assembler, size_t* pc_code, size_t* str_c)
{
    size_t hash = 0;
    struct commands* ptr_command = NULL;

    hash = hash_func(assembler->mas_str[*str_c]);
        
    ptr_command = (struct commands*) bsearch( &hash, commands, NUMBER_COMMANDS, sizeof(struct commands), hash_search_cmp);
    if (ptr_command == NULL)
    {
        printf("unrecognize command [%s] str: %zu\n", assembler->mas_str[*str_c], *str_c);
        return NULL;
    }

    if (strcmp(assembler->mas_str[*str_c], ptr_command->name) != 0)
    {
        printf("command name mismatch: expected %s, got %s str: %zu\n", 
               ptr_command->name, assembler->mas_str[*str_c], *str_c);
        return NULL;
    }

    assembler->buffer_out[*pc_code] = ptr_command->number;
    return ptr_command;
}

//!функция обработки метки
asm_err_t proccesing_label(struct main_str* assembler, size_t* pc_code, size_t* str_c, int asm_number)
{
    if (asm_number == 2) return SUCCSES;

    char* ptr = assembler->mas_str[*str_c] + 1;
    int arg_val = 0;

    if ( (sscanf(ptr, "%d", &arg_val)) == 0)
    {
        if (str_label_processing(assembler, pc_code, str_c)) return ERROR;
    }
    else
    {
        if (arg_val < 0)
        {
            LABLE_MISSTAKE_OUT;
        }

        assembler->lables.num_labels[arg_val] = (int) *pc_code;
    }

    return SUCCSES;
}

//!функция обработки комманды с аргументом
asm_err_t proccesing_witharg(struct main_str* assembler, size_t* pc_code, size_t* str_c, int asm_number)
{
    char* curr_str = assembler->mas_str[*str_c];
    int arg_val = 0;

    if (curr_str[0] == ':')
    {
        curr_str++;
        if (sscanf(curr_str, "%d", &arg_val) == 0)
        {
            if (proccesing_arg_label(assembler, curr_str, pc_code, str_c, asm_number)) return ERROR;
        }
    }
    else if (curr_str[0] == '[')
    {
        curr_str++;
        if (check_mem_arg(assembler, curr_str, pc_code, str_c)) return ERROR;

        char reg_name[3] = {0};
        reg_name[0] = toupper(curr_str[0]);
        reg_name[1] = 'X';
        
        assembler->buffer_out[*pc_code] = reg_name[0] - 'A';
    }
    else
    {
        if (sscanf(curr_str, "%d", &arg_val) == 0)
        {
            if (assembler->buffer_out[*pc_code - 1] != POPREG && 
                assembler->buffer_out[*pc_code - 1] != PUSHREG)
            {
                printf("bad syntax: expected number or register, got '%s' str: %zu\n", 
                       curr_str, *str_c);
                return ERROR;
            }

            if (strlen(curr_str) < 2 || !isalpha(curr_str[0]) || 
                toupper(curr_str[1]) != 'X')
            {
                printf("bad register syntax: expected format like 'AX', got '%s' str: %zu\n",
                       curr_str, *str_c);
                return ERROR;
            }

            assembler->buffer_out[*pc_code] = toupper(curr_str[0]) - 'A';
        }
        else 
        {
            assembler->buffer_out[*pc_code] = arg_val;
        }
    }

    return SUCCSES;
}

//-------------------------------------------------ВСПОМОГАТЕЛЬНЫЕ-ФУНКЦИИ---------------------------------------------------------

asm_err_t str_label_processing (struct main_str* assembler, size_t* pc_code, size_t* str_c)
{
    char* ptr = assembler->mas_str[*str_c] + 1;
    char name_label[MAX_NAME_LBL] = {};

    assembler->lables.current_ptr++;
    if (assembler->lables.current_ptr >= MAX_NAME_LBL)
    {
        printf("max number of labels is reached str: %zu", *str_c);
        return ERROR;
    }

    sscanf(ptr, "%s", name_label);
    if (equal_label_check(assembler, name_label, str_c)) return ERROR;
    sscanf(ptr, "%s", assembler->lables.str_labels[assembler->lables.current_ptr].name);

    assembler->lables.str_labels[assembler->lables.current_ptr].num = (int) *pc_code;
    return SUCCSES;
}

asm_err_t equal_label_check(struct main_str* assembler, char* name_label, size_t* str_c)
{
    // Проверяем только заполненные метки
    for (int i = 0; i <= assembler->lables.current_ptr; i++)
    {
        if (assembler->lables.str_labels[i].name[0] != '\0' &&
            my_strcmp(name_label, assembler->lables.str_labels[i].name) == 0)
        {
            printf("label '%s' already exists str: %zu\n", name_label, *str_c);
            return ERROR;
        }
    }

    return SUCCSES;
}

asm_err_t proccesing_arg_label(struct main_str* assembler, char* curr_str, size_t* pc_code, size_t* str_c, int asm_number )
{
    char name_label[MAX_NAME_LBL] = {};
    int cheaker = 1;
    int index = 0;

    sscanf(curr_str, "%s", name_label);
    
    // Ищем метку
    for (index = 0; index <= assembler->lables.current_ptr; index++)
    {
        if (assembler->lables.str_labels[index].name[0] != '\0')
        {
            cheaker = my_strcmp(name_label, assembler->lables.str_labels[index].name);
            if (cheaker == 0) break;
        }
    }

    if (cheaker != 0)
    {
        if (asm_number == 2)
        {
            printf("label '%s' does not exist str: %zu\n", name_label, *str_c);
            return ERROR;
        }
        else
        {
            printf("warning: forward reference to label '%s' str: %zu\n", name_label, *str_c);
            assembler->buffer_out[*pc_code] = 0;
            return SUCCSES;
        }
    }

    assembler->buffer_out[*pc_code] = assembler->lables.str_labels[index].num;
    return SUCCSES;
}

//!функция проверки синтаксиса для функция RAM
asm_err_t check_mem_arg(struct main_str* assembler, char* curr_str, size_t* pc_code, size_t* str_c)
{
    int index = 0;

    // Проверяем синтаксис регистра
    if (check_reg_syntax(curr_str, str_c) != SUCCSES) {
        return ERROR;
    }

    // Проверяем, что предыдущая команда не PUSHREG/POPREG (для них не нужно [])
    if (assembler->buffer_out[*pc_code - 1] == PUSHREG ||
        assembler->buffer_out[*pc_code - 1] == POPREG)
    {
        printf("bad syntax str: %zu - PUSHREG/POPREG don't need brackets\n", *str_c);
        return ERROR;
    }

    // Ищем закрывающую скобку
    while (curr_str[index] != ']' && curr_str[index] != '\0') {
        index++;
    }

    if (curr_str[index] != ']') {
        printf("bad syntax str: %zu - missing closing bracket ']'\n", *str_c);
        return ERROR;
    }

    // Проверяем, что после скобки ничего лишнего
    if (curr_str[index + 1] != '\0' && curr_str[index + 1] != ' ') {
        printf("bad syntax str: %zu - extra characters after ']'\n", *str_c);
        return ERROR;
    }

    return SUCCSES;
}

//!функция проверки синтаксиса регистров
asm_err_t check_reg_syntax(char* curr_str, size_t* str_c)
{

    if (strlen(curr_str) < 3)
    {
        printf("bad syntax reg arg str: %zu - too short\n", *str_c);
        return ERROR;
    }
    
    if (!isalpha(curr_str[0]))
    {
        printf("bad syntax reg arg str: %zu - not a letter\n", *str_c);
        return ERROR;
    }
    
    if (toupper(curr_str[1]) != 'X')
    {
        printf("bad syntax reg arg str: %zu - expected 'X' after register\n", *str_c);
        return ERROR;
    }
    
    if (curr_str[2] == ']')
    {
        return SUCCSES;
    }
    
    if (curr_str[2] == '\0' || curr_str[2] == ' ')
    {
        return SUCCSES;
    }
    
    printf("bad syntax reg arg str: %zu - unexpected character '%c'\n", *str_c, curr_str[2]);
    return ERROR;
}

//! Проверка строки на наличие коментариев
void check_comments (char* str)
{
    char* comment = strchr (str, ';');
    if (comment) *comment = '\0';
}


//------------------------------------------------------------------------------------------------------------------------------------------

int asm_deleter(struct main_str* assembler, char* buffer)
{
    free(buffer);
    free(assembler->buffer_out);
    free(assembler->mas_str);

    printf("asm deleted\n");
    return SUCCSES;
}

//!функция распечатки байт кода
int check_byte_code(int* mass, size_t len)
{
    printf("___________________________\n");
    printf("|      commands_buffer    |\n");
    printf("___________________________\n");
    for (size_t i = 1; i < len; i++)
    {
        printf("||%5zu: command | %5d||\n", i, mass[i]);
    }
    printf("___________________________\n");

    return 1;
}

//!функция создающая сигнатуру
int* signature_maker(int* code)
{
    ((char*)code)[1] = 'K';
    ((char*)code)[2] = 'K';
    ((char*)code)[3] = '1';

    return code;
}