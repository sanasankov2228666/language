#include "../stack/headers/stack1.h"
#include "../stack/headers/checker.h"
#include "spu.h"
#include "fileopen.h"

typedef int type;

int main(void)
{
    struct spu data_spu = {};
    create_spu(&data_spu);

    slicer();

    //!открытие файла с байткодом
    FILE* stream_bytecode = file_opener("SPU/translate_cmd.asm", "rb", __FUNCTION__, __FILE__, __LINE__);
    if (stream_bytecode == NULL) return 1;

    //!открытие файла записи ошибок
    data_spu.stream_error = file_opener("SPU/spu_errors.txt", "wb", __FUNCTION__, __FILE__, __LINE__);
    if (data_spu.stream_error == NULL) return 1;

    //!подсчёт количества комманд
    data_spu.len = size_file(stream_bytecode) / sizeof(int);

    //!создание массива с байткодом
    data_spu.code = mass_cmd_maker(stream_bytecode, data_spu.len);
    if (data_spu.code == NULL) return 1;

    fclose(stream_bytecode);

    //!проверка сигнатуры
    if( !(signature_version_check(data_spu.code)))
    {
        printf("signature or version is diffrient\n");
        spu_deleter(&data_spu);
        return 1;
    }

    //!запуск процессора
    printf("SPU launching\n");
    while ( spu_launcher(&data_spu) != HLT && data_spu.counter < data_spu.len + 1 );

    printf("SPU turning off\n");
    spu_deleter(&data_spu);

    return 0;
}
