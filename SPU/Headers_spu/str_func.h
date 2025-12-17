#ifndef STRFUNC_H_
#define STRFUNC_H_

size_t file_read(FILE* stream, char** buffer);

size_t size_file(FILE* stream);

char** make_mass(char* buffer, size_t len);

int changer(char** buffer);

size_t n_check(char* buffer);

int my_strcmp(char* str1, const char* str2);

#endif