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

#include <concepts>
#include <cstdint>
#include <source_location>
#include <string_view>
#include <vector>

namespace TestRunner {

enum class OutputMode : uint8_t { QUIET = 0, COMPACT, VERBOSE, TIMING };

enum class OnError : uint8_t { FAIL, CONTINUE };

struct Parameters {
  OutputMode output_mode{OutputMode::COMPACT};
  OnError on_error{OnError::FAIL};
  std::string_view test_name{};
};

};  // namespace TestRunner

namespace TestRunner::detail {

class Test {
 public:
  Test(std::string_view name, std::source_location location,
       bool expected_to_pass);
  virtual ~Test() = default;

  Test(const Test&)                     = delete;
  Test(Test&&)                          = delete;
  auto operator=(const Test&) -> Test&  = delete;
  auto operator=(const Test&&) -> Test& = delete;

  [[nodiscard]] auto run(OutputMode output, size_t max_name_length) const
      -> bool;

  [[nodiscard]] auto name() const -> std::string_view;

 protected:
  virtual void run_internal() const = 0;

 private:
  const std::string_view name_;
  const std::source_location location_;
  const bool expected_to_pass_;
};

class Runner {
 public:
  static void add(const Test* p_test);
  static auto run(const Parameters& parameters) -> size_t;
  static auto get() -> Runner&;

  [[nodiscard]] auto runTests(const Parameters& parameters) const -> size_t;

 private:
  Runner() = default;
  std::vector<const Test*> tests_;
};

}  // namespace TestRunner::detail

namespace TestRunner {

// ---------------------------------------------------------------------------
// Assertion functions (formerly macros)
// ---------------------------------------------------------------------------

inline void assert_true(
    bool condition, std::string_view message = {},
    std::source_location loc = std::source_location::current()) {
  if (!condition) {
    if (message.empty())
      throw std::source_location{loc};
    else
      throw message.data();
  }
}

inline void assert_false(
    bool condition, std::string_view message = {},
    std::source_location loc = std::source_location::current()) {
  if (condition) {
    if (message.empty())
      throw std::source_location{loc};
    else
      throw message.data();
  }
}

template <typename A, typename B>
  requires std::equality_comparable_with<A, B>
inline void expect_eq(
    A&& a, B&& b, std::string_view message = {},
    std::source_location loc = std::source_location::current()) {
  if (!(a == b)) {
    if (message.empty())
      throw std::source_location{loc};
    else
      throw message.data();
  }
}

template <typename A, typename B>
  requires std::equality_comparable_with<A, B>
inline void expect_ne(
    A&& a, B&& b, std::string_view message = {},
    std::source_location loc = std::source_location::current()) {
  if (a == b) {
    if (message.empty())
      throw std::source_location{loc};
    else
      throw message.data();
  }
}

template <std::floating_point T>
inline void expect_approx_eq(
    T a, T b, T epsilon = static_cast<T>(0.0001), std::string_view message = {},
    std::source_location loc = std::source_location::current()) {
  const T delta = (a > b) ? (a - b) : (b - a);
  if (delta > epsilon) {
    if (message.empty())
      throw std::source_location{loc};
    else
      throw message.data();
  }
}

[[noreturn]] inline void fail(const char* message) { throw message; }

// ---------------------------------------------------------------------------
// TEST / TEST_MUST_FAIL — these two must remain macros because they inject a
// struct definition and a static instance into the translation unit, which
// cannot be done with templates or functions.
// ---------------------------------------------------------------------------
// NOLINTBEGIN(cppcoreguidelines-macro-usage)

#define TEST(NAME)                                                \
  struct NAME : TestRunner::detail::Test {                        \
    NAME() : Test(#NAME, std::source_location::current(), true) { \
      TestRunner::detail::Runner::add(this);                      \
    }                                                             \
    void run_internal() const override;                           \
  };                                                              \
  namespace {                                                     \
  const NAME _instance_of_##NAME;                                 \
  }                                                               \
  void NAME::run_internal() const

#define TEST_MUST_FAIL(NAME)                                       \
  struct NAME : TestRunner::detail::Test {                         \
    NAME() : Test(#NAME, std::source_location::current(), false) { \
      TestRunner::detail::Runner::add(this);                       \
    }                                                              \
    void run_internal() const override;                            \
  };                                                               \
  namespace {                                                      \
  const NAME _instance_of_##NAME;                                  \
  }                                                                \
  void NAME::run_internal() const

// EXPECT_THROW must stay a macro: the statement `st` must be wrapped in a
// try/catch at the call site — it cannot be passed to a function without a
// lambda, which would silently change semantics for statements that capture
// local variables or use break/continue/return.
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

// NOLINTEND(cppcoreguidelines-macro-usage)

}  // namespace TestRunner

#endif  // TESTRUNNER_CORE_H
