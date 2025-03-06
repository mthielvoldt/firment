/** Provides a log interface
 * Each message contains: 
 * - Text payload capable of MAX_TEXT_LENGTH
 * - A single float for numeric values
 * - An auto-incremented message counter.
 * - A log level.
 */
#pragma once

#include <stdbool.h>

typedef enum _logLevel {
  LOG_VERBOSE,
  LOG_INFO,
  LOG_WARN,
  LOG_ERROR,
  LOG_SILENT,
} logLevel_t;

bool fmt_sendLog(logLevel_t level, const char msg[], float number);

void fmt_setLogLevel(logLevel_t level);