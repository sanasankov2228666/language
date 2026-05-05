#include "area_stack.h"
#include "debug.h"

#include "area_stack.h"
#include "debug.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

LangErr_t stack_scopes_init (stack_scopes* stack, size_t capacity)
{
    if (!stack)
    {
        D_PRINT("stack == NULL, ERROR\n");
        return LN_ERR;
    }

    stack->scopes = (scope*) calloc (capacity, sizeof (scope));
    if (!stack->scopes)
    {
        D_PRINT("stack->scopes = NULL, ERROR\n");
        return LN_ERR;
    }

    stack->capacity = capacity;
    stack->size = 0;
    stack->cur_scope = 0;
    stack->search_index = 0;
    stack->global.area_type = GLOBAL;

    if (scope_init (&stack->global, GLOBAL))
    {
        free (stack->scopes);
        stack->scopes = NULL;
        return LN_ERR;
    }
    stack->global.begin = SYSTEM_ZONE;

    return LN_OK;
}

LangErr_t scope_init (scope* sc, size_t area_type)
{
    if (!sc)
    {
        D_PRINT("scope == NULL, ERROR\n");
        return LN_ERR;
    }
    
    sc->table = (var*) calloc (MAX_VARS, sizeof (var));
    if (!sc->table)
    {
        D_PRINT("scope->table allocation failed, ERROR\n");
        return LN_ERR;
    }
    
    sc->var_count = 0;
    sc->var_capacity = MAX_VARS;
    sc->area_type = area_type;
    sc->begin = SYSTEM_ZONE;
    
    return LN_OK;
}

LangErr_t stack_scopes_destroy (stack_scopes* stack)
{
    if (!stack)
    {
        D_PRINT("stack == NULL\n");
        return LN_ERR;
    }

    for (size_t scope_idx = 0; scope_idx < stack->size; scope_idx++)
    {
        if (stack->scopes[scope_idx].table)
            free (stack->scopes[scope_idx].table);
    }

    if (stack->global.table)
        free (stack->global.table);

    free (stack->scopes);

    stack->scopes = NULL;
    stack->size = 0;
    stack->capacity = 0;
    stack->cur_scope = 0;
    stack->search_index = 0;

    return LN_OK;
}

int find_begin (stack_scopes* stack)
{
    int begin = stack->global.begin + (int) stack->global.var_count;

    for (size_t i = 0; i < stack->size; i++)
        begin += (int) stack->scopes[i].var_count;

    return begin;
}

LangErr_t enter_scope (stack_scopes* stack, size_t area_type)
{
    if (!stack)
    {
        D_PRINT("stack == NULL\n");
        return LN_ERR;
    }
    
    if (stack->size >= stack->capacity)
    {
        size_t new_capacity = stack->capacity * 2;
        if (new_capacity == 0) new_capacity = 4;
        
        scope* new_scopes = (scope*) realloc ( 
            stack->scopes, new_capacity * sizeof(scope));

        if (!new_scopes)
        {
            D_PRINT("realloc failed for scopes stack\n");
            return LN_ERR;
        }

        stack->scopes   = new_scopes;
        stack->capacity = new_capacity;
    }
    
    if (scope_init (&stack->scopes[stack->size], area_type))
        return LN_ERR;
    
    stack->scopes[stack->size].begin = find_begin (stack);

    stack->size++;
    stack->cur_scope = stack->size - 1;

    if (area_type == FUNC_AREA)
        stack->search_index = stack->cur_scope;

    return LN_OK;
}

LangErr_t exit_scope (stack_scopes* stack)
{
    if (!stack || stack->size == 0)
    {
        D_PRINT("ERROR, cannot exit from empty stack\n");
        return LN_ERR;
    }
    
    scope current_scope = stack->scopes[stack->cur_scope];
    free(current_scope.table);
    
    stack->search_index -= current_scope.area_type;
    stack->size--;
    stack->cur_scope = stack->size > 0 ? stack->size - 1 : 0;
    
    if (stack->capacity > 8 && stack->capacity >= 4 * stack->size)
    {
        size_t new_capacity = stack->capacity / 2;
        if (new_capacity < 4) new_capacity = 4;
        
        scope* new_scopes = (scope*) realloc (
            stack->scopes, new_capacity * sizeof(scope));

        if (new_scopes)
        {
            stack->scopes = new_scopes;
            stack->capacity = new_capacity;
        }
        else
        {
            D_PRINT("realloc failed for scopes stack\n");
            return LN_ERR;         
        }
    }

    return LN_OK;
}


var find_var (stack_scopes* stack, const char* name)
{
    if (!stack || !name) return not_exist();

    if (stack->size > 0)
    {
        for (int i = (int)stack->size - 1; i >= (int)stack->search_index; i--)
        {
            scope* current = &stack->scopes[i];
            for (size_t j = 0; j < current->var_count; j++)
            {
                if (strcmp(current->table[j].name, name) == 0)
                    return current->table[j];
            }
        }
    }

    for (size_t i = 0; i < stack->global.var_count; i++)
    {
        if (strcmp (stack->global.table[i].name, name) == 0)
            return stack->global.table[i];
    }

    return not_exist();
}

int init_var (stack_scopes* stack, char* name)
{
    if (find_var(stack, name).offset != NOT_EXIST)
    {
        D_PRINT("ERROR, var - %s already exist\n", name);
        return EXIST;
    }

    scope* cur_scope = NULL;
    int offset = 0;

    if (stack->size == 0)
    {
        cur_scope = &stack->global;
        offset = (int) cur_scope->var_count;
    }
    else
    {
        if (stack->search_index == 0)
            offset += (int) stack->global.var_count;

        cur_scope = &stack->scopes[stack->cur_scope];
        
        for (size_t i = stack->search_index; i <= stack->cur_scope; i++)
            offset += (int) stack->scopes[i].var_count;
    }

    if (cur_scope->var_count >= cur_scope->var_capacity)
    {
        D_PRINT("ERROR scope table overflow\n");
        return STCK_OVFL;
    }

    var* cur_var = &cur_scope->table[cur_scope->var_count];
    cur_var->name   = name;
    cur_var->offset = offset;
    cur_var->area   = (int) cur_scope->area_type;

    cur_scope->var_count++;
    return offset;
}

var not_exist ()
{
    var not_exist  = {NULL, NOT_EXIST};
    return not_exist;
}