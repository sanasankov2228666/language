#include "tree.h"
#include "lexems.h"
#include "debug.h"
#include "str_funcs.h"

// ================================================= ВЗАИМОДЕЙТСИВЕ С ДЕРЕВОМ =======================================================

node_t* NodeCreator (int type, tree_t val, node_t* left, node_t* right)
{
    node_t* node = (node_t*) calloc (1, sizeof(node_t));
    if (node == NULL)
    {
        D_PRINT ("node = NULL\n");
        return NULL;
    }
    
    node->left  = left;
    node->right = right;

    if (left != NULL) left->parent = node;
    if (right != NULL) right->parent = node;

    if (type == OP)
    {
        node->type   = type;
        node->val.op = val.op;

        return node;
    }

    else if (type == VAR || type == FUNCTION)
    {
        node->type    = type;
        node->val.var = val.var;

        return node;
    }

    else if (type == NUM)
    {
        node->type    = type;
        node->val.num = val.num;
        return node;
    }

    else if (type == CONNECTION)
    {
        node->type    = type;
        return node;
    }


    D_PRINT ("type doesnt exist\n");
    free(node);

    return NULL;
}

node_t* CreateNum(int value)
{
    tree_t val = {.num = value};
    return NodeCreator(NUM, val, NULL, NULL);
}

node_t* CreateVar(const char* name)
{
    char* copy_name = strdup (name);
    tree_t val = {.var = copy_name};
    free (copy_name);

    return NodeCreator(VAR, val, NULL, NULL);
}

node_t* CreateOp(int op, node_t* left, node_t* right)
{
    tree_t val = {.op = op};
    node_t* ret_node = NodeCreator(OP, val, left, right);

    return ret_node;
}

node_t* NodeCopy (node_t* node)
{
    node_t* node_copy = NodeCreator (node->type, node->val, NULL, NULL);
    return node_copy;
}

node_t* CopySubtree (node_t* node)
{
    if (!node) return NULL;
    
    node_t* new_node = NodeCreator (node->type, node->val, NULL, NULL);
    if (new_node == NULL) return NULL;

    new_node->left  = CopySubtree (node->left);
    new_node->right = CopySubtree (node->right);
    
    return new_node;
}

int LRpos (node_t* node)
{
    if (node->parent->left == node) return 1;
    else if (node->parent->right == node) return 2;
    else return -1;
}

//! функция удаления
void TreeDeleter (node_t* root)
{
    if (root == NULL) return;
    
    TreeDeleter (root->left);
    TreeDeleter (root->right);
    
    if ( root->type == VAR     ||
         root->type == FUNCTION ) 
    {   
         free (root->val.var);
    }

    free (root);
}


// ======================================================= ЧТЕНИЕ ДЕРЕВА ============================================================

static const char* S_TYPES[] = { "op" , "var", "num", "func", "cnct" };

static int S_NUM_TYPES = sizeof(S_TYPES) / sizeof(S_TYPES[0]);

node_t* ReadTree (FILE* stream)
{    
    char* buffer = NULL;
    file_read (stream, &buffer);
    char* buffer_copy = buffer;
    
    node_t* root = ReadNode (&buffer, NULL);

    free (buffer_copy);
    fclose (stream);

    return root;
}

node_t* ReadNode (char** cur_pose_ptr, node_t* parent)
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
                val.op = SearchKeyWordStd (val_start) + 1;
                break;
            case VAR:
            case FUNCTION:
                val.var = strdup (val_start);
                break;
            
            case NUM:
                val.num = atoi (val_start);
                break;

            default:
                break;
        }

        *cur = saved_char;
        
        cur++;
        
        node_t* node = NodeCreator (type, val, NULL, NULL);
        if (!node)
            return NULL;
        
        node->parent = parent;
        
        *cur_pose_ptr = cur;
        
        node->left =  ReadNode (cur_pose_ptr, node);
        node->right = ReadNode (cur_pose_ptr, node);
        
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


int check_type (const char* str)
{
    for (int i = 0; i < S_NUM_TYPES; i++)
    {
        if (strcmp(str, S_TYPES[i]) == 0)
        {
            return i + 1;
        }
    }
    
    D_PRINT("Error: unrecognized type '%s'\n", str);
    return -1;
}


// ================================================ ВЫВОД ДЕРЕВА В ФАЙЛ =========================================================

LangErr_t PrintTreeFile (node_t* root)
{
    FILE* middle_end_fp = file_opener ("source/middleend/middle_end", "w");
    if (!middle_end_fp) return LN_ERR;

    PrintNode (root, middle_end_fp);

    printf("tree was saved in middle end file\n");

    return LN_OK;
}

void PrintNode (node_t* root, FILE* stream)
{
    if (!root) 
    {
        fprintf (stream, " nil");
        return;
    }

    fprintf (stream, "(");
    
    switch (root->type)
    {
    case OP:
        fprintf(stream, "op: ");
        if (root->val.op >= 1 && root->val.op <= G_LEX_NUM)
            fprintf(stream, "\"%s\"", key_words[root->val.op - 1].std_name);
        else
            fprintf(stream, "\"%d\"", root->val.op);
        break;
    
    case VAR:
        fprintf (stream, "var: ");
        fprintf (stream, "\"%s\"", root->val.var);
        break;

    case FUNCTION:
        fprintf (stream, "func: ");
        fprintf (stream, "\"%s\"", root->val.var);
        break;

    case NUM:
        fprintf (stream, "num: ");
        fprintf (stream, "\"%d\"", root->val.num);
        break;
       
    case CONNECTION:
        fprintf (stream, "cnct: ");
        fprintf (stream, "\"cnct\"");
        break;

    default:
        fprintf (stream, "unknown:");
        break;
    }

    PrintNode (root->left, stream);
    PrintNode (root->right, stream);

    fprintf (stream, ")");
}