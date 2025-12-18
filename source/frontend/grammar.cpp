#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include "tree.h"
#include "grammar.h"
#include "config.h"
#include "token.h"
#include "lexems.h"


// =============================================== ОСНОВНАЯ ФУНКЦИЯ ЧТЕНИЯ =====================================================


node_t* ReadTree (data_lexer* data_lex)
{
    struct ast_data data = {};
    AstDataInit (&data, data_lex);

    node_t* root = GetProgram (&data);

    AstDataDeleter (&data);

    return root;
}


LangErr_t AstDataInit (ast_data* data, data_lexer* data_lex)
{
    data->tokens = data_lex->tokens;

    data->funcs = (func*) calloc ( MAX_FUNCS, sizeof (func));
    if (data->funcs == NULL)
    {
        D_PRINT ("data->funcs == NULL\n");
        return LN_ERR;
    }

    if (stack_scopes_init(&data->scopes, 8) != LN_OK)
    {
        D_PRINT("Failed to initialize scope stack\n");
        return LN_ERR;
    }

    data->pos = 0;
    return LN_OK;
}


LangErr_t AstDataDeleter (ast_data* data)
{
    free(data->funcs);

    stack_scopes_destroy(&data->scopes);

    data->pos = 0;
    data->func_pos = 0;

    return LN_OK;
}


// =============================================== ФУНКЦИИ ОБЛАСТИ ВИДИМОСТИ ====================================================


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

    if (scope_init(global_scope, GLOBAL))
    {
        free(global_scope);
        free(stack->scopes);
        stack->scopes = NULL;
        return LN_ERR;
    }
    
    stack->scopes[stack->size++] = global_scope;
    
    return LN_OK;
}

LangErr_t scope_init (scope* sc, int area_type)
{
    if (!sc)
    {
        D_PRINT("scope == NULL, ERROR\n");
        return LN_ERR;
    }
    
    sc->table = (char**) calloc (MAX_VARS, sizeof(char*));
    if (!sc->table)
    {
        D_PRINT("scope->table allocation failed, ERROR\n");
        return LN_ERR;
    }
    
    sc->var_count = 0;
    sc->var_capacity = MAX_VARS;
    sc->area_type = area_type;
    
    return LN_OK;
}


LangErr_t enter_scope (struct stack_scopes* stack, int area_type)
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
        
        scope** new_scopes = (scope**) realloc( 
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
    
    if (scope_init(new_scope, area_type))
    {
        free(new_scope);
        return LN_ERR;
    }
     
    stack->scopes[stack->size] = new_scope;
    stack->size++;
    stack->cur_scope = stack->size - 1;
    
    return LN_OK;
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


LangErr_t init_var(stack_scopes* stack, char* name)
{
    bool is_exist = find_var(stack, name);

    if (is_exist)
    {
        D_PRINT("ERROR, var - %s already exist\n", name);
        return LN_ERR;
    }

    scope* cur_scope = stack->scopes[stack->cur_scope];
    
    if (cur_scope->var_count >= cur_scope->var_capacity)
    {
        D_PRINT("ERROR: scope table overflow\n");
        return LN_ERR;
    }
    
    cur_scope->table[cur_scope->var_count++] = name;
    return LN_OK;
}


bool find_var(stack_scopes* stack, const char* name)
{
    if (!stack || !name || stack->size == 0)
    {
        return false;
    }
    
    for (size_t scope_idx = stack->size; scope_idx > 0; scope_idx--)
    {
        scope* current_scope = stack->scopes[scope_idx - 1];
        
        for (size_t var_idx = 0; var_idx < current_scope->var_count; var_idx++)
        {
            char* var_name = current_scope->table[var_idx];
            if (strcmp(var_name, name) == 0) 
                return true;
        }
    }
    
    return false;
}


LangErr_t stack_scopes_destroy(stack_scopes* stack)
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


// ============================================== ГРАМАТИКА И СОЗДАНИЕ ДЕРЕВА ====================================================


node_t* GetProgram(ast_data* data)
{
    node_t* root =  NULL;
    node_t* node =  NULL;
    node_t* old  =  NULL;

    while (CUR_TOKEN->type != END_TOKEN)
    {

        if (CUR_TOKEN->type == OP &&
            CUR_TOKEN->val.op == FUNC)
        {
            node = GetFunc (data);
        }
        
        else
        {
            node = GetStatement (data);
        }

        if (old) old->right = node;
        else root = node;

        if (!node) return NULL;

        old = node;
    }
    
    return root;
}


node_t* GetFunc (ast_data* data)
{
    node_t* func_op = CUR_TOKEN;
    data->pos++;

    if (CUR_TOKEN->type != CONNECTION)
    {
        D_PRINT("connection missed\n");
        return NULL;
    }

    node_t* connect1 = CUR_TOKEN;
    data->pos++;

    if (CUR_TOKEN->type != CONNECTION)
    {
        D_PRINT("connection missed\n");
        return NULL;
    }

    node_t* connect2 = CUR_TOKEN;
    data->pos++;

    SYNTAX_ERROR (CUR_TOKEN->type != FUNCTION);

    node_t* name = CUR_TOKEN;
    data->funcs[data->func_pos].name = CUR_TOKEN->val.var;

    int found = 0;
    for (size_t i = 0; i < data->func_pos && found != 1; i++)
    {
        if ( strcmp(data->funcs[i].name, name->val.var) == 0 )
            found = 1;
    }

    SYNTAX_ERROR (found);

    data->pos++;

    SYNTAX_ERROR (CUR_TOKEN->type != OP &&
                  CUR_TOKEN->val.op != L_PAR);
    data->pos++;

    if (enter_scope(&data->scopes, FUNCTION) != LN_OK)
    {
        D_PRINT("Failed to enter function scope\n");
        return NULL;
    }

    node_t* params = GetParams (data);
    data->func_pos++;
    
    node_t* body   = GetBlock (data);

    if (exit_scope(&data->scopes) != LN_OK)
    {
        D_PRINT("Failed to exit function scope\n");
        return NULL;
    }

    connect1->left = func_op;
    func_op->left  = name;

    func_op->right = connect2;
    
    connect2->left  = params;
    connect2->right = body;

    return connect1;
}

node_t* GetReturn(ast_data* data)
{
    SYNTAX_ERROR(CUR_TOKEN->type != OP || CUR_TOKEN->val.op != RETURN );
    node_t* return_op = CUR_TOKEN;
    data->pos++;
    
    node_t* expr = GetExpresion(data);
    
    SYNTAX_ERROR(CUR_TOKEN->type != OP || CUR_TOKEN->val.op != SEMIC);
    node_t* semic = CUR_TOKEN;
    data->pos++;
    
    return_op->left = expr;
    semic->left = return_op;

    return semic;
}

node_t* GetParams(ast_data* data)
{
    if (CUR_TOKEN->type == OP && CUR_TOKEN->val.op == R_PAR) 
    {
        data->pos++;
        return NULL;
    }

    SYNTAX_ERROR(CUR_TOKEN->type != VAR);
    
    node_t* root = CUR_TOKEN;
    node_t* old  = NULL;

    if (init_var(&data->scopes, CUR_TOKEN->val.var) != LN_OK)
    {
        D_PRINT("Failed to add parameter '%s'\n", CUR_TOKEN->val.var);
        return NULL;
    }

    data->pos++;
    
    while (1)
    {
        if (CUR_TOKEN->type == OP && CUR_TOKEN->val.op == R_PAR) 
        {
            data->pos++;
            break;
        }
        
        SYNTAX_ERROR(CUR_TOKEN->type != OP || CUR_TOKEN->val.op != COMMA);
        data->pos++;
        
        SYNTAX_ERROR(CUR_TOKEN->type != VAR);
        
        node_t* next_param = CUR_TOKEN;

        if (init_var(&data->scopes, CUR_TOKEN->val.var) != LN_OK)
        {
            D_PRINT("Failed to add parameter '%s'\n", CUR_TOKEN->val.var);
            return NULL;
        }

        data->pos++;
        
        if (!old) root->left = next_param;
        else old->left = next_param;

        old = next_param;
    }
    
    return root;
}


node_t* GetStatement (ast_data* data)
{
    node_t* root = NULL;

    if (CUR_TOKEN->type == OP && CUR_TOKEN->val.op == IF)
    {
        root = GetIf (data);
    }

    else if (CUR_TOKEN->type == OP && CUR_TOKEN->val.op == WHILE)
    {
        root = GetWhile (data);
    }

    else if (CUR_TOKEN->type == OP && CUR_TOKEN->val.op == RETURN)
    {
        root = GetReturn (data);
    }

    else if (CUR_TOKEN->type == FUNCTION)
    {
        root = GetCall (data);
        SYNTAX_ERROR(CUR_TOKEN->type != OP || CUR_TOKEN->val.op != SEMIC);
        node_t* semic = CUR_TOKEN;

        semic->left = root;
        root = semic;

        data->pos++;
    }
    
    else if (CUR_TOKEN->type == OP && CUR_TOKEN->val.op == INIT)
    {
        root = GetVarInit (data);
    }

    else if (CUR_TOKEN->type == VAR)
    {
        root = GetVarAct (data);
    }

    else
    {
        root = GetExpresion (data);
        SYNTAX_ERROR(CUR_TOKEN->type != OP || CUR_TOKEN->val.op != SEMIC);
        node_t* semic = CUR_TOKEN;

        semic->left = root;
        root = semic;

        data->pos++;
    }
    
    return root;  
}

node_t* GetIf (ast_data* data)
{
    node_t* else_node = NULL;
    node_t* else_body = NULL;

    node_t* root = CUR_TOKEN;
    data->pos++;

    if (CUR_TOKEN->type != CONNECTION)
    {
        D_PRINT("ERROR, connetion node missed\n");
        return NULL;
    }

    node_t* connection = CUR_TOKEN;
    data->pos++;

    SYNTAX_ERROR(CUR_TOKEN->type != OP || CUR_TOKEN->val.op != L_PAR);
    data->pos++;

    node_t* condition = GetExpresion (data);

    SYNTAX_ERROR(CUR_TOKEN->type != OP || CUR_TOKEN->val.op != R_PAR);
    data->pos++;


    node_t* body = GetBlock (data);

    if (CUR_TOKEN->type == OP && CUR_TOKEN->val.op == ELSE)
    {
        else_node = CUR_TOKEN;
        data->pos++;

        else_body = GetBlock (data);
    }

    root->left = condition;

    if (else_body) 
    {
        root->right = else_node;
        else_node->left  = body;
        else_node->right = else_body;
    }
    else root->right = body;

    connection->left = root;

    return connection;
}


node_t* GetWhile (ast_data* data)
{
    node_t* root = CUR_TOKEN;
    data->pos++;

    if (CUR_TOKEN->type != CONNECTION)
    {
        D_PRINT("ERROR, connetion node missed\n");
        return NULL;
    }

    node_t* connection = CUR_TOKEN;
    data->pos++;

    SYNTAX_ERROR(CUR_TOKEN->type != OP || CUR_TOKEN->val.op != L_PAR);
    data->pos++;

    node_t* condition = GetExpresion (data);

    SYNTAX_ERROR(CUR_TOKEN->type != OP || CUR_TOKEN->val.op != R_PAR);
    data->pos++;
    
    node_t* body = GetBlock (data);
    
    root->left = condition;
    root->right = body;

    connection->left = root;

    return connection;
}


node_t* GetExpresion (ast_data* data)
{
    return GetLogicOr (data);
}

node_t* GetLogicOr (ast_data* data)
{
    node_t* node = GetLogicAnd (data);
    
    while (CUR_TOKEN->type == OP && CUR_TOKEN->val.op == OR)
    {
        node_t* node_or = CUR_TOKEN;
        data->pos++;
        
        node_t* right = GetLogicAnd (data);
        
        node_or->left = node;
        node_or->right = right;
        node = node_or;
    }
    
    return node;
}

node_t* GetLogicAnd (ast_data* data)
{
    node_t* node = GetEquality (data);
    
    while (CUR_TOKEN->type == OP && CUR_TOKEN->val.op == AND)
    {
        node_t* node_and = CUR_TOKEN;
        data->pos++;
        
        node_t* right = GetEquality (data);
        
        node_and->left = node;
        node_and->right = right;
        node = node_and;
    }
    
    return node;
}


node_t* GetEquality (ast_data* data)
{
    node_t* node = GetCompare (data);
    
    while (CUR_TOKEN->type == OP && 
          (CUR_TOKEN->val.op == EQEQ || CUR_TOKEN->val.op == NEQ))
    {
        node_t* node_eq = CUR_TOKEN;
        data->pos++;
        
        node_t* right = GetCompare (data);
        
        node_eq->left = node;
        node_eq->right = right;
        node = node_eq;
    }
    
    return node;
}

node_t* GetCompare (ast_data* data)
{
    node_t* node = GetAddSub (data);

    while (CUR_TOKEN->type == OP &&
          (CUR_TOKEN->val.op == BIGR || CUR_TOKEN->val.op == SMLR ||
           CUR_TOKEN->val.op == EBIGR || CUR_TOKEN->val.op == ESMLR))
    {
        node_t* node_cmp = CUR_TOKEN;
        data->pos++;
        
        node_t* right = GetAddSub (data);
        
        node_cmp->left = node;
        node_cmp->right = right;
        node = node_cmp;
    }
    
    return node;
}

node_t* GetAddSub (ast_data* data)
{
    node_t* node = GetMulDiv (data);
    
    while (CUR_TOKEN->type == OP && 
          (CUR_TOKEN->val.op == ADD || CUR_TOKEN->val.op == SUB))
    {
        node_t* node_op = CUR_TOKEN;
        data->pos++;
        
        node_t* right = GetMulDiv (data);
        
        node_op->left = node;
        node_op->right = right;
        node = node_op;
    }
    
    return node;
}

node_t* GetMulDiv (ast_data* data)
{
    node_t* node = GetUnar (data);
    
    while (CUR_TOKEN->type == OP && 
          (CUR_TOKEN->val.op == MUL || CUR_TOKEN->val.op == DIV))
    {
        node_t* node_op = CUR_TOKEN;
        data->pos++;
        
        node_t* right = GetUnar (data);
        
        node_op->left = node;
        node_op->right = right;
        node = node_op;
    }
    
    return node;
}


node_t* GetUnar(ast_data* data)
{
    if (CUR_TOKEN->type == OP && CUR_TOKEN->val.op == SUB)
    {
        node_t* oper = CUR_TOKEN;
        data->pos++;

        node_t* val = GetVal(data);
        
        oper->left = val;
        return oper;
    }
    
    else if (CUR_TOKEN->type == OP && CUR_TOKEN->val.op == ADD)
    {
        data->pos++;
        return GetVal(data);
    }
    
    else return GetBracket(data);
}

node_t* GetBracket (ast_data* data)
{
    node_t* node = NULL;

    if (CUR_TOKEN->type == OP && CUR_TOKEN->val.op == L_PAR)
    {
        data->pos++;
        node = GetExpresion (data);
        
        SYNTAX_ERROR(CUR_TOKEN->type != OP && CUR_TOKEN->val.op != R_PAR);
        data->pos++;

        return node;
    }

    else return GetVal (data);
}

node_t* GetVal(ast_data* data)
{
    node_t* node = NULL;

    if (CUR_TOKEN->type == NUM)
    {
        node = CUR_TOKEN;
        data->pos++;
    }
    
    else if (CUR_TOKEN->type == VAR)
    {   
        if (!find_var(&data->scopes, CUR_TOKEN->val.var))
        {
            D_PRINT("ERROR: variable '%s' doesn't exist\n", CUR_TOKEN->val.var);
            return NULL;
        }

        node = CUR_TOKEN;
        data->pos++;
    }
    
    else if (CUR_TOKEN->type == FUNCTION)
    {
        node = GetCall(data);
    }

    else if (CUR_TOKEN->type == OP && CUR_TOKEN->val.op == L_PAR)
    {
        data->pos++;
        
        node = GetExpresion(data);
        
        SYNTAX_ERROR(CUR_TOKEN->type != OP || CUR_TOKEN->val.op != R_PAR);
        data->pos++;
    }

    else if (CUR_TOKEN->type == OP && 
            (CUR_TOKEN->val.op == SUB || 
             CUR_TOKEN->val.op == ADD) )
    {
        node_t* op_node = CUR_TOKEN;
        data->pos++;
        
        node_t* operand = GetVal(data);
        
        op_node->left = operand;
        node = op_node;
    }
    
    else SYNTAX_ERROR(1);

    return node;
}


node_t* GetBlock (ast_data* data)
{
    SYNTAX_ERROR(CUR_TOKEN->type != OP || CUR_TOKEN->val.op != L_BR);
    data->pos++;

    node_t* root = NULL;
    node_t* node = NULL;
    node_t* old  = NULL;

    if (enter_scope(&data->scopes, BLOCK) != LN_OK)
    {
        D_PRINT("Failed to enter block scope\n");
        return NULL;
    }

    while (CUR_TOKEN->type != OP || CUR_TOKEN->val.op != R_BR)
    {
        node = GetStatement (data);

        if (!node)
        {
            exit_scope(&data->scopes);
            return NULL;
        }

        if (old) old->right = node;
        else root = node;

        old = node;
    }

    SYNTAX_ERROR(CUR_TOKEN->type != OP || CUR_TOKEN->val.op != R_BR);
    data->pos++;

    if (exit_scope(&data->scopes) != LN_OK)
    {
        D_PRINT("Failed to exit block scope\n");
        return NULL;
    }

    return root;
}


node_t* GetVarInit (ast_data* data)
{
    node_t* init = CUR_TOKEN;
    data->pos++;

    SYNTAX_ERROR(CUR_TOKEN->type != VAR);
    node_t* var   = CUR_TOKEN;
    node_t* semic = NULL;
    
    if (init_var(&data->scopes, var->val.var) != LN_OK)
    {
        D_PRINT("Failed to initialize variable '%s'\n", var->val.var);
        return NULL;
    }

    data->pos++;

    SYNTAX_ERROR(CUR_TOKEN->type != OP || CUR_TOKEN->val.op != EQ);
    node_t* equal = CUR_TOKEN;
    data->pos++;

    node_t* expr = GetExpresion (data);

    SYNTAX_ERROR(CUR_TOKEN->type != OP || CUR_TOKEN->val.op != SEMIC);
    semic = CUR_TOKEN;
    data->pos++;

    equal->left  = var;
    equal->right = expr;

    init->left = equal;

    semic->left = init;

    return semic;
}

node_t* GetVarAct (ast_data* data)
{
    node_t* var = CUR_TOKEN;

    if (!find_var(&data->scopes, var->val.var))
    {
        D_PRINT("ERROR, variable '%s' doesn't exist\n", var->val.var);
        return NULL;
    }

    data->pos++;

    SYNTAX_ERROR(CUR_TOKEN->type != OP || CUR_TOKEN->val.op != EQ);
    node_t* equal = CUR_TOKEN;
    data->pos++;

    node_t* val = GetExpresion (data);

    SYNTAX_ERROR(CUR_TOKEN->type != OP || CUR_TOKEN->val.op != SEMIC);
    node_t* semic = CUR_TOKEN;
    data->pos++;

    semic->left  = equal;
    equal->left  = var;
    equal->right = val;

    return semic;
}

node_t* GetArgs (ast_data* data)
{
    if (CUR_TOKEN->type == OP && CUR_TOKEN->val.op == R_PAR) 
    {
        data->pos++;
        return NULL;
    }
    
    node_t* root = GetExpresion (data);
   
    while (CUR_TOKEN->type == OP && CUR_TOKEN->val.op == COMMA)
    {  
        node_t* comma = CUR_TOKEN;
        data->pos++;
        
        node_t* next_param = GetExpresion (data);
        
        comma->left = root;
        comma->right = next_param;
        
        root = comma;
    }
    
    SYNTAX_ERROR(CUR_TOKEN->type != OP ||CUR_TOKEN->val.op != R_PAR);
    data->pos++;
    
    return root;
}


node_t* GetCall (ast_data* data)
{
    node_t* function = CUR_TOKEN;
    int check = 1;

    SYNTAX_ERROR(CUR_TOKEN->type != FUNCTION);

    for (size_t i = 0; i < data->func_pos && check != 0; i++ )
    {
        check = strcmp(CUR_TOKEN->val.var, data->funcs[i].name);
    }

    if (check)
        printf("function %s doesnt exist\n", CUR_TOKEN->val.var);

    SYNTAX_ERROR(check);
    data->pos++;

    SYNTAX_ERROR(CUR_TOKEN->type != OP || CUR_TOKEN->val.op != L_PAR);
    data->pos++;

    node_t* args = GetArgs (data);

    function->left = args;

    return function;
}


// ================================================ ВЫВОД ДЕРЕВА В ФАЙЛ =========================================================


LangErr_t PrintTreeFile (node_t* root)
{
    FILE* middle_end_fp = file_opener (stderr, "middle_end", "w");
    if (!middle_end_fp) return LN_ERR;

    PrintNode (root, middle_end_fp);

    printf("tree was saved in middle end file\n");

    return LN_OK;
}

void PrintNode (node_t* root, FILE* stream)
{
    if (!root) 
    {
        fprintf (stream, " nil");
        return;
    }

    fprintf (stream, "(");
    
    switch (root->type)
    {
    case OP:
        fprintf (stream, "op: ");
        fprintf (stream, "\"%s\"", key_words[root->val.op - 1].std_name);
        break;
    
    case VAR:
        fprintf (stream, "var: ");
        fprintf (stream, "\"%s\"", root->val.var);
        break;

    case FUNCTION:
        fprintf (stream, "func: ");
        fprintf (stream, "\"%s\"", root->val.var);
        break;

    case NUM:
        fprintf (stream, "num: ");
        fprintf (stream, "\"%d\"", root->val.num);
        break;
       
    case CONNECTION:
        fprintf (stream, "cnct: ");
        fprintf (stream, "\"cnct\"");
        break;

    default:
        fprintf (stream, "unknown:");
        break;
    }

    PrintNode (root->left, stream);
    PrintNode (root->right, stream);

    fprintf (stream, ")");
}