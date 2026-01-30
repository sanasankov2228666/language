#include "tree.h"
#include "treereader.h"
#include "graphic_dump.h"

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
    
    fclose (middle_file);
    

}

