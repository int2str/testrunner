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

#include "testrunner/testrunner.h"

using namespace TestRunner;

TEST(AssertTrue) { assert_true(true); }
TEST_MUST_FAIL(AssertTrueFail) { assert_true(false); }
TEST_MUST_FAIL(AssertTrueFailWithMessage) {
  assert_true(false, "custom message");
}

TEST(AssertFalse) { assert_false(false); }
TEST_MUST_FAIL(AssertFalseFail) { assert_false(true); }

TEST(ExpectEq) { expect_eq(42, 42); }
TEST_MUST_FAIL(ExpectEqFail) { expect_eq('A', 'B'); }

TEST(ExpectNe) { expect_ne(41, 42); }
TEST_MUST_FAIL(ExpectNeFail) { expect_ne('A', 'A'); }

TEST(ExpectThrow) { EXPECT_THROW(throw true); }
TEST_MUST_FAIL(ExpectThrowFail) { EXPECT_THROW(); }

TEST(ExpectApproxEqFloat) { expect_approx_eq(1.0F, 1.00005F); }
TEST_MUST_FAIL(ExpectApproxEqFloatFail) { expect_approx_eq(1.0F, 2.0F); }

TEST(ExpectApproxEqDouble) { expect_approx_eq(1.0, 1.00005); }
TEST_MUST_FAIL(ExpectApproxEqDoubleFail) { expect_approx_eq(1.0, 2.0); }

TEST_MUST_FAIL(ForcedFail) { fail("Forced fail"); }
