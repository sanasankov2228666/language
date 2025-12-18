#include "tree.h"
#include "graphic_dump.h"
#include "grammar.h"
#include "token.h"

int main()
{
    FILE* fp = file_opener (stderr, "example.txt", "r");   

    data_lexer data = Tokenization(fp);
    if (data.error)
    {
        TokenArrayFree(&data.tokens);
        return 1;
    }
    
    DebugTokens(&data.tokens);

    tree tree_data = {};

    tree_data.html_out = file_opener(stderr, "html_dump.html", "w");
    tree_data.root = ReadTree (&data);

    tree_dump (&tree_data, "check");
    PrintTreeFile (tree_data.root);

    TokenArrayFree(&data.tokens);
    
    return 0;
}