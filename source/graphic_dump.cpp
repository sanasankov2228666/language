#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "graphic_dump.h"
#include "lexems.h"

void dot_node_gen(node_t* root, FILE* dot_file, size_t* number)
{
    if (root == NULL) return;

    const char* fill_color = (root->right == NULL && root->left == NULL) 
                           ? "lightgreen" 
                           : "wheat";

    const char* node_data = "UNKNOWN";
    char data_buffer[256] = {0};
    
    switch (root->type)
    {
        case OP:
        {
            int op_index = root->val.op - 1;
            if (op_index >= 0 && op_index < (int) LEX_NUM)
            {
                node_data = key_words[op_index].name;
            }

            else
            {
                snprintf(data_buffer, sizeof(data_buffer), "OP_%d", root->val.op);
                node_data = data_buffer;
            }
            break;
        }
        
        case VAR:
        case FUNCTION:
            if (root->val.var != NULL)
            {
                node_data = root->val.var;
            }
            else
            {
                node_data = "NULL";
            }
            break;
        
        case CONNECTION:
            
            

        case NUM:
            snprintf(data_buffer, sizeof(data_buffer), "%d", root->val.num);
            node_data = data_buffer;
            break;
            
        default:
            snprintf(data_buffer, sizeof(data_buffer), "type_%d", root->type);
            node_data = data_buffer;
            break;
    }
    
    const char* type_str = "";
    switch (root->type)
    {
        case OP:         type_str = "OP";       break;
        case VAR:        type_str = "VAR";      break;
        case FUNCTION:   type_str = "FUNC";     break;
        case CONNECTION: type_str = "CONNECT";  break;
        case NUM:        type_str = "NUM";      break;
        default:         type_str = "UNKNOWN";  break;
    }
    
    fprintf(dot_file, 
        "\tnode%zu [label = \"{parent = %p | type = %s | data = %s | {<left> left = %p|<right> right = %p} }\","
        " fillcolor = \"%s\", style = \"filled\"];\n",
        *number, root->parent, type_str, node_data, root->left, root->right, fill_color);

    size_t current_number = (*number)++;
    current_number = current_number;
    
    if (root->left != NULL)
    {
        dot_node_gen(root->left, dot_file, number);
    }
    
    if (root->right != NULL)
    {
        dot_node_gen(root->right, dot_file, number);
    }
}

void dot_arrw_gen(node_t* root, FILE* dot_file, size_t* number, size_t parent_number)
{
    if (root == NULL) return;

    size_t current_number = (*number)++;

    parent_number = parent_number;

    if (root->left != NULL)
    {
        fprintf(dot_file, "\tnode%zu:<left> -> node%zu;\n", 
                current_number, *number);
        dot_arrw_gen(root->left, dot_file, number, current_number);
    }
    
    if (root->right != NULL)
    {
        fprintf(dot_file, "\tnode%zu:<right> -> node%zu;\n", 
                current_number, *number);
        dot_arrw_gen(root->right, dot_file, number, current_number);
    }
}

LangErr_t dump_generator(tree* data, const char* filename, const char* reason)
{
    if (!data || !filename || !reason)
    {
        printf("Invalid arguments to dump_generator\n");
        return LN_ERR;
    }
    
    // Создаем директории если их нет
    system("mkdir -p dot png 2>/dev/null");
    
    char dot_filename[256] = {};
    snprintf(dot_filename, sizeof(dot_filename), "dot/%s.dot", filename);
    
    FILE* dot_file = fopen(dot_filename, "w");
    if (!dot_file)
    {
        printf("Error creating DOT file: %s\n", dot_filename);
        return LN_ERR;
    }
    
    fprintf(dot_file, "digraph Tree\n{\n");
    fprintf(dot_file, "\tnodesep=0.8;\n");
    fprintf(dot_file, "\tranksep=1;\n");
    fprintf(dot_file, "\tnode [shape=record, style=\"rounded,filled\", fillcolor=lightblue];\n");
    fprintf(dot_file, "\tedge [arrowsize=0.8];\n");
    fprintf(dot_file, "\tsplines=ortho;\n");
    fprintf(dot_file, "\tfontsize=12;\n");
    fprintf(dot_file, "\tlabel=\"%s\";\n", reason);
    fprintf(dot_file, "\tlabelloc=t;\n\n");

    size_t node_number = 0;
    if (data->root != NULL)
    {
        dot_node_gen(data->root, dot_file, &node_number);
    }

    size_t arrow_number = 0;
    if (data->root != NULL)
    {
        dot_arrw_gen(data->root, dot_file, &arrow_number, 0);
    }

    fprintf(dot_file, "\n}");
    fclose(dot_file);
    
    char command[512];
    snprintf(command, sizeof(command), 
             "dot -Tpng dot/%s.dot -o png/%s.png", 
             filename, filename);
    
    int result = system(command);
    if (result != 0)
    {
        printf("Warning: Graphviz (dot) not found or error generating PNG\n");
        printf("DOT file created: %s\n", dot_filename);
        return LN_ERR;
    }
    
    printf("generated: png/%s.png\n", filename);

    html_generator(filename, data);

    return LN_OK;
}

LangErr_t html_generator(const char* filename, tree* data)
{
    if (!data || !data->html_out)
    {
        printf("html_generator: invalid data\n");
        return LN_ERR;
    }
    
    fprintf(data->html_out, "<figure>\n");
    fprintf(data->html_out, "\t<img src=\"png/%s.png\"/>\n", filename);
    fprintf(data->html_out, "\t<figcaption>%s</figcaption>\n", filename);
    fprintf(data->html_out, "</figure>\n");
    fprintf(data->html_out, "<br>\n");
    
    return LN_OK;
}

void tree_dump(tree* data, const char* reason)
{
    if (!data)
    {
        printf("tree_dump: data is NULL\n");
        return;
    }
    
    static int dump_count = 0;
    char filename[64] = {};
    snprintf(filename, sizeof(filename), "tree_dump_%d", dump_count++);
    
    if (dump_generator(data, filename, reason) != LN_OK)
    {
        printf("PNG generation failed for dump %s\n", filename);
    }
}