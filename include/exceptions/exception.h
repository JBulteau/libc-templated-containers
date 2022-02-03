#ifndef _EXCEPTION_H__
#define _EXCEPTION_H__

#include <setjmp.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "exception_definition.h"

jmp_buf buffer;
jmp_buf __old_buffer;
int __error;

#define RegisterException(e) e,

typedef enum __exception_types {
    Exception = 1,
    RegisteredException
    TOP
} __exception_types_e;

#undef RegisterException
#define RegisterException(e) #e,

const char *__exception_names[] = { "Exception", RegisteredException };

typedef struct __exception_s {
    __exception_types_e type;
    char *function;
    char *file;
    int line;
    char *what;
} exception_t;

exception_t __exception;

#define try \
    __old_buffer[0] = buffer[0]; \
    __error = setjmp(buffer); \
    if (__error == 0)

#define catch(exception) \
    buffer[0] = __old_buffer[0]; \
    if (exception != __error) { \
        longjmp(buffer, __error); \
    } else

#define throw(exception, what) __throw(__func__, __FILE__, __LINE__, what, exception)

static inline void __throw(const char *function, const char *file, int line, const char *what, __exception_types_e exception_type)
{
    __exception.function = strdup(function);
    __exception.file = strdup(file);
    __exception.line = line;
    __exception.what = strdup(what);
    __exception.type = exception_type;
    longjmp(buffer, __exception.type);
}

void __attribute__((constructor)) init()
{
    int error = setjmp(buffer);

    if (error != 0) {
        dprintf(2, "Exception %s thrown in function '%s' (%s at line %i): \"%s\"\n", __exception_names[error - 1], __exception.function, __exception.file, __exception.line, __exception.what);
        exit(error);
    }
}

#endif