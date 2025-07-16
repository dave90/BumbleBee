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
#include "bumblebee/catalog/PredicateTables.h"

using namespace bumblebee;

namespace {

// A subclass just for testing, defined only in the test
class PredicateTablesTester : public PredicateTables {
public:
    PredicateTablesTester(const char* name, unsigned arity)
        : PredicateTables(name, arity) {
        types_ = std::vector<ConstantType>(arity, UNKNOWN);
    }

    void callUpdateTypes(std::vector<ConstantType> newTypes) {
        updateTypes(newTypes);
    }

    const std::vector<ConstantType>& getTypes() const {
        return types_;
    }

};

class PredicateTablesTest : public ::testing::Test {
protected:
    void SetUp() override {
        table = std::make_unique<PredicateTablesTester>("test", 3);
    }

    std::unique_ptr<PredicateTablesTester> table;
};

Atom generateRangeAtom(Predicate* p,const std::vector<IntervalTerm>& intervals) {
    terms_vector_t terms;
    for (auto& i : intervals)
        if (i.from == i.to)
            terms.emplace_back((int64_t)i.from);
        else
            terms.emplace_back(i);

    Atom fact(p, std::move(terms), AtomType::CLASSICAL);
    return fact;
}

// Function to generate the full Cartesian product table
std::vector<std::vector<int>> generateCartesianProduct(const IntervalTerm& a, const IntervalTerm& b, const IntervalTerm& c) {
    std::vector<std::vector<int>> result;

    for (int i = c.from; i <= c.to; ++i) {
        for (int j = b.from; j <= b.to; ++j) {
            for (int k = a.from; k <= a.to; ++k) {
                result.push_back({k, j, i});
            }
        }
    }

    return result;
}
TEST_F(PredicateTablesTest, NoChange) {
    std::vector<ConstantType> newTypes = {UNKNOWN, UNKNOWN, UNKNOWN};
    table->callUpdateTypes(newTypes);
    EXPECT_EQ(table->getTypes(), newTypes);
}

TEST_F(PredicateTablesTest, PromoteUnknown) {
    std::vector<ConstantType> newTypes = {TINYINT, SMALLINT, BIGINT};
    table->callUpdateTypes(newTypes);
    EXPECT_EQ(table->getTypes(), newTypes);
}

TEST_F(PredicateTablesTest, PromoteSmallerSize) {
    table->callUpdateTypes({SMALLINT, SMALLINT, SMALLINT});
    table->callUpdateTypes({INTEGER, INTEGER, UINTEGER});
    EXPECT_EQ(table->getTypes()[0], INTEGER);
    EXPECT_EQ(table->getTypes()[1], INTEGER);
    EXPECT_EQ(table->getTypes()[2], UINTEGER);
}

TEST_F(PredicateTablesTest, MixedSignednessSameSize) {
    table->callUpdateTypes({UINTEGER, INTEGER, UINTEGER});
    table->callUpdateTypes({INTEGER, INTEGER, INTEGER});
    EXPECT_EQ(table->getTypes()[0], BIGINT);  // bumped
    EXPECT_EQ(table->getTypes()[1], INTEGER);
    EXPECT_EQ(table->getTypes()[2], BIGINT);  // bumped
}

TEST_F(PredicateTablesTest, FloatToDoublePromotion) {
    table = std::make_unique<PredicateTablesTester>("test", 1);
    table->callUpdateTypes({FLOAT});
    table->callUpdateTypes({DOUBLE});
    EXPECT_EQ(table->getTypes()[0], DOUBLE);
}

TEST_F(PredicateTablesTest, TypeCountMismatchTriggersAssert) {
    std::vector<ConstantType> badSize = {INTEGER, INTEGER}; // only 2 types
    EXPECT_DEATH(table->callUpdateTypes(badSize), "Wrong number of terms for Fact");
}


TEST_F(PredicateTablesTest, TestAddSingleFact) {
    terms_vector_t terms;
    terms.emplace_back(1);
    terms.emplace_back(2);
    terms.emplace_back(3);
    Atom fact(table->predicate_.get(), std::move(terms), AtomType::CLASSICAL);
    table->addFact(fact);
    table->initializeChunks();
    std::cout << table->getChunk(0).toString() << std::endl;
    EXPECT_EQ(table->getCount(), 1);
}

TEST_F(PredicateTablesTest, TestAddFactsDifferentCType) {
    terms_vector_t terms;

    terms.emplace_back((uint8_t)1);
    terms.emplace_back(2);
    terms.emplace_back((int64_t)3);
    Atom fact1(table->predicate_.get(), std::move(terms), AtomType::CLASSICAL);
    table->addFact(fact1);
    terms.clear();

    terms.emplace_back((int8_t)1);
    terms.emplace_back(2);
    terms.emplace_back(3);
    Atom fact2(table->predicate_.get(), std::move(terms), AtomType::CLASSICAL);
    table->addFact(fact2);


    table->initializeChunks();
    std::cout << table->getChunk(0).toString() << std::endl;
    EXPECT_EQ(table->getCount(), 2);
    auto types = table->getTypes();
    EXPECT_EQ(types[0], SMALLINT);
    EXPECT_EQ(types[1], INTEGER);
    EXPECT_EQ(types[2], BIGINT);
}


TEST_F(PredicateTablesTest, TestAddFactsMultiChunks) {
    terms_vector_t terms;
    auto N = STANDARD_VECTOR_SIZE + 100;
    for (idx_t i = 0; i < N; ++i) {
        terms.emplace_back(i);
        terms.emplace_back(i*10);
        terms.emplace_back(i*100);
        Atom fact(table->predicate_.get(), std::move(terms), AtomType::CLASSICAL);
        table->addFact(fact);
        terms.clear();
    }

    table->initializeChunks();

    EXPECT_EQ(table->getCount(), N);
    EXPECT_EQ(table->chunkCount(), 2);
}


TEST_F(PredicateTablesTest, TestSmallSequence) {

    IntervalTerm i1 = {1,2};
    IntervalTerm i2 = {1,  3};
    IntervalTerm i3 = {0,1};
    std::vector<IntervalTerm> intervals = {i1,i2,i3};
    auto fact = generateRangeAtom(table->predicate_.get(), intervals);
    table->addFact(fact);

    table->initializeChunks();
    ASSERT_EQ(table->getCount(), 2*3*2);
    std::cout << table->getChunk(0).toString() <<std::endl;

    // check results
    auto cartesianTable = generateCartesianProduct(i1,i2,i3);
    for (unsigned i =0; i < cartesianTable.size() ; ++i) {
        for (unsigned j =0; j < cartesianTable[i].size() ; ++j) {
            EXPECT_EQ(Value((int64_t)cartesianTable[i][j]),table->getChunk(0).getValue(j,i));
        }
    }
}

TEST_F(PredicateTablesTest, TestSmallSequenceWithConstant) {
    IntervalTerm i1 = {1,2};
    IntervalTerm i2 = {5,  5};
    IntervalTerm i3 = {-1,1};
    std::vector<IntervalTerm> intervals = {i1,i2,i3};
    auto fact = generateRangeAtom(table->predicate_.get(), intervals);
    table->addFact(fact);

    table->initializeChunks();
    ASSERT_EQ(table->getCount(), 2*3);
    std::cout << table->getChunk(0).toString() <<std::endl;

    // check results
    auto cartesianTable = generateCartesianProduct(i1,i2,i3);
    for (unsigned i =0; i < cartesianTable.size() ; ++i) {
        for (unsigned j =0; j < cartesianTable[i].size() ; ++j) {
            EXPECT_EQ(Value((int64_t)cartesianTable[i][j]),table->getChunk(0).getValue(j,i));
        }
    }
}

TEST_F(PredicateTablesTest, TestSmallMultipleSequenceWithConstant) {
    IntervalTerm i1 = {1,2};
    IntervalTerm i2 = {5,  5};
    IntervalTerm i3 = {-1,1};
    std::vector<IntervalTerm> intervals = {i1,i2,i3};
    auto fact1 = generateRangeAtom(table->predicate_.get(), intervals);
    table->addFact(fact1);

    IntervalTerm i4 = {-2 , 0};
    intervals = {i4,i2,i3};
    auto fact2 = generateRangeAtom(table->predicate_.get(), intervals);
    table->addFact(fact2);

    table->initializeChunks();
    ASSERT_EQ(table->getCount(), 2*3 + 3*3);
    std::cout << table->getChunk(0).toString() <<std::endl;


    // check results first cartesian
    auto cartesianTable = generateCartesianProduct(i1,i2,i3);
    for (unsigned i =0; i < cartesianTable.size() ; ++i) {
        for (unsigned j =0; j < cartesianTable[i].size() ; ++j) {
            EXPECT_EQ(Value((int64_t)cartesianTable[i][j]),table->getChunk(0).getValue(j,i));
        }
    }

    // check results second cartesian
    auto offsetC1 = cartesianTable.size();
    auto cartesianTable2 = generateCartesianProduct(i4,i2,i3);
    for (unsigned i =0; i < cartesianTable2.size() ; ++i) {
        for (unsigned j =0; j < cartesianTable2[i].size() ; ++j) {
            EXPECT_EQ(Value((int64_t)cartesianTable2[i][j]),table->getChunk(0).getValue(j,i+offsetC1));
        }
    }
}


TEST_F(PredicateTablesTest, TestBigSequence) {
    IntervalTerm i1 = {-100,100};
    IntervalTerm i2 = {0,  5};
    IntervalTerm i3 = {-1,1};
    std::vector<IntervalTerm> intervals = {i1,i2,i3};
    auto fact = generateRangeAtom(table->predicate_.get(), intervals);
    table->addFact(fact);

    table->initializeChunks();
    ASSERT_EQ(table->getCount(), 201 * 6 * 3);
    auto chunksExpected = 201 * 6 * 3 / STANDARD_VECTOR_SIZE + 1;
    ASSERT_EQ(table->chunkCount(), chunksExpected);
    std::cout << table->getChunk(0).toString() <<std::endl;

    // check results
    auto cartesianTable = generateCartesianProduct(i1,i2,i3);
    for (unsigned i =0; i < cartesianTable.size() ; ++i) {
        for (unsigned j =0; j < cartesianTable[i].size() ; ++j) {
            EXPECT_EQ(Value((int64_t)cartesianTable[i][j]),table->getValue(j,i));
        }
    }
}

TEST_F(PredicateTablesTest, TestMultipleBigSequence) {
    IntervalTerm i1 = {-100,100};
    IntervalTerm i2 = {0,  5};
    IntervalTerm i3 = {-1,1};
    std::vector<IntervalTerm> intervals = {i1,i2,i3};
    auto fact1 = generateRangeAtom(table->predicate_.get(), intervals);
    table->addFact(fact1);
    auto fact2 = generateRangeAtom(table->predicate_.get(), intervals);
    table->addFact(fact2);

    table->initializeChunks();
    ASSERT_EQ(table->getCount(), 201 * 6 * 3 * 2);
    auto chunksExpected = 201 * 6 * 3 * 2 / STANDARD_VECTOR_SIZE + 1;
    ASSERT_EQ(table->chunkCount(), chunksExpected);
    // std::cout << table->getChunk(0).toString() <<std::endl;

}

// sequence with facts


}  // namespace
