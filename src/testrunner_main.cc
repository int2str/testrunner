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
  std::cout << "Usage: " << path << " [-v] [-q] [-h]\n\n";

  std::cout << "  -v  Verbose output; lists all test results\n";
  std::cout << "  -q  Quiet mode; only reports failures\n\n";
  std::cout << "      Default output mode is 'compact', which reports test\n";
  std::cout << "      statistics. Use -q for less detail and -v for more.\n\n";
}

auto main(int argc, char* argv[]) -> int {
  const auto args = std::span(argv, static_cast<size_t>(argc));
  auto output     = TestRunner::detail::Output::COMPACT;

  if (argc > 2) {
    usage(args[0]);
    return 1;
  }

  if (argc == 2) {
    const auto flag = std::string_view(args[1]);
    if (flag == "-v") {
      output = TestRunner::detail::Output::VERBOSE;

    } else if (flag == "-q") {
      output = TestRunner::detail::Output::QUIET;

    } else {
      usage(args[0]);
      return 2;
    }
  }

  return (TestRunner::detail::Runner::run(output) == 0 ? 0 : 1);
}
