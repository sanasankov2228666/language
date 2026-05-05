#include "translator.h"
#include "graphic_dump.h"
#include "tree.h"
#include "file_opener.h"
#include "debug.h"

int main( int argc, char* argv[] )
{
    if (argc < 2)
    {
        printf("ERROR the code file was not specified\n");
        return LN_ERR;
    }

    printf ("-------------------------- backend -------------------------\n");

    tree data = {};
    FILE* tree_fp = file_opener ("source/middleend/middle_end", "r");
    if (!tree_fp) return 1;

    data.root = ReadTree (tree_fp);
    tree_dump (&data, "check tree in backend");

    if (TranslateTree (data.root, argv[1]))
    {
        TreeDeleter (data.root);
        return 1;
    }

    printf ("\nFile %s was generated\n", argv[1]);
    
    TreeDeleter (data.root);

    printf ("---------------------------------------------------------\n");
    
    return 0;
}