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

#include <algorithm>
#include <chrono>
#include <cstddef>  // IWYU pragma: keep; For size_t
#include <format>
#include <iostream>
#include <span>
#include <string_view>

#include "testrunner/testrunner.h"

namespace TestRunner::detail {

using namespace std::literals;
constexpr auto GREEN = "\033[32m"sv;
constexpr auto RED   = "\033[31m"sv;
constexpr auto NOCOL = "\033[0m"sv;

Test::Test(std::string_view name, Location location, bool expected_to_pass)
    : name_(name), location_(location), expected_to_pass_(expected_to_pass) {}

[[nodiscard]] auto Test::run(OutputMode output,
                             size_t max_name_length) const -> bool {
  using clock                = std::chrono::high_resolution_clock;
  const auto test_started_at = clock::now();

  if (output >= OutputMode::VERBOSE)
    std::cout << name_ << std::string(max_name_length - name_.length(), ' ')
              << " | ";
  if (expected_to_pass_) {
    try {
      run_internal();
      if (output == OutputMode::TIMING) {
        const auto test_took =
            std::chrono::duration<double>(clock::now() - test_started_at);
        std::cout << std::format("{:7.4F}s | ", test_took.count());
      }
      if (output >= OutputMode::VERBOSE)
        std::cout << detail::GREEN << "PASS" << detail::NOCOL << "\n";
      return true;
    } catch (const char* err) {
      if (output < OutputMode::VERBOSE) std::cout << name_ << " ... ";
      std::cout << detail::RED << "FAIL" << detail::NOCOL << "\n";
      std::cerr << location_.file << ":" << location_.line << " " << err
                << "\n";
      return false;
    }

  } else {
    try {
      run_internal();
      if (output < OutputMode::VERBOSE) std::cout << name_ << " ... ";
      std::cout << detail::RED << "PASSED - It shouldn't have!" << detail::NOCOL
                << "\n";
      std::cerr << location_.file << ":" << location_.line << " test failed\n";
      return false;
    } catch (const char*) {
      if (output == OutputMode::TIMING) {
        const auto test_took =
            std::chrono::duration<double>(clock::now() - test_started_at);
        std::cout << std::format("{:7.4F}s | ", test_took.count());
      }
      if (output >= OutputMode::VERBOSE) {
        std::cout << detail::GREEN << "PASS (failed as expeced)"
                  << detail::NOCOL << "\n";
      }
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
      std::cout << std::format("Running {} test(s) ...\n", tests_.size());
    else
      std::cout << std::format("Running tests matching '{}' ...\n",
                               parameters.test_name);
    std::cout << std::string(72, '-') << "\n";
  }

  auto max_name_length = size_t{};
  for (const auto* test : tests_)
    max_name_length = std::max(max_name_length, test->name().length());

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

  size_t skipped = tests_.size() - passed - failed;
  if (!parameters.test_name.empty() and skipped == tests_.size()) {
    std::cerr << std::format("No test matching '{}' found.\n",
                             parameters.test_name);
    return 1;
  }

  if (parameters.output_mode >= OutputMode::VERBOSE)
    std::cout << std::string(72, '-') << "\n";

  if (parameters.output_mode != OutputMode::QUIET) {
    if (passed == tests_.size())
      std::cout << std::format("All done. {} test(s) passed.\n", passed);
  }

  if (failed != 0 or
      (skipped != 0 and parameters.output_mode != OutputMode::QUIET)) {
    std::cout << std::format("{} tests passed, {} failed", passed, failed);
    if (skipped != 0) std::cout << std::format(" ({} skipped)", skipped);
    std::cout << "\n";
  }

  if (parameters.output_mode == OutputMode::TIMING) {
    const auto test_took =
        std::chrono::duration<double>(tests_finished_at - tests_started_at);
    std::cout << std::format("Tests completed in {:0.4F}s.\n",
                             test_took.count());
  }

  return failed;
}

}  // namespace TestRunner::detail

void usage(std::string_view path) {
  std::cout << "Usage: " << path << " [-v] [-q] [-c] [-1 <test_name>] [-h]\n\n";

  std::cout << "  -1  Run single test case <test_name>\n";
  std::cout << "  -c  Continue after a test fails\n";
  std::cout << "  -v  Verbose output; lists all test results\n";
  std::cout << "  -t  Verbose output with timing for each test\n";
  std::cout << "  -q  Quiet mode; only reports failures\n\n";
  std::cout << "      Default output mode is 'compact', which reports test\n";
  std::cout << "      statistics. Use -q for less detail and -v for more.\n\n";
}

auto main(int argc, char* argv[]) -> int {
  const auto args = std::span(argv, static_cast<size_t>(argc));
  auto parameters = TestRunner::Parameters{};

  for (auto arg_it = std::next(args.begin()); arg_it < args.end(); ++arg_it) {
    const auto flag = std::string_view{*arg_it};
    if (flag == "-v") {
      parameters.output_mode = TestRunner::OutputMode::VERBOSE;

    } else if (flag == "-t") {
      parameters.output_mode = TestRunner::OutputMode::TIMING;

    } else if (flag == "-q") {
      parameters.output_mode = TestRunner::OutputMode::QUIET;

    } else if (flag == "-c") {
      parameters.on_error = TestRunner::OnError::CONTINUE;

    } else if (flag == "-1") {
      if (++arg_it == args.end()) {
        std::cerr << "Must specify test name for '-1' flag.\n\n";
        usage(args[0]);
        return 1;
      }
      parameters.test_name = std::string_view{*arg_it};

    } else {
      usage(args[0]);
      return 2;
    }
  }

  return (TestRunner::detail::Runner::run(parameters) == 0 ? 0 : 1);
}
