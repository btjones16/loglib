#include <errno.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <sys/file.h>

#include "log.h"

/**
 * 
 */
struct LogConfig {
  bool setup;
  pthread_mutex_t lock;
  log_t level;
  FILE * stdout;
  FILE * stderr;
  bool stdout_should_be_closed;
  bool stderr_should_be_closed;
};

/**
 *
 */
static struct LogConfig config =  {
  .setup = false,
  .lock = PTHREAD_MUTEX_INITIALIZER,
  .level = LOG_INFO,
  .stdout_should_be_closed = false,
  .stderr_should_be_closed = false
};

/**
 *
 */
static void log_setup() {
  pthread_mutex_lock(&config.lock);
  config.stdout = stdout;
  config.stderr = stderr;
  config.setup = true;
  pthread_mutex_unlock(&config.lock);
}

/**
 *
 */
log_t log_get_level() {
  if(!config.setup) log_setup();
  return config.level;
}

/**
 * 
 */
void log_set_level(log_t level) {
  if(!config.setup) log_setup();
  pthread_mutex_lock(&config.lock);
  config.level = level;
  pthread_mutex_unlock(&config.lock);
}

void log_set_stderr_file(char * filename) {
  if(!config.setup) log_setup();
  pthread_mutex_lock(&config.lock); /* Lock the module. */
  /* Save the old stream in case we cannot open a new one. */
  FILE * old_stderr = config.stderr;
  if((config.stderr = fopen(filename, "w")) == NULL) {
    /* Restore the old stream and note the error. */
    config.stderr = old_stderr;
    pthread_mutex_unlock(&config.lock); /* Unlock the module. */
    log_error("I could not change stderr to %s with error %d.", 
	      filename, errno);
  } else {
    /* Only close the old stream if a new one could be opened. */
    if(config.stderr_should_be_closed)
      fclose(old_stderr);
    config.stderr_should_be_closed = true;
    pthread_mutex_unlock(&config.lock); /* Unlock the module. */
  }
}

void log_set_stdout_file(char * filename) {
  if(!config.setup) log_setup();
  pthread_mutex_lock(&config.lock); /* Lock the module. */
  /* Save the old stream in case we cannot open a new one. */
  FILE * old_stdout = config.stdout;
  if((config.stdout = fopen(filename, "w")) == NULL) {
    /* Restore the old stream and note the error. */
    config.stdout = old_stdout;
    pthread_mutex_unlock(&config.lock); /* Unlock the module. */
    log_error("I could not change stdout to %s with error %d.", 
	      filename, errno);
  } else {
    /* Only close the old stream if a new one could be opened. */
    if(config.stdout_should_be_closed)
      fclose(old_stdout);
    config.stdout_should_be_closed = true;
    pthread_mutex_unlock(&config.lock); /* Unlock the module. */
  }
}

void log_set_stderr(FILE * stream) {
  pthread_mutex_lock(&config.lock);
  if(config.stderr_should_be_closed) fclose(config.stderr);
  config.stderr = stream;
  config.stderr_should_be_closed = false;
  pthread_mutex_unlock(&config.lock);
}

void log_set_stdout(FILE * stream) {
  if(!config.setup) log_setup();
  pthread_mutex_lock(&config.lock);
  if(config.stdout_should_be_closed) fclose(config.stdout);
  config.stdout = stream;
  config.stdout_should_be_closed = false;
  pthread_mutex_unlock(&config.lock);
}

void log_msg(const log_t level, const char * restrict format, ...) {
  if(!config.setup) log_setup();
  /* Get the current time (not critical / no lock needed). */
  if(level <= config.level) {
    /* Messages less severe than config.level are not logged. */
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    char time_str[25];
    strftime(time_str, 25, "%a %d %b %Y %H:%M:%S", timeinfo);
    /* Get the level string (not critical / no lock needed). */
    char * level_str;
    FILE * stream;
    switch(level) {
    case LOG_FATAL:
      level_str = "FATAL";
      stream = config.stderr;
      break;
    case LOG_ERROR:
      level_str = "ERROR";
      stream = config.stderr;
      break;
    case LOG_WARNING:
      level_str = "WARNING";
      stream = config.stderr;
      break;
    case LOG_INFO:
      level_str = "INFO";
      stream = config.stdout;
      break;
    case LOG_DEBUG:
      level_str = "DEBUG";
      stream = config.stdout;
      break;
    case LOG_TRACE:
      level_str = "TRACE";
      stream = config.stdout;
      break;
    default:
      level_str = "UNKNOWN";
      stream = config.stdout;
      break;
    }
    /* Get the variable length arguments. (not critical / no lock needed)*/
    va_list args;
    va_start(args, format);
    /* Do the actual printing. This is critical and needs to be locked. */
    pthread_mutex_lock(&config.lock);
    flock(fileno(stream), LOCK_EX); /* Lock the file. */
    fprintf(stream, "[%s] %s: ", time_str, level_str);
    vfprintf(stream, format, args);
    fprintf(stream, "\n");
    flock(fileno(stream), LOCK_UN); /* Unlock the file. */
    pthread_mutex_unlock(&config.lock);
    /* Signal the end of the variable length arguments. */
    va_end(args);
  }
  fflush(stdout);
  fflush(stderr);
}
