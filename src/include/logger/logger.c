#include "logger.h"
#include <stdarg.h>
#include <time.h>

#define COLOR_RESET   "\x1b[0m"
#define COLOR_INFO    "\x1b[32m"  // Green
#define COLOR_WARN    "\x1b[33m"  // Yellow
#define COLOR_ERROR   "\x1b[31m"  // Red

void log_write(const char* level, const char* file, int line, const char* fmt, ...) {
    const char* color;

    if (level[0] == 'I') color = COLOR_INFO;
    else if (level[0] == 'W') color = COLOR_WARN;
    else if (level[0] == 'E') color = COLOR_ERROR;
    else color = COLOR_RESET;

    time_t t = time(NULL);
    struct tm* lt = localtime(&t);

    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", lt);

    // Colored level only
    fprintf(stderr, "[%s] [", timestamp);
    fprintf(stderr, "%s%s%s", color, level, COLOR_RESET);
    fprintf(stderr, "] (%s:%d): ", file, line);

    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fputc('\n', stderr);
}
