#include "optimizations.h"
#include "tree.h"
#include "debug.h"
#include "graphic_dump.h"
#include "grammar.h"
#include "file_opener.h"

int main (void)
{
    FILE* midlend_file = file_opener ("source/middleend/middle_end", "r");
    if (!midlend_file) return LN_ERR;

    printf ("------------------------ middleend -----------------------\n");

    tree data = {};
    node_t* root = ReadTree (midlend_file);
    if (!root) 
    {
        D_PRINT ("ERROR, root is NULL, problems with reading tree");
        return LN_ERR;
    }

    node_t* new_root = simplifier (root);
    if (!new_root) 
    {
        D_PRINT ("ERROR, symplyfy problems new root is NULL");
        return LN_ERR;
    }

    data.root = new_root;
    tree_dump (&data, "check tree in middleend");

    PrintTreeFile (new_root);
    TreeDeleter (new_root);

    printf ("---------------------------------------------------------\n");

    return LN_OK;
}