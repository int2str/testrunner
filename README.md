# TestRunner
A simple C++ unit test framework.
Don't use this framework in your project. There are much better frameworks available. For example, Google's googletest (https://github.com/google/googletest).

# Why?
This framework was created mainly for research purposes to see how it could be done. It was also created to test AVR code with minimal external dependencies.

# How to use
Run "ninja" to create a local test binary to run a self test.
Otherwise, build the code as a library and create your own unit tests by including "testrunner_core.h" in your unit test files and include "testrunner_main.h" in one compilation unit to an executable test binary. Optionally include "testrunner_selftest.h" to ensure testrunner isn't broken.