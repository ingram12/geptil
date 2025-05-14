#ifndef LOG_H
#define LOG_H

#include <stdio.h>

#define LOG_INFO(...)    log_write("INFO", __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARN(...)    log_write("WARN", __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...)   log_write("ERROR", __FILE__, __LINE__, __VA_ARGS__)

void log_write(const char* level, const char* file, int line, const char* fmt, ...);

#endif
