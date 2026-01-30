#ifndef OPTIMIZATIONS_H
#define OPTIMIZATIONS_H

#include "tree.h"

#define TOLOW 10e-10

double CalcTree(node_t* root);

node_t* simplifier(node_t* root);

node_t* simplifySpecCases(node_t* node);

#endif