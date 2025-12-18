#include "translator.h"
#include "graphic_dump.h"

int main (void)
{
    tree data = {};
    FILE* tree_fp = file_opener (stderr, "middle_end", "r");
    if (!tree_fp) return 1;

    data.root = read_tree (tree_fp);
    tree_dump (&data, "check after middle tree");

    if (TranslateTree (data.root, "translate.asm"))
    {
        deleter(data.root);
        return 1;
    }

    deleter (data.root);

    return 0;
}