#include <gtest/gtest.h>
#include "StringUtils.hpp"

// Basic exact matches
TEST(StringUtilsTest, ExactMatch) {
    EXPECT_TRUE(StringUtils::match("abc", "abc"));
    EXPECT_FALSE(StringUtils::match("abc", "abd"));
    EXPECT_FALSE(StringUtils::match("abc", ""));
    EXPECT_FALSE(StringUtils::match("", "abc"));
}

// Empty strings
TEST(StringUtilsTest, EmptyStrings) {
    EXPECT_TRUE(StringUtils::match("", "")); // Both empty
    EXPECT_FALSE(StringUtils::match("", "*a")); // Empty vs pattern with *
    EXPECT_TRUE(StringUtils::match("", "*"));   // Pattern '*' can match empty string
}

// Wildcard matches
TEST(StringUtilsTest, WildcardMatch) {
    EXPECT_TRUE(StringUtils::match("abc", "a*c"));
    EXPECT_TRUE(StringUtils::match("abc", "*bc"));
    EXPECT_TRUE(StringUtils::match("abc", "ab*"));
    EXPECT_TRUE(StringUtils::match("abc", "*b*"));
    EXPECT_FALSE(StringUtils::match("abc", "a*d"));
    EXPECT_TRUE(StringUtils::match("hello", "h*o"));
    EXPECT_TRUE(StringUtils::match("hello", "*lo"));
    EXPECT_TRUE(StringUtils::match("hello", "he*"));
    EXPECT_FALSE(StringUtils::match("hello", "he*c"));
}

// Complex patterns
TEST(StringUtilsTest, ComplexPatterns) {
    EXPECT_TRUE(StringUtils::match("abcd", "a*cd"));
    EXPECT_TRUE(StringUtils::match("abxyzcd", "a*cd"));
    EXPECT_FALSE(StringUtils::match("abxyz", "a*cd"));
    EXPECT_TRUE(StringUtils::match("aaaaaaaaaa", "a*a"));
    EXPECT_FALSE(StringUtils::match("abc", "*d*"));
    EXPECT_TRUE(StringUtils::match("abbbbcc", "a*c"));
    EXPECT_TRUE(StringUtils::match("abbbbcc", "*b*c"));
    EXPECT_FALSE(StringUtils::match("abbbbcc", "*d*c"));
}

// If you have a separate test main file, you don't need main here.
// If you want this file to be self-contained, you can add a main:
// int main(int argc, char **argv) {
//     ::testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }
