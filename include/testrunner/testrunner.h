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

enum class OutputMode { VERBOSE, COMPACT, QUIET };

enum class OnError { FAIL, CONTINUE };

struct Parameters {
  OutputMode output_mode{OutputMode::COMPACT};
  OnError on_error{OnError::FAIL};
  std::string_view test_name{};
};

};  // namespace TestRunner

namespace TestRunner::detail {

using namespace std::literals;
constexpr auto GREEN = "\033[32m"sv;
constexpr auto RED   = "\033[31m"sv;
constexpr auto NOCOL = "\033[0m"sv;

class Test {
 public:
  struct Location {
    std::string_view file;
    size_t line;
  };

  Test(std::string_view name, Location location, bool expected_to_pass)
      : name_(name), location_(location), expected_to_pass_(expected_to_pass) {}
  virtual ~Test() = default;

  [[nodiscard]] auto run(OutputMode output) const -> bool {
    if (output == OutputMode::VERBOSE) std::cout << name_ << " ... ";
    if (expected_to_pass_) {
      try {
        run_internal();
        if (output == OutputMode::VERBOSE)
          std::cout << detail::GREEN << "PASS" << detail::NOCOL << "\n";
        return true;
      } catch (const char* err) {
        if (output != OutputMode::VERBOSE) std::cout << name_ << " ... ";
        std::cout << detail::RED << "FAIL" << detail::NOCOL << "\n";
        std::cerr << location_.file << ":" << location_.line << " " << err
                  << "\n";
        return false;
      }

    } else {
      try {
        run_internal();
        if (output != OutputMode::VERBOSE) std::cout << name_ << " ... ";
        std::cout << detail::RED << "PASSED - It shouldn't have!"
                  << detail::NOCOL << "\n";
        std::cerr << location_.file << ":" << location_.line
                  << " test failed\n";
        return false;
      } catch (const char*) {
        if (output == OutputMode::VERBOSE) {
          std::cout << detail::GREEN << "PASS (failed as expeced)"
                    << detail::NOCOL << "\n";
        }
        return true;
      }
    }
  }

  [[nodiscard]] auto name() const -> std::string_view { return name_; }

  Test(const Test&)                     = delete;
  Test(Test&&)                          = delete;
  auto operator=(const Test&) -> Test&  = delete;
  auto operator=(const Test&&) -> Test& = delete;

 protected:
  virtual void run_internal() const = 0;

 private:
  const std::string_view name_;
  const Location location_;
  const bool expected_to_pass_;
};

class Runner {
 public:
  static void add(const Test* p_test) { get().tests_.push_back(p_test); }
  static auto run(const Parameters& parameters) -> size_t {
    return get().runTests(parameters);
  }

 private:
  static auto get() -> Runner& {
    static Runner test_runner;
    return test_runner;
  }

  [[nodiscard]] auto runTests(const Parameters& parameters) const -> size_t {
    size_t passed = 0;
    size_t failed = 0;

    if (parameters.output_mode == OutputMode::VERBOSE) {
      if (parameters.test_name.empty())
        std::cout << "Running " << tests_.size() << " test(s) ...\n";
      else
        std::cout << "Running tests matching '" << parameters.test_name
                  << "' ...\n";
      std::cout << "----------------------------------------\n";
    }

    for (const auto* test : tests_) {
      if (parameters.test_name.empty() or
          test->name().starts_with(parameters.test_name)) {
        if (test->run(parameters.output_mode)) {
          ++passed;
        } else {
          ++failed;
          if (parameters.on_error == OnError::FAIL) break;
        }
      }
    }

    size_t skipped = tests_.size() - passed - failed;
    if (!parameters.test_name.empty() and skipped == tests_.size()) {
      std::cerr << "No test matching '" << parameters.test_name << "' found.\n";
      return 1;
    }

    if (parameters.output_mode == OutputMode::VERBOSE)
      std::cout << "----------------------------------------\n";

    if (parameters.output_mode != OutputMode::QUIET) {
      if (passed == tests_.size())
        std::cout << "All done. " << passed << " test(s) passed.\n";
    }

    if (failed != 0 or
        (skipped != 0 and parameters.output_mode != OutputMode::QUIET)) {
      std::cout << passed << " test(s) passed, " << failed << " failed";
      if (skipped != 0) std::cout << " (" << skipped << " skipped)";
      std::cout << "\n";
    }

    return failed;
  }

  Runner() = default;
  std::vector<const Test*> tests_;
};

}  // namespace TestRunner::detail

namespace TestRunner {

// In the future, it would be nice to figure out a way to do this wihout macros.
// That future isn't now ...
// NOLINTBEGIN(cppcoreguidelines-macro-usage)

#define TEST(NAME)                                     \
  struct NAME : TestRunner::detail::Test {             \
    NAME() : Test(#NAME, {__FILE__, __LINE__}, true) { \
      TestRunner::detail::Runner::add(this);           \
    }                                                  \
    void run_internal() const override;                \
  };                                                   \
  namespace {                                          \
  const NAME _instance_of_##NAME;                      \
  }                                                    \
  void NAME::run_internal() const

#define TEST_MUST_FAIL(NAME)                            \
  struct NAME : TestRunner::detail::Test {              \
    NAME() : Test(#NAME, {__FILE__, __LINE__}, false) { \
      TestRunner::detail::Runner::add(this);            \
    }                                                   \
    void run_internal() const override;                 \
  };                                                    \
  namespace {                                           \
  const NAME _instance_of_##NAME;                       \
  }                                                     \
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
    float delta         = ((a) - (b));                             \
    if (delta < 0) delta *= -1;                                    \
    if (delta > EPSILON)                                           \
      throw("EXPECT_FLOAT_IS_APPROX " #a " -> " #b " failed ..."); \
  }

#define EXPECT_DOUBLE_IS_APPROX(a, b)                               \
  {                                                                 \
    const double EPSILON = 0.0001;                                  \
    double delta         = ((a) - (b));                             \
    if (delta < 0) delta *= -1;                                     \
    if (delta > EPSILON)                                            \
      throw("EXPECT_DOUBLE_IS_APPROX " #a " -> " #b " failed ..."); \
  }

#define FAIL(message) \
  { throw(message); }

// NOLINTEND(cppcoreguidelines-macro-usage)

}  // namespace TestRunner

#endif  // TESTRUNNER_CORE_H
