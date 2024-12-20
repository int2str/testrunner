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

#include <cstdint>
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
  struct Location {
    std::string_view file;
    size_t line;
  };

  Test(std::string_view name, Location location, bool expected_to_pass);
  virtual ~Test() = default;

  Test(const Test&)                     = delete;
  Test(Test&&)                          = delete;
  auto operator=(const Test&) -> Test&  = delete;
  auto operator=(const Test&&) -> Test& = delete;

  [[nodiscard]] auto run(OutputMode output,
                         size_t max_name_length) const -> bool;

  [[nodiscard]] auto name() const -> std::string_view;

 protected:
  virtual void run_internal() const = 0;

 private:
  const std::string_view name_;
  const Location location_;
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
