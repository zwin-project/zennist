#include "log.h"

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

void
ZennistLogStderr(ZennistLogImportance verbosity, const char *fmt, va_list args)
{
  switch (verbosity) {
    case ZennistLogImportance::ZENNIST_ERROR:
      fprintf(stderr, "[ERROR]");
      break;
    case ZennistLogImportance::ZENNIST_WARN:
      fprintf(stderr, "[WARN]");
      break;
    case ZennistLogImportance::ZENNIST_INFO:
      fprintf(stderr, "[INFO]");
      break;
    case ZennistLogImportance::ZENNIST_DEBUG:
      fprintf(stderr, "[DEBUG]");
      break;
    default:
      break;
  }

  fprintf(stderr, " ");

  vfprintf(stderr, fmt, args);

  fprintf(stderr, "\n");
}

void
ZennistLog(ZennistLogImportance verbosity, const char *format, ...)
{
  va_list args;
  va_start(args, format);
  ZennistLogStderr(verbosity, format, args);
  va_end(args);
}
