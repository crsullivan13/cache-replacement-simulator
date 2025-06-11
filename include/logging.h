#include <stdio.h>
#include <stdarg.h>

#ifdef LOGGING
#define LOG(fmt, ...) \
            _clog(__FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)

void _clog(const char* file, const char* func, const int line, const char* fmt, ...) {
    va_list args;

    fprintf(stderr, "[%s|%s|%d] ", file, func, line);

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fprintf(stderr, "\n");
}
#else
#define LOG(fmt, ...) {}
#endif