#ifndef LANGUAGE_H_
#define LANGUAGE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef int LangErr_t;  

enum error
{
    LN_OK   = 0,
    LN_ERR  = 1,
};

// типы в узле 
enum types
{
    OP         = 1,
    VAR        = 2,
    NUM        = 3,
    FUNCTION   = 4,
    CONNECTION = 5
};

// значение узла
union tree_t
{
    int  op;
    char*  var; 
    double num;
};

// структруа узла
struct node_t
{
    node_t* parent = NULL;
    int     type   = 0;
    tree_t  val;

    size_t line      = 0;
    size_t column    = 0;
    size_t start_pos = 0;

    node_t* right = NULL;
    node_t* left  = NULL;
};

// структура дерева
struct tree
{
    node_t* root     = NULL;
    size_t  size     = 0;
    size_t  error    = 0;
    FILE*   log      = NULL;
    FILE*   html_out = NULL;
};


node_t* NodeCreator (FILE* log, int type, tree_t val, node_t* left, node_t* right);

node_t* CreateNum(double value);

node_t* CreateVar(const char* name);

node_t* CreateOp(int op, node_t* left, node_t* right);

node_t* NodeCopy (node_t* node);

node_t* CopySubtree(node_t* node);

int LRpos(node_t* node);

FILE* file_opener (FILE* log, const char * Filename, const char * Mode);

void deleter (node_t* root);

#endif