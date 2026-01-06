#include <stdio.h>
#include <string.h>
#include "lexems.h"


// ===================================================== МАССИВ С ОПЕРАТОРАМИ ==========================================================

struct type_lexem key_words[] =
{
    
    {IF,     "observer" ,    "if"     ,    8},
    {ELSE,   "else"     ,    "else"   ,    4},
    {WHILE,  "mine"     ,    "while"  ,    4},
    {FUNC,   "craft"    ,    "func"   ,    4},
    {RETURN, "result"   ,    "return" ,    6},
    {INIT,   "spawn"    ,    "init"   ,    4},
    {PRINT,  "print"    ,    "print"  ,    5},
    {IN,     "in"       ,    "in"     ,    2},
    {SQRT,   "mineroot" ,    "sqrt"   ,    2}, 
    

    {EQ,     "is"        ,       "=",       1},
    {BIGR,   "above"     ,       ">",       5},
    {SMLR,   "less"      ,       "<",       4},
    {EBIGR,  "at_most"   ,       ">=",      7},
    {ESMLR,  "at_least"  ,       "<=",      8},
    {EQEQ,   "same"      ,       "==",      4},
    {NEQ,    "different" ,       "!=",      9},
    

    {L_PAR,  "with"          ,    "(",     4  },
    {R_PAR,  "components"    ,    ")",     10 },
    {L_BR,   "workbench"     ,    "{",     14 },
    {R_BR,   "close"         ,    "}",     15 },
    {SEMIC,  "bedrock"       ,    ";",     7  },
    {COMMA,  "and"           ,    ",",     3  },
    

    {ADD,    "stack"   ,     "+",     5},
    {SUB,    "unstack" ,     "-",     7},
    {MUL,    "entchar" ,     "*",     7},
    {DIV,    "split"   ,     "/",     5},
    

    {AND,   "redstone_and" ,   "&&",    12},
    {OR,    "redstone_or"  ,   "||",    11},
};


int LEX_NUM = sizeof(key_words) / sizeof(key_words[0]);


int SearchKeyWord (char* str)
{
    int check = 1;
    for (int i = 0; i < LEX_NUM; i++)
    {
        check = strcmp( str , key_words[i].name);
        if (check == 0) return (int) i;
    }
    
    return UNREC_KEY;
}

int SearchKeyWordStd (char* str)
{
    int check = 1;
    for (int i = 0; i < LEX_NUM; i++)
    {
        check = strcmp( str , key_words[i].std_name);
        if (check == 0) return (int) i + 1;
    }
    
    return UNREC_KEY;
}

// ===================================================================================================================================