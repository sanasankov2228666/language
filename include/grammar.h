#ifndef READ_TREE_H_
#define READ_TREE_H_

#include <stdio.h>

#include "tree.h"
#include "tokenization.h"
#include "area_stack.h"
#include "options.h"

#define SYNTAX_ERROR(condition, text) do\
{\
    if ( (condition) && is_correct)\
    {\
        is_correct = false;\
        printf ("\n==========ERROR===========");\
        printf ("\nSyntax error at %s:%d (GARAMAR RULE: %s)\n", __FILE__, __LINE__, __func__); \
        printf ("\n%s\n", text); \
        printf ("position: ... line %zu, %zu\n", CUR_TOKEN.line, CUR_TOKEN.column );\
        printf ("\n==========================\n");\
        return  NULL; \
    }\
}\
while(0)

#define ENTER_SCOPE(TYPE) \
if (enter_scope (&data->scopes, TYPE) != LN_OK)\
{\
    D_PRINT("Failed to enter function scope\n"); \
    return NULL; \
}\

#define EXIT_SCOPE \
if (exit_scope (&data->scopes) != LN_OK)\
{\
    D_PRINT("Failed to enter function scope\n");\
    return NULL;\
}\

#define CUR_TOKEN data->tokens.arr[data->pos]
#define NXT_TOKEN data->tokens.arr[data->pos + 1]

struct func
{
    char* name = NULL;
    char* vars_loc[MAX_FUNC_PARAMS] = {};
};

// =================================================== ГЛАВНЫЕ ФУНКЦИИ ========================================================

struct ast_data
{
    tokens_arr tokens   = {};
    stack_scopes scopes = {};
    func*  funcs = NULL;
    size_t func_pos = 0;
    size_t pos = 0;
};

node_t* MakeTree         (data_lexer* data_lex);
LangErr_t AstDataInit    (ast_data* data, data_lexer* data_lex);
LangErr_t AstDataDeleter (ast_data* data);

// ================================================= ФУНКЦИИ ДЛЯ ЧТЕНИЯ ========================================================

node_t* GetProgram   (ast_data* data);
node_t* GetFunc      (ast_data* data);
node_t* GetParams    (ast_data* data);
node_t* GetReturn    (ast_data* data);
node_t* GetStatement (ast_data* data);
node_t* GetIf        (ast_data* data);
node_t* GetWhile     (ast_data* data);
node_t* GetPrint     (ast_data* data);
node_t* GetIn        (ast_data* data);
node_t* GetExpresion (ast_data* data);
node_t* GetLogicAnd  (ast_data* data);
node_t* GetLogicOr   (ast_data* data);
node_t* GetEquality  (ast_data* data);
node_t* GetCompare   (ast_data* data);
node_t* GetAddSub    (ast_data* data);
node_t* GetMulDiv    (ast_data* data);
node_t* GetUnar      (ast_data* data);
node_t* GetBracket   (ast_data* data);
node_t* GetVal       (ast_data* data);
node_t* GetBlock     (ast_data* data);
node_t* GetVarInit   (ast_data* data);
node_t* GetVarAct    (ast_data* data);
node_t* GetArgs      (ast_data* data);
node_t* GetCall      (ast_data* data);

#endif