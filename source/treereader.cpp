#include "config.h"
#include "lexems.h"
#include "str_funcs.h"
#include "treereader.h"

// =================================================== МАССИВ С ТИПАМИ =====================================================


static const char* types[] = { "op" , "var", "num", "func", "cnct" };

static int NUM_TYPES = sizeof(types) / sizeof(types[0]);


// =================================================== ЧТЕНИЕ ДЕРЕВА =======================================================


node_t* read_tree(FILE* stream)
{    
    char* buffer = NULL;
    file_read(stream, &buffer);
    char* buffer_copy = buffer;
    
    node_t* root = read_node(&buffer, NULL);

    free(buffer_copy);
    fclose(stream);

    return root;
}

node_t* read_node(char** cur_pose_ptr, node_t* parent)
{
    if (!cur_pose_ptr || !*cur_pose_ptr)
        return NULL;
    
    char* cur = *cur_pose_ptr;
    
    while (*cur == ' ' || *cur == '\t' || *cur == '\n') cur++;
    
    if (*cur == '(')
    {
        cur++;
        
        while (*cur == ' ' || *cur == '\t') cur++;
        
        char* type_start = cur;
        while (*cur != ':' && *cur != '\0') cur++;
        
        if (*cur == '\0')
        {
            D_PRINT("Error: unexpected end\n");
            return NULL;
        }
        
        char saved_char = *cur;
        *cur = '\0';
        int type = check_type(type_start);
        *cur = saved_char;
        
        if (type < 0)
        {
            D_PRINT("Error: unknown type '%s'\n", type_start);
            return NULL;
        }
        
        cur++;
        while (*cur == ' ' || *cur == '\t') cur++;
        
        if (*cur != '"')
        {
            D_PRINT("Error: expected '\"' after type\n");
            return NULL;
        }
        cur++;
        
        char* val_start = cur;
        while (*cur != '"' && *cur != '\0') cur++;
        
        if (*cur == '\0')
        {
            D_PRINT("Error: unexpected end of file in value\n");
            return NULL;
        }
        
        saved_char = *cur;
        *cur = '\0';

        tree_t val = {};

        switch (type)
        {
        case OP:
            val.op = SearchKeyWordStd (val_start);
            break;
        case VAR:
        case FUNCTION:
            val.var = strdup(val_start);
            break;
        
        case NUM:
            val.num = atoi(val_start);
            break;

        default:
            break;
        }

        *cur = saved_char;
        
        cur++;
        
        node_t* node = NodeCreator(stderr, type, val, NULL, NULL);
        if (!node)
            return NULL;
        
        node->parent = parent;
        
        *cur_pose_ptr = cur;
        
        node->left = read_node(cur_pose_ptr, node);
        node->right = read_node(cur_pose_ptr, node);
        
        cur = *cur_pose_ptr;
        
        while (*cur == ' ' || *cur == '\t') cur++;
        if (*cur == ')') cur++;

        else
        {
            D_PRINT("Error: expected ')' but found '%c'\n", *cur);
            free(node);
            return NULL;
        }
        
        *cur_pose_ptr = cur;
        return node;
    }

    else if (strncmp(cur, "nil", 3) == 0)
    {
        cur += 3;
        while (*cur == ' ' || *cur == '\t' || *cur == '\n')cur++;
        *cur_pose_ptr = cur;

        return NULL;
    }
    
    D_PRINT("Error: unrecognized format at '%c'\n", *cur);
    return NULL;
}


int check_type(const char* str)
{
    for (int i = 0; i < NUM_TYPES; i++)
    {
        if (strcmp(str, types[i]) == 0)
        {
            return i + 1;
        }
    }
    
    D_PRINT("Error: unrecognized type '%s'\n", str);
    return -1;
}
