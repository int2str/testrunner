// TestRunner - A simple C++ unit test framework
// Copyright (C) 2020-2024 Andre Eisenbach
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

#include "testrunner/testrunner.h"

#include <algorithm>
#include <chrono>
#include <cstddef>  // IWYU pragma: keep; For size_t
#include <format>
#include <iostream>
#include <print>
#include <ranges>
#include <source_location>
#include <string_view>

namespace TestRunner::detail {

using namespace std::literals;
constexpr auto GREEN = "\033[32m"sv;
constexpr auto RED   = "\033[31m"sv;
constexpr auto NOCOL = "\033[0m"sv;

Test::Test(std::string_view name, std::source_location location,
           bool expected_to_pass)
    : name_(name), location_(location), expected_to_pass_(expected_to_pass) {}

[[nodiscard]] auto Test::run(OutputMode output, size_t max_name_length) const
    -> bool {
  using clock                = std::chrono::high_resolution_clock;
  const auto test_started_at = clock::now();

  const auto print_timing = [&] {
    if (output == OutputMode::TIMING) {
      const auto elapsed =
          std::chrono::duration<double>(clock::now() - test_started_at);
      std::print("{:7.4F}s | ", elapsed.count());
    }
  };

  const auto print_name_if_not_verbose = [&] {
    if (output < OutputMode::VERBOSE) std::print("{} ... ", name_);
  };

  if (output >= OutputMode::VERBOSE)
    std::print("{}{} | ", name_,
               std::string(max_name_length - name_.length(), ' '));

  if (expected_to_pass_) {
    try {
      run_internal();
      print_timing();
      if (output >= OutputMode::VERBOSE) std::println("{}PASS{}", GREEN, NOCOL);
      return true;
    } catch (const char* err) {
      print_name_if_not_verbose();
      std::println("{}FAIL{}", RED, NOCOL);
      std::cerr << std::format("{}:{}:{} assertion failed: {}\n",
                               location_.file_name(), location_.line(),
                               location_.column(), err);
      return false;
    } catch (const std::source_location& loc) {
      print_name_if_not_verbose();
      std::println("{}FAIL{}", RED, NOCOL);
      std::cerr << std::format("{}:{}: assertion failed at {}:{}\n",
                               location_.file_name(), location_.line(),
                               loc.file_name(), loc.line());
      return false;
    }

  } else {
    try {
      run_internal();
      print_name_if_not_verbose();
      std::println("{}PASSED - It shouldn't have!{}", RED, NOCOL);
      std::cerr << std::format("{}:{}: test failed\n", location_.file_name(),
                               location_.line());
      return false;
    } catch (const char*) {
      print_timing();
      if (output >= OutputMode::VERBOSE)
        std::println("{}PASS (failed as expected){}", GREEN, NOCOL);
      return true;
    } catch (const std::source_location&) {
      print_timing();
      if (output >= OutputMode::VERBOSE)
        std::println("{}PASS (failed as expected){}", GREEN, NOCOL);
      return true;
    }
  }
}

[[nodiscard]] auto Test::name() const -> std::string_view { return name_; }

void Runner::add(const Test* p_test) { get().tests_.push_back(p_test); }
auto Runner::run(const Parameters& parameters) -> size_t {
  return get().runTests(parameters);
}

auto Runner::get() -> Runner& {
  static Runner test_runner;
  return test_runner;
}

[[nodiscard]] auto Runner::runTests(const Parameters& parameters) const
    -> size_t {
  auto passed = size_t{};
  auto failed = size_t{};

  if (parameters.output_mode >= OutputMode::VERBOSE) {
    if (parameters.test_name.empty())
      std::println("Running {} test(s) ...", tests_.size());
    else
      std::println("Running tests matching '{}' ...", parameters.test_name);
    std::println("{}", std::string(72, '-'));
  }

  const auto max_name_length = [&] -> size_t {
    auto it = std::ranges::max_element(
        tests_, {}, [](const Test* t) { return t->name().length(); });
    return (it != tests_.end()) ? (*it)->name().length() : size_t{};
  }();

  using clock                 = std::chrono::high_resolution_clock;
  const auto tests_started_at = clock::now();

  for (const auto* test : tests_) {
    if (parameters.test_name.empty() or
        test->name().starts_with(parameters.test_name)) {
      if (test->run(parameters.output_mode, max_name_length)) {
        ++passed;
      } else {
        ++failed;
        if (parameters.on_error == OnError::FAIL) break;
      }
    }
  }

  const auto tests_finished_at = clock::now();

  const size_t skipped = tests_.size() - passed - failed;
  if (!parameters.test_name.empty() and skipped == tests_.size()) {
    std::println(stderr, "No test matching '{}' found.", parameters.test_name);
    return 1;
  }

  if (parameters.output_mode >= OutputMode::VERBOSE)
    std::println("{}", std::string(72, '-'));

  if (parameters.output_mode != OutputMode::QUIET) {
    if (passed == tests_.size())
      std::println("All done. {} test(s) passed.", passed);
  }

  if (failed != 0 or
      (skipped != 0 and parameters.output_mode != OutputMode::QUIET)) {
    std::print("{} tests passed, {} failed", passed, failed);
    if (skipped != 0) std::print(" ({} skipped)", skipped);
    std::println("");
  }

  if (parameters.output_mode == OutputMode::TIMING) {
    const auto elapsed =
        std::chrono::duration<double>(tests_finished_at - tests_started_at);
    std::println("Tests completed in {:0.4F}s.", elapsed.count());
  }

  return failed;
}

}  // namespace TestRunner::detail
