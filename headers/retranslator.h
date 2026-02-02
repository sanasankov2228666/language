#ifndef RETRANSLATOR_H
#define RETRANSLATOR_H

#include <stdio.h>
#include "tree.h"

#define MID_PRNT(...)\
do\
{\
fprintf (output_fp, __VA_ARGS__);\
}\
while (0)\

// ==================================================== ФУНКЦИИ ДЛЯ РЕТРАНСЛЯЦИИ ====================================================


LangErr_t Retranslator          (node_t* root, char* out_name);

LangErr_t RetranslateNode       (node_t* node, FILE* output_fp);

LangErr_t RetranslateOp         (node_t* node, FILE* output_fp);

LangErr_t RetranslateVar        (node_t* node, FILE* output_fp);

LangErr_t RetranslateNum        (node_t* node, FILE* output_fp);

LangErr_t RetranslateCallFunc   (node_t* node, FILE* output_fp);

LangErr_t RetranslateCalc       (node_t* node, FILE* output_fp);

LangErr_t RetranslateFuncParams (node_t* node, FILE* output_fp);

LangErr_t RetranslateUn         (node_t* node, FILE* output_fp);

LangErr_t RetranslateVarInit    (node_t* node, FILE* output_fp);

LangErr_t RetranslateFunc       (node_t* node, FILE* output_fp);

LangErr_t RetranslateEq         (node_t* node, FILE* output_fp);

LangErr_t RetranslateIf         (node_t* node, FILE* output_fp);

LangErr_t RetranslateCond       (node_t* node, FILE* output_fp);

LangErr_t RetranslateWhile      (node_t* node, FILE* output_fp);

LangErr_t RetranslateReturn     (node_t* node, FILE* output_fp);

LangErr_t RetranslatePrint      (node_t* node, FILE* output_fp);

LangErr_t RetranslateIn         (node_t* node, FILE* output_fp);

LangErr_t RetranslateSemic      (node_t* node, FILE* output_fp);

#endif