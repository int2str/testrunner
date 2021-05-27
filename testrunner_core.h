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
#include <string_view>
#include <vector>

namespace TestRunner {

class Test {
 public:
  Test(const std::string_view &name, const std::string_view &file, size_t line,
       bool expected_to_pass = true)
      : name_(name),
        file_(file),
        line_(line),
        expected_to_pass_(expected_to_pass) {}
  virtual ~Test(){};

  void run() const {
    std::cout << getName() << " ... ";
    try {
      run_internal();

      if (isExpectedToPass()) {
        std::cout << "\033[32mPASS\033[0m\n";
      } else {
        std::cout << "\033[31mPASSED - It shouldn't have!\033[0m\n";
        std::cerr << getFile() << ":" << getLine() << " test failed\n";
        throw("Test should have failed ...");
      }

    } catch (const char *err) {
      if (isExpectedToPass()) {
        std::cout << "\033[31mFAIL\033[0m\n";
        std::cerr << getFile() << ":" << getLine() << " " << err << "\n";
        throw(err);
      } else {
        std::cout << "\033[32mPASS (failed as expeced)\033[0m\n";
      }
    }
  }

  constexpr std::string_view getName() const { return name_; }
  constexpr std::string_view getFile() const { return file_; }
  constexpr size_t getLine() const { return line_; }
  constexpr bool isExpectedToPass() const { return expected_to_pass_; }

 protected:
  virtual void run_internal() const = 0;

  const std::string_view name_;
  const std::string_view file_;
  const size_t line_;
  const bool expected_to_pass_;
};

class Runner {
 public:
  static Runner &get() {
    static Runner testRunner;
    return testRunner;
  }

  void registerTest(const Test *p_test) { tests_.push_back(p_test); }

  size_t runAllTests() {
    size_t passed = 0;
    std::cout << "Running " << tests_.size() << " test(s) ...\n";
    std::cout << "----------------------------------------\n";

    try {
      for (const auto test : tests_) {
        test->run();
        ++passed;
      }
    } catch (...) {
    }

    std::cout << "----------------------------------------\n";
    if (passed == tests_.size()) {
      std::cout << "All done. " << passed << " test(s) passed.\n";
    } else {
      std::cout << static_cast<int>(passed) << " test(s) passed, "
                << static_cast<int>(tests_.size() - passed) << " failed.\n";
    }

    return tests_.size() - passed;
  }

 private:
  Runner() {}
  std::vector<const Test *> tests_;
};

#define TEST(NAME)                                   \
  struct NAME : TestRunner::Test {                   \
    NAME() : Test(#NAME, __FILE__, __LINE__, true) { \
      TestRunner::Runner::get().registerTest(this);  \
    }                                                \
    void run_internal() const override;              \
  };                                                 \
  static NAME _instance_of_##NAME;                   \
  void NAME::run_internal() const

#define TEST_MUST_FAIL(NAME)                          \
  struct NAME : TestRunner::Test {                    \
    NAME() : Test(#NAME, __FILE__, __LINE__, false) { \
      TestRunner::Runner::get().registerTest(this);   \
    }                                                 \
    void run_internal() const override;               \
  };                                                  \
  static NAME _instance_of_##NAME;                    \
  void NAME::run_internal() const

#define ASSERT_TRUE(t)                             \
  do {                                             \
    if (!(t)) throw("ASSERT_TRUE(" #t ") failed"); \
  } while (0)

#define ASSERT_FALSE(t)                          \
  do {                                           \
    if (t) throw("ASSERT_FALSE(" #t ") failed"); \
  } while (0)

#define EXPECT_EQ(a, b)                                                        \
  do {                                                                         \
    if (!((a) == (b))) throw("EXPECT_EQ expected " #a " to equal " #b " ..."); \
  } while (0)

#define EXPECT_NE(a, b)                                               \
  do {                                                                \
    if (((a) == (b)))                                                 \
      throw("EXPECT_NE expected " #a " to be unequal to " #b " ..."); \
  } while (0)

#define FAIL(message) \
  do {                \
    throw(message);   \
  } while (0)

}  // namespace TestRunner

#endif  // TESTRUNNER_CORE_H