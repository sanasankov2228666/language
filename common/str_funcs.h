#ifndef STR_H_
#define STR_H_

#include "stdio.h"

size_t file_read(FILE* stream, char** buffer);

size_t size_file(FILE* stream);

FILE* file_opener (const char * Filename, const char * Mode);

#endif