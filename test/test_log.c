#include <setjmp.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>

#include "log.h"
#include "testing_utilities.h"

/* cmockery must be included last. Otherwise the declarations of malloc, 
 * calloc, and free in cmockery will conflict with stdlib.h. */
#include <cmockery.h>

/**
 * Unit tests for the log module.
 */
/**< \todo Test that the log module is thread safe. */
/**< \todo Test that the log module acquires an advisory lock before writing. */

/** 
 * Tests that log_get_level() returns the last log level that was specified by
 * _log_set_level(). Together with test_set_level(), this test is designed to 
 * test that log_set_level() is setting the log level correctly.
 */
void test_set_level_and_get_level(void ** state) { 
  log_t state_set = LOG_TRACE;
  log_set_level(state_set);
  assert_int_equal((int) state_set, (int) log_get_level());
}

/**
 * Tests that log_msg() respects the level set by log_set_level(). Together with 
 * the previous test, this test is deisgned to ensure that log_set_level() is 
 * achieving the desired functionality.
 */
void test_set_level(void ** state) {
  /* Open a temp file and tell the logger to write to it. */
  FILE * fid = tmpfile();
  log_set_stdout(fid);
  log_set_level(LOG_DEBUG);
  /* Write some output to the tempfile and check as it writes. */
  log_msg(LOG_DEBUG - 1, "This should be logged.");
  check_num_lines(fid, 1);
  log_msg(LOG_DEBUG, "This should be logged.");
  check_num_lines(fid, 2);
  log_msg(LOG_DEBUG + 1, "This should not be logged.");
  check_num_lines(fid, 2);
  /* Close the temp file and cleanup. */
  fclose(fid);
}

/**
 * 
 */
void test_set_stdout(void ** state) { 
  /* Open a temp file and pass it to LogSetStdOut(). */
  FILE * fid = tmpfile();
  log_set_stdout(fid);
  /* Set the standard output to stdout. */
  log_set_stdout(stdout);
  /* Try to write data to fid, this will fail if it was closed. */
  char data[8];
  assert_int_equal(8, fwrite(data, sizeof(char), 8, fid));
  /* Close the file and cleanup. */
  fclose(fid);
}

/**
 * 
 */
void test_set_stderr(void ** state) {
  /* Open a temp file and pass it to LogSetStdOut(). */
  FILE * fid = tmpfile();
  log_set_stderr(fid);
  /* Set the standard output to stdout. */
  log_set_stderr(stderr);
  /* Try to write data to fid, this will fail if it was closed. */
  char data[8];
  assert_int_equal(8, fwrite(data, sizeof(char), 8, fid));
  /* Close the file and cleanup. */
  fclose(fid);
}

/**
 * Tests that the log_set_stdout_file() opens the specified file when it is
 * passed in and closes it again when another stream is passed in.
 */
void test_set_stdout_file(void ** state) { 
  /* Get the name of temp file. */
  char * filename = tmpnam(NULL);
  /* Set the standard output to stdout so that any files are closed. */
  log_set_stdout(stdout);
  /* Count the number of open files. */
  int num_files_open = count_open_files();
  /* Set the standard output and check that a new file was opened. */
  log_set_stdout_file(filename);
  assert_int_equal(num_files_open + 1, count_open_files());
  /* Reset the standard output and check that the file was closed. */
  log_set_stdout(stdout);
  assert_int_equal(num_files_open, count_open_files());
  /* Remove the temp file. */
  remove(filename);
}

/**
 * Tests that the log_set_stderr_file() opens the specified file when it is
 * passed in and closes it again when another stream is passed in.
 */
void test_set_stderr_file(void ** state) {
  /* Get the name of temp file. */
  char * filename = tmpnam(NULL);
  /* Set the standard error stream to stderr so that any files are closed. */
  log_set_stdout(stderr);
  /* Count the number of open files. */
  int num_files_open = count_open_files();
  /* Set the standard output and check that a new file was opened. */
  log_set_stderr_file(filename);
  assert_int_equal(num_files_open + 1, count_open_files());
  /* Reset the standard output and check that the file was closed. */
  log_set_stderr(stderr);
  assert_int_equal(num_files_open, count_open_files());
  /* Remove the temp file. */
  remove(filename);
}

/**
 * Tests that log_msg() records messages with level greater than 
 * LOG_WARNING to the standard output stream and messages with level
 * less than or equal to LOG_WARNING to the standard error stream.
 */
void test_log_msg(void ** state) {
  /* Set the log level to LOG_TRACE (log all messages). */
  log_set_level(LOG_TRACE);
  /* 
   * Open a temp file, and tell the logger to write to it. Then check that 1 
   * line has been written to it.
   */
  for(int i = LOG_TRACE; i > LOG_WARNING; --i) {
    FILE * fid = tmpfile();
    log_set_stdout(fid);
    log_msg(i, "This is message %d.", i);
    check_num_lines(fid, 1);
  }
  for(int i = LOG_WARNING; i >= LOG_FATAL; --i) {
    FILE * fid = tmpfile();
    log_set_stderr(fid);
    log_msg(i, "This is message %d.", i);
    check_num_lines(fid, 1);
  }
}

/** /todo Too much redundancy in the test_log_* tests. */
/**
 * Tests that log_fatal() writes to the standard error stream and includes
 * the word FATAL in the output.
 */
void test_log_fatal(void ** state) {
  /* Open a test file and write to it. */
  FILE * fid = tmpfile();
  log_set_stderr(fid);
  log_fatal("Message.");
  /* Read the string. */
  rewind(fid);
  char msg[0xff]; /* Long enough and then some. */
  fread(msg, sizeof(char), 0xff, fid);
  /* Check that the string contains FATAL. */
  assert_true(strstr(msg, "FATAL") != NULL);
}

/**
 * Tests that log_error() writes to the standard error stream and includes
 * the word ERROR in the output.
 */
void test_log_error(void ** state) {
  /* Open a test file and write to it. */
  FILE * fid = tmpfile();
  log_set_stderr(fid);
  log_error("Message.");
  /* Read the string. */
  rewind(fid);
  char msg[0xff]; /* Long enough and then some. */
  fread(msg, sizeof(char), 0xff, fid);
  /* Check that the string contains FATAL. */
  assert_true(strstr(msg, "ERROR") != NULL);
}

/**
 * Tests that log_warning() writes to the standard error stream and includes
 * the word WARNING in the output.
 */
void test_log_warning(void ** state) {
  /* Open a test file and write to it. */
  FILE * fid = tmpfile();
  log_set_stderr(fid);
  log_warning("Message.");
  /* Read the string. */
  rewind(fid);
  char msg[0xff]; /* Long enough and then some. */
  fread(msg, sizeof(char), 0xff, fid);
  /* Check that the string contains FATAL. */
  assert_true(strstr(msg, "WARNING") != NULL);
}

/**
 * Tests that log_info() writes to the standard output stream and includes
 * the word INFO in the output.
 */
void test_log_info(void ** state) {
  /* Open a test file and write to it. */
  FILE * fid = tmpfile();
  log_set_stdout(fid);
  log_info("Message.");
  /* Read the string. */
  rewind(fid);
  char msg[0xff]; /* Long enough and then some. */
  fread(msg, sizeof(char), 0xff, fid);
  /* Check that the string contains FATAL. */
  assert_true(strstr(msg, "INFO") != NULL);
}
/**
 * Tests that log_debug() writes to the standard output stream and includes
 * the word DEBUG in the output.
 */
void test_log_debug(void ** state) {
  /* Set the log level to LOG_DEBUG so the message is logged. */
  log_set_level(LOG_DEBUG);
  /* Open a test file and write to it. */
  FILE * fid = tmpfile();
  log_set_stdout(fid);
  log_debug("Message.");
  /* Read the string. */
  rewind(fid);
  char msg[0xff]; /* Long enough and then some. */
  fread(msg, sizeof(char), 0xff, fid);
  /* Check that the string contains FATAL. */
  assert_true(strstr(msg, "DEBUG") != NULL);
}

/**
 * Tests that log_trace() writes to the standard output stream and includes
 * the word TRACE in the output.
 */
void test_log_trace(void ** state) {
  /* Set the log level to LOG_TRACE so the message is logged. */
  log_set_level(LOG_TRACE);
  /* Open a test file and write to it. */
  FILE * fid = tmpfile();
  log_set_stdout(fid);
  log_trace("Message.");
  /* Read the string. */
  rewind(fid);
  char msg[0xff]; /* Long enough and then some. */
  fread(msg, sizeof(char), 0xff, fid);
  /* Check that the string contains FATAL. */
  assert_true(strstr(msg, "TRACE") != NULL);
}


int main(int argc, char ** argv) {
    /* Set up the unit test runner. */
  const UnitTest tests[] = {
    unit_test(test_set_level_and_get_level),
    unit_test(test_set_level),
    unit_test(test_set_stderr),
    unit_test(test_set_stderr_file),
    unit_test(test_set_stdout),
    unit_test(test_set_stdout_file),
    unit_test(test_log_msg),
    unit_test(test_log_fatal),
    unit_test(test_log_error),
    unit_test(test_log_warning),
    unit_test(test_log_info),
    unit_test(test_log_debug),
    unit_test(test_log_trace)
  };
  /* Run the tests and print the results. */
  return run_tests(tests);
}
