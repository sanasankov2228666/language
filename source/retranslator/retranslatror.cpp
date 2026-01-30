#include "tree.h"
#include "lexems.h"
#include "treereader.h"
#include "config.h"

LangErr_t Retranslator (node_t* root,FILE* mid_file, char* out_name)
{
    FILE* output_fp = file_opener (stderr, out_name, "w");
    if (!output_fp) return LN_ERR;

    if (RestranslateNode (root, output_fp))
        return LN_ERR;
}

LangErr_t RestranslateNode (node_t* node, FILE* output_fp)
{
    if (!node || !output_fp)
    {
        D_PRINT ("ERROR in func parametrs");
        return LN_ERR;
    }

    LangErr_t err = LN_OK;

    if (node->left)
    {
        err = RestranslateNode (node->left, output_fp);
        if (err) return err;          
    }

    if (node->right)
    {
        err = RestranslateNode (node->right, output_fp);
        if (err) return err; 
    }

    return LN_OK;
}