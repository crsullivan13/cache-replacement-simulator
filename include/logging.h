#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>
#include <stdarg.h>

#ifdef LOGGING
#define LOG(fmt, ...) \
            _clog(__FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)

void _clog(const char* file, const char* func, const int line, const char* fmt, ...);

#else
#define LOG(fmt, ...) {}
#endif

#endif