#include "optimizations.h"
#include "treereader.h"
#include "config.h"
#include "graphic_dump.h"
#include "grammar.h"

int main(void)
{
    FILE* midlend_file = file_opener (stderr, "source/middleend/middle_end", "r");
    if (!midlend_file) return LN_ERR;

    tree data = {};
    node_t* root = read_tree(midlend_file);
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
    deleter (new_root);

    return 0;
}