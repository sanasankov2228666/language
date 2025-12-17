#ifndef SPU_H_
#define SPU_H_

#include <stdio.h>
#include <math.h>
#include "enum.h"
#include "str_func.h"

//! разрешение окна вывода
#define HORIZONTAL_LEN  100
#define VERTICAL_LEN    40

//! параментры для прцоессора
#define RAM_CAPACITY 4000
#define NUM_REG      8

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

typedef int cmd_err_t;
typedef cmd_err_t (*cmd_spu) (struct spu*);

//!дебаг
#ifdef DEBUG
#define DBG(...) printf(__VA_ARGS__)
#else
#define DBG(...) //
#endif

#ifdef VIDEO_MODE
#define VIDEO(...)  __VA_ARGS__
#define BASIK_OUT(...)
#else
#define VIDEO(...) 
#define BASIK_OUT(...)  __VA_ARGS__
#endif

#define CHECKER_FUNC int error = check_func(*data_spu);\
                     if (error) return error;

#define REG_PROTECTOR if (register_protection(data_spu->code [data_spu->counter], data_spu->stream_error)) return ERROR;

#define ADRES_PROTECTOR if (data_spu->code[data_spu->counter] < 0 || data_spu->code[data_spu->counter] >= (int)data_spu->len)\
                        {\
                            fprintf(data_spu->stream_error, "adres %d doesnt exist\n", data_spu->code[data_spu->counter]);\
                            \
                            return ERROR;\
                        }

enum
{
    SUCCSES = 0,
    ERROR   = 1,
};

//----------------------------------------------------ОБЕРТКИ-ПРОЦЕССОРА------------------------------------------------------------------------

#define OPERATION(data_spu, operate)\
        \
        stack_push(&data_spu->main_stk, pop1 operate pop2);\
        DBG("operation %c, %d %c %d = %d\n", #operate[0], pop1, #operate[0], pop2, pop1 operate pop2);\
        data_spu->counter++;

#define JUMP_COND(data_spu, condition)\
        \
        pop1 = stack_pop(&data_spu->main_stk);\
        pop2 = stack_pop(&data_spu->main_stk);\
        \
        data_spu->counter++;\
        \
        if (pop2 condition pop1)\
        {\
            ADRES_PROTECTOR;\
            data_spu->counter = (size_t)data_spu->code[data_spu->counter];\
            DBG("jumped on %zu command\n", data_spu->counter);\
        }\
        \
        else data_spu->counter++;

//----------------------------------------------------------------------------------------------------------------------------------------------

struct spu
{
    struct stk main_stk = {};
    struct stk data_func = {};
    int* code = NULL;
    size_t len = 0;
    size_t counter = 1;
    int reg [NUM_REG] = {};
    int* ram = NULL;
    FILE* stream_error = NULL;
};

//--------------------------------------------------------------ПРОЦЕССОР-----------------------------------------------------------------------
int spu_launcher(struct spu* data_spu);

//--------------------КОМАНДЫ--------------------
int cmd_push        (struct spu* data_spu);
int cmd_out         (struct spu* data_spu);
int cmd_calc        (struct spu* data_spu);
int cmd_jumps_cond  (struct spu* data_spu);
int cmd_jump        (struct spu* data_spu);
int cmd_sqrt        (struct spu* data_spu);
int cmd_in          (struct spu* data_spu);
int cmd_pushreg     (struct spu* data_spu);
int cmd_popreg      (struct spu* data_spu);
int cmd_popm        (struct spu* data_spu);
int cmd_pushm       (struct spu* data_spu);
int cmd_call        (struct spu* data_spu);
int cmd_ret         (struct spu* data_spu);
int cmd_draw        (struct spu* data_spu);
//------------------------------------------------

int* mass_cmd_maker(FILE* stream, size_t len);

int signature_version_check(int* code);

int register_protection(int number_reg, FILE* stream_out);

int check_func(struct spu data_spu);

void create_spu(struct spu* data_spu);

void spu_deleter (struct spu* data_spu);

int spu_dump(struct spu data_spu, const char* func, const char* file, int LINE);

#endif
