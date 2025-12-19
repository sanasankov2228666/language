#include "area_stack.h"
#include "config.h"

LangErr_t stack_scopes_init (stack_scopes* stack, size_t capacity)
{
    if (!stack)
    {
        D_PRINT("stack == NULL, ERROR\n");
        return LN_ERR;
    }
    
    stack->scopes = (scope**) calloc (capacity, sizeof(scope*));
    if (!stack->scopes)
    {
        D_PRINT("stack->scopes = NULL, ERROR\n");
        return LN_ERR;
    }
    
    stack->capacity = capacity;
    stack->size = 0;
    stack->cur_scope = 0;
    
    scope* global_scope = (scope*) calloc (1, sizeof(scope));
    if (!global_scope)
    {
        free(stack->scopes);
        stack->scopes = NULL;
        return LN_ERR;
    }

    if (scope_init(global_scope))
    {
        free(global_scope);
        free(stack->scopes);
        stack->scopes = NULL;
        return LN_ERR;
    }
    
    stack->scopes[stack->size++] = global_scope;
    
    return LN_OK;
}

LangErr_t scope_init (scope* sc)
{
    if (!sc)
    {
        D_PRINT("scope == NULL, ERROR\n");
        return LN_ERR;
    }
    
    sc->table = (var*) calloc (MAX_VARS, sizeof(var));
    if (!sc->table)
    {
        D_PRINT("scope->table allocation failed, ERROR\n");
        return LN_ERR;
    }
    
    sc->var_count = 0;
    sc->var_capacity = MAX_VARS;
    
    return LN_OK;
}


LangErr_t enter_scope (stack_scopes* stack)
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
        
        scope** new_scopes = (scope**) realloc ( 
            stack->scopes, new_capacity * sizeof(scope*));

        if (!new_scopes)
        {
            D_PRINT("realloc failed for scopes stack\n");
            return LN_ERR;
        }

        stack->scopes = new_scopes;
        stack->capacity = new_capacity;
    }
    
    scope* new_scope = (scope*) calloc (1, sizeof(scope));
    if (!new_scope)
    {
        D_PRINT("malloc failed for new scope\n");
        return LN_ERR;
    }
    
    if (scope_init(new_scope))
    {
        free(new_scope);
        return LN_ERR;
    }

    new_scope->begin = find_begin(stack);
     
    stack->scopes[stack->size] = new_scope;
    stack->size++;
    stack->cur_scope = stack->size - 1;

    return LN_OK;
}


int find_begin (stack_scopes* stack)
{
    int begin = 100;

    for (size_t i = 0; i < stack->size; i++)
    {
        begin = begin + (int) stack->scopes[i]->var_count;
    }

    return begin;
}


LangErr_t exit_scope (stack_scopes* stack)
{
    if (!stack || stack->size == 0)
    {
        D_PRINT("ERROR, cannot exit from empty stack\n");
        return LN_ERR;
    }
    
    if (stack->size == 1)
    {
        D_PRINT("ERROR, cannot exit global scope\n");
        return LN_OK;
    }
    
    scope* current_scope = stack->scopes[stack->size - 1];

    free(current_scope->table);
    free(current_scope);
    
    stack->size--;
    stack->cur_scope = stack->size - 1;
    
    if (stack->capacity > 8 && stack->capacity >= 4 * stack->size)
    {
        size_t new_capacity = stack->capacity / 2;
        if (new_capacity < 4) new_capacity = 4;
        
        scope** new_scopes = (scope**) realloc (
            stack->scopes, new_capacity * sizeof(scope*));

        if (new_scopes)
        {
            stack->scopes = new_scopes;
            stack->capacity = new_capacity;
        }
    }
    
    return LN_OK;
}


int init_var(stack_scopes* stack, char* name)
{
    var result = find_var(stack, name);

    if (result.offset == ERR_FIND)
    {
        D_PRINT("ERROR in find_var parameters\n");
        return -1;
    }

    if (result.offset != N_EXIST)
    {
        D_PRINT("ERROR, var - %s already exist\n", name);
        return -1;
    }

    scope* cur_scope = stack->scopes[stack->cur_scope];
    
    if (cur_scope->var_count >= cur_scope->var_capacity)
    {
        D_PRINT("ERROR: scope table overflow\n");
        return -1;
    }

    int offset = (int) cur_scope->var_count;

    cur_scope->table[cur_scope->var_count].name  = name;
    cur_scope->table[cur_scope->var_count].offset = offset;

    cur_scope->var_count++;
    return offset;
}


var find_var(stack_scopes* stack, const char* name)
{
    var dont_exist    = {};
    dont_exist.offset = N_EXIST;
    var err    = {};
    err.offset = ERR_FIND;

    if (!stack || !name || stack->size == 0)
    {
        return err;
    }
    
    for (size_t scope_idx = stack->size; scope_idx > 0; scope_idx--)
    {
        scope* current_scope = stack->scopes[scope_idx - 1];
        
        for (size_t var_idx = 0; var_idx < current_scope->var_count; var_idx++)
        {
            char* var_name = current_scope->table[var_idx].name;

            if (strcmp(var_name, name) == 0) 
                return current_scope->table[var_idx];
        }
    }
    
    return dont_exist;
}


int get_var_address(stack_scopes* stack, const char* name)
{
    var result = find_var(stack, name);
    
    if (result.offset == ERR_FIND)
    {
        D_PRINT("ERROR: invalid parameters\n");
        return -1;
    }
    
    if (result.offset == N_EXIST)
    {
        D_PRINT("ERROR: variable '%s' not found\n", name);
        return -1;
    }
    
    return result.offset;
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
        if (stack->scopes[scope_idx] != NULL)
        {
            free(stack->scopes[scope_idx]->table);
            free(stack->scopes[scope_idx]);
            stack->scopes[scope_idx] = NULL;
        }
    }
    
    free(stack->scopes);
    
    stack->scopes = NULL;
    stack->size = 0;
    stack->capacity = 0;
    stack->cur_scope = 0;
    
    return LN_OK;
}


