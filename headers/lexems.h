#ifndef LEXEMS_H
#define LEXEMS_H

#include <stdio.h>

#define UNREC_KEY -1

struct type_lexem
{
    int numb   = 0;
    const char* name = NULL;
    const char* std_name = NULL;
    size_t len = 0;
};

extern struct type_lexem key_words[];

extern int LEX_NUM;


enum {
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
    SEMIC,
    COMMA,
    
    ADD,
    SUB,
    MUL,
    DIV,
    
    AND,
    OR
};

int SearchKeyWord    (char* str);

int SearchKeyWordStd (char* str);

#endif