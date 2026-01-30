#include <cstdint>
#include <sys/stat.h>
#include <assert.h>
#include <ctype.h>
#include "tree.h"
#include "token.h"
#include "lexems.h"
#include "config.h"
#include "str_funcs.h"


//==============================================
//         ТОКЕНИЗАТОР ОСНОВНАЯ ФУНКЦИЯ 
//==============================================


data_lexer Tokenization (FILE* code)
{
    data_lexer data = {};
    DataLexerInit(&data, code);

    node_t* token = {};
    while (1)
    {
        token = ProcessToken (&data);
        if (!token)
        {
            D_PRINT ("ERROR, token == NULL\n");
            data.error = 1;
            free(data.source);
            return data;
        }

        if (token->type == ERR_TOKEN)
        {
            D_PRINT ("ERROR processing token at line %zu, col %zu\n\n", data.line, data.column);  
                                                          
            TokenArrayFree(&data.tokens);
            free(data.source);
            free(token);

            data.error = 1;
            data.source = NULL;
            return data;
        }
        
        PutToken (&data.tokens, token);
        ConnectionAdd (&data, token);

        if (token->type == END_TOKEN) break;
    }

    free(data.source);
    return data;
}


//==============================================
//               ОБРАБОТКА ТОКЕНА
//==============================================

node_t* ProcessToken (data_lexer* data)
{
    if (!data) return ErrToken();

    SkipSpace(data);

    while (*(data->current_pos) == '/' && *(data->current_pos + 1) == '/')
    {
        while (*(data->current_pos) != '\n' && *(data->current_pos) != '\0')
        {
            data->current_pos++;
            data->column++;
        }
        SkipSpace(data);
    }

    if (*(data->current_pos) == '\0' || *(data->current_pos) == EOF)
    return EndToken();
    
    node_t* token = {};

    if ( isdigit (*(data->current_pos)) )
    {
        token = ProcessingNum (data);
        return token;
    }

    else if ( isalpha( *(data->current_pos) ) || *(data->current_pos) == '_' )
    {
        char* start = data->current_pos;
        MovePose (data);

        bool check_is_func = CheckFunc (data);
        
        char old_sym = *(data->current_pos);
        *(data->current_pos) = '\0';
        
        int key = SearchKeyWord (start);
        
        if (key != UNREC_KEY)
        {
            token = ProcessingKeyWord (data, key);
            *(data->current_pos) = old_sym;
            
            return token;
        }

        else if (key == UNREC_KEY)
        {
            token = ProcessingIdentificator (data, start, check_is_func);
            *(data->current_pos) = old_sym;

            return token;
        }
    }

    D_PRINT("Unknown character: '%c' at line %zu, col %zu\n", 
        *(data->current_pos),  data->line, data->column);

    return ErrToken();
}


// Обработка скобок и стоп символов
node_t* ProcessingBrackets (data_lexer* data)
{
    node_t* token = (node_t*) calloc (1, sizeof(node_t));
    token->type = OP;

    token->line   = data->line;
    token->column = data->column;

    char old_sym = *(data->current_pos + 1);
    *(data->current_pos + 1) = '\0';

    int index = SearchKeyWord (data->current_pos);
    token->val.op = key_words[index].numb;

    *(data->current_pos + 1) = old_sym;
    data->current_pos++;

    return token;
}


// Обработка числа
node_t* ProcessingNum (data_lexer* data)
{
    node_t* token = (node_t*) calloc (1, sizeof(node_t));
    token->type = NUM;
    
    token->line = data->line;
    token->column = data->column;

    char* start = data->current_pos;
    char* end = NULL;

    token->val.num = (int) strtod(start, &end);
    
    size_t len = size_t (end - start);
    data->current_pos = end;
    data->column = data->column + len;

    return token;
}

// Обработка оператора
node_t* ProcessingOperator(data_lexer* data)
{
    node_t* token = (node_t*) calloc (1, sizeof(node_t));
    token->type = OP;
    
    token->line = data->line;
    token->column = data->column;

    char* start = data->current_pos;
    
    while (!isalpha(*(data->current_pos)) && 
           !isdigit(*(data->current_pos)) && 
              *(data->current_pos) != ' ' && 
              *(data->current_pos) != 'w'  )
    {
        data->current_pos++;
        data->column++;        
    }

    char old_sym = *(data->current_pos);
    *(data->current_pos) = '\0';

    int index = SearchKeyWord (start);
    if (index == UNREC_KEY)
    {
        D_PRINT("ERROR, unrecognaize operator line: %zu col: %zu\n\n",
            data->line, data->column);

        return NULL;
    }

    token->val.op = key_words[index].numb;

    *(data->current_pos) = old_sym;

    return token;
}


// Обработка ключевых слов
node_t* ProcessingKeyWord (data_lexer* data, int key)
{
    node_t* token = (node_t*) calloc (1, sizeof(node_t));
    if (!token) return ErrToken();

    token->type   = OP;

    token->line = data->line;
    token->column = data->column;

    token->val.op = key_words[key].numb;

    return token;
}


// Обработка идентификатора
node_t* ProcessingIdentificator (data_lexer* data, char* start,  bool check_is_func)
{
    node_t* token = (node_t*) calloc (1, sizeof(node_t));
    if (!token) return ErrToken ();

    if (check_is_func)
        token->type = FUNCTION;

    else token->type = VAR;

    token->line   = data->line;
    token->column = data->column;

    token->val.var = strdup(start);

    return token;
}

// Создание связующего узла
LangErr_t CreateConnection (data_lexer* data)
{
    node_t* connect = (node_t*) calloc (1, sizeof(node_t));
    if (!connect) return LN_ERR;

    connect->type = CONNECTION;

    connect->line   = data->line;
    connect->column = data->column;

    PutToken (&data->tokens, connect);

    return LN_OK;
}


// Добавление связующего узла
LangErr_t ConnectionAdd (data_lexer* data, node_t* token)
{
    if (token->type == OP &&  (token->val.op == IF || token->val.op == WHILE) )
    {
        return (CreateConnection (data));
    } 

    else if (token->type == OP &&  (token->val.op == FUNC) )
    {
        return CreateConnection (data) || CreateConnection (data);
    }

    return LN_OK;
}

// =================================================== ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ =========================================================


// Создание структуры лексера
LangErr_t DataLexerInit (data_lexer* data, FILE* fp)
{
    data->source_len = file_read(fp, &data->source);
    TokenArrayInit(&data->tokens, 100);

    data->current_pos = data->source;

    return LN_OK;
}


// Сдвиг позиции после слова
char* MovePose (data_lexer* data)
{
    char* pose = data->current_pos;

    while (isalnum(*pose) || *pose == '_')
    {
        data->column++;
        pose++;
    }

    data->current_pos = pose;
    return pose;
}


// Пропуск пробелов
void SkipSpace (data_lexer* data)
{
    while ( isspace( *(data->current_pos)) && *(data->current_pos) != '\0' )
    {
        if (*(data->current_pos) == '\n')
        {
            data->line++;
            data->column = 0;
        }

        else if (*(data->current_pos) == '\t')
        {
            data->column++;
        }

        else if (*(data->current_pos) == ' ')
        {
            data->column++;
        }

        (data->current_pos)++;
    }
}

// Токен конца
node_t* EndToken ()
{
    node_t* end_tkn = (node_t*) calloc (1, sizeof(node_t));
    end_tkn->type = END_TOKEN;

    return end_tkn;
}

// Токен ошибки
node_t* ErrToken ()
{
    node_t* err_tkn = (node_t*) calloc (1, sizeof(node_t)) ;
    err_tkn->type = ERR_TOKEN;

    return err_tkn;
}


// Проверка чем является  индентификатор
bool CheckFunc(data_lexer* data)
{
    char* pos = data->current_pos;
    
    while (*pos == ' ' || *pos == '\t') pos++;

    if (strncmp( pos, "with", 4) == 0) return true;

    return false;
}


//================================================== ФУНКЦИИ БУФЕРА ТОКЕНОВ =============================================================


// Создание массива с лексмами
LangErr_t TokenArrayInit (tokens_arr* data, size_t capacity)
{
    assert(data);

    data->arr = (node_t**) calloc (capacity, sizeof(node_t*));
    if ( data->arr == NULL)
    {
        D_PRINT("DATA->ARR == NULL");

        return LN_ERR;
    }

    data->capacity = capacity;
    data->size = 0;
    
    return LN_OK;
}


// Получить из массива
node_t* GetToken (tokens_arr* data, size_t place)
{
    assert(data);

    if (place >= data->size)
    {
        D_PRINT("location %zu dont exist, capacity %zu\n", place, data->capacity);
        return NULL;
    }

    return data->arr[place];
}

// Полодить в массив
LangErr_t PutToken (tokens_arr* data, node_t* val)
{
    assert (data);

    if (data->size >= data->capacity)
    {
        data->arr = (node_t**) realloc(data->arr, data->capacity * 2 * sizeof(node_t*));
        if ( data->arr == NULL)
        {
            D_PRINT("DATA->ARR == NULL");
            return LN_ERR;
        }

        data->capacity = data->capacity * 2;
    }

    data->arr[data->size++] = val;

    return LN_OK;
}

// Получение зантых ячеек
size_t TokenArraySize(const tokens_arr* data)
{
    return data->size;
}

// Получение размера ячеек
size_t TokenArrayCapacity(const tokens_arr* data)
{
    return data->capacity;
}

// Удаление массива
void TokenArrayFree(tokens_arr* data)
{
    if (data && data->arr)
    {
        for (size_t i = 0; i < data->size; i++)
        {
            if (data->arr[i])
            {
                if ((data->arr[i]->type == VAR || data->arr[i]->type == FUNCTION ) &&
                    data->arr[i]->val.var)
                {
                    free(data->arr[i]->val.var);
                }
                free(data->arr[i]);
            }
        }
        free(data->arr);
        data->arr = NULL;
        data->capacity = 0;
        data->size = 0;
    }
}



//===============================================
//               ФУНКЦИЯ ОТЛАДКИ
//===============================================


// Печать массива токенов
void DebugTokens(const tokens_arr* tokens)
{
    if (!tokens || tokens->size == 0)
    {
        printf("No tokens to display\n");
        return;
    }

    printf("\n═══════════════════════════════════\n");
    printf("ТОКЕНЫ (%zu шт):\n", tokens->size);
    printf("═══════════════════════════════════\n");
    
    for (size_t i = 0; i < tokens->size; i++)
    {
        node_t* t = tokens->arr[i];
        
        printf("[%2zu] %2zu:%-2zu ", i, t->line, t->column);
        
        switch (t->type)
        {
            case OP:
                    printf("OP   '%s'\n", key_words[t->val.op - 1].name);
                break;
                
            case NUM:
                printf("NUM  '%d'\n", t->val.num);
                break;
                
            case VAR:
                printf("VAR  '%s'\n", t->val.var);
                break;

            case FUNCTION:
                printf("FUNC '%s'\n", t->val.var);
                break;

            case CONNECTION:
                printf("LINK 'link'\n");
                break;
                
            case END_TOKEN:
                printf("END_TOKEN\n");
                break;
                
            case ERR_TOKEN:
                printf("ERROR_TOKEN\n");
                break;
                
            default:
                printf("UNKNOWN type:%d\n", t->type);
                break;
        }
    }
    printf("═══════════════════════════════════\n\n");
}