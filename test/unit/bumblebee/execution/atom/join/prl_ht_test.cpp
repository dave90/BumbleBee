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


#include <numeric>
#include <thread>

#include "bumblebee/catalog/PredicateTables.hpp"
#include <gtest/gtest.h>

#include "../../../BumbleBaseTest.hpp"
#include "bumblebee/ClientContext.hpp"
#include "bumblebee/common/vector_operations/VectorOperations.hpp"
#include "bumblebee/parallel/ThreadContext.hpp"
#include "bumblebee/execution/JoinPRLHashTable.hpp"

using namespace bumblebee;


class JoinRLHTTest : public BumbleBaseTest {

    protected:

    ClientContext clientContext;


    DataChunk scanHT(std::unique_ptr<JoinPRLHashTable>& ht, idx_t offset) {
        DataChunk group;
        group.initialize(ht->getTypes());

        ht->scan(offset, group);

        return group;
    }


};


TEST_F(JoinRLHTTest, HTSimpleTest) {
    vector<ConstantType> types = {INTEGER, UINTEGER};
    vector<idx_t> keys = {0};
    vector<idx_t> payload = {1};
    vector<ConstantType> keyTypes;
    for (auto c:keys) keyTypes.push_back(types[c]);
    vector<vector<Value>> data;
    addData(data, vector{0,0,0,0});
    addData(data, vector{0,10,20,30});
    DataChunk chunk = generateDataChunk(types, data);


    std::unique_ptr<JoinPRLHashTable> ht = std::make_unique<JoinPRLHashTable>(*clientContext.bufferManager_, types, keys, payload, 8, false);
    ht->addChunk(chunk);

    idx_t ltruple = 0, rtuple= 0;
    data.clear();
    addData(data, vector{0});
    DataChunk lchunk1 = generateDataChunk(keyTypes, data);
    Vector hash1(UBIGINT);
    lchunk1.hash(hash1);
    DataChunk result1;
    SelectionVector lsel(STANDARD_VECTOR_SIZE), rsel(STANDARD_VECTOR_SIZE);
    ht->probe(ltruple,rtuple, lchunk1, hash1, lsel, rsel, result1);
    EXPECT_EQ(result1.getSize(), chunk.getSize());
    DataChunk fresult1;
    fresult1.initializeEmpty(chunk.getTypes());
    fresult1.slice(lchunk1, lsel, result1.getSize());
    fresult1.slice(result1, FlatVector::INCREMENTAL_SELECTION_VECTOR, result1.getSize(), keys.size());
    compareChunks(fresult1, chunk);

    data.clear();
    addData(data, vector{1});
    DataChunk lchunk2 = generateDataChunk(keyTypes, data);
    Vector hash2(UBIGINT);
    lchunk2.hash(hash2);
    DataChunk result2;
    ht->probe(ltruple,rtuple, lchunk2, hash2,lsel, rsel, result2);
    EXPECT_EQ(result2.getSize(), 0);
}



TEST_F(JoinRLHTTest, HTNoPayloadTest) {
    vector<ConstantType> types = {INTEGER, UINTEGER};
    vector<idx_t> keys = {0,1};
    vector<idx_t> payload = {};
    vector<ConstantType> keyTypes;
    for (auto c:keys) keyTypes.push_back(types[c]);
    // create chunk:
    // -2 -1 0 1 2 3 -2 -1 0 1 2 3
    //  0  1 2 3 4 5  0  1 2 3 4 5
    vector<vector<Value>> data;
    addData(data, geenrateSequence(-2,3,STANDARD_VECTOR_SIZE,1));
    addData(data,geenrateSequence(0,5,STANDARD_VECTOR_SIZE,1));

    DataChunk chunk = generateDataChunk(types, data);


    std::unique_ptr<JoinPRLHashTable> ht = std::make_unique<JoinPRLHashTable>(*clientContext.bufferManager_, types, keys, payload, HT_INIT_CAPACITY, true);
    ht->addChunk(chunk);
    ht->addChunk(chunk);
    EXPECT_EQ(ht->getSize(), 6);

    data.clear();
    addData(data, vector{0,4,-1,3});
    addData(data, vector{0,4,1,5});
    DataChunk lchunk1 = generateDataChunk(keyTypes, data);
    Vector hash1(UBIGINT);
    lchunk1.hash(hash1);
    SelectionVector mSel(STANDARD_VECTOR_SIZE);
    SelectionVector nmSel(STANDARD_VECTOR_SIZE);
    idx_t mcount = 0, nmcount =0;
    ht->match(lchunk1, hash1,mSel, mcount, nmSel, nmcount);
    EXPECT_EQ(mcount , 2);
    EXPECT_EQ(mSel.getIndex(0) , 2); // -1 1
    EXPECT_EQ(mSel.getIndex(1) , 3); // 3 5
    EXPECT_EQ(nmcount , 2);
    EXPECT_EQ(nmSel.getIndex(0) , 0); // 0 0
    EXPECT_EQ(nmSel.getIndex(1) , 1); // 4 4
}

TEST_F(JoinRLHTTest, HTProbeMultipleCallsChunkingSingleLeftKey) {
    // Right side: MANY payload rows for the SAME key -> requires multiple probe() calls
    vector<ConstantType> types = {INTEGER, UINTEGER};
    vector<idx_t> keys = {0};
    vector<idx_t> payload = {1};
    vector<ConstantType> keyTypes; for (auto c: keys) keyTypes.push_back(types[c]);

    const idx_t N = STANDARD_VECTOR_SIZE * 2 + 13; // force > 2 chunks of results
    vector<int> r_key(N, 7);                       // same key on the right
    vector<int> r_payload = geenrateSequence(0, N - 1, 1);

    vector<vector<Value>> r_data;
    addData(r_data, r_key);
    addData(r_data, r_payload);
    DataChunk rchunk = generateDataChunk(types, r_data);

    auto ht = std::make_unique<JoinPRLHashTable>(*clientContext.bufferManager_, types, keys, payload, HT_INIT_CAPACITY, true);
    ht->addChunk(rchunk);

    // Left: single key "7"
    vector<vector<Value>> l_data;
    addData(l_data, vector<int>{7});
    DataChunk lchunk = generateDataChunk(keyTypes, l_data);
    Vector lhash(UBIGINT); lchunk.hash(lhash);


    // Expected rows set assembled from the original right chunk
    std::unordered_set<std::string> expected_rows;
    expected_rows.reserve(N);
    for (idx_t i = 0; i < N; ++i) {
        expected_rows.insert( std::to_string(r_payload[i]) );
    }
    SelectionVector lsel(STANDARD_VECTOR_SIZE), rsel(STANDARD_VECTOR_SIZE);

    // Collect results across multiple probe() calls
    std::unordered_set<std::string> got_rows;
    idx_t total = 0;
    idx_t ltuple = 0, rtuple = 0;
    while (true) {
        DataChunk out;
        ht->probe(ltuple, rtuple, lchunk, lhash, lsel, rsel,out);
        if (out.getSize() == 0) break;
        total += out.getSize();
        for (idx_t i = 0; i < out.getSize(); ++i) {
            std::string row = out.getValue(0, i).toString() ;
            got_rows.insert(row);
        }
    }

    EXPECT_EQ(total, N);
    EXPECT_EQ(got_rows.size(), expected_rows.size());
    EXPECT_EQ(got_rows, expected_rows);
    EXPECT_EQ(ltuple, 1); // left tuple fully consumed
    EXPECT_EQ(rtuple, 0); // scanner reset
}


TEST_F(JoinRLHTTest, HTNoPayloadBigMatchTest) {
    // -----------------------------
    // Build a big right-side table: 2 chunks of exactly STANDARD_VECTOR_SIZE each
    // Schema: two INTEGER key columns, no payload
    // Keys are unique across the two chunks:
    //   chunk A: (i, i*i)          for i in [0, SVS-1]
    //   chunk B: (i, i*i)          for i in [SVS, 2*SVS-1]
    // -----------------------------
    const idx_t SVS = STANDARD_VECTOR_SIZE;

    vector<ConstantType> types = {INTEGER, INTEGER};
    vector<idx_t> keys = {0, 1};
    vector<idx_t> payload = {};

    // chunk A
    vector<int> a0 = geenrateSequence(0, (int)SVS - 1, 1);
    vector<int> a1;
    a1.reserve(a0.size());
    for (auto v : a0) a1.push_back(v * v);

    vector<vector<Value>> rA;
    addData(rA, a0);
    addData(rA, a1);
    DataChunk rchunkA = generateDataChunk(types, rA);

    // chunk B
    vector<int> b0 = geenrateSequence((int)SVS, (int)SVS * 2 - 1, 1);
    vector<int> b1; b1.reserve(b0.size());
    for (auto v : b0) b1.push_back(v * v);

    vector<vector<Value>> rB;
    addData(rB, b0);
    addData(rB, b1);
    DataChunk rchunkB = generateDataChunk(types, rB);

    // Build hash table (keys only)
    auto ht = std::make_unique<JoinPRLHashTable>(
        *clientContext.bufferManager_, types, keys, payload, HT_INIT_CAPACITY, true
    );
    ht->addChunk(rchunkA);
    ht->addChunk(rchunkB);

    // -----------------------------
    // LEFT big mixed workload (size = 2*SVS):
    //   - First half (0..SVS-1): MATCHES with duplicates -> (k, k*k) where k = i % 16 (appears many times)
    //   - Second half (SVS..2*SVS-1): NON-MATCHES -> (k, k*k + 1) where k = i % 16 (intentionally off-by-one)
    // This ensures: mcount == SVS, nmcount == SVS and exercises repeated keys.
    // -----------------------------
    const idx_t L = SVS * 2;
    vector<int> l0; l0.resize(L);
    vector<int> l1; l1.resize(L);

    for (idx_t i = 0; i < SVS; ++i) {
        int k = (int)(i % 16);
        l0[i] = k;
        l1[i] = k * k;            // exact match present in chunk A
    }
    for (idx_t i = SVS; i < L; ++i) {
        int k = (int)(i % 16);
        l0[i] = k;
        l1[i] = k * k + 1;        // guaranteed non-match
    }

    vector<vector<Value>> lMix;
    addData(lMix, l0);
    addData(lMix, l1);

    vector<ConstantType> keyTypes; for (auto c : keys) keyTypes.push_back(types[c]);
    DataChunk lchunk = generateDataChunk(keyTypes, lMix);
    Vector lhash(UBIGINT, lchunk.getSize());
    lchunk.hash(lhash);

    SelectionVector mSel(L), nmSel(L);
    idx_t mcount = 0, nmcount = 0;

    ht->match(lchunk, lhash, mSel, mcount, nmSel, nmcount);

    // Expect exactly half matches, half non-matches
    EXPECT_EQ(mcount, SVS);
    EXPECT_EQ(nmcount, SVS);

    // Validate the sets of indices (order is not guaranteed across collision paths)
    std::unordered_set<idx_t> expected_match, expected_nomatch, got_match, got_nomatch;
    expected_match.reserve(SVS);
    expected_nomatch.reserve(SVS);
    for (idx_t i = 0; i < SVS; ++i) expected_match.insert(i);
    for (idx_t i = SVS; i < L;  ++i) expected_nomatch.insert(i);

    for (idx_t i = 0; i < mcount; ++i) got_match.insert(mSel.getIndex(i));
    for (idx_t i = 0; i < nmcount; ++i) got_nomatch.insert(nmSel.getIndex(i));

    EXPECT_EQ(got_match, expected_match);
    EXPECT_EQ(got_nomatch, expected_nomatch);

    // Sanity: correct schema remembered
    EXPECT_TRUE(ht->checkKeysAndPayloads(keys, payload));


    // -----------------------------
    // Edge case : All non-matches (different key range)
    // -----------------------------
    vector<int> nn0, nn1;
    nn0.reserve(SVS); nn1.reserve(SVS);
    for (idx_t i = 0; i < SVS; ++i) {
        nn0.push_back((int)(2 * SVS + 1000 + i));   // out of right range
        nn1.push_back(42);                          // arbitrary, does not exist
    }
    vector<vector<Value>> lAllNo;
    addData(lAllNo, nn0);
    addData(lAllNo, nn1);
    DataChunk lNo = generateDataChunk(keyTypes, lAllNo);
    Vector hNo(UBIGINT, lNo.getSize());
    lNo.hash(hNo);

    SelectionVector mSelNo(SVS), nmSelNo(SVS);
    idx_t mcountNo = 0, nmcountNo = 0;
    ht->match(lNo, hNo, mSelNo, mcountNo, nmSelNo, nmcountNo);
    EXPECT_EQ(mcountNo, 0);
    EXPECT_EQ(nmcountNo, SVS);
    for (idx_t i = 0; i < SVS; ++i) {
        EXPECT_EQ(nmSelNo.getIndex(i), i); // every left row is a non-match
    }

    // -----------------------------
    // Edge case : Duplicates on the left (both matching and non-matching)
    // -----------------------------
    vector<vector<Value>> lDup;
    // Two present keys, each duplicated twice
    int presentA = 3;                   // exists as (3, 9)
    int presentB = (int)SVS + 7;        // exists from chunk B: (SVS+7, (SVS+7)^2)
    // Two absent keys, each duplicated twice
    int absentA  = -123456;             // never inserted
    int absentB  = (int)(2 * SVS + 555);// outside inserted range

    addData(lDup, vector<int>{
        presentA, presentA, presentB, presentB, absentA, absentA, absentB, absentB
    });
    addData(lDup, vector<int>{
        presentA*presentA, presentA*presentA,
        presentB*presentB, presentB*presentB,
        1, 1, 2, 2
    });

    DataChunk lD = generateDataChunk(keyTypes, lDup);
    Vector hD(UBIGINT,lD.getSize()); lD.hash(hD);
    SelectionVector mSelD(8), nmSelD(8);
    idx_t mcountD = 0, nmcountD = 0;
    ht->match(lD, hD, mSelD, mcountD, nmSelD, nmcountD);

    // Expect 4 matches (two duplicates for each present key) and 4 non-matches
    EXPECT_EQ(mcountD, 4);
    EXPECT_EQ(nmcountD, 4);

    // Check multiplicity by counting indices (positions 0..7 in lDup)
    int match_hits = 0, nonmatch_hits = 0;
    std::multiset<idx_t> mpos, npos;
    for (idx_t i = 0; i < mcountD; ++i) mpos.insert(mSelD.getIndex(i)), ++match_hits;
    for (idx_t i = 0; i < nmcountD; ++i) npos.insert(nmSelD.getIndex(i)), ++nonmatch_hits;

    // Matched should be positions {0,1,2,3}, non-matched {4,5,6,7} with exact multiplicity
    EXPECT_EQ(mpos.count(0), 1); EXPECT_EQ(mpos.count(1), 1);
    EXPECT_EQ(mpos.count(2), 1); EXPECT_EQ(mpos.count(3), 1);
    EXPECT_EQ(npos.count(4), 1); EXPECT_EQ(npos.count(5), 1);
    EXPECT_EQ(npos.count(6), 1); EXPECT_EQ(npos.count(7), 1);
}
