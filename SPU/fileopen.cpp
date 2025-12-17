#include <stdio.h>
#include <math.h>
#include "fileopen.h"

FILE* file_opener(const char* file_name, const char* type_p, const char* func, const char* file, int line)
{
    FILE* stream = fopen(file_name, type_p);

    if (stream == NULL)
    {
        printf("In function %s, %s:%d\n", func, file, line);
        printf("error in file open %s\n", file_name);
        return NULL;
    }
    
    return stream;
}