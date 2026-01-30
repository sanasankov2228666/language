#include "tree.h"
#include "config.h"
#include "lexems.h"

node_t* NodeCreator (FILE* log, int type, tree_t val, node_t* left, node_t* right)
{
    node_t* node = (node_t*) calloc (1, sizeof(node_t));
    if (node == NULL)
    {
        DBG(log, "node = NULL");
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


    DBG( log, "type doesnt exist\n");
    free(node);

    return NULL;
}

node_t* CreateNum(int value)
{
    tree_t val = {.num = value};
    return NodeCreator(stderr, NUM, val, NULL, NULL);
}

node_t* CreateVar(const char* name)
{
    char* copy_name = strdup (name);
    tree_t val = {.var = copy_name};
    free (copy_name);

    return NodeCreator(stderr, VAR, val, NULL, NULL);
}

node_t* CreateOp(int op, node_t* left, node_t* right)
{
    tree_t val = {.op = op};
    node_t* ret_node = NodeCreator(stderr, OP, val, left, right);

    return ret_node;
}

node_t* NodeCopy (node_t* node)
{
    node_t* node_copy = NodeCreator (stderr, node->type, node->val, NULL, NULL);
    return node_copy;
}

node_t* CopySubtree(node_t* node)
{
    if (!node) return NULL;
    
    node_t* new_node = NodeCreator(stderr, node->type, node->val, NULL, NULL);
    if (new_node == NULL) return NULL;

    new_node->left = CopySubtree(node->left);
    new_node->right = CopySubtree(node->right);
    
    return new_node;
}

int LRpos(node_t* node)
{
    if (node->parent->left == node) return 1;
    else if (node->parent->right == node) return 2;
    else return -1;
}

FILE* file_opener (FILE* log, const char * Filename, const char * Mode)
{
    FILE* stream = fopen(Filename, Mode);
    if (stream == NULL)
    {
        DBG(log, "file %s open error", Filename);
        return NULL;
    }

    return stream;
}

//! функция удаления
void deleter (node_t* root)
{
    if (root == NULL) return;
    
    deleter(root->left);
    deleter(root->right);
    
    if ( root->type == VAR     ||
         root->type == FUNCTION ) 
    {   
         free(root->val.var);
    }

    free(root);
}

// ================================================ ВЫВОД ДЕРЕВА В ФАЙЛ =========================================================


LangErr_t PrintTreeFile (node_t* root)
{
    FILE* middle_end_fp = file_opener (stderr, "source/middleend/middle_end", "w");
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
        fprintf (stream, "op: ");
        fprintf (stream, "\"%s\"", key_words[root->val.op - 1].std_name);
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