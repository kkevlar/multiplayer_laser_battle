/**
 * Copyright (c) 2020 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See `log.c` for details.
 */

#ifndef LOG_H
#define LOG_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#define LOG_VERSION "0.1.0"
#define LOG_USE_COLOR

typedef struct
{
    va_list ap;
    const char* fmt;
    const char* file;
    const char* func;
    struct tm* time;
    void* udata;
    int line;
    int level;
} log_Event;

typedef void (*log_LogFn)(log_Event* ev);
typedef void (*log_LockFn)(bool lock, void* udata);

enum
{
    LOG_TRACE,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL
};

#define log_trace(...) log_log(LOG_TRACE, __FILE__, __func__, __LINE__, __VA_ARGS__)
#define log_debug(...) log_log(LOG_DEBUG, __FILE__, __func__, __LINE__, __VA_ARGS__)
#define log_info(...) log_log(LOG_INFO, __FILE__, __func__, __LINE__, __VA_ARGS__)
#define log_warn(...) log_log(LOG_WARN, __FILE__, __func__, __LINE__, __VA_ARGS__)
#define log_error(...) log_log(LOG_ERROR, __FILE__, __func__, __LINE__, __VA_ARGS__)
#define log_fatal(...) log_log(LOG_FATAL, __FILE__, __func__, __LINE__, __VA_ARGS__)
#ifdef __cplusplus
extern "C"
{
#endif
    const char* log_level_string(int level);
    void log_set_lock(log_LockFn fn, void* udata);
    void log_set_level(int level);
    void log_set_quiet(bool enable);
    int log_add_callback(log_LogFn fn, void* udata, int level);
    int log_add_fp(FILE* fp, int level);

    void log_log(int level, const char* file, const char* func, int line, const char* fmt, ...);

#ifdef __cplusplus
}
#endif

#define NULLCHECK(it)                   \
    if (!it)                            \
    {                                   \
        log_fatal("Failed nullcheck!"); \
        exit(255);                      \
    }

#endif

#ifdef _MSC_VER
#define CHECK_RETURN_VAL
#else
#define CHECK_RETURN_VAL __attribute__((warn_unused_result))
#endif
