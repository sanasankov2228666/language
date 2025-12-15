#ifndef TOKEN_H_
#define TOKEN_H_

#include "tree.h"

// ----------------------------------------------------------ТОКЕНЫ----------------------------------------------------------------------

#define UNREC_KEY -1

#define ZERO_ARR 

#define END_TOKEN -1

#define ERR_TOKEN -2

struct type_lexem
{
    int numb   = 0;
    const char* name = NULL;
    size_t len = 0;
};

extern struct type_lexem key_words[];

extern int LEX_NUM;

// ================ ВСЕ ТОКЕНЫ ====================
enum tokens
{
    IF = 1,
    ELSE,
    WHILE,
    FUNC,
    RETURN,
    INIT,
    
    EQ,
    BIGR,
    SMLR,
    EBIGR,
    ESMLR,
    EQEQ,
    NEQ,
    
    L_PAR,
    R_PAR,
    L_BR,
    R_BR,
    L_SBR,
    R_SBR,
    SEMIC,
    COMMA,
    COLON,
    DOT,
    
    ADD,
    SUB,
    MUL,
    DIV,
    POW,
    
    SIN,
    COS,
    TG,
    CTG,
    LN,
    LOG,
    SH,
    CH,
    ARSIN,
    ARCOS,
    ARTG,
    ARCTG,
    EXP,
    TH,
    CTH,

    AND,
    OR,
};


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
};

// ======================================================= ОСНОВНЫЕ ФУНКЦИИ ==========================================================

data_lexer Tokenization   (FILE* code);

node_t* ProcessToken       (data_lexer* data);

node_t* ProcessingBrackets (data_lexer* data);

node_t* ProcessingNum      (data_lexer* data);

node_t* ProcessingOperator (data_lexer* data);

node_t* ProcessingKeyWord  (data_lexer* data, int key);

node_t* ProcessingIdentificator (data_lexer* data, char* start, bool func);

LangErr_t ConnectionAdd    (data_lexer* data, node_t* token);


// =================================================== ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ =========================================================


LangErr_t DataLexerInit (data_lexer* data, FILE* fp);

char* MovePose (data_lexer* data);

node_t* EndToken ();

node_t* ErrToken ();

void SkipSpace (data_lexer* data);

int SearchKeyWord(char* str);

bool CheckFunc (data_lexer* data);


// ====================================================== СТРОКОВЫЕ ФУНКЦИИ ============================================================


size_t file_read(FILE* stream, char** buffer);

long int size_file(FILE* stream);


// ==================================================== ФУНКЦИИ БУФЕРА ТОКЕНОВ ==========================================================


LangErr_t TokenArrayInit (tokens_arr* data, size_t capacity);

node_t* GetToken (tokens_arr* data, size_t place);

LangErr_t PutToken (tokens_arr* data, node_t* val);

size_t TokenArraySize(const tokens_arr* data);

size_t TokenArrayCapacity(const tokens_arr* data);

void TokenArrayFree(tokens_arr* data);

void DebugTokens(const tokens_arr* tokens);


#endif