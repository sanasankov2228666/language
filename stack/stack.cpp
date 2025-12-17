#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "headers/stack1.h"
#include "headers/checker.h"


typedef int type;

//!фунция создания кода
void stack_creator(struct stk* data, size_t capacity)
{
    data->stack = (type*) calloc (capacity + 2, sizeof(data->stack[0]));
    STK_CHECK(
        for (size_t i = 1; i < capacity + 1; i++)
        {
            data->stack[i] = POISON;
        }
    );

    data->size = 0;
    data->capacity = capacity;

    STK_CHECK(
        data->stack[0] = KANAREKA1;
        data->stack[capacity + 1] = KANAREKA2;

        data->hash = hash_check(*data);

        cheacker(data, __FUNCTION__, __FILE__, __LINE__);
    );
}

//!функция внесения в стек
void stack_push(struct stk* data, type value)
{
    STK_CHECK(cheacker(data, __FUNCTION__, __FILE__, __LINE__););

    if (data->capacity == data->size)
    {
        data->stack = (type*) realloc (data->stack, (data->capacity + 1) * 2 * sizeof(data->stack[0]));

        STK_CHECK(
            if (data->stack == NULL)
            {
                data->error = bad_pointer;
            }

            data->stack[data->capacity + 1] = 0;
            data->capacity = (data->capacity) * 2;
            data->stack[data->capacity + 1] = KANAREKA2;

            for (size_t i = data->size + 1; i < data->capacity + 1; i++)
            {
                data->stack[i] = POISON;
            }
        );
    }

    data->stack[data->size + 1] = value;
    data->size = data->size + 1;

    STK_CHECK(
        data->hash = hash_check(*data);

        cheacker(data, __FUNCTION__, __FILE__, __LINE__);
    );
}

//!функция доставания из стека
int stack_pop(struct stk* data)
{
    STK_CHECK(
        cheacker(data, __FUNCTION__, __FILE__, __LINE__);

        if (data->size == 0)
        {
            data->error = data->error | bad_size;
            return 0;
        }
    );

    int parametr = data->stack[data->size];

    STK_CHECK(data->stack[data->size] = POISON;);

    data->size = data->size - 1;

    STK_CHECK(
        data->hash = hash_check(*data);
        cheacker(data, __FUNCTION__, __FILE__, __LINE__);
    );

    return parametr;
}

//!функция удаления стека
void stack_deleter(struct stk* data)
{
    if (data->stack == NULL)
    {
        printf("ERROR IN stack_deleter\n");
    }

    data->capacity = 0;
    data->size = 0;
    free (data->stack);
    data->stack = NULL;
}


