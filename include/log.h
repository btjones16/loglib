#ifndef __LOGLIB_INCLUDE_LOG_H__
#define __LOGLIB_INCLUDE_LOG_H__

#include <stdio.h>
#include <stdlib.h>

/**
 * \defgroup Log Log module
 * The log module provides functionality for writing error messages and other 
 * information to log files. The functions here may be used in place of printf,
 * fprintf, etc. Using this module instead of directly printing allows for log 
 * messages to be disabled at either compile time or runtime, but left in within
 * the source code in case they are needed in the future. In addition, all log
 * messages include the severity level and a timestamp.
 * 
 * The log library is implemented in C, but define guards a provided to ensure 
 * compatibility with C++ compilers.
 *
 * All log messages have the format "[TIMESTAMP] SEVERITY: MESSAGE".
 * \{
 */

/**
 * Determines the severity of the message.
 * 
 * The severity of each message is indicated by the value of type log_t. If 
 * additional detail is required, these values may be redefined and intermediate
 * number may be used.
 * LOG_FATAL: fatal errors that will cause the program to crash.
 * LOG_FATAL + 1 to LOG_ERROR: errors that require recovery and may
 * compromise the results, but are possibly recoverable.
 * LOG_ERROR + 1 to LOG_WARNING: warnings that do not require 
 * recovery but may compromise the results and should be investigated.
 * LOG_WARNING + 1 to LOG_INFO: standard runtime output.
 * LOG_INFO + 1 to LOG_DEBUG: Debugging information. Debug messages
 * may be useful for developers when debugging the model, but are unlikely to
 * be helpful during model runs. Debug messages incur a minimal runtime 
 * performance penalty.
 * LOG_TRACE + 1 or greater: Detailed tracing information for tracking 
 * the state of the model. Tracing information may incur substantial 
 * performance overhead and should not be logged except when needed.
 */
typedef enum {
  LOG_FATAL   = 0, /**< Fatal errors that result in a program crash. */
  LOG_ERROR   = 1, /**< Errors that may be possible to recover from. */
  LOG_WARNING = 2, /**< Warnings that are possibly problematic. */
  LOG_INFO    = 3, /**< Standard runtime logging information. */
  LOG_DEBUG   = 4, /**< Debug information that is usually not needed. */
  LOG_TRACE   = 5  /**< Detailed trace information. */
} log_t;

/**
 * \defgroup LogConfig Log configuration functions.
 *
 * Configure the log destinations and which messages will be logged.
 * \{
 */

/**
 * Sets the level of the least severe message to be logged.
 * 
 * Sets the process wide level of the least severe message to be logged. Any
 * message with level greater than the value set by log_set_level(), is 
 * ignored and will not be logged. Prior to the first call to 
 * log_set_level(), only messages with log level less than or equal to 
 * LOG_INFO are logged.
 * \param level The level of the least severe message to be logged.
 */
#ifdef __cplusplus
extern "C"
#endif
void log_set_level(log_t level);

/**
 * Returns the current level of the least severe message to be logged.
 * \return The level of the least severe message to be logged.
 */
#ifdef __cplusplus
extern "C"
#endif
log_t log_get_level();

/**
 * Sets the stream of the standard error output.
 * 
 * Sets the name of a file for logging warning, error, and fatal messages. 
 * This function overrides any settings previously defined by 
 * log_set_stderr_file(). This stream must be left open by the caller. It will
 * be replaced but not closed  on the next call to log_set_stderr() or
 * log_set_stderr_file().
 * \param stream An open file connection where the output should be logged.
 */
#ifdef __cplusplus
extern "C"
#endif
void log_set_stderr(FILE * stream);

/**
 * Sets the filename of the standard error stream.
 * 
 * Sets the name of a file for logging warning, error, and fatal messages. 
 * This function overrides any settings previously defined by 
 * log_set_stderr(). log_set_stderr_file() will open the specified file and
 * leave it open until the next call to log_set_stderr_file() or 
 * log_set_stderr(). 
 * \param filename The name of standard error stream where the output should 
 * be logged.
 */
#ifdef __cplusplus
extern "C"
#endif
void log_set_stderr_file(char * filename);

/**
 * Sets the stream of the standard output.
 * 
 * Sets the name of a file for logging info, debug, and trace messages.
 * This function overrides any settings previously defined by 
 * log_set_stdout_file(). This stream must be left open by the caller. It 
 * will be replaced but not closed  on the next call to log_set_stdout() or
 * log_set_stdout_file().
 * \param stream An open file connection where the output should be logged.
 */
#ifdef __cplusplus
extern "C"
#endif
void log_set_stdout(FILE * stream);

/**
 * Sets the filename of the standard output stream.
 * 
 * Sets the name of a file for logging info, debug, and trace messages.
 * This function overrides any settings previously defined by 
 * log_set_stdout(). log_set_stdout_file() will open the specified file and
 * leave it open until the next call to log_set_stdout_file() or 
 * log_set_stdout().
 * \param filename The name of standard output stream where the output should
 *  be logged.
 */
#ifdef __cplusplus
extern "C"
#endif
void log_set_stdout_file(char * filename);

/** \} */

/**
 * \defgroup logging Logging functions

 * Log messages to the standard output device and erroror warning message to 
 * the standard error stream. These streams default to stdout and stderr, 
 * and can be changed to alternate locations with the Log configuration 
 * functions.
 *
 * \param The format argument to be passed to vfprintf.
 * \param The variable length arguments to be passed to vfprintf.
 * \{
 */

/**
 * Logs the specified message to either standard output or error.
 * 
 * Logs the message to the standard output stream if level is at least 
 * LOG_INFO, or to the standard error stream if not. If level is greater
 * than level set by log_set_level(), then no logging occurs. In general, it 
 * is preferred to use one of the log macros. log_msg() is both thread safe 
 * and can be called on the same log files by multiple cooperating processes 
 * (i.e. respect advisory locks placed by flock()).
 *  
 * If called from code compiled with NVCC (device code executed on the GPU), the
 * log utilities do nothing.
 * 
 * \param level The severity level of the message to be logged.
 */
#ifdef __cplusplus
extern "C"
#define restrict __restrict
#endif
void log_msg(const log_t level, const char * restrict format, ...);

/**
 * Logs a fatal error (and crashes the program in DEBUG mode).
 */
#ifdef DEBUG
#define log_fatal(format, ...)						\
  {									\
    log_msg(LOG_FATAL, format, ##__VA_ARGS__);			\
    exit(EXIT_FAILURE);							\
  }
#else
#define log_fatal(format, ...)                                          \
  log_msg(LOG_FATAL, format, ##__VA_ARGS__);
#endif

/**
 * Logs a recoverable error (and crashes the program in DEBUG mode).
 */
#ifdef DEBUG
#define log_error(format,...)						\
  {									\
    log_msg(LOG_ERROR, format, ##__VA_ARGS__);			\
    exit(EXIT_FAILURE);							\
  }
#else
#define log_error(format, ...)                                           \
  log_msg(LOG_ERROR, format, ##__VA_ARGS__)
#endif

/**
 * Logs a warning (and crashes the program in DEBUG mode).
 */
#ifdef DEBUG
#define log_warning(format, ...)					\
  {									\
    log_msg(LOG_WARNING, format, ##__VA_ARGS__);			\
    exit(EXIT_FAILURE);							\
  }
#else
#define log_warning(format, ...)                                        \
  log_msg(LOG_WARNING, format, ##__VA_ARGS__);
#endif

/**
 * Logs standard runtime information.
 */
#define log_info(format, ...)                                           \
  log_msg(LOG_INFO, format, ##__VA_ARGS__)

/**
 * Logs debugging information (or does nothing in release builds).
 * 
 * Logs information that may be useful for debugging, but does not incur a
 * substantial performance cost. Any calls to log_debug() are removed at
 * runtime.
 */
#ifdef RELEASE
#define log_debug(format, ...) 
#else
#define log_debug(format, ...)                                         \
  log_msg(LOG_DEBUG, format, ##__VA_ARGS__)
#endif

/**
 * Logs tracing information (or does nothing in release builds).
 * 
 * Traces are intended as the way to monitor processes in detail during 
 * development. Therefore, any calls to log_trace() are removed during release
 * builds. In debugging builds, calls to log_trace() are passed through to 
 * log_msg() so format and ... should be appropriate for the corresponding 
 * parameters for log_msg().
 */
#ifdef RELEASE
#define log_trace(format, ...) 
#else
#define log_trace(format, ...)	                                       \
  log_msg(LOG_TRACE, format, ##__VA_ARGS__)
#endif

/** \} */ /* Logging functions */

/** \} */ /* Log module */
#endif
