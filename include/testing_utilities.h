#ifndef __LOGLIB_TESTING_UTILITIES_H__
#define __LOGLIB_TESTING_UTILITIES_H__
/**
 * Counts the number of lines in the file and fails if not equal to num_lines.
 * \todo Rename to wc and add -l, -c, -w options.
 * \param fid An open file pointer.
 * \param num_lines The expected number of newlines in the file.
 */
void check_num_lines(FILE * fid, const int num_lines);

/**
 * Counts the number of files currrently in use by this process.
 * \return The number of files currently open by this process.
 */
int count_open_files();

#endif
