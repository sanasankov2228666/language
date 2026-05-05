#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

#include "tree.h"
#include "debug.h"
#include "lexems.h"
#include "grammar.h"
#include "tokenization.h"

bool is_correct = true;

// =============================================== ОСНОВНАЯ ФУНКЦИЯ ЧТЕНИЯ =====================================================


node_t* MakeTree (data_lexer* data_lex)
{
    ast_data data = {};
    AstDataInit (&data, data_lex);

    node_t* root = GetProgram (&data);

    AstDataDeleter (&data);

    return root;
}

LangErr_t AstDataInit (ast_data* data, data_lexer* data_lex)
{
    data->tokens = data_lex->tokens;

    data->funcs = (func*) calloc (MAX_FUNCS, sizeof (func));
    if (data->funcs == NULL)
    {
        D_PRINT ("data->funcs == NULL\n");
        return LN_ERR;
    }

    if ( stack_scopes_init (&data->scopes, 8) != LN_OK )
    {
        D_PRINT ("Failed to initialize scope stack\n");
        return LN_ERR;
    }

    data->pos = 0;
    return LN_OK;
}

LangErr_t AstDataDeleter (ast_data* data)
{
    free (data->funcs);
 
    stack_scopes_destroy (&data->scopes);

    data->pos = 0;
    data->func_pos = 0;

    return LN_OK;
}

// ============================================== ГРАМАТИКА И СОЗДАНИЕ ДЕРЕВА ====================================================

node_t* GetProgram (ast_data* data)
{
    node_t* root =  NULL;
    node_t* node =  NULL;
    node_t* old  =  NULL;

    while (CUR_TOKEN.type != END_TOKEN)
    {

        if (CUR_TOKEN.type == OP &&
            CUR_TOKEN.val.op == FUNC)
        {
            node = GetFunc (data);
        }
        
        else node = GetStatement (data);

        if (old) old->right = node;
        else root = node;

        if (!node) return NULL;

        old = node;
    }
    
    return root;
}


node_t* GetFunc (ast_data* data)
{
    node_t* func_op = &CUR_TOKEN;
    data->pos++;

    if (CUR_TOKEN.type != CONNECTION)
    {
        D_PRINT("connection missed\n");
        return NULL;
    }

    node_t* connect1 = &CUR_TOKEN;
    data->pos++;

    if (CUR_TOKEN.type != CONNECTION)
    {
        D_PRINT("connection missed\n");
        return NULL;
    }

    node_t* connect2 = &CUR_TOKEN;
    data->pos++;

    SYNTAX_ERROR ( CUR_TOKEN.type != FUNCTION,
                   "expected name of function" );

    node_t* name = &CUR_TOKEN;
    data->funcs[data->func_pos].name = CUR_TOKEN.val.var;

    bool found = false;
    for (size_t i = 0; i < data->func_pos && found != 1; i++)
    {
        if ( strcmp(data->funcs[i].name, name->val.var) == 0 )
            found = true;
    }

    SYNTAX_ERROR (found, "function is already exist");

    data->pos++;

    SYNTAX_ERROR ( CUR_TOKEN.type   != OP &&
                   CUR_TOKEN.val.op != L_PAR, 
                   "expected '('");
    data->pos++;

    ENTER_SCOPE (FUNC_AREA);

    node_t* params = GetParams (data);
    data->func_pos++;
    
    node_t* body   = GetBlock (data);

    EXIT_SCOPE;

    connect1->left = func_op;
    func_op->left  = name;

    func_op->right = connect2;
    
    connect2->left  = params;
    connect2->right = body;

    return connect1;
}

node_t* GetReturn(ast_data* data)
{
    SYNTAX_ERROR ( CUR_TOKEN.type   != OP   || 
                   CUR_TOKEN.val.op != RETURN,
                   "expected return");

    node_t* return_op = &CUR_TOKEN;
    data->pos++;
    
    node_t* expr = GetExpresion(data);
    
    SYNTAX_ERROR( CUR_TOKEN.type   != OP  || 
                  CUR_TOKEN.val.op != SEMIC,
                  "expected ';'");

    node_t* semic = &CUR_TOKEN;
    data->pos++;
    
    return_op->left = expr;
    semic->left = return_op;

    return semic;
}

node_t* GetParams (ast_data* data)
{
    if (CUR_TOKEN.type == OP && CUR_TOKEN.val.op == R_PAR) 
    {
        data->pos++;
        return NULL;
    }

    SYNTAX_ERROR (CUR_TOKEN.type != VAR, "expected variable name");
    
    node_t* root = &CUR_TOKEN;
    node_t* old  = NULL;

    if (init_var (&data->scopes, CUR_TOKEN.val.var) == EXIST)
    {
        D_PRINT ("Failed to add parameter '%s'\n", CUR_TOKEN.val.var);
        return NULL;
    }

    data->pos++;
    
    while (true)
    {
        if (CUR_TOKEN.type == OP && CUR_TOKEN.val.op == R_PAR) 
        {
            data->pos++;
            break;
        }
        
        SYNTAX_ERROR (CUR_TOKEN.type   != OP  || 
                      CUR_TOKEN.val.op != COMMA,
                      "expected ','");

        data->pos++;
        
        SYNTAX_ERROR (CUR_TOKEN.type != VAR, "expected variable name");
        
        node_t* next_param = &CUR_TOKEN;

        if (init_var(&data->scopes, CUR_TOKEN.val.var) == EXIST)
        {
            D_PRINT ("Failed to add parameter '%s'\n", CUR_TOKEN.val.var);
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

    if ( CUR_TOKEN.type == OP )
    {
        switch (CUR_TOKEN.val.op)
        {
            case IF:     return GetIf (data);
            case WHILE:  return GetWhile (data);
            case RETURN: return GetReturn (data);
            case PRINT:  return GetPrint (data);
            case INIT:   return GetVarInit (data);
            default:     return NULL;
        }
    }

    else if (CUR_TOKEN.type == FUNCTION)
    {
        root = GetCall (data);

        SYNTAX_ERROR ( CUR_TOKEN.type   != OP  ||
                       CUR_TOKEN.val.op != SEMIC,
                       "expected ';'");

        node_t* semic = &CUR_TOKEN;

        semic->left = root;
        root = semic;

        data->pos++;
    }

    else if (CUR_TOKEN.type == VAR)
    {
        root = GetVarAct (data);
    }

    else
    {
        root = GetExpresion (data);

        SYNTAX_ERROR (CUR_TOKEN.type   != OP  ||
                      CUR_TOKEN.val.op != SEMIC,
                      "expected ';'");

        node_t* semic = &CUR_TOKEN;
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

    node_t* root = &CUR_TOKEN;
    data->pos++;

    if (CUR_TOKEN.type != CONNECTION)
    {
        D_PRINT("ERROR, connetion node missed\n");
        return NULL;
    }

    node_t* connection = &CUR_TOKEN;
    data->pos++;

    SYNTAX_ERROR (CUR_TOKEN.type   != OP  ||
                  CUR_TOKEN.val.op != L_PAR,
                  "expected '('");

    data->pos++;

    node_t* condition = GetExpresion (data);

    SYNTAX_ERROR (CUR_TOKEN.type   != OP  ||
                  CUR_TOKEN.val.op != R_PAR,
                  "expected ')'");

    data->pos++;

    ENTER_SCOPE (BLOCK_AREA);
    node_t* body = GetBlock (data);
    EXIT_SCOPE;

    if (CUR_TOKEN.type == OP && CUR_TOKEN.val.op == ELSE)
    {
        else_node = &CUR_TOKEN;
        data->pos++;

        ENTER_SCOPE (BLOCK_AREA);
        else_body = GetBlock (data);
        EXIT_SCOPE;
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
    node_t* root = &CUR_TOKEN;
    data->pos++;

    if (CUR_TOKEN.type != CONNECTION)
    {
        D_PRINT("ERROR, connetion node missed\n");
        return NULL;
    }

    node_t* connection = &CUR_TOKEN;
    data->pos++;

    SYNTAX_ERROR (CUR_TOKEN.type   != OP  ||
                  CUR_TOKEN.val.op != L_PAR,
                  "expected '('");

    node_t* condition = GetExpresion (data);

    SYNTAX_ERROR (CUR_TOKEN.type   != OP  ||
                  CUR_TOKEN.val.op != R_PAR,
                  "expected ')'");

    data->pos++;
    
    ENTER_SCOPE (BLOCK_AREA);
    node_t* body = GetBlock (data);
    EXIT_SCOPE;

    root->left = condition;
    root->right = body;

    connection->left = root;

    return connection;
}


node_t* GetPrint (ast_data* data)
{
    node_t* print = &CUR_TOKEN;
    data->pos++;

    node_t* exp = GetExpresion (data);

    SYNTAX_ERROR ( CUR_TOKEN.type   != OP  ||
                   CUR_TOKEN.val.op != SEMIC,
                   "expected ';'");

    node_t* semic = &CUR_TOKEN;
    data->pos++;

    semic->left = print;
    print->left = exp;

    return semic;
}

node_t* GetIn (ast_data* data)
{
    node_t* in = &CUR_TOKEN;
    data->pos++;

    return in;
}


node_t* GetExpresion (ast_data* data)
{
    return GetLogicOr (data);
}

node_t* GetLogicOr (ast_data* data)
{
    node_t* node = GetLogicAnd (data);
    
    while (CUR_TOKEN.type == OP && CUR_TOKEN.val.op == OR)
    {
        node_t* node_or = &CUR_TOKEN;
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
    
    while (CUR_TOKEN.type == OP && CUR_TOKEN.val.op == AND)
    {
        node_t* node_and = &CUR_TOKEN;
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
    
    while ( CUR_TOKEN.type == OP && 
            (CUR_TOKEN.val.op == EQEQ || CUR_TOKEN.val.op == NEQ) )
    {
        node_t* node_eq = &CUR_TOKEN;
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

    while ( CUR_TOKEN.type == OP &&
           (CUR_TOKEN.val.op == BIGR  || CUR_TOKEN.val.op == SMLR  ||
            CUR_TOKEN.val.op == EBIGR || CUR_TOKEN.val.op == ESMLR) )
    {
        node_t* node_cmp = &CUR_TOKEN;
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
    
    while (CUR_TOKEN.type == OP && 
          (CUR_TOKEN.val.op == ADD || CUR_TOKEN.val.op == SUB))
    {
        node_t* node_op = &CUR_TOKEN;
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
    
    while (CUR_TOKEN.type == OP && 
          (CUR_TOKEN.val.op == MUL || CUR_TOKEN.val.op == DIV))
    {
        node_t* node_op = &CUR_TOKEN;
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
    if (CUR_TOKEN.type == OP && CUR_TOKEN.val.op == SUB)
    {
        node_t* oper = &CUR_TOKEN;
        data->pos++;

        node_t* val = GetVal(data);
        
        oper->left = val;
        return oper;
    }

    else if (CUR_TOKEN.type == OP && CUR_TOKEN.val.op == SQRT)
    {
        node_t* oper = &CUR_TOKEN;
        data->pos++;

        node_t* val = GetVal (data);
        
        oper->left = val;
        return oper;
    }
    
    else if (CUR_TOKEN.type == OP && CUR_TOKEN.val.op == ADD)
    {
        data->pos++;
        return GetVal (data);
    }
    
    else return GetBracket (data);
}

node_t* GetBracket (ast_data* data)
{
    node_t* node = NULL;

    if (CUR_TOKEN.type == OP && CUR_TOKEN.val.op == L_PAR)
    {
        data->pos++;
        node = GetExpresion (data);
        
        SYNTAX_ERROR (CUR_TOKEN.type   != OP  ||
                      CUR_TOKEN.val.op != R_PAR,
                      "expected ')'");

        data->pos++;
        
        return node;
    }

    else return GetVal (data);
}

node_t* GetVal(ast_data* data)
{
    node_t* node = NULL;

    if (CUR_TOKEN.type == NUM)
    {
        node = &CUR_TOKEN;
        data->pos++;
    }
    
    else if (CUR_TOKEN.type == OP && CUR_TOKEN.val.op == IN)
    {
        node = GetIn (data);
    }

    else if (CUR_TOKEN.type == VAR)
    {   
        if (find_var(&data->scopes, CUR_TOKEN.val.var).offset == NOT_EXIST)
        { 
            D_PRINT ("ERROR: variable '%s' doesn't exist\n", CUR_TOKEN.val.var);
            return NULL;
        }

        node = &CUR_TOKEN;
        data->pos++;
    }
    
    else if (CUR_TOKEN.type == FUNCTION)
    {
        node = GetCall (data);
    }

    else if (CUR_TOKEN.type == OP && CUR_TOKEN.val.op == IN)
    {
        node = GetIn (data);
    }

    else if (CUR_TOKEN.type   == OP  && 
            (CUR_TOKEN.val.op == SUB || 
             CUR_TOKEN.val.op == ADD) )
    {
        node_t* op_node = &CUR_TOKEN;
        data->pos++;
        
        node_t* operand = GetVal(data);
        
        op_node->left = operand;
        node = op_node;
    }
    
    else SYNTAX_ERROR(1, "expected initialization");

    return node;
}


node_t* GetBlock (ast_data* data)
{
    SYNTAX_ERROR (CUR_TOKEN.type   != OP || 
                  CUR_TOKEN.val.op != L_BR,
                  "expected '{'");

    data->pos++;

    node_t* root = NULL;
    node_t* node = NULL;
    node_t* old  = NULL;

    while (CUR_TOKEN.type != OP || CUR_TOKEN.val.op != R_BR)
    {
        node = GetStatement (data);

        if (!node)
        {
            D_PRINT ("node == NULL\n");
            return NULL;
        }

        if (old) old->right = node;
        else root = node;

        old = node;
    }

    SYNTAX_ERROR (CUR_TOKEN.type   != OP || 
                  CUR_TOKEN.val.op != R_BR,
                  "expected '}'");

    data->pos++;

    return root;
}


node_t* GetVarInit (ast_data* data)
{
    node_t* init = &CUR_TOKEN;
    data->pos++;

    SYNTAX_ERROR (CUR_TOKEN.type != VAR, "expected variable name");

    node_t* var   = &CUR_TOKEN;
    node_t* semic = NULL;
    
    if (init_var (&data->scopes, var->val.var) == EXIST)
    {
        D_PRINT ("Failed to initialize variable '%s'\n", var->val.var);
        return NULL;
    }

    data->pos++;

    SYNTAX_ERROR (CUR_TOKEN.type   != OP || 
                  CUR_TOKEN.val.op != EQ ,
                  "expected '='");

    node_t* equal = &CUR_TOKEN;
    data->pos++;

    node_t* expr = GetExpresion (data);

    SYNTAX_ERROR (CUR_TOKEN.type   != OP  ||
                  CUR_TOKEN.val.op != SEMIC,
                  "expected ';'");


    semic = &CUR_TOKEN;
    data->pos++;

    equal->left  = var;
    equal->right = expr;

    init->left = equal;

    semic->left = init;

    return semic;
}

node_t* GetVarAct (ast_data* data)
{
    node_t* var = &CUR_TOKEN;

    if (find_var(&data->scopes, var->val.var).offset == NOT_EXIST)
    {
        D_PRINT("ERROR, variable '%s' doesn't exist\n", var->val.var);
        return NULL;
    }

    data->pos++;

    SYNTAX_ERROR (CUR_TOKEN.type   != OP || 
                  CUR_TOKEN.val.op != EQ ,
                  "expected '='");

    node_t* equal = &CUR_TOKEN;
    data->pos++;

    node_t* val = GetExpresion (data);

    SYNTAX_ERROR (CUR_TOKEN.type   != OP  ||
                  CUR_TOKEN.val.op != SEMIC,
                  "expected ';'");

    node_t* semic = &CUR_TOKEN;
    data->pos++;

    semic->left  = equal;
    equal->left  = var;
    equal->right = val;

    return semic;
}

node_t* GetArgs (ast_data* data)
{
    if (CUR_TOKEN.type == OP && CUR_TOKEN.val.op == R_PAR) 
    {
        data->pos++;
        return NULL;
    }
    
    node_t* root = GetExpresion (data);
   
    while (CUR_TOKEN.type == OP && CUR_TOKEN.val.op == COMMA)
    {  
        node_t* comma = &CUR_TOKEN;
        data->pos++;
        
        node_t* next_param = GetExpresion (data);
        
        comma->left  = root;
        comma->right = next_param;
        
        root = comma;
    }
    
    SYNTAX_ERROR (CUR_TOKEN.type   != OP  ||
                  CUR_TOKEN.val.op != R_PAR,
                  "expected ')'");
    data->pos++;
    
    return root;
}


node_t* GetCall (ast_data* data)
{
    node_t* function = &CUR_TOKEN;
    int check = 1;

    SYNTAX_ERROR (CUR_TOKEN.type != FUNCTION, "expected functiom name");

    for (size_t i = 0; i < data->func_pos && check != 0; i++ )
    {
        check = strcmp (CUR_TOKEN.val.var, data->funcs[i].name);
    }

    if (check)
        printf ("function %s doesnt exist\n", CUR_TOKEN.val.var);

    SYNTAX_ERROR(check, "");
    data->pos++;

    SYNTAX_ERROR (CUR_TOKEN.type   != OP  ||
                  CUR_TOKEN.val.op != L_PAR,
                  "expected '('");
    data->pos++;

    node_t* args = GetArgs (data);

    function->left = args;

    return function;
}
