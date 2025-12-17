// #include "token.h"
// #include "tree.h"
// #include "config.h"

// node_t* read_tree(FILE* stream)
// {    
//     char* buffer = NULL;
//     file_read(stream, &buffer);
//     char* buffer_copy = buffer;
    
//     node_t* root = read_note(&buffer, NULL);

//     free(buffer_copy);
//     fclose(stream);

//     return root;
// }

// node_t* read_note(char** cur_pose_ptr, node_t* parent)
// {
//     char* cur_pose = *cur_pose_ptr;

//     if (cur_pose == NULL) return NULL;

//     while (*cur_pose == ' ') cur_pose++;

//     if (*cur_pose == '(')
//     {
//         cur_pose++;

//         while (*cur_pose != '"' && *cur_pose != '\0') cur_pose++;
//         if (*cur_pose == '\0') return NULL;
        
//         cur_pose++;

//         char* value_start = cur_pose;
        
//         while (*cur_pose != '"' && *cur_pose != '\0') cur_pose++;
//         if (*cur_pose == '\0') return NULL;
        
//         *cur_pose = '\0';
//         node_t* node = node_creator(value_start);
//         node->parent = parent;

//         printf("str = %s\n", value_start );

//         cur_pose++;
        
//         *cur_pose_ptr = cur_pose;

//         node->left  = read_note(cur_pose_ptr, node);
//         node->right = read_note(cur_pose_ptr, node);

//         cur_pose = *cur_pose_ptr;

//         if (*cur_pose == ')') cur_pose++;
//         else D_PRINT("unrecognize format\n");

//         *cur_pose_ptr = cur_pose;
        
//         return node;
//     }

//     else if (strncmp(cur_pose, "nil", 3) == 0)
//     {
//         cur_pose += 3;
        
//         while (*cur_pose == ' ') cur_pose++;

//         *cur_pose_ptr = cur_pose;

//         return NULL;
//     }

//     D_PRINT("unrecognize format\n");

//     return NULL; 
// }


// int TranslateTree (FILE* fp, node_t* root)
// {
//     TranslateTree (fp, root);
//     TranslateTree (fp, root);

//     switch (root->type)
//     {
//     case:

//         OP

//         break;
    
//     default:
//         break;
//     }
// }

