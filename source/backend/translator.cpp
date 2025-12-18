#include "token.h"
#include "tree.h"
#include "config.h"
#include "translator.h"
#include "lexems.h"
#include "str_funcs.h"

// =================================================== МАССИВ С ТИПАМИ =====================================================


static const char* types[] = { "op" , "var", "num", "func", "cnct" };

static int NUM_TYPES = sizeof(types) / sizeof(types[0]);


// =================================================== ЧТЕНИЕ ДЕРЕВА =======================================================


node_t* read_tree(FILE* stream)
{    
    char* buffer = NULL;
    file_read(stream, &buffer);
    char* buffer_copy = buffer;
    
    node_t* root = read_node(&buffer, NULL);

    free(buffer_copy);
    fclose(stream);

    return root;
}

node_t* read_node(char** cur_pose_ptr, node_t* parent)
{
    if (!cur_pose_ptr || !*cur_pose_ptr)
        return NULL;
    
    char* cur = *cur_pose_ptr;
    
    while (*cur == ' ' || *cur == '\t' || *cur == '\n') cur++;
    
    if (*cur == '(')
    {
        cur++;
        
        while (*cur == ' ' || *cur == '\t') cur++;
        
        char* type_start = cur;
        while (*cur != ':' && *cur != '\0') cur++;
        
        if (*cur == '\0')
        {
            D_PRINT("Error: unexpected end\n");
            return NULL;
        }
        
        char saved_char = *cur;
        *cur = '\0';
        int type = check_type(type_start);
        *cur = saved_char;
        
        if (type < 0)
        {
            D_PRINT("Error: unknown type '%s'\n", type_start);
            return NULL;
        }
        
        cur++;
        while (*cur == ' ' || *cur == '\t') cur++;
        
        if (*cur != '"')
        {
            D_PRINT("Error: expected '\"' after type\n");
            return NULL;
        }
        cur++;
        
        char* val_start = cur;
        while (*cur != '"' && *cur != '\0') cur++;
        
        if (*cur == '\0')
        {
            D_PRINT("Error: unexpected end of file in value\n");
            return NULL;
        }
        
        saved_char = *cur;
        *cur = '\0';

        tree_t val = {};

        switch (type)
        {
        case OP:
            val.op = SearchKeyWordStd (val_start);
            break;
        case VAR:
        case FUNCTION:
            val.var = strdup(val_start);
            break;
        
        case NUM:
            val.num = atoi(val_start);
            break;

        default:
            break;
        }

        *cur = saved_char;
        
        cur++;
        
        node_t* node = NodeCreator(stderr, type, val, NULL, NULL);
        if (!node)
            return NULL;
        
        node->parent = parent;
        
        *cur_pose_ptr = cur;
        
        node->left = read_node(cur_pose_ptr, node);
        node->right = read_node(cur_pose_ptr, node);
        
        cur = *cur_pose_ptr;
        
        while (*cur == ' ' || *cur == '\t') cur++;
        if (*cur == ')') cur++;

        else
        {
            D_PRINT("Error: expected ')' but found '%c'\n", *cur);
            free(node);
            return NULL;
        }
        
        *cur_pose_ptr = cur;
        return node;
    }

    else if (strncmp(cur, "nil", 3) == 0)
    {
        cur += 3;
        while (*cur == ' ' || *cur == '\t' || *cur == '\n')cur++;
        *cur_pose_ptr = cur;

        return NULL;
    }
    
    D_PRINT("Error: unrecognized format at '%c'\n", *cur);
    return NULL;
}


int check_type(const char* str)
{
    for (int i = 0; i < NUM_TYPES; i++)
    {
        if (strcmp(str, types[i]) == 0)
        {
            return i + 1;
        }
    }
    
    D_PRINT("Error: unrecognized type '%s'\n", str);
    return -1;
}


// ================================================ ТРАНСЛЯЦИЯ ДЕРЕВА ========================================================


LangErr_t TranslateTree (node_t* root, const char* filename)
{
    stack_scopes stack = {};
    stack_scopes_init (&stack, 100);

    FILE* asm_file = file_opener (stderr, filename, "w");
    if (!asm_file) return LN_ERR;

    if (TranslateNode(root, asm_file, &stack))
    {
        stack_scopes_destroy(&stack);
        fclose (asm_file);
        return LN_ERR;
    }

    fprintf(asm_file, "HLT\n");
    
    fclose(asm_file);
    stack_scopes_destroy(&stack);

    return LN_OK;
}

LangErr_t TranslateNode (node_t* node, FILE* asm_file, stack_scopes* stack)
{
    if (!node) return LN_OK;
    
    switch (node->type)
    {
        case OP:
            return TranslateOp (node, asm_file, stack);
            break;
            
        case VAR:
            return TranslateVar (node, asm_file, stack);
            break;
        
        case NUM:
            fprintf (asm_file, "PUSH %d\n", node->val.num);
            break;
            
        case FUNCTION:
            return TranslateCallFunc (node, asm_file, stack);
            break;
            
        case CONNECTION:
            TranslateNode (node->left,  asm_file, stack);
            TranslateNode (node->right, asm_file, stack);
            return LN_OK;

        default:
            return LN_ERR;
    }

    return LN_OK;
}

LangErr_t TranslateOp(node_t* node, FILE* asm_file, stack_scopes* stack)
{
    if (!node || !asm_file || !stack)
    {
        D_PRINT ("ERROR in func params\n");
        return LN_ERR;
    }

    switch (node->val.op)
    {
        case ADD:
        case SUB:
        case MUL:
        case DIV:
            return TranslateCalc (node, asm_file, stack);
    
        case INIT:
            return TranslateVarInit (node, asm_file, stack);

        case FUNC:
            return TranslateFunc (node, asm_file, stack);

        case EQ:
            return TranslateEq (node, asm_file, stack);

        case BIGR:
        case SMLR:
        case ESMLR:
        case EBIGR:
        case EQEQ:
        case NEQ:
            return TranslateCond (node, asm_file, stack);

        case IF: 
            return  TranslateIf (node, asm_file, stack);
        case WHILE:
            return  TranslateWhile (node, asm_file, stack);
        case RETURN:
            return  TranslateReturn (node, asm_file, stack);
            
        case SEMIC:
            if (node->left)
                TranslateNode (node->left, asm_file, stack);
            if (node->right)
                TranslateNode (node->right, asm_file, stack);
            return LN_OK;

        default:
            break;
    }

    return LN_ERR;
}

LangErr_t TranslateCalc (node_t* node, FILE* asm_file, stack_scopes* stack)
{
    if (!node) return LN_OK;

    TranslateNode (node->left,  asm_file, stack);
    TranslateNode (node->right, asm_file, stack);

    switch (node->val.op)
    {
    case ADD: fprintf(asm_file, "ADD\n");
        break;
    
    case SUB: fprintf(asm_file, "SUB\n");
        break;

    case MUL: fprintf(asm_file, "MULT\n");
        break;

    case DIV: fprintf(asm_file, "DIV\n");
        break;

    default:
        break;
    }

    return LN_OK;
}

LangErr_t TranslateCond (node_t* node, FILE* asm_file, stack_scopes* stack)
{
    if (!node || !asm_file || !stack)
    {
        D_PRINT ("ERROR in func params\n");
        return LN_ERR;
    }

    static int cond_count = 0;
    cond_count++;

    TranslateNode (node->left , asm_file, stack);
    TranslateNode (node->right, asm_file, stack);

    fprintf(asm_file, "PUSH 0\n");
    fprintf(asm_file, "POPREG %s\n", CMP_REG);

    switch (node->val.op)
    {
    case EQEQ:  fprintf (asm_file, "JNE");
        break;
    case NEQ:   fprintf (asm_file, "JE");
        break;
    case BIGR:  fprintf (asm_file, "JBE");
        break;
    case EBIGR: fprintf (asm_file, "JB");
        break;
    case SMLR:  fprintf (asm_file, "JA");
        break;
    case ESMLR: fprintf (asm_file, "JAE");
        break;    

    default:
        D_PRINT("ERROR: cond isnt exist\n");
        return LN_ERR;
        break;
    }

    fprintf (asm_file, " :false%d\n", cond_count);

    fprintf (asm_file, "PUSH 1\n");
    fprintf (asm_file, "POPREG %s\n", CMP_REG);

    fprintf (asm_file, ":false%d\n", cond_count);

    fprintf (asm_file, "PUSHREG %s\n", CMP_REG);

    return LN_OK;
}

LangErr_t TranslateIf (node_t* node, FILE* asm_file, stack_scopes* stack)
{
    if (!node || !asm_file || !stack)
    {
        D_PRINT ("ERROR in func params\n");
        return LN_ERR;
    }

    static int if_count = 0;
    if_count++;

    enter_scope (stack);

    TranslateCond (node->left, asm_file, stack);

    fprintf (asm_file, "PUSH 0\n");
    fprintf (asm_file, "JE :endif%d\n", if_count);

    TranslateNode (node->right, asm_file, stack);

    fprintf (asm_file, ":endif%d\n", if_count);

    exit_scope (stack);

    return LN_OK;
}

LangErr_t TranslateWhile (node_t* node, FILE* asm_file, stack_scopes* stack)
{
    static int while_count = 0;
    while_count++;
 
    enter_scope (stack);

    fprintf(asm_file, ":while_start%d\n", while_count);
    
    TranslateCond (node->left, asm_file, stack);

    fprintf (asm_file, "PUSH 0\n");
    fprintf (asm_file, "JE :endwhile%d\n", while_count);

    TranslateNode (node->right, asm_file, stack);
    
    fprintf(asm_file, "JUMP :while_start%d\n", while_count);

    fprintf (asm_file, ":endwhile%d\n", while_count);

    exit_scope (stack);

    return LN_OK;
}

LangErr_t TranslateReturn (node_t* node, FILE* asm_file, stack_scopes* stack)
{
    if (!node || !asm_file || !stack)
    {
        D_PRINT ("ERROR in func params\n");
        return LN_ERR;
    }

    TranslateNode (node->left, asm_file, stack);

    fprintf (asm_file, "RET\n");

    return LN_OK;
}

LangErr_t TranslateVarInit (node_t* node, FILE* asm_file, stack_scopes* stack)
{
    if (!node || !asm_file || !stack)
    {
        D_PRINT ("ERROR in func params\n");
        return LN_ERR;
    }

    node_t* param = node->left->left;
    int adres = init_var (stack, param->val.var);
    if (adres < 0)
    {
        D_PRINT ("ERROR, in adress\n");
        return LN_ERR;
    }

    fprintf (asm_file, "\n;Инцилизация переменной\n");
    fprintf (asm_file, "PUSH %d\n", adres);
    fprintf (asm_file, "POPREG %s\n", ADR_REG );

    TranslateNode (node->left->right, asm_file, stack);
    fprintf (asm_file, "POPM [%s]\n\n", ADR_REG );

    return LN_OK;
}

LangErr_t HelpFuncParamInit (stack_scopes* stack, char* name, FILE* asm_file)
{
    int adres = init_var (stack, name);

    fprintf (asm_file, "PUSH %d\n", adres);
    fprintf (asm_file, "POPREG %s\n", ADR_REG);

    fprintf (asm_file, "POPM [%s]\n", ADR_REG);

    return LN_OK;
}

LangErr_t TranslateFunc (node_t* node, FILE* asm_file, stack_scopes* stack)
{
    if (!node || !asm_file || !stack)
    {
        D_PRINT ("ERROR in func params\n");
        return LN_ERR;
    }

    node_t* name = node->left;
    fprintf (asm_file, ":%s\n", name->val.var);
    
    enter_scope (stack);
    
    node_t* param = node->right->left;

    while (param != NULL)
    {
        HelpFuncParamInit (stack, param->val.var, asm_file);
        param = param->left;
    }

    node_t* body = node->right->right;
    TranslateNode (body, asm_file, stack);
    
    exit_scope (stack);

    return LN_OK;
}

LangErr_t TranslateVar(node_t* node, FILE* asm_file, stack_scopes* stack)
{
    if (!node || !asm_file || !stack)
    {
        D_PRINT ("ERROR in func params\n");
        return LN_ERR;
    }

    int adres = get_var_address (stack, node->val.var);

    fprintf (asm_file, "PUSH %d\n", adres);
    fprintf (asm_file, "POPREG %s\n", ADR_REG);

    fprintf (asm_file, "PUSHM [%s]\n", ADR_REG);

    return LN_OK;
}

LangErr_t TranslateEq (node_t* node, FILE* asm_file, stack_scopes* stack)
{
    if (!node || !asm_file || !stack)
    {
        D_PRINT ("ERROR in func params\n");
        return LN_ERR;
    }

    int adres = get_var_address (stack, node->left->val.var);

    TranslateNode (node->right, asm_file, stack);

    fprintf (asm_file, "PUSH %d\n", adres);
    fprintf (asm_file, "POPREG %s\n", ADR_REG);

    fprintf (asm_file, "POPM [%s]\n", ADR_REG);
    
    return LN_OK;
}

LangErr_t TranslateCallFunc (node_t* node, FILE* asm_file, stack_scopes* stack)
{
    if (!node || !asm_file || !stack)
    {
        D_PRINT ("ERROR in func params\n");
        return LN_ERR;
    }

    node_t* params = node->left;

    while (params != NULL)
    {
        if (!params->right) TranslateNode (params, asm_file, stack);
        else TranslateNode (params->right, asm_file, stack);

        params = params->left;
    }

    fprintf (asm_file, "CALL :%s\n", node->val.var);

    return LN_OK;
}