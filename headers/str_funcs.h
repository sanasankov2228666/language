#ifndef STR_H_
#define STR_H_

#include "stdio.h"

size_t file_read(FILE* stream, char** buffer);

long int size_file(FILE* stream);

char** make_mass(char* buffer, size_t len);

int changer(char** buffer);

size_t n_check(char* buffer);

#endif