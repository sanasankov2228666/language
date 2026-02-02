#include "tree.h"
#include "treereader.h"
#include "graphic_dump.h"
#include "retranslator.h"
#include "config.h"

int main ( int argc, char* argv[] )
{
    if (argc < 3)
    {
        printf("ERROR, input and output files are not recognized\n");
        return LN_ERR;
    }

    FILE* middle_file = file_opener (stderr, argv[1], "r");
    if (!middle_file) return LN_ERR;

    tree data = {};
    data.root = read_tree (middle_file);
    tree_dump (&data, "check tree in retranslation");
    
    if (Retranslator (data.root, argv[2]))
    {
        D_PRINT ("ERROR in retranslating");
        return LN_ERR;
    }

    printf ("\nRetranslation complete in file %s\n", argv[2]);
    deleter (data.root);

    return LN_OK;
}

