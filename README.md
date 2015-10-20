# LogLib

The log module provides functionality for writing error messages and other 
information to log files. The functions here may be used in place of printf,
fprintf, etc. Using this module instead of directly printing allows for log 
messages to be disabled at either compile time or runtime, but left in within
the source code in case they are needed in the future. In addition, all log
messages include the severity level and a timestamp.

The log library is implemented in C, but define guards are provided to ensure 
compatibility with C++ compilers.

All log messages have the format "[TIMESTAMP] SEVERITY: MESSAGE".

## Installation
LogLib is built and installed with the CMake utility (https://cmake.org).

For details on installing software with CMake, see the documentation at
https://cmake.org. On OS X or Linux, LogLib may be simply compiled by executing
the following commands.
```
git clone ssh://git@github.com/btjones16/loglib
cd loglib
mkdir build
cd build
cmake ..
make
```

The default build sequence simply compiles the library, but does not install it.
To install the library, modify the line `cmake ..` to read `cmake .. -DCMAKE_INSTALL_PREFIX=[PATH]`, where [PATH] is the name of the installation path. For instance, to install to `/usr/local`, use `cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local`. This creates an `install` target, so LogLib can be installed with `make install`.

## Testing
Unit testing relies on the CuTest (http://cutest.sourceforge.net/) library which is packaged with this source code. To compile and run the unit tests, execute the following 2 commands.

```
make test_log
./test_log
```
