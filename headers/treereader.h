#ifndef TREEREADER_H
#define TREEREADER_H

#include "tree.h"

node_t* read_tree (FILE* stream);

node_t* read_node (char** cur_pose_ptr, node_t* parent);

int check_type (const char* str);

#endif