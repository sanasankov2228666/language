#ifndef STACK_H_
#define STACK_H_
#include <stddef.h>

#define KANAREKA1 -100
#define KANAREKA2 -200
#define POISON -2

typedef int type;



#ifdef STK_DBG
#define STK_CHECK(...) __VA_ARGS__
#else 
#define STK_CHECK(...) 
#endif

//!структура стека
struct stk
{
    type* stack = NULL;
    size_t size = 0;
    size_t capacity = 0;
    size_t error;
    double hash = 0;
};

void stack_creator(struct stk* data, size_t capacity);

void stack_push(struct stk* data, type value);

int stack_pop(struct stk* data);

void stack_deleter(struct stk* data);

#endif
