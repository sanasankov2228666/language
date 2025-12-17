#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <cstdint>
#include "headers/stack1.h"
#include "headers/checker.h"

#define eps 0.0000000001
#define inf 1000000000

//!функция вывода ошибок
int stack_dump(struct stk data, const char* func, const char* file, int LINE)
{
    int counter = 0;

    if (data.error & 1)
    {
        slicer();
        printf("bad size!\n");
        dump_helper(data, func, file, LINE);
        counter++;
    }

    if (data.error & 2)
    {
        slicer();
        printf("bad capasicity!\n");
        dump_helper(data, func, file, LINE);
        counter++;
    }

    if (data.error & 4)
    {
        slicer();
        printf("bad pointer!\n");
        dump_helper(data, func, file, LINE);
        counter++;
    }

    if (data.error & 8)
    {
        slicer();
        printf("kanareyka changed!\n");
        dump_helper(data, func, file, LINE);
        counter++;
    }

    if (data.error & 16)
    {
        slicer();
        printf("Poison stack data changed!\n");
        dump_helper(data, func, file, LINE);
        counter++;
    }

    if (data.error & 32)
    {
        slicer();
        printf("Hash is different\n");
        dump_helper(data, func, file, LINE);
        counter++;
    }

    return counter;
}

//!фунция фиксирующая ошибки
int verify(struct stk* data)
{
    int counter = 0;
    if (data->size > inf || data->size > data->capacity)
    {
        data->error = data->error | bad_size; // 00000001
        counter++;
    }

    if (data->capacity > inf || data->capacity == 0)
    {
        data->error = data->error | bad_capacity; // 00000010
        counter++;
    }

    if (data->stack == NULL)
    {
        data->error = data->error | bad_pointer; // 00000100
        counter++;
    }

    if ((data->stack != NULL) &&
        (data->stack[0] != KANAREKA1 || data->stack[data->capacity + 1] != KANAREKA2))
    {
        data->error = data->error | kanareika_error; // 00001000
        counter++;
    }

    if (data->stack != NULL)
    {
        for (size_t i = data->size + 1; i < data->capacity; i++)
        {
            if (data->stack[i] != POISON)
            {
                data->error = data->error | poison_error; // 00010000
            }
        }
    }

    if ((data->hash - hash_check(*data)) > eps)
    {
        data->error = data->error | hash_error; // 00100000
        counter++;
    }

    return counter;
}

//!функция проверки
int cheacker(struct stk* data, const char* func, const char* file, int line)
{
    int a = verify(data);
    int b = stack_dump(*data, func, file, line);

    data->error = 0;

    return (a && b);
}

//!хэш функция
double hash_check(struct stk data)
{
    intptr_t stack_ptr = (intptr_t)data.stack;

    double elem1 = 0;
    double elem2 = 0;

    if (data.stack != NULL)
    {
        for (size_t i = 0; i < data.capacity + 2; i++)
        {
            elem1 = elem1 + 1.23 * data.stack[i];
        }
    }

    elem2 = elem1 + 3.14 * (double)data.error;

    double zero_check = elem2 + 1;
    if (zero_check < eps) 
    {
        zero_check = 1;
    }

    double ptr_component = (double)((stack_ptr + 1));
    double size_component = (double)(data.size + data.capacity);

    return (elem1 / zero_check) * size_component * ptr_component;
}


//!помощь вывода для функции проверки
void dump_helper(struct stk data, const char* func, const char* file, int LINE)
{
    printf(">ERROR<\n");
    printf("In function %s, %s:%d\n", func, file, LINE);
    printf("Stack = [%p], \n", data.stack);
    printf("size = %zu\n", data.size);
    printf("capacity = %zu\n", data.capacity);
    printf("data:\n");
    for (size_t i = 0; i < data.capacity + 2; i++)
    {
        printf("[%zu] = %d\n", i, data.stack[i]);
    }

}

//!функция разделитель
void slicer(void)
{
    printf("______________________________________________________________________________________________________________\n");
}
