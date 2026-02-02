#include "tree.h"
#include "graphic_dump.h"
#include "grammar.h"
#include "token.h"

int main( int argc, char* argv[] )
{
    if (argc < 2)
    {
        printf("ERROR the code file was not specified\n");
        return LN_ERR;
    }

    FILE* source_code = file_opener (stderr, argv[1], "r");   

    data_lexer data = Tokenization(source_code);
    if (data.error)
    {
        TokenArrayFree(&data.tokens);
        return 1;
    }
    
    DebugTokens (&data.tokens);

    tree tree_data = {};

    tree_data.html_out = file_opener (stderr, "dumps/html_dump.html", "w");
    tree_data.root = ReadTree (&data);

    tree_dump (&tree_data, "check");
    PrintTreeFile (tree_data.root);

    TokenArrayFree(&data.tokens);

    return 0;
}