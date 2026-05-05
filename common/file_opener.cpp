#include "file_opener.h"

FILE* file_opener (const char * Filename, const char * Mode)
{
    FILE* stream = fopen(Filename, Mode);
    if (stream == NULL)
    {
        printf ("file %s open error", Filename);
        return NULL;
    }

    return stream;
}