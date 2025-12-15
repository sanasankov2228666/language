#ifndef DOTGENERATOR_H_

#include <stdio.h>
#include "tree.h"

//-------------------------------------------------------ГЕНЕРАТОР-ДАМПА-----------------------------------------------------------

LangErr_t dump_generator(tree* data, const char* filename, const char* reason);

void dot_node_gen(node_t* root, FILE* dot_file, size_t* number);

void dot_arrw_gen(node_t* root, FILE* dot_file, size_t* number, size_t parent_number);

void tree_dump(tree* data, const char* reason);

LangErr_t html_generator(const char* filename, tree* data);

#endif