#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <cstdint>
#include <sys/stat.h>
#include <assert.h>
#include "str_funcs.h"

//!чтение в один большой массив
size_t file_read(FILE* stream, char** buffer)
{
    size_t number = size_file(stream);
    *buffer = (char*)calloc((number + 2), sizeof(**buffer));
    if (*buffer == NULL)
    {
        printf("error in file read\n");
        return 0;
    }

    size_t check = fread(*buffer, sizeof(char), (size_t)number, stream);
    if (check != number)
    {
        printf("error in fread\n");
        return 0;
    }

    return (size_t)number;
}

size_t size_file(FILE* stream)
{
    if (stream == NULL)
    {
        printf("error in size_file stream adres\n");
    }

    struct stat file = {};
    int descriptor = fileno(stream);
    fstat(descriptor, &file);

    return (size_t)file.st_size;
}

