# LogLib

The log module provides functionality for writing error messages and other 
information to log files. The functions here may be used in place of printf,
fprintf, etc. Using this module instead of directly printing allows for log 
messages to be disabled at either compile time or runtime, but left in within
the source code in case they are needed in the future. In addition, all log
messages include the severity level and a timestamp.

The log library is implemented in C, but define guards a provided to ensure 
compatibility with C++ compilers.

All log messages have the format "[TIMESTAMP] SEVERITY: MESSAGE".

