#include "flexui/Log.hpp"

#include <stdio.h>
#include <stdarg.h>

void fui_log(const char* severity, const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    printf("[%s] ", severity);
    vfprintf(stdout, format, ap);
    printf("\n");
    va_end(ap);
}
