#ifndef AREA_STACK_H
#define AREA_STACK_H

#include <stdio.h>
#include "tree.h"
#include "options.h"

#define STCK_OVFL -30001
#define EXIST -30002
#define NOT_EXIST -30003

#define GLOBAL 11
#define FUNC_AREA  1
#define BLOCK_AREA 0

#define SYSTEM_ZONE 100

struct var
{
    char* name = NULL;
    int offset = 0;
    int area   = 0;
};

struct scope
{
    var* table = NULL;
    size_t var_count = 0;
    size_t var_capacity = 0;
    size_t area_type = 0;
    int begin = SYSTEM_ZONE;
};

struct stack_scopes
{
    scope  global = {};
    scope* scopes = NULL;
    size_t size      = 0;
    size_t capacity  = 0;
    size_t cur_scope = 0;
    size_t search_index = 0; 
};

LangErr_t stack_scopes_init    (stack_scopes* stack, size_t capacity);
LangErr_t stack_scopes_destroy (stack_scopes* stack);

LangErr_t scope_init  (scope* sc, size_t area_type);
LangErr_t enter_scope (stack_scopes* stack, size_t area_type);
LangErr_t exit_scope  (stack_scopes* stack);

int find_begin (stack_scopes* stack);
int init_var   (stack_scopes* stack, char* name);
var find_var   (stack_scopes* stack, const char* name);
var not_exist  ();

#endif