#ifndef CHECHER_H_
#define CHECHER_H_

enum
{
    correct = 0,
    bad_size = 1,
    bad_capacity = 2,
    bad_pointer = 4,
    kanareika_error = 8,
    poison_error = 16,
    hash_error = 32,
};

//!функции проверки

int stack_dump(struct stk data, const char* func, const char* FILE, int LINE);

int verify(struct stk* data);

int cheacker(struct stk* data, const char* func, const char* file, int line);

double hash_check(struct stk data);

void dump_helper(struct stk data, const char* func, const char* FILE, int LINE);

void slicer(void);

#endif
