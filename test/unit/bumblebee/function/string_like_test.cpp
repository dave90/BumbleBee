/*
 * Copyright (C) 2025 Davide Fuscà
 *
 * This file is part of BumbleBee.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <gtest/gtest.h>

#include "bumblebee/function/predicate/StringLike.hpp"
#include "bumblebee/function/PredFunction.hpp"
#include "bumblebee/planner/filter/TableFilter.hpp"
#include "../BumbleBaseTest.hpp"

namespace bumblebee {

// ============================================================
// Test fixture
// ============================================================

class StringLikeTest : public BumbleBaseTest {
protected:
    ClientContext context;

    // ----- string_t helpers -----

    // Creates a BumbleString from a std::string.
    // For strings longer than BumbleString::PREFIX_LENGTH (11) the caller must
    // keep 'source' alive for the lifetime of the returned string_t.
    static string_t makeStr(const string& source) {
        return string_t(source.c_str(), static_cast<uint32_t>(source.size()));
    }

    // ----- DataChunk helpers (flat, single STRING column) -----

    // Builds a single-column DataChunk holding 'rows' as STRING values.
    DataChunk makeChunkWithStrings(const vector<string>& rows) {
        vector<LogicalType> types = {LogicalTypeId::STRING};
        vector<vector<Value>> data(1);
        for (const auto& s : rows)
            data[0].push_back(Value(s));
        return generateDataChunk(types, data);
    }

    // Extracts all string values from the single STRING column of a DataChunk.
    vector<string> extractStrings(DataChunk& chunk) {
        vector<string> result;
        result.reserve(chunk.getSize());
        for (idx_t row = 0; row < chunk.getSize(); row++) {
            Value v = chunk.getValue(0, row);
            string s = v.toString();
            if (v.isDoubleQuotedString())
                s = s.substr(1, s.size() - 2);
            result.push_back(std::move(s));
        }
        return result;
    }

    // ----- DataChunk helpers (constant vector) -----

    // Builds a single-column DataChunk whose STRING column is a CONSTANT_VECTOR
    // holding 'value'. The chunk has 'count' logical rows, all equal to that value,
    // exercising the selectConstant path in UnaryExecution::select.
    DataChunk makeConstantStringChunk(const string& value, idx_t count) {
        DataChunk chunk;
        chunk.initializeEmpty(vector<LogicalType>{LogicalTypeId::STRING});
        // Use brace-init to avoid the most-vexing-parse with Value(value).
        Vector constVec{Value(value)};
        chunk.data_[0].reference(constVec);
        chunk.setCapacity(count);
        chunk.setCardinality(count);
        return chunk;
    }

    // ----- DataChunk helpers (dictionary vector) -----

    // Builds a single-column DataChunk whose STRING column is a DICTIONARY_VECTOR.
    // 'rows' acts as the dictionary; logical row i resolves to rows[selIndices[i]].
    // This exercises the selectGeneric path in UnaryExecution::select via orrify.
    DataChunk makeDictionaryStringChunk(const vector<string>& rows,
                                        const vector<idx_t>& selIndices) {
        const idx_t count = static_cast<idx_t>(selIndices.size());
        auto chunk = makeChunkWithStrings(rows);
        SelectionVector sel(count);
        for (idx_t i = 0; i < count; i++)
            sel.setIndex(i, selIndices[i]);
        chunk.slice(sel, count);
        return chunk;
    }

    // ----- Full pipeline helpers -----

    // Runs the complete StringLike pipeline (bind → init → function) on a
    // single-column string DataChunk and returns the filtered output.
    DataChunk applyLikeFilter(const string& pattern, DataChunk& input) {
        // Value is non-copyable; build the inputs vector with emplace_back.
        vector<Value> inputs;
        inputs.emplace_back(pattern);
        vector<LogicalType> inputTypes = {LogicalTypeId::STRING};
        TableFilterSet filters;
        auto functionPtr = StringLikeFunc().getFunction({});
        auto& predFunction = static_cast<PredFunction&>(*functionPtr);
        std::unordered_map<string, Value> params;
        std::unordered_map<string, idx_t> bindVarName;
        bindVarName["COL"] = 0; // string column is at index 0
        vector<LogicalType> returnTypes;
        vector<string> names = {"COL"};

        auto bind = predFunction.bindFunction_(context, inputs, inputTypes, params,
                                               bindVarName, returnTypes, names, filters);
        auto fopd = predFunction.initFunction_(context, bind.get());
        DataChunk output;
        output.initializeEmpty(input.getTypes());
        predFunction.function_(context, bind.get(), fopd.get(), &input, output);
        return output;
    }

    // Convenience wrapper: builds a chunk from 'strings', applies 'pattern',
    // and returns the matched string values.
    vector<string> filterStrings(const string& pattern, const vector<string>& strings) {
        auto input = makeChunkWithStrings(strings);
        auto output = applyLikeFilter(pattern, input);
        return extractStrings(output);
    }
};

// ============================================================
// A. StringLikeData pattern-parsing tests
//
// These tests verify that the constructor correctly splits the pattern
// into segments and sets hasStartPercentage_ / hasEndPercentage_.
// ============================================================

TEST_F(StringLikeTest, Parsing_NoWildcard) {
    string pat = "hello";
    StringLikeData d(pat);
    EXPECT_EQ(d.segments_, (vector<string>{"hello"}));
    EXPECT_FALSE(d.hasStartPercentage_);
    EXPECT_FALSE(d.hasEndPercentage_);
}

TEST_F(StringLikeTest, Parsing_StartPercentOnly) {
    string pat = "%hello";
    StringLikeData d(pat);
    EXPECT_EQ(d.segments_, (vector<string>{"hello"}));
    EXPECT_TRUE(d.hasStartPercentage_);
    EXPECT_FALSE(d.hasEndPercentage_);
}

TEST_F(StringLikeTest, Parsing_EndPercentOnly) {
    string pat = "hello%";
    StringLikeData d(pat);
    EXPECT_EQ(d.segments_, (vector<string>{"hello"}));
    EXPECT_FALSE(d.hasStartPercentage_);
    EXPECT_TRUE(d.hasEndPercentage_);
}

TEST_F(StringLikeTest, Parsing_BothPercents_Contains) {
    string pat = "%hello%";
    StringLikeData d(pat);
    EXPECT_EQ(d.segments_, (vector<string>{"hello"}));
    EXPECT_TRUE(d.hasStartPercentage_);
    EXPECT_TRUE(d.hasEndPercentage_);
}

TEST_F(StringLikeTest, Parsing_MiddlePercent_TwoSegments) {
    string pat = "foo%bar";
    StringLikeData d(pat);
    EXPECT_EQ(d.segments_, (vector<string>{"foo", "bar"}));
    EXPECT_FALSE(d.hasStartPercentage_);
    EXPECT_FALSE(d.hasEndPercentage_);
}

TEST_F(StringLikeTest, Parsing_StartsAndEndsWithPercent_TwoInnerSegments) {
    string pat = "%foo%bar%";
    StringLikeData d(pat);
    EXPECT_EQ(d.segments_, (vector<string>{"foo", "bar"}));
    EXPECT_TRUE(d.hasStartPercentage_);
    EXPECT_TRUE(d.hasEndPercentage_);
}

TEST_F(StringLikeTest, Parsing_ThreeSegments) {
    string pat = "pre%mid%suf";
    StringLikeData d(pat);
    EXPECT_EQ(d.segments_, (vector<string>{"pre", "mid", "suf"}));
    EXPECT_FALSE(d.hasStartPercentage_);
    EXPECT_FALSE(d.hasEndPercentage_);
}

TEST_F(StringLikeTest, Parsing_OnlyPercent_EmptySegments) {
    // A bare '%' matches everything; segments must be empty and both flags set.
    string pat = "%";
    StringLikeData d(pat);
    EXPECT_TRUE(d.segments_.empty());
    EXPECT_TRUE(d.hasStartPercentage_);
    EXPECT_TRUE(d.hasEndPercentage_);
}

TEST_F(StringLikeTest, Parsing_ConsecutivePercents) {
    // Two adjacent '%' produce no empty segment between them.
    string pat = "foo%%bar";
    StringLikeData d(pat);
    EXPECT_EQ(d.segments_, (vector<string>{"foo", "bar"}));
    EXPECT_FALSE(d.hasStartPercentage_);
    EXPECT_FALSE(d.hasEndPercentage_);
}



// ============================================================
// B. StringLikeData::match() direct unit tests
//
// These tests call match() directly and verify that it returns the expected
// boolean for each pattern / string combination.
// ============================================================

// --- Exact match (no wildcards) ---

TEST_F(StringLikeTest, Match_Exact_SameString) {
    string pat = "hello";
    StringLikeData d(pat);
    string s = "hello";
    auto st = makeStr(s);
    EXPECT_TRUE(d.match(st));
}

TEST_F(StringLikeTest, Match_Exact_DifferentString) {
    string pat = "hello";
    StringLikeData d(pat);
    string s = "world";
    auto st = makeStr(s);
    EXPECT_FALSE(d.match(st));
}

TEST_F(StringLikeTest, Match_Exact_StringTooLong) {
    string pat = "hello";
    StringLikeData d(pat);
    string s = "hello!";
    auto st = makeStr(s);
    EXPECT_FALSE(d.match(st));
}

TEST_F(StringLikeTest, Match_Exact_StringTooShort) {
    string pat = "hello";
    StringLikeData d(pat);
    string s = "hell";
    auto st = makeStr(s);
    EXPECT_FALSE(d.match(st));
}

// --- Starts with (pattern "prefix%") ---

TEST_F(StringLikeTest, Match_StartsWith_Matches) {
    string pat = "hello%";
    StringLikeData d(pat);
    string s = "hello world";
    auto st = makeStr(s);
    EXPECT_TRUE(d.match(st));
}

TEST_F(StringLikeTest, Match_StartsWith_ExactlyPrefix) {
    // String equals the prefix exactly (empty tail after prefix).
    string pat = "hello%";
    StringLikeData d(pat);
    string s = "hello";
    auto st = makeStr(s);
    EXPECT_TRUE(d.match(st));
}

TEST_F(StringLikeTest, Match_StartsWith_WrongPrefix) {
    string pat = "hello%";
    StringLikeData d(pat);
    string s = "world hello";
    auto st = makeStr(s);
    EXPECT_FALSE(d.match(st));
}

TEST_F(StringLikeTest, Match_StartsWith_EmptyString) {
    string pat = "hello%";
    StringLikeData d(pat);
    string s;
    auto st = makeStr(s);
    EXPECT_FALSE(d.match(st));
}

// --- Ends with (pattern "%suffix") ---

TEST_F(StringLikeTest, Match_EndsWith_Matches) {
    string pat = "%world";
    StringLikeData d(pat);
    string s = "hello world";
    auto st = makeStr(s);
    EXPECT_TRUE(d.match(st));
}

TEST_F(StringLikeTest, Match_EndsWith_ExactlySuffix) {
    string pat = "%world";
    StringLikeData d(pat);
    string s = "world";
    auto st = makeStr(s);
    EXPECT_TRUE(d.match(st));
}

TEST_F(StringLikeTest, Match_EndsWith_WrongSuffix) {
    string pat = "%world";
    StringLikeData d(pat);
    string s = "world hello";
    auto st = makeStr(s);
    EXPECT_FALSE(d.match(st));
}

TEST_F(StringLikeTest, Match_EndsWith_StringTooShort) {
    string pat = "%world";
    StringLikeData d(pat);
    string s = "wor";
    auto st = makeStr(s);
    EXPECT_FALSE(d.match(st));
}

// --- Contains (pattern "%substring%") ---

TEST_F(StringLikeTest, Match_Contains_InMiddle) {
    string pat = "%foo%";
    StringLikeData d(pat);
    string s = "xxxfooyyy";
    auto st = makeStr(s);
    EXPECT_TRUE(d.match(st));
}

TEST_F(StringLikeTest, Match_Contains_AtStart) {
    string pat = "%foo%";
    StringLikeData d(pat);
    string s = "foobar";
    auto st = makeStr(s);
    EXPECT_TRUE(d.match(st));
}

TEST_F(StringLikeTest, Match_Contains_AtEnd) {
    string pat = "%foo%";
    StringLikeData d(pat);
    string s = "barfoo";
    auto st = makeStr(s);
    EXPECT_TRUE(d.match(st));
}

TEST_F(StringLikeTest, Match_Contains_ExactSubstring) {
    string pat = "%foo%";
    StringLikeData d(pat);
    string s = "foo";
    auto st = makeStr(s);
    EXPECT_TRUE(d.match(st));
}

TEST_F(StringLikeTest, Match_Contains_NotPresent) {
    string pat = "%foo%";
    StringLikeData d(pat);
    string s = "barbar";
    auto st = makeStr(s);
    EXPECT_FALSE(d.match(st));
}

// --- Middle wildcard (pattern "prefix%suffix") ---

TEST_F(StringLikeTest, Match_MiddleWildcard_Matches) {
    string pat = "foo%bar";
    StringLikeData d(pat);
    string s = "fooXXXbar";
    auto st = makeStr(s);
    EXPECT_TRUE(d.match(st));
}

TEST_F(StringLikeTest, Match_MiddleWildcard_EmptyMiddle) {
    // Prefix and suffix are adjacent (zero chars in the wildcard slot).
    string pat = "foo%bar";
    StringLikeData d(pat);
    string s = "foobar";
    auto st = makeStr(s);
    EXPECT_TRUE(d.match(st));
}

TEST_F(StringLikeTest, Match_MiddleWildcard_WrongPrefix) {
    string pat = "foo%bar";
    StringLikeData d(pat);
    string s = "xooXXXbar";
    auto st = makeStr(s);
    EXPECT_FALSE(d.match(st));
}

TEST_F(StringLikeTest, Match_MiddleWildcard_WrongSuffix) {
    string pat = "foo%bar";
    StringLikeData d(pat);
    string s = "fooXXXbaz";
    auto st = makeStr(s);
    EXPECT_FALSE(d.match(st));
}

TEST_F(StringLikeTest, Match_MiddleWildcard_MissingSuffix) {
    string pat = "foo%bar";
    StringLikeData d(pat);
    string s = "fooXXX";
    auto st = makeStr(s);
    EXPECT_FALSE(d.match(st));
}

TEST_F(StringLikeTest, Match_MiddleWildcard_ExtraCharsAfterSuffix) {
    // "foobarX" ends with "barX", not "bar", so it must not match.
    string pat = "foo%bar";
    StringLikeData d(pat);
    string s = "foobarX";
    auto st = makeStr(s);
    EXPECT_FALSE(d.match(st));
}

// --- Multiple middle segments (%seg1%seg2%) ---

TEST_F(StringLikeTest, Match_MultipleSegments_OrderMatters) {
    string pat = "%foo%bar%";
    StringLikeData d(pat);

    // Both segments present in the correct order → match
    string s1 = "xfoobar";
    auto st1 = makeStr(s1);
    EXPECT_TRUE(d.match(st1));

    // Segments in reversed order → no match
    string s2 = "xbarxxxfoo";
    auto st2 = makeStr(s2);
    EXPECT_FALSE(d.match(st2));

    // First segment absent → no match
    string s3 = "xbar";
    auto st3 = makeStr(s3);
    EXPECT_FALSE(d.match(st3));
}

TEST_F(StringLikeTest, Match_MultipleSegments_WithSeparation) {
    string pat = "%foo%bar%";
    StringLikeData d(pat);
    string s = "prefixfooMIDDLEbarsuffix";
    auto st = makeStr(s);
    EXPECT_TRUE(d.match(st));
}

// --- Three-segment pattern (prefix%mid%suffix) ---

TEST_F(StringLikeTest, Match_ThreeSegments_Matches) {
    string pat = "pre%mid%suf";
    StringLikeData d(pat);
    string s = "prexmidxsuf";
    auto st = makeStr(s);
    EXPECT_TRUE(d.match(st));
}

TEST_F(StringLikeTest, Match_ThreeSegments_EmptyWildcardSlots) {
    string pat = "pre%mid%suf";
    StringLikeData d(pat);
    string s = "premidsuf";
    auto st = makeStr(s);
    EXPECT_TRUE(d.match(st));
}

TEST_F(StringLikeTest, Match_ThreeSegments_ExtraCharsAtEnd) {
    // Must end exactly with "suf" — a trailing char breaks the match.
    string pat = "pre%mid%suf";
    StringLikeData d(pat);
    string s = "premidsufX";
    auto st = makeStr(s);
    EXPECT_FALSE(d.match(st));
}

TEST_F(StringLikeTest, Match_ThreeSegments_ExtraCharsAtStart) {
    // Must start exactly with "pre" — a leading char breaks the match.
    string pat = "pre%mid%suf";
    StringLikeData d(pat);
    string s = "Xprexmidxsuf";
    auto st = makeStr(s);
    EXPECT_FALSE(d.match(st));
}

// --- Underscore wildcard (generic match path) ---

TEST_F(StringLikeTest, Match_Underscore_SingleCharWildcard) {
    string pat = "h_llo";
    StringLikeData d(pat);
    string yes = "h_llo"; auto syes = makeStr(yes); EXPECT_TRUE(d.match(syes));
    string no1 = "hllo";  auto sno1 = makeStr(no1); EXPECT_FALSE(d.match(sno1));
    string no2 = "heello";auto sno2 = makeStr(no2); EXPECT_FALSE(d.match(sno2));
}

TEST_F(StringLikeTest, Match_Underscore_InNeedleBetweenPercents) {
    // '%long_keyword%' — the '_' is a wildcard matching any one character.
    string pat = "%long_keyword%";
    StringLikeData d(pat);

    string s1 = "some prefix before long_keyword and a suffix after";
    string_t st1(s1.c_str(), static_cast<uint32_t>(s1.size()));
    EXPECT_TRUE(d.match(st1));

    string s2 = "some prefix before longkeyword and a suffix after";
    string_t st2(s2.c_str(), static_cast<uint32_t>(s2.size()));
    // no char between "long" and "keyword" → _ can't match zero chars → false
    EXPECT_FALSE(d.match(st2));
}

// --- Long strings (non-inlined, > BumbleString::PREFIX_LENGTH = 11 chars) ---

TEST_F(StringLikeTest, Match_LongString_StartsWith) {
    string pat = "Hello, World!%";
    StringLikeData d(pat);

    string s1 = "Hello, World! This is a longer string.";
    string_t st1(s1.c_str(), static_cast<uint32_t>(s1.size()));
    EXPECT_TRUE(d.match(st1));

    string s2 = "Goodbye, World! Something different.";
    string_t st2(s2.c_str(), static_cast<uint32_t>(s2.size()));
    EXPECT_FALSE(d.match(st2));
}

// --- Needle size variations exercising all find() specialisations ---
// The internal find() function dispatches on needle size to:
//   1 byte  → memchr only
//   2 bytes → containsAligned<uint16_t>
//   3 bytes → containsUnaligned<uint32_t, 3>
//   4 bytes → containsAligned<uint32_t>
//   5-7 bytes → containsUnaligned<uint64_t, N>
//   8 bytes → containsAligned<uint64_t>
//   >8 bytes → containsGeneric

TEST_F(StringLikeTest, Match_NeedleSize1_SingleChar) {
    string pat = "%x%";
    StringLikeData d(pat);
    string s1 = "abxcd"; auto st1 = makeStr(s1); EXPECT_TRUE(d.match(st1));
    string s2 = "abcd";  auto st2 = makeStr(s2); EXPECT_FALSE(d.match(st2));
}

TEST_F(StringLikeTest, Match_NeedleSize2_TwoChars) {
    string pat = "%ab%";
    StringLikeData d(pat);
    string s1 = "xabx"; auto st1 = makeStr(s1); EXPECT_TRUE(d.match(st1));
    string s2 = "xacx"; auto st2 = makeStr(s2); EXPECT_FALSE(d.match(st2));
}

TEST_F(StringLikeTest, Match_NeedleSize3_ThreeChars) {
    string pat = "%abc%";
    StringLikeData d(pat);
    string s1 = "xabcx"; auto st1 = makeStr(s1); EXPECT_TRUE(d.match(st1));
    string s2 = "xabx";  auto st2 = makeStr(s2); EXPECT_FALSE(d.match(st2));
}

TEST_F(StringLikeTest, Match_NeedleSize4_FourChars) {
    string pat = "%abcd%";
    StringLikeData d(pat);
    string s1 = "xabcdx"; auto st1 = makeStr(s1); EXPECT_TRUE(d.match(st1));
    string s2 = "xabcx";  auto st2 = makeStr(s2); EXPECT_FALSE(d.match(st2));
}

TEST_F(StringLikeTest, Match_NeedleSize5_FiveChars) {
    string pat = "%abcde%";
    StringLikeData d(pat);
    string s1 = "xabcdex"; auto st1 = makeStr(s1); EXPECT_TRUE(d.match(st1));
    string s2 = "xabcdx";  auto st2 = makeStr(s2); EXPECT_FALSE(d.match(st2));
}

TEST_F(StringLikeTest, Match_NeedleSize8_EightChars) {
    string pat = "%abcdefgh%";
    StringLikeData d(pat);
    string s1 = "xabcdefghx"; auto st1 = makeStr(s1); EXPECT_TRUE(d.match(st1));
    string s2 = "xabcdefgx";  auto st2 = makeStr(s2); EXPECT_FALSE(d.match(st2));
}

TEST_F(StringLikeTest, Match_NeedleSizeLarge_Generic) {
    // Needle > 8 bytes exercises containsGeneric.
    string pat = "%abcdefghijkl%";
    StringLikeData d(pat);
    string s1 = "xxabcdefghijklxx"; auto st1 = makeStr(s1); EXPECT_TRUE(d.match(st1));
    string s2 = "xxabcdefghijkxx";  auto st2 = makeStr(s2); EXPECT_FALSE(d.match(st2));
}

// ============================================================
// C. Full pipeline (bind → init → function) tests
//
// These tests exercise the complete path from StringLikeFunc::getFunction()
// through bind / init / execute, verifying that the DataChunk output
// contains exactly the rows whose string column matched the pattern.
// ============================================================

TEST_F(StringLikeTest, Filter_AllMatch) {
    vector<string> input = {"apple", "apricot", "avocado"};
    auto result = filterStrings("a%", input);
    EXPECT_EQ(result.size(), 3u);
}

TEST_F(StringLikeTest, Filter_NoneMatch) {
    vector<string> input = {"banana", "cherry", "date"};
    auto result = filterStrings("a%", input);
    EXPECT_TRUE(result.empty());
}

TEST_F(StringLikeTest, Filter_PartialMatch_StartsWith) {
    vector<string> input = {"apple", "banana", "apricot", "cherry"};
    auto result = filterStrings("a%", input);
    EXPECT_EQ(result.size(), 2u);
    EXPECT_EQ(result[0], "apple");
    EXPECT_EQ(result[1], "apricot");
}

TEST_F(StringLikeTest, Filter_PartialMatch_EndsWith) {
    vector<string> input = {"foobar", "bazbar", "foobaz", "xbar"};
    auto result = filterStrings("%bar", input);
    EXPECT_EQ(result.size(), 3u);
    EXPECT_EQ(result[0], "foobar");
    EXPECT_EQ(result[1], "bazbar");
    EXPECT_EQ(result[2], "xbar");
}

TEST_F(StringLikeTest, Filter_PartialMatch_Contains) {
    vector<string> input = {"xfooy", "foo", "barfoo", "bar", "fooz"};
    auto result = filterStrings("%foo%", input);
    EXPECT_EQ(result.size(), 4u);
    EXPECT_EQ(result[0], "xfooy");
    EXPECT_EQ(result[1], "foo");
    EXPECT_EQ(result[2], "barfoo");
    EXPECT_EQ(result[3], "fooz");
}

TEST_F(StringLikeTest, Filter_ExactMatch) {
    vector<string> input = {"hello", "hello!", "hell", "world"};
    auto result = filterStrings("hello", input);
    EXPECT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0], "hello");
}

TEST_F(StringLikeTest, Filter_MiddleWildcard) {
    vector<string> input = {"foobar", "fooXYZbar", "fooXbar", "xfoobar", "foobaz"};
    auto result = filterStrings("foo%bar", input);
    EXPECT_EQ(result.size(), 3u);
    EXPECT_EQ(result[0], "foobar");
    EXPECT_EQ(result[1], "fooXYZbar");
    EXPECT_EQ(result[2], "fooXbar");
}

TEST_F(StringLikeTest, Filter_MultipleSegments) {
    vector<string> input = {"foobar", "foobarbaz", "barfoo", "foobaz"};
    auto result = filterStrings("%foo%bar%", input);
    EXPECT_EQ(result.size(), 2u);
    EXPECT_EQ(result[0], "foobar");
    EXPECT_EQ(result[1], "foobarbaz");
}

TEST_F(StringLikeTest, Filter_Underscore_Wildcard) {
    // 'h_llo' should match "hello", "hXllo", etc. but not "hllo" or "heello".
    vector<string> input = {"h_llo", "hllo", "h_llo", "heello", "world"};
    auto result = filterStrings("h_llo", input);
    EXPECT_EQ(result.size(), 2u);
    EXPECT_EQ(result[0], "h_llo");
    EXPECT_EQ(result[1], "h_llo");
}

TEST_F(StringLikeTest, Filter_EmptyInput) {
    vector<string> input;
    auto result = filterStrings("foo%", input);
    EXPECT_TRUE(result.empty());
}

TEST_F(StringLikeTest, Filter_SingleElement_Match) {
    vector<string> input = {"foobar"};
    auto result = filterStrings("foo%", input);
    EXPECT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0], "foobar");
}

TEST_F(StringLikeTest, Filter_SingleElement_NoMatch) {
    vector<string> input = {"barfoo"};
    auto result = filterStrings("foo%", input);
    EXPECT_TRUE(result.empty());
}

TEST_F(StringLikeTest, Filter_EmptyStringInInput_NotMatched) {
    // An empty string cannot match "hel%" (which requires at least 3 chars).
    vector<string> input = {"", "hello", "world"};
    auto result = filterStrings("hel%", input);
    EXPECT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0], "hello");
}

// --- Large dataset ---

TEST_F(StringLikeTest, Filter_LargeDataset_CorrectMatchCount) {
    // 1 024 rows: even-indexed rows match "alpha%", odd-indexed rows don't.
    const idx_t N = 1024;
    vector<string> strings;
    strings.reserve(N);
    for (idx_t i = 0; i < N; i++) {
        if (i % 2 == 0)
            strings.push_back("alpha_" + std::to_string(i));
        else
            strings.push_back("beta_" + std::to_string(i));
    }

    auto result = filterStrings("alpha%", strings);
    EXPECT_EQ(result.size(), N / 2);
    for (const auto& s : result)
        EXPECT_EQ(s.compare(0, 6, "alpha_"), 0) << "Unexpected match: " << s;
}

// ============================================================
// D. Constant vector tests
//
// When all rows in the input column share the same value the execution engine
// takes the selectConstant fast path. These tests confirm that the filter
// correctly returns all rows or no rows depending on whether the single
// stored value matches the pattern.
// ============================================================

TEST_F(StringLikeTest, Filter_ConstantVector_StartsWith_AllMatch) {
    // "apple_fruit" starts with "a" → all 6 rows must be returned.
    const idx_t count = 6;
    auto chunk = makeConstantStringChunk("apple_fruit", count);
    ASSERT_EQ(chunk.data_[0].getVectorType(), VectorType::CONSTANT_VECTOR);

    auto output = applyLikeFilter("a%", chunk);
    EXPECT_EQ(output.getSize(), count);
}

TEST_F(StringLikeTest, Filter_ConstantVector_StartsWith_NoMatch) {
    // "banana" does not start with "a" → 0 rows returned.
    const idx_t count = 6;
    auto chunk = makeConstantStringChunk("banana", count);
    ASSERT_EQ(chunk.data_[0].getVectorType(), VectorType::CONSTANT_VECTOR);

    auto output = applyLikeFilter("a%", chunk);
    EXPECT_EQ(output.getSize(), 0u);
}

TEST_F(StringLikeTest, Filter_ConstantVector_EndsWith_AllMatch) {
    // "prefix_bar" ends with "bar" → all rows match "%bar".
    const idx_t count = 4;
    auto chunk = makeConstantStringChunk("prefix_bar", count);
    ASSERT_EQ(chunk.data_[0].getVectorType(), VectorType::CONSTANT_VECTOR);

    auto output = applyLikeFilter("%bar", chunk);
    EXPECT_EQ(output.getSize(), count);
}

TEST_F(StringLikeTest, Filter_ConstantVector_EndsWith_NoMatch) {
    const idx_t count = 4;
    auto chunk = makeConstantStringChunk("prefix_baz", count);
    ASSERT_EQ(chunk.data_[0].getVectorType(), VectorType::CONSTANT_VECTOR);

    auto output = applyLikeFilter("%bar", chunk);
    EXPECT_EQ(output.getSize(), 0u);
}

TEST_F(StringLikeTest, Filter_ConstantVector_Contains_AllMatch) {
    // "xfooy" contains "foo" → all rows match "%foo%".
    const idx_t count = 5;
    auto chunk = makeConstantStringChunk("xfooy", count);
    ASSERT_EQ(chunk.data_[0].getVectorType(), VectorType::CONSTANT_VECTOR);

    auto output = applyLikeFilter("%foo%", chunk);
    EXPECT_EQ(output.getSize(), count);
}

TEST_F(StringLikeTest, Filter_ConstantVector_Contains_NoMatch) {
    const idx_t count = 5;
    auto chunk = makeConstantStringChunk("nothing_here", count);
    ASSERT_EQ(chunk.data_[0].getVectorType(), VectorType::CONSTANT_VECTOR);

    auto output = applyLikeFilter("%foo%", chunk);
    EXPECT_EQ(output.getSize(), 0u);
}

TEST_F(StringLikeTest, Filter_ConstantVector_Exact_AllMatch) {
    const idx_t count = 3;
    auto chunk = makeConstantStringChunk("hello", count);
    ASSERT_EQ(chunk.data_[0].getVectorType(), VectorType::CONSTANT_VECTOR);

    auto output = applyLikeFilter("hello", chunk);
    EXPECT_EQ(output.getSize(), count);
}

TEST_F(StringLikeTest, Filter_ConstantVector_Exact_NoMatch) {
    const idx_t count = 3;
    auto chunk = makeConstantStringChunk("hello!", count);
    ASSERT_EQ(chunk.data_[0].getVectorType(), VectorType::CONSTANT_VECTOR);

    auto output = applyLikeFilter("hello", chunk);
    EXPECT_EQ(output.getSize(), 0u);
}

// ============================================================
// E. Dictionary vector tests
//
// A DICTIONARY_VECTOR holds a flat child plus a selection vector; reading row i
// resolves to child[sel[i]]. UnaryExecution::select dispatches to selectGeneric
// (via orrify) for this type, so each row is evaluated independently.
// ============================================================

TEST_F(StringLikeTest, Filter_DictionaryVector_StartsWith_PartialMatch) {
    // Dictionary: {"apple", "banana", "cherry"}
    // Logical rows:  apple, banana, cherry, apple, banana  (5 rows)
    // Pattern "a%" matches only "apple" → rows 0 and 3.
    vector<string> dict = {"apple", "banana", "cherry"};
    vector<idx_t>  sel  = {0,       1,        2,        0,       1};
    auto chunk = makeDictionaryStringChunk(dict, sel);
    ASSERT_EQ(chunk.data_[0].getVectorType(), VectorType::DICTIONARY_VECTOR);

    auto output = applyLikeFilter("a%", chunk);
    EXPECT_EQ(output.getSize(), 2u);

    auto strs = extractStrings(output);
    EXPECT_EQ(strs[0], "apple");
    EXPECT_EQ(strs[1], "apple");
}

TEST_F(StringLikeTest, Filter_DictionaryVector_StartsWith_AllMatch) {
    // Every dictionary entry starts with "a", so all rows match.
    vector<string> dict = {"apple", "apricot", "avocado"};
    vector<idx_t>  sel  = {0, 1, 2, 0, 2, 1};
    auto chunk = makeDictionaryStringChunk(dict, sel);
    ASSERT_EQ(chunk.data_[0].getVectorType(), VectorType::DICTIONARY_VECTOR);

    auto output = applyLikeFilter("a%", chunk);
    EXPECT_EQ(output.getSize(), static_cast<idx_t>(sel.size()));
}

TEST_F(StringLikeTest, Filter_DictionaryVector_StartsWith_NoneMatch) {
    // No dictionary entry starts with "a".
    vector<string> dict = {"banana", "cherry", "date"};
    vector<idx_t>  sel  = {0, 1, 2, 0, 1};
    auto chunk = makeDictionaryStringChunk(dict, sel);
    ASSERT_EQ(chunk.data_[0].getVectorType(), VectorType::DICTIONARY_VECTOR);

    auto output = applyLikeFilter("a%", chunk);
    EXPECT_EQ(output.getSize(), 0u);
}

TEST_F(StringLikeTest, Filter_DictionaryVector_Contains_PartialMatch) {
    // Dictionary: {"xfooy", "bar", "foobaz"}
    // Selection:   0,        1,     2,         0,     2   (5 rows)
    // "%foo%" matches "xfooy" (0), "foobaz" (2), so rows 0, 2, 3, 4 match.
    vector<string> dict = {"xfooy", "bar", "foobaz"};
    vector<idx_t>  sel  = {0,        1,     2,         0,     2};
    auto chunk = makeDictionaryStringChunk(dict, sel);
    ASSERT_EQ(chunk.data_[0].getVectorType(), VectorType::DICTIONARY_VECTOR);

    auto output = applyLikeFilter("%foo%", chunk);
    EXPECT_EQ(output.getSize(), 4u);

    auto strs = extractStrings(output);
    EXPECT_EQ(strs[0], "xfooy");
    EXPECT_EQ(strs[1], "foobaz");
    EXPECT_EQ(strs[2], "xfooy");
    EXPECT_EQ(strs[3], "foobaz");
}

TEST_F(StringLikeTest, Filter_DictionaryVector_Exact_RepeatedEntries) {
    // Dictionary: {"match", "no"}; rows alternate. Exact pattern "match".
    vector<string> dict = {"match", "no"};
    vector<idx_t>  sel  = {0, 1, 0, 0, 1, 0};   // 4 "match", 2 "no"
    auto chunk = makeDictionaryStringChunk(dict, sel);
    ASSERT_EQ(chunk.data_[0].getVectorType(), VectorType::DICTIONARY_VECTOR);

    auto output = applyLikeFilter("match", chunk);
    EXPECT_EQ(output.getSize(), 4u);

    auto strs = extractStrings(output);
    for (const auto& s : strs)
        EXPECT_EQ(s, "match");
}

TEST_F(StringLikeTest, Filter_DictionaryVector_EndsWith_PartialMatch) {
    // Dictionary: {"foobar", "foobaz", "xbar"}
    // Selection:   0,         1,        2,     0,    1    (5 rows)
    // "%bar" matches "foobar" (0) and "xbar" (2):
    //   row0→dict[0]="foobar"  ✓
    //   row1→dict[1]="foobaz"  ✗
    //   row2→dict[2]="xbar"    ✓
    //   row3→dict[0]="foobar"  ✓
    //   row4→dict[1]="foobaz"  ✗
    vector<string> dict = {"foobar", "foobaz", "xbar"};
    vector<idx_t>  sel  = {0,         1,        2,     0,    1};
    auto chunk = makeDictionaryStringChunk(dict, sel);
    ASSERT_EQ(chunk.data_[0].getVectorType(), VectorType::DICTIONARY_VECTOR);

    auto output = applyLikeFilter("%bar", chunk);
    EXPECT_EQ(output.getSize(), 3u);

    auto strs = extractStrings(output);
    EXPECT_EQ(strs[0], "foobar");
    EXPECT_EQ(strs[1], "xbar");
    EXPECT_EQ(strs[2], "foobar");
}

// ============================================================
// F. Multi-column DataChunk tests
//
// Verify that colIndex_ is used correctly: the filter must operate on
// the bound column (not always column 0) and the output must include
// all columns with only the matching rows.
// ============================================================

TEST_F(StringLikeTest, Filter_MultiColumn_FiltersCorrectColumn) {
    // Two-column chunk: col 0 = INTEGER IDs, col 1 = STRING names
    // Bind with colIndex_ = 1 so the filter applies to col 1.
    // Pattern "a%" matches "apple" (row 0) and "apricot" (row 2).
    vector<LogicalType> types = {LogicalTypeId::INTEGER, LogicalTypeId::STRING};
    vector<vector<Value>> data(2);
    data[0].emplace_back(1); data[0].emplace_back(2); data[0].emplace_back(3); data[0].emplace_back(4);
    data[1].emplace_back("apple"); data[1].emplace_back("banana"); data[1].emplace_back("apricot"); data[1].emplace_back("cherry");
    auto input = generateDataChunk(types, data);

    vector<Value> inputs;
    inputs.emplace_back("a%");
    vector<LogicalType> inputTypes = {LogicalTypeId::STRING};
    TableFilterSet filters;
    auto functionPtr = StringLikeFunc().getFunction({});
    auto& predFunction = static_cast<PredFunction&>(*functionPtr);
    std::unordered_map<string, Value> params;
    std::unordered_map<string, idx_t> bindVarName;
    bindVarName["NAME"] = 1; // string column is at index 1
    vector<LogicalType> returnTypes;
    vector<string> names = {"NAME"};

    auto bind = predFunction.bindFunction_(context, inputs, inputTypes, params,
                                           bindVarName, returnTypes, names, filters);
    auto fopd = predFunction.initFunction_(context, bind.get());
    DataChunk output;
    output.initializeEmpty(input.getTypes());
    predFunction.function_(context, bind.get(), fopd.get(), &input, output);

    // Should return 2 rows: apple (id=1) and apricot (id=3)
    ASSERT_EQ(output.getSize(), 2u);

    // Check INTEGER column (col 0)
    EXPECT_EQ(output.getValue(0, 0).toString(), "1");
    EXPECT_EQ(output.getValue(0, 1).toString(), "3");

    // Check STRING column (col 1)
    auto v0 = output.getValue(1, 0).toString();
    if (output.getValue(1, 0).isDoubleQuotedString()) v0 = v0.substr(1, v0.size() - 2);
    EXPECT_EQ(v0, "apple");

    auto v1 = output.getValue(1, 1).toString();
    if (output.getValue(1, 1).isDoubleQuotedString()) v1 = v1.substr(1, v1.size() - 2);
    EXPECT_EQ(v1, "apricot");
}

} // namespace bumblebee