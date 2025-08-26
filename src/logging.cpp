#include "logging.h"

void _clog(const char* file, const char* func, const int line, const char* fmt, ...) {
    va_list args;

    fprintf(stderr, "[%s|%s|%d] ", file, func, line);

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    //fprintf(stderr, "\n");
}