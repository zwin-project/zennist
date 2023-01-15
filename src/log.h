#pragma once

#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

typedef enum {
  ZENNIST_SILENT = 0,
  ZENNIST_ERROR = 1,
  ZENNIST_WARN = 2,
  ZENNIST_INFO = 3,
  ZENNIST_DEBUG = 4,
  ZENNIST_LOG_IMPORTANCE_LAST,
} ZennistLogImportance;

#ifdef __GNUC__
#define ATTRIB_PRINTF(start, end) __attribute__((format(printf, start, end)))
#else
#define ATTRIB_PRINTF(start, end)
#endif

typedef void (*terminate_callback_t)(int exit_code);

void ZennistLog(ZennistLogImportance verbosity, const char *format, ...)
    ATTRIB_PRINTF(2, 3);

#define ZennistError(fmt, ...)                                            \
  ZennistLog(ZENNIST_ERROR, "[zennist] [%s:%d] " fmt, __FILE__, __LINE__, \
      ##__VA_ARGS__)

#define ZennistWarn(fmt, ...)                                            \
  ZennistLog(ZENNIST_WARN, "[zennist] [%s:%d] " fmt, __FILE__, __LINE__, \
      ##__VA_ARGS__)

#define ZennistInfo(fmt, ...)                                            \
  ZennistLog(ZENNIST_INFO, "[zennist] [%s:%d] " fmt, __FILE__, __LINE__, \
      ##__VA_ARGS__)

#define ZennistDebug(fmt, ...)                                            \
  ZennistLog(ZENNIST_DEBUG, "[zennist] [%s:%d] " fmt, __FILE__, __LINE__, \
      ##__VA_ARGS__)
