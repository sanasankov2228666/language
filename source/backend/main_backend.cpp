#include "translator.h"
#include "graphic_dump.h"
#include "treereader.h"

int main( int argc, char* argv[] )
{
    if (argc < 2)
    {
        printf("ERROR the code file was not specified\n");
        return LN_ERR;
    }

    tree data = {};
    FILE* tree_fp = file_opener (stderr, "source/middleend/middle_end", "r");
    if (!tree_fp) return 1;

    data.root = read_tree (tree_fp);
    tree_dump (&data, "check tree in backend");

    if (TranslateTree (data.root, argv[1]))
    {
        deleter(data.root);
        return 1;
    }

    printf ("\nFile %s was generated\n", argv[1]);
    
    deleter (data.root);

    return 0;
}