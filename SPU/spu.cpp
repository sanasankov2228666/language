#include "../stack/headers/stack1.h"
#include <unistd.h> 
#include <time.h>

//!включение режима дебага
//#define DEBUG

//!включение режима видео
#define VIDEO_MODE

#include "spu.h"

//!номер версии процессора
#define VERSION '1'

static struct commands
{
    int number = 0;
    int check_inf = 0;
    cmd_spu func = NULL;
}

//! массив структур дял команд
commands_arr[NUMBER_COMMANDS] =
{
    {PUSH,    0, cmd_push       },     {ADD,     2, cmd_calc       },
    {SUB,     2, cmd_calc       },     {DIV,     2, cmd_calc       },
    {MULT,    2, cmd_calc       },     {SQvRT,   1, cmd_sqrt       },
    {IN,      0, cmd_in         },     {HLT,     0, NULL           },
    {OUT,     1, cmd_out        },     {PUSHREG, 0, cmd_pushreg    },
    {POPREG,  1, cmd_popreg     },     {PUSHM,   0, cmd_pushm      },
    {POPM,    1, cmd_popm       },     {JUMP,    0, cmd_jump       },
    {JB,      2, cmd_jumps_cond },     {JBE,     2, cmd_jumps_cond },
    {JA,      2, cmd_jumps_cond },     {JAE,     2, cmd_jumps_cond },
    {JE,      2, cmd_jumps_cond },     {JNE,     2, cmd_jumps_cond },
    {CALL,    0, cmd_call       },     {RET,     0, cmd_ret        },
    {DRAW,    0, cmd_draw       }
};

//---------------------------------------------------------ПРОЦЕССОР----------------------------------------------------------------------------

//!процессор
int spu_launcher(struct spu* data_spu)
{
    int index = 0;
    int error = 0;

    DBG("\n[command]: %d\n{\n", data_spu->code[data_spu->counter]);

    index = data_spu->code[data_spu->counter] - 1;

    if (index < 0 || index > NUMBER_COMMANDS)
    {
        fprintf(data_spu->stream_error, "unrecognaize command %zu", data_spu->counter);
        return HLT;
    }

    if (data_spu->code[data_spu->counter] == HLT) return HLT;

    error = commands_arr[index].func(data_spu);
    if  (error) return HLT;


    DBG("}\n");
    return commands_arr[index].number;
}

cmd_err_t cmd_push( struct spu* data_spu )
{
    CHECKER_FUNC;
    data_spu->counter++;

    stack_push(&data_spu->main_stk, data_spu->code[data_spu->counter]);
    DBG("pushed %d\n", data_spu->code[data_spu->counter]);

    data_spu->counter++;

    return SUCCSES;
}

cmd_err_t cmd_out( struct spu* data_spu)
{
    CHECKER_FUNC;
    printf("** stack out = %d **\n", stack_pop(&data_spu->main_stk));

    data_spu->counter++;

    return 0;
}

cmd_err_t cmd_calc(struct spu* data_spu)
{
    CHECKER_FUNC;

    int swch = data_spu->code[data_spu->counter];
    int pop1 = stack_pop(&data_spu->main_stk);
    int pop2 = stack_pop(&data_spu->main_stk);

    switch (swch)
    {
    case ADD:

        OPERATION(data_spu, +)
        break;

    case SUB:

        OPERATION(data_spu, -);
        break;

    case MULT:

        OPERATION(data_spu, *);
        break;

    case DIV:
        if (pop2 == 0 ) return ERROR;

        OPERATION(data_spu, /);
        break;

    default:
        break;
    }

    return SUCCSES;
}

cmd_err_t cmd_jumps_cond(struct spu* data_spu)
{
    CHECKER_FUNC;

    int swch = data_spu->code[data_spu->counter];
    int pop1 = 0;
    int pop2 = 0;

    switch (swch)
    {
    case JB:
        JUMP_COND(data_spu, <)
        break;

    case JBE:
        JUMP_COND(data_spu, <=)
        break;

    case JA:
        JUMP_COND(data_spu, >)
        break;

    case JAE:
        JUMP_COND(data_spu, >=)
        break;

    case JE:
        JUMP_COND(data_spu, ==)
        break;

    case JNE:
        JUMP_COND(data_spu, !=)
        break;

    default:
        break;
    }

    return SUCCSES;
}

cmd_err_t cmd_jump(struct spu* data_spu)
{
    CHECKER_FUNC;
    data_spu->counter++;

    ADRES_PROTECTOR
    data_spu->counter = (size_t)data_spu->code[data_spu->counter];

    return 0;
}

cmd_err_t cmd_sqrt(struct spu* data_spu)
{
    CHECKER_FUNC;

    if (data_spu->main_stk.stack[data_spu->main_stk.size] < 0)
    {
        fprintf(data_spu->stream_error, "squre root parametr < 0\n");
        return ERROR;
    }

    int val = (int)(sqrt(stack_pop(&data_spu->main_stk)));
    stack_push(&data_spu->main_stk, val);
    DBG("the square root, resault = %d\n", val);

    data_spu->counter++;
    return SUCCSES;
}

cmd_err_t cmd_in(struct spu* data_spu)
{
    int val = 0;
    printf("push parametr:\n");

    int input_check = scanf("%d", &val);
    while (input_check == 0)
    {
        while (getchar() != '\n');
        printf("it must be a number\n");
        printf("rewrite push parametr:\n");
        input_check = scanf("%d", &val);
    }

    stack_push(&data_spu->main_stk, val);
    DBG("pushed %d\n", val);

    data_spu->counter++;
    return SUCCSES;
}

cmd_err_t cmd_pushreg(struct spu* data_spu)
{
    CHECKER_FUNC;
    data_spu->counter++;

    //REG_PROTECTOR
    stack_push(&data_spu->main_stk, data_spu->reg[data_spu->code[data_spu->counter]]);
    DBG("pushed from reg %c, value: %d\n",
    data_spu->code[data_spu->counter] + 65, data_spu->reg[data_spu->code[data_spu->counter]]);

    data_spu->counter++;
    return SUCCSES;
}

cmd_err_t cmd_popreg(struct spu* data_spu)
{
    CHECKER_FUNC;
    data_spu->counter++;

    REG_PROTECTOR;
    data_spu->reg[data_spu->code[data_spu->counter]] = stack_pop(&(data_spu->main_stk));
    DBG("reg %c poped %d\n", data_spu->code[data_spu->counter] + 65, data_spu->reg[data_spu->code[data_spu->counter]]);

    (data_spu->counter)++;
    return SUCCSES;
}

cmd_err_t cmd_popm(struct spu* data_spu)
{
    CHECKER_FUNC;

    data_spu->counter++;

    REG_PROTECTOR;
    if (data_spu->reg[ data_spu->code [data_spu->counter] ] >= RAM_CAPACITY)
    {
        fprintf(data_spu->stream_error, "the memory location does not exist %d\n",
        data_spu->reg[ data_spu->code [data_spu->counter] ]);
        return ERROR;
    }

    data_spu->ram[ data_spu->reg[ data_spu->code [data_spu->counter] ] ] = stack_pop(&data_spu->main_stk);

    data_spu->counter++;
    return SUCCSES;
}

cmd_err_t cmd_pushm(struct spu* data_spu)
{
    CHECKER_FUNC;
    data_spu->counter++;

    REG_PROTECTOR;
    if (data_spu->reg[ data_spu->code [data_spu->counter] ] >= RAM_CAPACITY)
    {
        fprintf(data_spu->stream_error, "the memory location does not exist\n");
        return ERROR;
    }

    stack_push(&data_spu->main_stk, data_spu->ram[ data_spu->reg[ data_spu->code [data_spu->counter] ] ]);

    data_spu->counter++;
    return SUCCSES;
}

cmd_err_t cmd_call(struct spu* data_spu)
{
    (data_spu->counter)++;
    stack_push(&(data_spu->data_func), (int) ((data_spu->counter) + 1));

    ADRES_PROTECTOR;
    data_spu->counter = (size_t) data_spu->code[data_spu->counter];
    DBG("call on %zu command\n", data_spu->counter);

    return SUCCSES;
}

cmd_err_t cmd_ret(struct spu* data_spu)
{
    data_spu->counter++;

    data_spu->counter = (size_t) stack_pop(&data_spu->data_func);
    ADRES_PROTECTOR;

    DBG("ret on %zu command", data_spu->counter);

    return SUCCSES;
}

BASIK_OUT(
cmd_err_t cmd_draw(struct spu* data_spu)
{
    printf("\n");
    printf("--------------------V-RAM-OUTPUT--------------------\n");
    for (size_t i_v = 0; i_v < VERTICAL_LEN; i_v++ )
    {
        for ( int i_h = 0; i_h < HORIZONTAL_LEN; i_h++)
        {
            printf(ANSI_COLOR_RED" %c " ANSI_COLOR_RESET,  data_spu->ram[i_h + HORIZONTAL_LEN * i_v] );
        }

        printf("   |\n");
    }
    printf("----------------------------------------------------\n");

    data_spu->counter++;

    return SUCCSES;
});

VIDEO(
cmd_err_t cmd_draw(struct spu* data_spu)
{
    static struct timespec last_time = {0, 0};
    
    struct timespec current_time;
    clock_gettime(CLOCK_MONOTONIC, &current_time);
    
    if (last_time.tv_sec == 0) 
    {
        last_time = current_time;
    }
    
    long elapsed_ns = (current_time.tv_sec - last_time.tv_sec) * 1000000000L + 
                     (current_time.tv_nsec - last_time.tv_nsec);
    
    const long frame_time_ns = 25000000L;
    
    if (elapsed_ns < frame_time_ns)
    {
        long sleep_ns = frame_time_ns - elapsed_ns;
        struct timespec ts = {0, sleep_ns};
        nanosleep(&ts, NULL);
    }
    
    printf("\033[H");
    
    for (size_t i = 0; i < VERTICAL_LEN * HORIZONTAL_LEN; i++) 
    {
        printf("%c", data_spu->ram[i]);
    }
    fflush(stdout);
    
    clock_gettime(CLOCK_MONOTONIC, &last_time);

    data_spu->counter++;
    return SUCCSES;
});

//-------------------------------------------------------------------------------------------------------------------------------------------


//!функция читающая байт код из файла в массив
int* mass_cmd_maker(FILE* stream, size_t len)
{
    int* commands = (int*) calloc (len, sizeof(int));
    if (commands == NULL)
    {
        printf("calloc error, %s %s:%d", __FILE__, __func__, __LINE__);
        return NULL;
    }

    fread(commands, sizeof(int), len, stream);

    return commands;
}


//!проверка сигнатуры
int signature_version_check(int* code)
{
    ((char*)code)[1] = 'K';
    ((char*)code)[2] = 'K';
    ((char*)code)[3] = VERSION;

    return ( ((char*)code)[1] == 'K' && ((char*)code)[2] == 'K' && ((char*)code)[3] == '1');;
}

//!проверка на вызов несуществующих регистров
int register_protection(int number_reg, FILE* stream_error)
{
    if (number_reg > NUM_REG - 1)
    {
        fprintf(stream_error, "register %c doesent exist\n", number_reg + 65);
        return 1;
    }

    return 0;
}

int check_func(struct spu data_spu)
{
    int swch = commands_arr[data_spu.code[data_spu.counter] - 1].check_inf;

    switch (swch)
    {
    case 2:
        if (data_spu.main_stk.size < 2)
        {
            fprintf(data_spu.stream_error, "stack has %zu param\n", data_spu.main_stk.size);
            return 1;
        }

        break;

    case 1:
        if (data_spu.main_stk.size < 1)
        {
            fprintf(data_spu.stream_error, "stack has %zu param\n", data_spu.main_stk.size);
            return 1;
        }

        break;

    default:
        break;
    }

    return 0;
}

void create_spu(struct spu* data_spu)
{
    stack_creator(&data_spu->main_stk, 1000000);
    stack_creator(&data_spu->data_func, 100);
    data_spu->ram = (int*) calloc (RAM_CAPACITY, sizeof(int));
}

//!функция чистки процессора
void spu_deleter(struct spu* data_spu)
{   
    data_spu->len = 0;
    data_spu->counter = 0;

    for (int i = 0; i < NUM_REG; i++)
    {
        data_spu->reg[i] = 0;
    }

    fclose(data_spu->stream_error);
    free(data_spu->code);
    stack_deleter(&data_spu->main_stk);
    stack_deleter(&data_spu->data_func);

    free(data_spu->ram);

    printf("spu deleted\n");
    printf("-------------------------------------------------------------------------------------------\n");
}

int spu_dump(struct spu data_spu, const char* func, const char* file, int LINE)
{
    printf("-------------------------STACK-INFO---------------------------\n");
    printf("DATA STACK\n");
    printf("In function %s, %s:%d\n", func, file, LINE);
    printf("Stack = [%p], \n", data_spu.main_stk.stack);
    printf("size = %zu\n", data_spu.main_stk.size);
    printf("capacity = %zu\n", data_spu.main_stk.capacity);
    printf("data:\n");
    for (size_t i = 0; i < data_spu.main_stk.capacity + 2; i++)
    {
        printf("[%zu] = %d\n", i, data_spu.main_stk.stack[i]);
    }

    printf("\n");

    printf("DATA FUNC\n");
    printf("In function %s, %s:%d\n", func, file, LINE);
    printf("Stack = [%p], \n", data_spu.data_func.stack);
    printf("size = %zu\n", data_spu.data_func.size);
    printf("capacity = %zu\n", data_spu.data_func.capacity);
    printf("data:\n");
    for (size_t i = 0; i < data_spu.data_func.capacity + 2; i++)
    {
        printf("[%zu] = %d\n", i, data_spu.data_func.stack[i]);
    }

    printf("-------------------------------------------------------------\n\n");

    printf("-------------------------SPU-INFO----------------------------\n\n");


    printf("___________________________\n");
    printf("|        code_buffer      |\n");
    printf("___________________________\n");
    for (size_t i = 1; i < data_spu.len; i++)
    {
        printf("||%5zu: command | %5d||\n", i, data_spu.code[i]);
    }
    printf("___________________________\n");

    printf("_____registers_____\n");
    for (int i = 0; i < NUM_REG; i++)
    {
        printf("| REG %c = %5d |\n", i + 65, data_spu.reg[i]);
    }
    printf("___________________\n");

    printf("----------------------RAM-OUTPUT--------------------\n");

    for (int i_v = 0; i_v < 40; i_v++ )
    {
        for ( int i_h = 0; i_h < RAM_CAPACITY / 40; i_h++)
        {
            printf(" %c " ,  data_spu.ram[i_h + HORIZONTAL_LEN * i_v] );
        }

        printf("   |\n");
    }
    printf("----------------------------------------------------\n");


    return 0;
}
