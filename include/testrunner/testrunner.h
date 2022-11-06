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

namespace {
constexpr const char *GREEN = "\033[32m";
constexpr const char *RED = "\033[31m";
constexpr const char *NOCOL = "\033[0m";
}  // namespace

namespace TestRunner {

class Test {
 public:
  Test(std::string_view name, std::string_view file, size_t line,
       bool expected_to_pass)
      : name_(name),
        file_(file),
        line_(line),
        expected_to_pass_(expected_to_pass) {}
  virtual ~Test() = default;

  void run() const {
    std::cout << name_ << " ... ";

    if (expected_to_pass_) {
      try {
        run_internal();
        std::cout << GREEN << "PASS" << NOCOL << "\n";
      } catch (const char *err) {
        std::cout << RED << "FAIL" << NOCOL << "\n";
        std::cerr << file_ << ":" << line_ << " " << err << "\n";
        throw(err);
      }

    } else {
      try {
        run_internal();
        std::cout << RED << "PASSED - It shouldn't have!" << NOCOL << "\n";
        std::cerr << file_ << ":" << line_ << " test failed\n";
      } catch (const char *) {
        std::cout << GREEN << "PASS (failed as expeced)" << NOCOL << "\n";
        return;
      }
      throw("Test should have failed ...");
    }
  }

 protected:
  virtual void run_internal() const = 0;

  const std::string_view name_;
  const std::string_view file_;
  const size_t line_;
  const bool expected_to_pass_;
};

class Runner {
 public:
  static void add(const Test *p_test) { get().tests_.push_back(p_test); }
  static size_t run() { return get().runAllTests(); }

 private:
  static Runner &get() {
    static Runner testRunner;
    return testRunner;
  }

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
      std::cout << passed << " test(s) passed, " << (tests_.size() - passed)
                << " failed.\n";
    }

    return tests_.size() - passed;
  }

  Runner() = default;
  std::vector<const Test *> tests_;
};

#define TEST(NAME)                                   \
  struct NAME : TestRunner::Test {                   \
    NAME() : Test(#NAME, __FILE__, __LINE__, true) { \
      TestRunner::Runner::add(this);                 \
    }                                                \
    void run_internal() const override;              \
  };                                                 \
  static NAME _instance_of_##NAME;                   \
  void NAME::run_internal() const

#define TEST_MUST_FAIL(NAME)                          \
  struct NAME : TestRunner::Test {                    \
    NAME() : Test(#NAME, __FILE__, __LINE__, false) { \
      TestRunner::Runner::add(this);                  \
    }                                                 \
    void run_internal() const override;               \
  };                                                  \
  static NAME _instance_of_##NAME;                    \
  void NAME::run_internal() const

#define ASSERT_TRUE(t)                             \
  {                                                \
    if (!(t)) throw("ASSERT_TRUE(" #t ") failed"); \
  }

#define ASSERT_FALSE(t)                          \
  {                                              \
    if (t) throw("ASSERT_FALSE(" #t ") failed"); \
  }

#define EXPECT_EQ(a, b)                                                        \
  {                                                                            \
    if (!((a) == (b))) throw("EXPECT_EQ expected " #a " to equal " #b " ..."); \
  }

#define EXPECT_NE(a, b)                                               \
  {                                                                   \
    if (((a) == (b)))                                                 \
      throw("EXPECT_NE expected " #a " to be unequal to " #b " ..."); \
  }

#define EXPECT_THROW(st)                                          \
  {                                                               \
    bool did_throw = false;                                       \
    try {                                                         \
      st;                                                         \
    } catch (...) {                                               \
      did_throw = true;                                           \
    }                                                             \
    if (!did_throw) throw "EXPECT_THROW statement did not throw"; \
  }

#define EXPECT_FLOAT_IS_APPROX(a, b)                               \
  {                                                                \
    const float EPSILON = 0.0001;                                  \
    float delta = ((a) - (b));                                     \
    if (delta < 0) delta *= -1;                                    \
    if (delta > EPSILON)                                           \
      throw("EXPECT_FLOAT_IS_APPROX " #a " -> " #b " failed ..."); \
  }

#define FAIL(message) \
  { throw(message); }

}  // namespace TestRunner

#endif  // TESTRUNNER_CORE_H
