#ifndef TOKEN_H_
#define TOKEN_H_

#include "tree.h"

// ----------------------------------------------------------ТОКЕНЫ----------------------------------------------------------------------


#define ZERO_ARR 

#define END_TOKEN -1

#define ERR_TOKEN -2


// =========== СТРУКТУРА МАССИВА ТОКЕНОВ ============
struct tokens_arr
{
    node_t** arr = NULL;
    size_t size     = 0;
    size_t capacity = 0;
};


// =============== СТРУКТУРА ЛЕКСЕРА ================
struct data_lexer
{
    char* source = NULL;
    size_t source_len  = 0;

    tokens_arr tokens  = {};

    char* current_pos = NULL;

    size_t line      = 1;
    size_t column    = 1;

    int error = 0;
};

// ======================================================= ОСНОВНЫЕ ФУНКЦИИ ==========================================================

data_lexer Tokenization    (FILE* code);

node_t* ProcessToken       (data_lexer* data);

node_t* ProcessingBrackets (data_lexer* data);

node_t* ProcessingNum      (data_lexer* data);

node_t* ProcessingOperator (data_lexer* data);

node_t* ProcessingKeyWord  (data_lexer* data, int key);

node_t* ProcessingIdentificator (data_lexer* data, char* start, bool check_is_func);

LangErr_t CreateConnection (data_lexer* data);

LangErr_t ConnectionAdd    (data_lexer* data, node_t* token);


// =================================================== ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ =========================================================


LangErr_t DataLexerInit (data_lexer* data, FILE* fp);

char* MovePose (data_lexer* data);

node_t* EndToken ();

node_t* ErrToken ();

void SkipSpace (data_lexer* data);

bool CheckFunc(data_lexer* data);


// ==================================================== ФУНКЦИИ БУФЕРА ТОКЕНОВ ==========================================================


LangErr_t TokenArrayInit (tokens_arr* data, size_t capacity);

node_t* GetToken (tokens_arr* data, size_t place);

LangErr_t PutToken (tokens_arr* data, node_t* val);

size_t TokenArraySize(const tokens_arr* data);

size_t TokenArrayCapacity(const tokens_arr* data);

void TokenArrayFree(tokens_arr* data);

void DebugTokens(const tokens_arr* tokens);


#endif