#include "tree.h"
#include "lexems.h"
#include "treereader.h"
#include "config.h"
#include "retranslator.h"

static int scope = 0;

// =================================================== РЕТРАНСЛЯЦИЯ ДЕРЕВА ==========================================================

//! Основная функция ретрасляции
LangErr_t Retranslator (node_t* root, char* out_name)
{
    FILE* output_fp = file_opener (stderr, out_name, "w");
    if (!output_fp) return LN_ERR;

    if (RetranslateNode (root, output_fp))
        return LN_ERR;

    fclose (output_fp);
    
    return LN_OK;
}

//! Функция для ретрансляции узла
LangErr_t RetranslateNode (node_t* node, FILE* output_fp)
{
    if (!node || !output_fp)
    {
        D_PRINT ("ERROR in func parametrs\n");
        return LN_ERR;
    }

    switch (node->type)
    {
    case OP:
        return RetranslateOp (node, output_fp);
        
    case VAR:
        return RetranslateVar (node, output_fp);
    
    case NUM:
        return RetranslateNum (node, output_fp);

    case FUNCTION:
        return RetranslateCallFunc (node, output_fp);
    
    case CONNECTION:
        if (node->left)
            RetranslateNode (node->left, output_fp);
        if (node->right)
            RetranslateNode (node->right, output_fp);
        return LN_OK;

    default:
        return LN_ERR;
    }

    return LN_OK;
}

//! Функция для ретрансляции операторов
LangErr_t RetranslateOp (node_t* node, FILE* output_fp)
{
    if (!node || !output_fp)
    {
        D_PRINT ("ERROR in func parametrs");
        return LN_ERR;
    }

    switch (node->val.op)
    {
        case ADD:
        case MUL:
        case DIV:
            return RetranslateCalc (node, output_fp);

        case SUB:
            if (node->right)
                return RetranslateCalc (node, output_fp);
            else
                return RetranslateUn (node, output_fp);

        case INIT:
            return RetranslateVarInit (node, output_fp);

        case FUNC:
            return RetranslateFunc (node, output_fp);

        case EQ:
            return RetranslateEq (node, output_fp);

        case SQRT:
            return RetranslateUn (node, output_fp);

        case BIGR:
        case SMLR:
        case ESMLR:
        case EBIGR:
        case EQEQ:
        case NEQ:
            return RetranslateCond (node, output_fp);

        case IF: 
            return  RetranslateIf (node, output_fp);
        case WHILE:
            return  RetranslateWhile (node, output_fp);
        case RETURN:
            return  RetranslateReturn (node, output_fp);
        case PRINT:
            return RetranslatePrint (node, output_fp);
        case IN:
            return RetranslateIn (node, output_fp);

        case COMMA:
            if (node->left)
                RetranslateNode (node->left, output_fp);

            MID_PRNT(" and ");

            if (node->right)
                RetranslateNode (node->right, output_fp);

            return LN_OK;
            break;

        case SEMIC:
            return RetranslateSemic (node, output_fp);

        default: 
            return LN_ERR;
    }

    return LN_ERR;
}

//! Функция для ретрансляции переменных
LangErr_t RetranslateVar (node_t* node, FILE* output_fp)
{
    if (!node || !output_fp)
    {
        D_PRINT ("ERROR in func parametrs");
        return LN_ERR;
    }

    MID_PRNT(" %s ", node->val.var);

    return LN_OK;
}

//! Функция для ретрансляции чисел
LangErr_t RetranslateNum (node_t* node, FILE* output_fp)
{
    if (!node || !output_fp)
    {
        D_PRINT ("ERROR in func parametrs");
        return LN_ERR;
    }
    
    MID_PRNT(" %d ", node->val.num);
 
    return LN_OK;
}

//! Функция для ретрансляции вызова функции
LangErr_t RetranslateCallFunc (node_t* node, FILE* output_fp)
{
    if (!node || !output_fp)
    {
        D_PRINT ("ERROR in func parametrs");
        return LN_ERR;
    }

    MID_PRNT(" %s with ", node->val.var);

    if (RetranslateNode (node->left, output_fp))
        return LN_ERR;

    MID_PRNT(" components ");
    
    return LN_OK;
}

//! Функция для ретрансляции вычислительных операторов
LangErr_t RetranslateCalc (node_t* node, FILE* output_fp)
{
    if (!node || !output_fp)
    {
        D_PRINT ("ERROR in func parametrs");
        return LN_ERR;
    }
    
    MID_PRNT(" with ");

    if (node->left)
        RetranslateNode (node->left, output_fp);

    switch (node->val.op)
    {
        case ADD: MID_PRNT(" stack ");
            break;

        case SUB: MID_PRNT(" unstack ");
            break;

        case MUL: MID_PRNT(" entchar ");
            break;

        case DIV: MID_PRNT(" split ");
            break;    
        
        default: return LN_ERR;
    }

    if (node->left)
        RetranslateNode (node->right, output_fp);

    MID_PRNT(" components ");

    return LN_OK;
}

//! Функция для ретрансляции унарных операторов
LangErr_t RetranslateUn (node_t* node, FILE* output_fp)
{
    if (!node || !output_fp)
    {
        D_PRINT ("ERROR in func parametrs");
        return LN_ERR;
    }
    
    switch (node->val.op)
    {
        case SUB:
            MID_PRNT(" with unstack ");
            RetranslateNode (node->left, output_fp);
            MID_PRNT(" components ");
            break;

        case SQRT:
            MID_PRNT(" with mineroot ");
            RetranslateNode (node->left, output_fp);
            MID_PRNT(" components ");
            break;

        default: return LN_ERR;
    }

    return LN_OK;
}

//! Функция для ретрансляции инцилизации переменных
LangErr_t RetranslateVarInit (node_t* node, FILE* output_fp)
{
    if (!node || !output_fp || !node->left || !node->left->left)
    {
        D_PRINT ("ERROR in func parametrs");
        return LN_ERR;
    }

    char* name = node->left->left->val.var;

    MID_PRNT("spawn %s is ", name);

    if (RetranslateNode (node->left->right, output_fp))
        return LN_ERR;

    //MID_PRNT(" bedrock\n");

    return LN_OK;
}

//! Функция для ретрансляции объявления функции
LangErr_t RetranslateFunc (node_t* node, FILE* output_fp)
{
    if (!node || !output_fp || !node->left)
    {
        D_PRINT ("ERROR in func parametrs");
        return LN_ERR;
    }

    char* name = node->left->val.var;

    MID_PRNT("craft %s with ", name);

    node_t* param =  node->right->left;

    for (; param != NULL; param = param->left)
    {
        if (param->left)
            MID_PRNT(" %s and ", param->val.var );
        else 
            MID_PRNT(" %s ", param->val.var );
    }

    MID_PRNT("components\n");
    MID_PRNT("workbench\n\t");
    
    scope++;

    if (RetranslateNode (node->right->right, output_fp))
        return LN_ERR;

    scope--;
    MID_PRNT("close\n");

    return LN_OK;
}

//! Функция для ретрансляции оператора присваивания
LangErr_t RetranslateEq (node_t* node, FILE* output_fp)
{
    if (!node || !output_fp)
    {
        D_PRINT ("ERROR in func parametrs");
        return LN_ERR;
    }

    char* name = node->left->val.var;

    MID_PRNT("%s is ", name);

    if (RetranslateNode (node->right, output_fp))
        return LN_ERR;

    //MID_PRNT("bedrock\n");

    return LN_OK;
}

//! Функция для ретрансляции if оператора
LangErr_t RetranslateIf (node_t* node, FILE* output_fp)
{
    if (!node || !output_fp || !node->left || !node->right)
    {
        D_PRINT ("ERROR in func parametrs");
        return LN_ERR;
    }
    
    scope++;
    MID_PRNT("observer with ");

    if (RetranslateCond (node->left, output_fp))
        return LN_ERR;

    MID_PRNT(" components\n");
    MID_PRNT("workbench\n");

    if (node->right->val.op == ELSE)
    {
        if (node->right->left)
            RetranslateNode (node->right->left, output_fp);
        MID_PRNT("close\n");
        
        MID_PRNT("else\nworkbench\n");
        if (node->right->right)
            RetranslateNode (node->right->right, output_fp);
        MID_PRNT("close\n");
    }


    else
    {
        RetranslateNode (node->right, output_fp);
        MID_PRNT("close\n");
    }

    scope--;
    return LN_OK;
}

//! Функция для ретрансляции условий
LangErr_t RetranslateCond (node_t* node, FILE* output_fp)
{
    if (!node || !output_fp)
    {
        D_PRINT ("ERROR in func parametrs");
        return LN_ERR;
    }
    
    if (node->left)
        RetranslateNode (node->left, output_fp);

    switch (node->val.op)
    {
        case BIGR: MID_PRNT(" above ");
            break;

        case SMLR: MID_PRNT(" less ");
            break;
        
        case EBIGR: MID_PRNT(" at_most ");
            break;

        case ESMLR: MID_PRNT(" at_least ");
            break;

        case EQEQ: MID_PRNT(" same ");
            break;

        case NEQ: MID_PRNT(" different ");
            break;

        default: return LN_ERR;
    }

    if (node->right)
        RetranslateNode (node->right, output_fp);

    return LN_OK;
}

//! Функция для ретрансляции while оператора
LangErr_t RetranslateWhile (node_t* node, FILE* output_fp)
{
    if (!node || !output_fp)
    {
        D_PRINT ("ERROR in func parametrs");
        return LN_ERR;
    }

    scope++;
    MID_PRNT("mine with ");

    if (RetranslateCond (node->left, output_fp))
        return LN_ERR;

    MID_PRNT(" components\n");
    MID_PRNT("workbench\n\t");

    if (node->right)
        RetranslateNode (node->right, output_fp);
        
    MID_PRNT("close\n");

    scope--;
    return LN_OK;
}

//! Функция для ретрансляции возрата функции
LangErr_t RetranslateReturn (node_t* node, FILE* output_fp)
{
    if (!node || !output_fp)
    {
        D_PRINT ("ERROR in func parametrs");
        return LN_ERR;
    }

    MID_PRNT("result ");

    if (RetranslateNode (node->left, output_fp))
        return LN_ERR;

    //MID_PRNT(" bedrock\n");

    return LN_OK;
}

//! Функция для ретрансляции оператора печати
LangErr_t RetranslatePrint (node_t* node, FILE* output_fp)
{
    if (!node || !output_fp)
    {
        D_PRINT ("ERROR in func parametrs");
        return LN_ERR;
    }

    MID_PRNT("print ");

    if (RetranslateNode (node->left, output_fp))
        return LN_ERR;

    //MID_PRNT(" bedrock\n");

    return LN_OK;
}

//! Функция для ретрансляции оператора ввода
LangErr_t RetranslateIn (node_t* node, FILE* output_fp)
{
    if (!node || !output_fp)
    {
        D_PRINT ("ERROR in func parametrs");
        return LN_ERR;
    }
    
    MID_PRNT(" in ");

    return LN_OK;
}

//! Функция для ретрансляции ;
LangErr_t RetranslateSemic (node_t* node, FILE* output_fp)
{
    if (!node || !output_fp)
    {
        D_PRINT ("ERROR in func parametrs");
        return LN_ERR;
    }
    
    if (node->left)
        RetranslateNode (node->left, output_fp);

    fprintf (output_fp, " bedrock\n");

    // for (int i = 0; i < scope; i++)
    //     fprintf (output_fp, "\t");

    if (node->right)
        RetranslateNode (node->right, output_fp);

    return LN_OK;
}