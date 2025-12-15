#ifndef CONFIG_H_
#define CONFIG_H_

#include "tree.h"
#include <stdio.h>
#include <time.h>

#define DEBUG

#ifdef DEBUG
    #define D_PRINT(...) printf("In function %s, %s:%d\n", __func__, __FILE__, __LINE__);\
                         printf(__VA_ARGS__)

    #define DBG(log, ...)\
    do\
    {\
    if (log)\
    { \
        time_t _now = time(NULL); \
        struct tm* _timeinfo = localtime(&_now); \
        fprintf(log, "========ERROR==========\n"); \
        fprintf(log, "Time: %s", asctime(_timeinfo)); \
        fprintf(log, "In function %s, %s:%d\n", __func__, __FILE__, __LINE__); \
        fprintf(log, "Message: "); \
        fprintf(log, __VA_ARGS__); \
        fprintf(log, "\n=======================\n\n"); \
        fflush(log); \
    }\
    \
    fprintf(stderr, "ERROR in %s at %s:%d\n", __func__, __FILE__, __LINE__); \
    fprintf(stderr, __VA_ARGS__); \
    fprintf(stderr, "\n\n"); \
    \
    } while(0)
#else
    #define D_PRINT(...) //
    #define DBG(log, ...)
#endif

//макрос проверки указателя 
#define PTR_CLC_CHECK(ptr)\
         if (ptr == NULL) \
         {\
            printf("NULL pointer after CALLOC\n");\
            return LN_ERR;\
         }\

#define PTR_RLC_CHECK(ptr)\
         if (ptr == NULL) \
         {\
            printf("NULL pointer after REALLOC\n");\
            free (ptr);\
            return ERROR;\
         }\

#endif