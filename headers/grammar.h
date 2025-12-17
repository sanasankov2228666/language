#ifndef READ_TREE_H_
#define READ_TREE_H_

#include <stdio.h>
#include "tree.h"
#include "token.h"

#define SYNTAX_ERROR(condition) do\
{\
    if (condition)\
    {\
        printf("\n==========ERROR===========");\
        printf("\nSyntax error at %s:%d (GARAMAR RULE: %s)\n", __FILE__, __LINE__, __func__); \
        printf("position: ... line %zu, %zu\n", CUR_TOKEN->line, CUR_TOKEN->column );\
        printf("\n==========================\n");\
        return  NULL; \
    }\
}\
while(0)

#define UNREC -1

#define GLOBAL 9

#define MAX_FUNC_PARAMS 10
#define MAX_VARS        100
#define MAX_FUNCS       1000

#define CUR_TOKEN data->tokens.arr[data->pos]
#define NXT_TOKEN data->tokens.arr[data->pos + 1]

struct func
{
    char* name      = NULL;
    char* vars_loc[MAX_FUNC_PARAMS] = {};
};


// ================================================= ОБЛАСТЬ ВИДИМОСТИ =========================================================

#define BLOCK 10

struct scope
{
    char** table = NULL;
    size_t var_count = 0;
    size_t var_capacity = 0;
    int area_type = GLOBAL; 
};


struct stack_scopes
{
    scope** scopes = NULL;
    size_t size      = 0;
    size_t capacity  = 0;
    size_t cur_scope = 0;
};


LangErr_t stack_scopes_init (stack_scopes* stack, size_t capacity);

LangErr_t scope_init (scope* sc, int area_type);

LangErr_t enter_scope (struct stack_scopes* stack, int area_type);

LangErr_t exit_scope (stack_scopes* stack);

LangErr_t init_var (stack_scopes* stack, char* name);

bool find_var(stack_scopes* stack, const char* name);

LangErr_t stack_scopes_destroy(stack_scopes* stack);


// =================================================== ГЛАВНЫЕ ФУНКЦИИ ========================================================


struct ast_data
{
    tokens_arr tokens   = {};
    stack_scopes scopes = {};

    func*  funcs = NULL;
    size_t func_pos = 0;

    size_t pos = 0;
};


node_t*   ReadTree (data_lexer* data_lex);

LangErr_t AstDataInit (ast_data* data, data_lexer* data_lex);

LangErr_t AstDataDeleter (ast_data* data);


// ================================================= ФУНКЦИИ ДЛЯ ЧТЕНИЯ ========================================================


node_t* GetProgram   (ast_data* data);

node_t* GetFunc      (ast_data* data);

node_t* GetParams    (ast_data* data);

node_t* GetReturn    (ast_data* data);

node_t* GetStatement (ast_data* data);

node_t* GetIf        (ast_data* data);

node_t* GetWhile     (ast_data* data);

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


// ================================================ ВЫВОД ДЕРЕВА В ФАЙЛ =========================================================


LangErr_t PrintTreeFile (node_t* root);

void PrintNode (node_t* root, FILE* stream);


// ============================================================================================================================

#endif
