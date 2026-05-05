#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdio.h>
#include <time.h>

enum error
{
    LN_OK   = 0,
    LN_ERR  = 1,
};

#define DEBUG

#ifdef DEBUG
    #define D_PRINT(...) printf("In function %s, %s:%d\n", __func__, __FILE__, __LINE__);\
                         printf(__VA_ARGS__)
#else
    #define D_PRINT(...) //
#endif

// макрос проверки указателя 
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