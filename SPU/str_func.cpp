#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <cstdint>
#include <sys/stat.h>
#include <assert.h>
#include "str_func.h"

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


char** make_mass(char* buffer, size_t len)
{
    char** buffer_w = (char**)calloc(len, sizeof(char*));
    if (buffer_w == NULL)
    {
        printf("error int calloc make_mass\n");
    }

    char* ptr = buffer;
    buffer_w[0] = ptr;

    for (size_t i = 1; i < len; i++ )
    {
        while (*ptr != ' ' && *ptr != '\n' && *ptr != '\0' && *ptr != ';')
        {
            ptr++;
        }

        if (*ptr == ';')
        {
            while (*ptr != '\n' && *ptr != '\0') ptr++;
        }

        while (*ptr == ' ' || *ptr == '\n') ptr++;

        buffer_w[i] = ptr;
    }

    return buffer_w;
}

int changer(char** buffer)
{
    size_t i = 0;

    while ((*buffer)[i] != '\0')
    {
        if ((*buffer)[i] == ' ')
        {
            (*buffer)[i] = '\0';
        }

        if ((*buffer)[i] == '\n')
        {
            (*buffer)[i] = '\0';
        }

        if ((*buffer)[i] == ';')
        {
            (*buffer)[i] = '\0';
        }

        i++;
    }

    return (i > 0);
}

size_t n_check(char* buffer)
{
    if (buffer == NULL)
    {
        printf("error in n_chech bufer adres\n");
        return 0;
    }

    size_t len = 1;
    size_t index = 0;

    while (buffer[index] != '\0')
    {
        if (buffer[index] == '\n')
        {
            len++;
        }

        index++;
    }

    return len;
}

int my_strcmp(char* str1, const char* str2)
{
    int div = 0;
    size_t i = 0;

    if (strlen(str1) != strlen(str2))
    {
        return 1;
    }

    while (str1[i] != '\0' && str2[i] != '\0')
    {
        div = tolower (str1[i]) - tolower (str2[i]);
        i++;
        if (div != 0)
        {
            break;
        }
    }
    return div;
}