#include <setjmp.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>

#include "log.h"
#include "testing_utilities.h"
#include "cutest-1.5/CuTest.h"

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
void test_set_level_and_get_level(CuTest * tc) {
  log_t state_set = LOG_TRACE;
  log_set_level(state_set);
  CuAssertIntEquals(tc, (int) state_set, (int) log_get_level());
}

/**
 * Tests that log_msg() respects the level set by log_set_level(). Together with
 * the previous test, this test is deisgned to ensure that log_set_level() is
 * achieving the desired functionality.
 */
void test_set_level(CuTest * tc) {
  /* Open a temp file and tell the logger to write to it. */
  FILE * fid = tmpfile();
  log_set_stdout(fid);
  log_set_level(LOG_DEBUG);
  /* Write some output to the tempfile and check as it writes. */
  log_msg(LOG_DEBUG - 1, "This should be logged.");
  check_num_lines(fid, 1, tc);
  log_msg(LOG_DEBUG, "This should be logged.");
  check_num_lines(fid, 2, tc);
  log_msg(LOG_DEBUG + 1, "This should not be logged.");
  check_num_lines(fid, 2, tc);
  /* Close the temp file and cleanup. */
  fclose(fid);
}

/**
 *
 */
void test_set_stdout(CuTest * tc) {
  /* Open a temp file and pass it to LogSetStdOut(). */
  FILE * fid = tmpfile();
  log_set_stdout(fid);
  /* Set the standard output to stdout. */
  log_set_stdout(stdout);
  /* Try to write data to fid, this will fail if it was closed. */
  char data[8];
  CuAssertIntEquals(tc, 8, fwrite(data, sizeof(char), 8, fid));
  /* Close the file and cleanup. */
  fclose(fid);
}

/**
 *
 */
void test_set_stderr(CuTest * tc) {
  /* Open a temp file and pass it to LogSetStdOut(). */
  FILE * fid = tmpfile();
  log_set_stderr(fid);
  /* Set the standard output to stdout. */
  log_set_stderr(stderr);
  /* Try to write data to fid, this will fail if it was closed. */
  char data[8];
  CuAssertIntEquals(tc, 8, fwrite(data, sizeof(char), 8, fid));
  /* Close the file and cleanup. */
  fclose(fid);
}

/**
 * Tests that the log_set_stdout_file() opens the specified file when it is
 * passed in and closes it again when another stream is passed in.
 */
void test_set_stdout_file(CuTest * tc) {
  /* Get the name of temp file. */
  char * filename = tmpnam(NULL);
  /* Set the standard output to stdout so that any files are closed. */
  log_set_stdout(stdout);
  /* Count the number of open files. */
  int num_files_open = count_open_files();
  /* Set the standard output and check that a new file was opened. */
  log_set_stdout_file(filename);
  CuAssertIntEquals(tc, num_files_open + 1, count_open_files());
  /* Reset the standard output and check that the file was closed. */
  log_set_stdout(stdout);
  CuAssertIntEquals(tc, num_files_open, count_open_files());
  /* Remove the temp file. */
  remove(filename);
}

/**
 * Tests that the log_set_stderr_file() opens the specified file when it is
 * passed in and closes it again when another stream is passed in.
 */
void test_set_stderr_file(CuTest * tc) {
  /* Get the name of temp file. */
  char * filename = tmpnam(NULL);
  /* Set the standard error stream to stderr so that any files are closed. */
  log_set_stdout(stderr);
  /* Count the number of open files. */
  int num_files_open = count_open_files();
  /* Set the standard output and check that a new file was opened. */
  log_set_stderr_file(filename);
  CuAssertIntEquals(tc, num_files_open + 1, count_open_files());
  /* Reset the standard output and check that the file was closed. */
  log_set_stderr(stderr);
  CuAssertIntEquals(tc, num_files_open, count_open_files());
  /* Remove the temp file. */
  remove(filename);
}

/**
 * Tests that log_msg() records messages with level greater than
 * LOG_WARNING to the standard output stream and messages with level
 * less than or equal to LOG_WARNING to the standard error stream.
 */
void test_log_msg(CuTest * tc) {
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
    check_num_lines(fid, 1, tc);
  }
  for(int i = LOG_WARNING; i >= LOG_FATAL; --i) {
    FILE * fid = tmpfile();
    log_set_stderr(fid);
    log_msg(i, "This is message %d.", i);
    check_num_lines(fid, 1, tc);
  }
}

/** /todo Too much redundancy in the test_log_* tests. */
/**
 * Tests that log_fatal() writes to the standard error stream and includes
 * the word FATAL in the output.
 */
void test_log_fatal(CuTest * tc) {
  /* Open a test file and write to it. */
  FILE * fid = tmpfile();
  log_set_stderr(fid);
  log_fatal("Message.");
  /* Read the string. */
  rewind(fid);
  char msg[0xff]; /* Long enough and then some. */
  fread(msg, sizeof(char), 0xff, fid);
  /* Check that the string contains FATAL. */
  CuAssertTrue(tc, strstr(msg, "FATAL") != NULL);
}

/**
 * Tests that log_error() writes to the standard error stream and includes
 * the word ERROR in the output.
 */
void test_log_error(CuTest * tc) {
  /* Open a test file and write to it. */
  FILE * fid = tmpfile();
  log_set_stderr(fid);
  log_error("Message.");
  /* Read the string. */
  rewind(fid);
  char msg[0xff]; /* Long enough and then some. */
  fread(msg, sizeof(char), 0xff, fid);
  /* Check that the string contains FATAL. */
  CuAssertTrue(tc, strstr(msg, "ERROR") != NULL);
}

/**
 * Tests that log_warning() writes to the standard error stream and includes
 * the word WARNING in the output.
 */
void test_log_warning(CuTest * tc) {
  /* Open a test file and write to it. */
  FILE * fid = tmpfile();
  log_set_stderr(fid);
  log_warning("Message.");
  /* Read the string. */
  rewind(fid);
  char msg[0xff]; /* Long enough and then some. */
  fread(msg, sizeof(char), 0xff, fid);
  /* Check that the string contains FATAL. */
  CuAssertTrue(tc, strstr(msg, "WARNING") != NULL);
}

/**
 * Tests that log_info() writes to the standard output stream and includes
 * the word INFO in the output.
 */
void test_log_info(CuTest * tc) {
  /* Open a test file and write to it. */
  FILE * fid = tmpfile();
  log_set_stdout(fid);
  log_info("Message.");
  /* Read the string. */
  rewind(fid);
  char msg[0xff]; /* Long enough and then some. */
  fread(msg, sizeof(char), 0xff, fid);
  /* Check that the string contains FATAL. */
  CuAssertTrue(tc, strstr(msg, "INFO") != NULL);
}
/**
 * Tests that log_debug() writes to the standard output stream and includes
 * the word DEBUG in the output.
 */
void test_log_debug(CuTest * tc) {
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
  CuAssertTrue(tc, strstr(msg, "DEBUG") != NULL);
}

/**
 * Tests that log_trace() writes to the standard output stream and includes
 * the word TRACE in the output.
 */
void test_log_trace(CuTest * tc) {
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
  CuAssertTrue(tc, strstr(msg, "TRACE") != NULL);
}

CuSuite * setup_test_suite() {
  CuSuite * suite = CuSuiteNew();
  SUITE_ADD_TEST(suite, test_set_level_and_get_level);
  SUITE_ADD_TEST(suite, test_set_level);
  SUITE_ADD_TEST(suite, test_set_stdout);
  SUITE_ADD_TEST(suite, test_set_stderr);
  SUITE_ADD_TEST(suite, test_set_stdout_file);
  SUITE_ADD_TEST(suite, test_set_stderr_file);
  SUITE_ADD_TEST(suite, test_log_msg);
  SUITE_ADD_TEST(suite, test_log_fatal);
  SUITE_ADD_TEST(suite, test_log_error);
  SUITE_ADD_TEST(suite, test_log_warning);
  SUITE_ADD_TEST(suite, test_log_info);
  SUITE_ADD_TEST(suite, test_log_debug);
  SUITE_ADD_TEST(suite, test_log_trace);
  return suite;
}

int main(void) {
  /* Create the tests. */
  CuString * output = CuStringNew();
  CuSuite * suite = CuSuiteNew();
  CuSuiteAddSuite(suite, setup_test_suite());
  /* Run the tests. */
  CuSuiteRun(suite);
  /* Print the results. */
  CuSuiteSummary(suite, output);
  CuSuiteDetails(suite, output);
  printf("%s\n", output->buffer);
}
