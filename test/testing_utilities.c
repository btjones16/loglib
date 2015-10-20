#include <errno.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>


#include "testing_utilities.h"

void check_num_lines(FILE * fid, const int num_lines, CuTest * tc) {
  /* Rewind to the beginning of the file. */
  rewind(fid);
  /* Read each character and increment the number of lines for each \n. */
  int ch, lines=0;
  while (EOF != (ch=getc(fid)))
    if (ch == '\n')
      ++lines;
  /* Check the actual and expected number of lines. */
  CuAssertIntEquals(tc, num_lines, lines);
}

int count_open_files() { 
  /* 
   * Iterate through all possible file descriptors, checking if the file
   * descriptor is valid for each. The number of valid file descriptors is the
   * number of oepn files. 
   */
  int num_files = 0;
  int max_file_descriptor = getdtablesize();
  struct stat stats;
  for(int i = 0; i <= max_file_descriptor; ++i )
    if(fstat(i, &stats) == 0)
      ++num_files;
  return num_files; 
}
