#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "logging.h"

const char *log_s[] = {"D", "I", "M", "W", "E", "F"};

void wlog(loglevel level, const char *s, ...)
{
    char fmt[strlen(s)+5];
    snprintf(fmt, sizeof(fmt), "%s: %s\n", log_s[level], s);
    va_list args;
    va_start(args, s);
    vfprintf(stderr, fmt, args);
    va_end(args);
    if ( level >= FATAL ) exit(1);
}
