// TestRunner - A simple C++ unit test framework
// Copyright (C) 2020 Andre Eisenbach
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// See LICENSE for a copy of the GNU General Public License or see
// it online at <http://www.gnu.org/licenses/>.

#ifndef TESTRUNNER_CORE_H
#define TESTRUNNER_CORE_H

#include <iostream>
#include <string>
#include <string_view>
#include <vector>

class Test {
public:
  Test(const std::string &name, const std::string &file, size_t line,
       bool expected_to_pass = true)
      : name_(name), file_(file), line_(line),
        expected_to_pass_(expected_to_pass) {}
  virtual ~Test(){};

  virtual void run() = 0;

  constexpr std::string_view getName() const { return name_; }
  constexpr std::string_view getFile() const { return file_; }
  constexpr size_t getLine() const { return line_; }
  constexpr bool isExpectedToPass() const { return expected_to_pass_; }

protected:
  const std::string name_;
  const std::string file_;
  const size_t line_;
  const bool expected_to_pass_;
};

class TestRunner {
public:
  static TestRunner &get() {
    static TestRunner testRunner;
    return testRunner;
  }

  void registerTest(Test *p_test) { tests_.push_back(p_test); }

  void runAllTests() {
    size_t tests = 0;
    size_t passed = 0;
    std::cout << "Running " << tests_.size() << " test(s) ...\n";
    std::cout << "----------------------------------------\n";

    try {
      for (auto test : tests_) {
        ++tests;
        invoke(test);
        ++passed;
      }
    } catch (...) {
    }

    std::cout << "----------------------------------------\n";
    if (passed == tests) {
      std::cout << "All done. " << passed << " test(s) passed.\n";
    } else {
      std::cout << (int)passed << " test(s) passed, " << (int)(tests - passed)
                << " failed.\n";
    }
  }

private:
  std::vector<Test *> tests_;

  void invoke(Test *test) {
    std::cout << test->getName() << " ... ";
    try {
      test->run();
      if (test->isExpectedToPass()) {
        std::cout << "\033[32mPASS\033[0m\n";
      } else {
        std::cout << "\033[31mPASSED - It shouldn't have!\033[0m\n";
        std::cerr << test->getFile() << ":" << test->getLine()
                  << " test failed\n";
        throw(1);
      }
    } catch (const char *err) {
      if (test->isExpectedToPass()) {
        std::cout << "\033[31mFAIL\033[0m\n";
        std::cerr << test->getFile() << ":" << test->getLine() << " " << err
                  << "\n";
        throw(2);
      } else {
        std::cout << "\033[32mPASS (failed as expeced)\033[0m\n";
      }
    }
  }
};

#define TEST(NAME)                                                             \
  struct NAME : Test {                                                         \
    NAME() : Test(#NAME, __FILE__, __LINE__, true) {                           \
      TestRunner::get().registerTest(this);                                    \
    }                                                                          \
    void run() override;                                                       \
  };                                                                           \
  static NAME _instance_of_##NAME;                                             \
  void NAME::run()

#define TEST_MUST_FAIL(NAME)                                                   \
  struct NAME : Test {                                                         \
    NAME() : Test(#NAME, __FILE__, __LINE__, false) {                          \
      TestRunner::get().registerTest(this);                                    \
    }                                                                          \
    void run() override;                                                       \
  };                                                                           \
  static NAME _instance_of_##NAME;                                             \
  void NAME::run()

#define ASSERT_TRUE(t)                                                         \
  do {                                                                         \
    if (!(t))                                                                  \
      throw("ASSERT_TRUE(" #t ") failed");                                     \
  } while (0);

#define ASSERT_FALSE(t)                                                        \
  do {                                                                         \
    if (t)                                                                     \
      throw("ASSERT_FALSE(" #t ") failed");                                    \
  } while (0);

#define EXPECT_EQ(a, b)                                                        \
  do {                                                                         \
    if (!((a) == (b)))                                                         \
      throw("EXPECT_EQ expected " #a " to equal " #b " ...");                  \
  } while (0);

#define FAIL(message)                                                          \
  do {                                                                         \
    throw(message);                                                            \
  } while (0);

#endif  // TESTRUNNER_CORE_H