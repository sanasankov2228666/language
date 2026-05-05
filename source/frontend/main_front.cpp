#include "tree.h"
#include "graphic_dump.h"
#include "tokenization.h"
#include "debug.h"
#include "grammar.h"
#include "file_opener.h"

int main ( int argc, char* argv[] )
{
    if (argc < 2)
    {
        printf ("ERROR the code file was not specified\n");
        return LN_ERR;
    }

    printf ("------------------------ frontend -------------------\n");

    FILE* source_code = file_opener (argv[1], "r");   
    if (!source_code) return LN_ERR;

    data_lexer data = Tokenization (source_code);
    if (data.error)
    {
        TokenArrayFree (&data.tokens);
        fclose (source_code);
        return LN_ERR;
    }
    
    DebugTokens (&data.tokens);
    fclose (source_code);

    tree tree_data = {};
    tree_data.root = MakeTree (&data);

    tree_dump (&tree_data, "check");
    PrintTreeFile (tree_data.root);

    TokenArrayFree (&data.tokens);

    printf ("---------------------------------------------------------\n");

    return LN_OK;
}