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

#include "testrunner_core.h"

TEST(AssertTrue) { ASSERT_TRUE(true); }
TEST_MUST_FAIL(AssertTrueFail) { ASSERT_TRUE(false); }

TEST(AssertFalse) { ASSERT_FALSE(false); }
TEST_MUST_FAIL(AssertFalseFail) { ASSERT_FALSE(true); }

TEST(ExpectEq) { EXPECT_EQ(42, 42); }
TEST_MUST_FAIL(ExpectEqFail) { EXPECT_EQ('A', 'B'); }

TEST(ExpectNe) { EXPECT_NE(41, 42); }
TEST_MUST_FAIL(ExpectNeFail) { EXPECT_NE('A', 'A'); }

TEST(ExpectThrow) { EXPECT_THROW(throw true); }
TEST_MUST_FAIL(ExpectThrowFail) { EXPECT_THROW(); }

TEST_MUST_FAIL(ForcedFail) { FAIL("Forced fail"); }
