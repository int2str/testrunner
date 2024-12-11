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

#include <cstddef>  // For size_t
#include <iostream>
#include <span>
#include <string_view>

#include "testrunner/testrunner.h"

void usage(std::string_view path) {
  std::cout << "Usage: " << path << " [-v] [-q] [-c] [-1 <test_name>] [-h]\n\n";

  std::cout << "  -1  Run single test case <test_name>\n";
  std::cout << "  -c  Continue after a test fails\n";
  std::cout << "  -v  Verbose output; lists all test results\n";
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
