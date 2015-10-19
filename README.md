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

Note: CMake will most likely display an error that it cannot find the cmockery
library at present. This is a known issue that will hopefully be resolved in a
future release. It does not impact the functionality of the library.

The default build sequence simply compiles the library, but does not install it.
To install the library, modify the line `cmake ..` to read `cmake .. -DCMAKE_INSTALL_PREFIX=[PATH]`, where [PATH] is the name of the installation path. For instance, to install to `/usr/local`, use `cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local`. This creates an `install` target, so LogLib can be installed with `make install`.

## Testing
Unit testing is currently provided for LogLib using the cmockery library (https://code.google.com/p/cmockery/). This is likely to result in a warning from CMake that it cannot find the cmockery framework. You will likely need to modify the makefile generated for the test code, or follow the instructions in the CMake warning to get the unit testing to work. Once the cmockery framework is installed and has been identified, the unit tests can be compiled and executed with the following commands. The unit testing is an optional, but not required, aspect of this code and the log library will work fine without it.

```
make test_log
make test
```
