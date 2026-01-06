#ifndef AREA_STACK_H
#define AREA_STACK_H

#include <stdio.h>
#include "tree.h"

#define MAX_VARS 1000
#define N_EXIST -1

#define SYSTEM_ZONE 100

struct var
{
    char* name = NULL;
    int offset  = 0;
};

struct scope
{
    var* table = NULL;
    size_t var_count = 0;
    size_t var_capacity = 0;
    int begin = SYSTEM_ZONE;
};


struct stack_scopes
{
    scope** scopes = NULL;
    size_t size      = 0;
    size_t capacity  = 0;
    size_t cur_scope = 0;
};


LangErr_t stack_scopes_init (stack_scopes* stack, size_t capacity);

LangErr_t scope_init (scope* sc);

LangErr_t enter_scope (stack_scopes* stack);

int find_begin (stack_scopes* stack);

LangErr_t exit_scope (stack_scopes* stack);

int init_var (stack_scopes* stack, char* name);

int find_var_in_area(scope* cur, const char* name);

LangErr_t stack_scopes_destroy (stack_scopes* stack);


#endif