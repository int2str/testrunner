# TestRunner
A simple C++ unit test framework.
Don't use this framework in your project. There are much better frameworks available. For example, Google's googletest (https://github.com/google/googletest).

# Why?
This framework was created mainly for research purposes to see how it could be done. It was also created to test AVR code with minimal external dependencies.

# How to use
Run `bazel test //:*` to create a local test binary to run a self test.
Otherwise, build the code as a library and create your own unit tests by including "testrunner/testrunner.h" in your unit test files and include "@testrunner//:main" as a dependency for your test binary.

# Conan
Create the local Conan package with:

    conan create . -s compiler.cppstd=23

Consumer CMake projects can then use:

    find_package(testrunner REQUIRED)

    add_executable(my_tests my_tests.cc)
    target_link_libraries(my_tests PRIVATE testrunner::main)

Use `testrunner::main` when you want TestRunner's default test executable entry point. Use `testrunner::testrunner` instead if your test binary provides its own `main()`.

TestRunner requires a C++23 compiler and standard library.

# Simplest possible test

your_first_test.cc:

    #include <testrunner/testrunner.h>

    TEST(ThisTestNeverFails) {
      TestRunner::assert_true(true);
    }

That's it :)
