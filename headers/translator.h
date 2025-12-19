#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include "tree.h"
#include "area_stack.h"

#define TREE_FILE_CHECK(cond)\
if (cond)\
{\
    D_PRINT("bad middle end tree\n");\
    return NULL;\
}\

#define CMP_REG "CX"
#define ADR_REG "AX"


node_t* read_tree (FILE* stream);

node_t* read_node (char** cur_pose_ptr, node_t* parent);

int check_type (const char* str);


// ===================================================== ФУНКЦИИ ТРАНСЛЯЦИИ ==============================================================


LangErr_t TranslateTree     (node_t* root, const char* filename);

LangErr_t CompileOnlyFunc   (node_t* node, FILE* asm_file, stack_scopes* stack);

LangErr_t CompileNotFunc    (node_t* node, FILE* asm_file, stack_scopes* stack);

LangErr_t TranslateNodeSpecial (node_t* node, FILE* asm_file, stack_scopes* stack);

LangErr_t TranslateNode     (node_t* node, FILE* asm_file, stack_scopes* stack);

LangErr_t TranslateOp       (node_t* node, FILE* asm_file, stack_scopes* stack);

LangErr_t TranslateCalc     (node_t* node, FILE* asm_file, stack_scopes* stack);

LangErr_t TranslateCond     (node_t* node, FILE* asm_file, stack_scopes* stack);

LangErr_t TranslateIf       (node_t* node, FILE* asm_file, stack_scopes* stack);

LangErr_t TranslateWhile    (node_t* node, FILE* asm_file, stack_scopes* stack);

LangErr_t TranslateReturn   (node_t* node, FILE* asm_file, stack_scopes* stack);

LangErr_t TranslateVarInit  (node_t* node, FILE* asm_file, stack_scopes* stack);

LangErr_t TranslateFunc     (node_t* node, FILE* asm_file, stack_scopes* stack);

LangErr_t TranslateVar      (node_t* node, FILE* asm_file, stack_scopes* stack);

LangErr_t TranslateEq       (node_t* node, FILE* asm_file, stack_scopes* stack);

LangErr_t TranslateCallFunc (node_t* node, FILE* asm_file, stack_scopes* stack);

LangErr_t HelpFuncParamInit (stack_scopes* stack, char* name, FILE* asm_file);


// =======================================================================================================================================



#endif