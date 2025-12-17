#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "assembler.h"
#include "str_func.h"
#include "fileopen.h"

typedef int type;

int main( int argc, char* argv[] )
{
    if (argc < 2)
    {
        printf("ERROR the code file was not specified\n");
        return ERROR;
    }
    
    struct main_str assembler = {};
    
    //!открываем файл с командами
    FILE* stream = file_opener( argv[1], "rb", __FUNCTION__, __FILE__, __LINE__);
    if (stream == NULL) return 1;
    
    //!читаем весь текст в один буфер
    char* buffer_commands = NULL;
    file_read(stream, &buffer_commands);
    //puts(buffer_commands);
    fclose(stream);
    
    //!создаем массив указателей на буфер с текстом
    assembler.len = 2 * n_check(buffer_commands) + 1;
    assembler.mas_str = make_mass(buffer_commands, assembler.len);
    changer(&buffer_commands);
    if (assembler.mas_str == NULL) return ERROR;
    
    //!создаю массив с байт кодом
    assembler.buffer_out = (int*) calloc (assembler.len, sizeof(int));
    
    //!компиляция
    hash_sort();
    size_t check_ret = byte_code_maker(&assembler, 1);
    BYTE_CODE_ERROR
    check_ret = byte_code_maker(&assembler, 2);
    BYTE_CODE_ERROR

    assembler.len = check_ret;

    //!создаём сигнатуру
    signature_maker(assembler.buffer_out);

    //!открываем файл с байткодом
    assembler.stream_out = file_opener("SPU/translate_cmd.asm", "wb", __FUNCTION__, __FILE__, __LINE__);
    if (stream == NULL)
    {
        asm_deleter(&assembler, buffer_commands);
        return ERROR;
    }

    //!записываем файл с байткодом
    fwrite(assembler.buffer_out, assembler.len, sizeof(assembler.buffer_out[0]), assembler.stream_out);

    printf("succsesly assembled\n");

    //check_byte_code(assembler.buffer_out, assembler.len);

    fclose(assembler.stream_out);
    asm_deleter(&assembler, buffer_commands);

    return SUCCSES;
}