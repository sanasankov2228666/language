#include "optimizations.h"
#include "token.h"
#include "tree.h"
#include "config.h"
#include "lexems.h"
#include "str_funcs.h"
#include "math.h"

double CalcTree(node_t* root)
{
    if (root == NULL) return NAN;

    if (root->type == NUM) 
        return root->val.num;

    if (root->type   == OP  && 
       (root->val.op == ADD || 
        root->val.op == SUB || 
        root->val.op == MUL || 
        root->val.op == DIV))
    {
        double left_val = CalcTree(root->left);
        double right_val = CalcTree(root->right);

        switch (root->val.op)
        {
            case ADD:   return left_val + right_val;
            case SUB:   return left_val - right_val;
            case MUL:   return left_val * right_val;
            case DIV:   return (fabs(right_val) < TOLOW) ? NAN : left_val / right_val;;

            default:    return NAN;
        }
    }
    
    return NAN;
}

node_t* simplifier(node_t* root)
{
    if (root == NULL) return NULL;

    root->left  = simplifier(root->left);
    root->right = simplifier(root->right);

    if (root->type == OP) 
    {    
        if (root->left && root->right &&
            root->left->type  == NUM  && 
            root->right->type == NUM) 
        {
            double result = CalcTree (root);
            if (!isnan(result)) 
            {
                node_t* new_node = CreateNum ((int) result);
                
                deleter(root);
                return new_node;
            }
        }

        node_t* simplified = simplifySpecCases(root);
        
        if (simplified != root) 
        {
            deleter(root);
            return simplified;
        }
    }
    
    return root;
}

node_t* simplifySpecCases(node_t* node)
{
    if (!node || !node->left || !node->right) return node;

    if (node->type == ADD || node->type == SUB)
    {
        if (node->left->type == NUM && fabs(node->left->val.num) < TOLOW)
        {
            return CopySubtree (node->right);
        }

        else if (node->right->type == NUM && fabs(node->right->val.num) < TOLOW)
        {
            return CopySubtree (node->left);
        }
    }

    else if (node->type == MUL)
    {
        if ( (node->left->type  == NUM && fabs(node->left->val.num)  < TOLOW)  || 
             (node->right->type == NUM && fabs(node->right->val.num) < TOLOW)  )
        {
            return CreateNum(0);
        }

        else if ( node->left->type  == NUM && fabs(node->left->val.num - 1) < TOLOW )
        {
            return CopySubtree (node->right);
        }

        else if ( node->right->type == NUM && fabs(node->right->val.num - 1) < TOLOW )
        {
            return CopySubtree (node->left);
        }
    }

    else if (node->type == DIV)
    {
        if ( node->left->type  == NUM && fabs(node->left->val.num - 1) < TOLOW )
        {
            return CreateNum(0);
        }

        else if ( node->right->type == NUM && fabs(node->right->val.num - 1) < TOLOW )
        {
            return CopySubtree (node->left);
        }
    }

    return node;
}
