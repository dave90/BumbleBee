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
#include "bumblebee/execution/JoinHashTable.hpp"

#include "bumblebee/common/Helper.hpp"
#include "bumblebee/common/operator/ComparisonOperators.hpp"
#include "bumblebee/common/vector_operations/VectorOperations.hpp"

namespace bumblebee{


struct InitHashJoin {

    static bool bloomCouldContains(uint64_t bloom, uint64_t hash) {
        switch (JoinHashTable::BLOOM_SIZE) {
            case 16:
                return bloom16CouldContains(bloom, hash);
            case 0:
                return true;
            default:
                ErrorHandler::errorNotImplemented("Error, bloom filter size not supported");
        }
        return false;
    }


    template <class LEFT_TYPE,class RIGHT_TYPE, class OP>
    static idx_t operation( LEFT_TYPE*__restrict ldata,const SelectionVector* ldatasel, RIGHT_TYPE*__restrict rdata,
        uint64_t*__restrict hdata, const SelectionVector* hdatasel,
        uint64_t*__restrict bdata, const SelectionVector* bdatasel, uint64_t*__restrict directory,
        idx_t lsize, idx_t &lpos, idx_t &rpos, SelectionVector& lsel, SelectionVector& rsel) {

        idx_t result_count = 0;
        for (; lpos < lsize; lpos++) {
            auto bucket = bdata[bdatasel->getIndex(lpos)];
            auto hash = hdata[hdatasel->getIndex(lpos)];
            // check bloom filter
            auto bloom = JoinHashTable::dirBloom(directory, bucket);
            if (!bloomCouldContains(bloom, hash)) continue;

            auto roffset =  JoinHashTable::dirBegin(directory, bucket);
            auto rend = JoinHashTable::dirEnd(directory, bucket);
            auto rsize = rend - roffset;
            idx_t lposition = ldatasel->getIndex(lpos);
            for (;rpos < rsize; rpos++) {
                if (result_count == STANDARD_VECTOR_SIZE) {
                    // out of space!
                    return result_count;
                }
                auto rposition = roffset + rpos;
                if (OP::operation(ldata[lposition], rdata[rposition])) {
                    // emit tuple
                    lsel.setIndex(result_count, lpos);
                    rsel.setIndex(result_count, rposition); // insert the index in the vector
                    result_count++;
                }
            }
            rpos = 0;
        }
        return result_count;
    }

    template <class LEFT_TYPE,class RIGHT_TYPE, class OP>
    static idx_t operation( Vector& left, Vector& right, Vector& lhash, Vector& buckets, directory_t& directory,
        idx_t lsize, idx_t &lpos, idx_t &rpos,
        SelectionVector& lsel, SelectionVector& rsel, idx_t currentMatch) {
        BB_ASSERT(right.getVectorType() == VectorType::FLAT_VECTOR);
        BB_ASSERT(buckets.getType() == UBIGINT);

        VectorData left_data, bucket_data, hash_data;
        left.orrify(lsize, left_data);
        buckets.orrify(lsize, bucket_data);
        lhash.orrify(lsize, hash_data);

        auto ldata = (LEFT_TYPE*) left_data.data_;
        auto ldatasel = left_data.sel_;
        auto rdata = FlatVector::getData<RIGHT_TYPE>(right);
        auto bdata = (uint64_t*) bucket_data.data_;
        auto bdatasel = bucket_data.sel_;
        auto hdata = (uint64_t*)hash_data.data_;
        auto hdatasel = hash_data.sel_;

        return operation<LEFT_TYPE, RIGHT_TYPE, OP>(ldata, ldatasel, rdata, hdata, hdatasel, bdata, bdatasel, directory.get(), lsize, lpos, rpos, lsel, rsel);
    }
};


struct RefineHashJoin {

    template <class LEFT_TYPE,class RIGHT_TYPE, class OP>
    static idx_t operation( LEFT_TYPE*__restrict ldata,const SelectionVector* ldatasel,
        RIGHT_TYPE*__restrict rdata, uint64_t*__restrict hdata, const SelectionVector* hdatasel,
        uint64_t*__restrict bdata, const SelectionVector* bdatasel, uint64_t*__restrict directory,
        idx_t lsize, idx_t &lpos, idx_t &rpos, SelectionVector& lsel, SelectionVector& rsel, idx_t currentMatch) {

        idx_t result_count = 0;
        for (idx_t i = 0; i < currentMatch; i++) {
            auto lidx = lsel.getIndex(i);
            auto ridx = rsel.getIndex(i);
            auto left_idx = ldatasel->getIndex(lidx);
            auto right_idx = ridx;
            if (OP::operation(ldata[left_idx], rdata[right_idx])) {
                lsel.setIndex(result_count, lidx);
                rsel.setIndex(result_count, ridx);
                result_count++;
            }
        }
        return result_count;
    }

    template <class LEFT_TYPE,class RIGHT_TYPE, class OP>
    static idx_t operation( Vector& left, Vector& right, Vector& lhash, Vector& buckets, directory_t& directory,
        idx_t lsize, idx_t &lpos, idx_t &rpos, SelectionVector& lsel, SelectionVector& rsel, idx_t currentMatch) {
        BB_ASSERT(right.getVectorType() == VectorType::FLAT_VECTOR);
        BB_ASSERT(buckets.getType() == UBIGINT);

        VectorData left_data, bucket_data, hash_data;
        left.orrify(lsize, left_data);
        buckets.orrify(lsize, bucket_data);
        lhash.orrify(lsize, hash_data);

        auto ldata = (LEFT_TYPE*) left_data.data_;
        auto ldatasel = left_data.sel_;
        auto rdata = FlatVector::getData<RIGHT_TYPE>(right);
        auto bdata = (uint64_t*)bucket_data.data_;
        auto bdatasel = bucket_data.sel_;
        auto hdata = (uint64_t*)hash_data.data_;
        auto hdatasel = hash_data.sel_;


        return operation<LEFT_TYPE, RIGHT_TYPE, OP>(ldata, ldatasel, rdata, hdata, hdatasel, bdata, bdatasel, directory.get(), lsize, lpos, rpos, lsel, rsel, currentMatch);
    }
};


template <class NLTYPE, class OP>
idx_t hashJoinEqualTypeSwitch(Vector &left, Vector &right, Vector &lhash, Vector &buckets, directory_t& directory,
                                                 idx_t lsize, idx_t &lpos, idx_t &rpos, SelectionVector &lsel, SelectionVector &rsel,
                                                 idx_t currentMatch) {
    BB_ASSERT(left.getType() == right.getType());
    switch (left.getType()) {
        case ConstantType::TINYINT:
            return NLTYPE::template operation<int8_t,int8_t, OP>(left, right, lhash, buckets, directory, lsize, lpos, rpos, lsel, rsel, currentMatch);
        case ConstantType::SMALLINT:
            return NLTYPE::template operation<int16_t,int16_t,OP>(left, right, lhash, buckets, directory, lsize, lpos, rpos, lsel, rsel, currentMatch);
        case ConstantType::INTEGER:
            return NLTYPE::template operation<int32_t,int32_t,OP>(left, right, lhash, buckets, directory, lsize, lpos, rpos, lsel, rsel, currentMatch);
        case ConstantType::BIGINT:
            return NLTYPE::template operation<int64_t,int64_t,OP>(left, right, lhash, buckets, directory, lsize, lpos, rpos, lsel, rsel, currentMatch);
        case ConstantType::UTINYINT:
            return NLTYPE::template operation<uint8_t,uint8_t,OP>(left, right, lhash, buckets, directory, lsize, lpos, rpos, lsel, rsel, currentMatch);
        case ConstantType::USMALLINT:
            return NLTYPE::template operation<uint16_t,uint16_t,OP>(left, right, lhash, buckets, directory, lsize, lpos, rpos, lsel, rsel, currentMatch);
        case ConstantType::UINTEGER:
            return NLTYPE::template operation<uint32_t,uint32_t,OP>(left, right, lhash, buckets, directory, lsize, lpos, rpos, lsel, rsel, currentMatch);
        case ConstantType::UBIGINT:
            return NLTYPE::template operation<uint64_t,uint64_t,OP>(left, right, lhash, buckets, directory, lsize, lpos, rpos, lsel, rsel, currentMatch);
        case ConstantType::FLOAT:
            return NLTYPE::template operation<float,float,OP>(left, right, lhash, buckets, directory, lsize, lpos, rpos, lsel, rsel, currentMatch);
        case ConstantType::DOUBLE:
            return NLTYPE::template operation<double,double,OP>(left, right, lhash, buckets, directory, lsize, lpos, rpos, lsel, rsel, currentMatch);
        case ConstantType::STRING:	{
            return NLTYPE::template operation<string_t,string_t,OP>(left, right, lhash, buckets, directory, lsize, lpos, rpos, lsel, rsel, currentMatch);
        }
        default:
            ErrorHandler::errorNotImplemented("Unimplemented type for select operation!");
    }
    return 0;
}


template<class LEFT_TYPE, class RIGHT_TYPE, class COMMON_TYPE, class OP>
struct ComparisonCommonCast {
    static inline bool operation(LEFT_TYPE left, RIGHT_TYPE right) {
        return OP::operation(static_cast<COMMON_TYPE>(left), static_cast<COMMON_TYPE>(right));
    }
};

template <class NLTYPE, class LEFT_TYPE, class RIGHT_TYPE , class OP>
idx_t hashJoinCommonTypeSwitch(Vector &left, Vector &right, Vector &lhash, Vector &buckets, directory_t& directory,
                                             idx_t lsize, idx_t &lpos, idx_t &rpos, SelectionVector &lsel, SelectionVector &rsel,
                                             idx_t currentMatch) {
    auto commonType = getCommonType(left.getType(), right.getType());

    // for int and uint collapse to int 64 bit
    // for decimal collapse to double
    switch (commonType) {
        case ConstantType::TINYINT:
        case ConstantType::SMALLINT:
        case ConstantType::INTEGER:
        case ConstantType::UTINYINT:
        case ConstantType::USMALLINT:
        case ConstantType::UINTEGER:
        case ConstantType::UBIGINT:
        case ConstantType::BIGINT:
            return NLTYPE::template operation<LEFT_TYPE,RIGHT_TYPE, ComparisonCommonCast<LEFT_TYPE, RIGHT_TYPE, int64_t, OP>>(left, right, lhash, buckets, directory, lsize, lpos, rpos, lsel, rsel, currentMatch);
        case ConstantType::FLOAT:
        case ConstantType::DOUBLE:
            return NLTYPE::template operation<LEFT_TYPE,RIGHT_TYPE, ComparisonCommonCast<LEFT_TYPE, RIGHT_TYPE, double, OP>>(left, right, lhash, buckets, directory, lsize, lpos, rpos, lsel, rsel, currentMatch);
        default:
            ErrorHandler::errorNotImplemented("Unimplemented type for select operation!");
    }
    return 0;
}

template <class NLTYPE, class LEFT_TYPE, class OP>
idx_t hashJoinRightTypeSwitch(Vector &left, Vector &right, Vector &lhash, Vector &buckets, directory_t& directory,
                                             idx_t lsize, idx_t &lpos, idx_t &rpos, SelectionVector &lsel, SelectionVector &rsel,
                                             idx_t currentMatch) {
    switch (right.getType()) {
        case ConstantType::TINYINT:
            return hashJoinCommonTypeSwitch<NLTYPE,LEFT_TYPE,int8_t,OP>(left, right, lhash, buckets, directory, lsize, lpos, rpos, lsel, rsel, currentMatch);
        case ConstantType::SMALLINT:
            return hashJoinCommonTypeSwitch<NLTYPE,LEFT_TYPE,int16_t,OP>(left, right, lhash, buckets, directory, lsize, lpos, rpos, lsel, rsel, currentMatch);
        case ConstantType::INTEGER:
            return hashJoinCommonTypeSwitch<NLTYPE,LEFT_TYPE,int32_t,OP>(left, right, lhash, buckets, directory, lsize, lpos, rpos, lsel, rsel, currentMatch);
        case ConstantType::BIGINT:
            return hashJoinCommonTypeSwitch<NLTYPE,LEFT_TYPE,int64_t,OP>(left, right, lhash, buckets, directory, lsize, lpos, rpos, lsel, rsel, currentMatch);
        case ConstantType::UTINYINT:
            return hashJoinCommonTypeSwitch<NLTYPE,LEFT_TYPE,uint8_t,OP>(left, right, lhash, buckets, directory, lsize, lpos, rpos, lsel, rsel, currentMatch);
        case ConstantType::USMALLINT:
            return hashJoinCommonTypeSwitch<NLTYPE,LEFT_TYPE,uint16_t,OP>(left, right, lhash, buckets, directory, lsize, lpos, rpos, lsel, rsel, currentMatch);
        case ConstantType::UINTEGER:
            return hashJoinCommonTypeSwitch<NLTYPE,LEFT_TYPE,uint32_t,OP>(left, right, lhash, buckets, directory, lsize, lpos, rpos, lsel, rsel, currentMatch);
        case ConstantType::UBIGINT:
            return hashJoinCommonTypeSwitch<NLTYPE,LEFT_TYPE,uint64_t,OP>(left, right, lhash, buckets, directory, lsize, lpos, rpos, lsel, rsel, currentMatch);
        case ConstantType::FLOAT:
            return hashJoinCommonTypeSwitch<NLTYPE,LEFT_TYPE,float,OP>(left, right, lhash, buckets, directory, lsize, lpos, rpos, lsel, rsel, currentMatch);
        case ConstantType::DOUBLE:
            return hashJoinCommonTypeSwitch<NLTYPE,LEFT_TYPE,double,OP>(left, right, lhash, buckets, directory, lsize, lpos, rpos, lsel, rsel, currentMatch);
        default:
            ErrorHandler::errorNotImplemented("Unimplemented type for select operation!");
            return 0;
    }
}


template <class NLTYPE, class OP>
idx_t hashJoinLeftTypeSwitch(Vector &left, Vector &right, Vector &lhash, Vector &buckets, directory_t& directory,
                                             idx_t lsize, idx_t &lpos, idx_t &rpos, SelectionVector &lsel, SelectionVector &rsel,
                                             idx_t currentMatch) {
    // cannot compare string with different types
    BB_ASSERT(left.getType() != ConstantType::STRING && right.getType() != ConstantType::STRING);
    // left type != right type
    switch (left.getType()) {
        case ConstantType::TINYINT:
            return hashJoinRightTypeSwitch<NLTYPE,int8_t,OP>(left, right, lhash, buckets, directory, lsize, lpos, rpos, lsel, rsel, currentMatch);
        case ConstantType::SMALLINT:
            return hashJoinRightTypeSwitch<NLTYPE,int16_t,OP>(left, right, lhash, buckets, directory, lsize, lpos, rpos, lsel, rsel, currentMatch);
        case ConstantType::INTEGER:
            return hashJoinRightTypeSwitch<NLTYPE,int32_t,OP>(left, right, lhash, buckets, directory, lsize, lpos, rpos, lsel, rsel, currentMatch);
        case ConstantType::BIGINT:
            return hashJoinRightTypeSwitch<NLTYPE,int64_t,OP>(left, right, lhash, buckets, directory, lsize, lpos, rpos, lsel, rsel, currentMatch);
        case ConstantType::UTINYINT:
            return hashJoinRightTypeSwitch<NLTYPE,uint8_t,OP>(left, right, lhash, buckets, directory, lsize, lpos, rpos, lsel, rsel, currentMatch);
        case ConstantType::USMALLINT:
            return hashJoinRightTypeSwitch<NLTYPE,uint16_t,OP>(left, right, lhash, buckets, directory, lsize, lpos, rpos, lsel, rsel, currentMatch);
        case ConstantType::UINTEGER:
            return hashJoinRightTypeSwitch<NLTYPE,uint32_t,OP>(left, right, lhash, buckets, directory, lsize, lpos, rpos, lsel, rsel, currentMatch);
        case ConstantType::UBIGINT:
            return hashJoinRightTypeSwitch<NLTYPE,uint64_t,OP>(left, right, lhash, buckets, directory, lsize, lpos, rpos, lsel, rsel, currentMatch);
        case ConstantType::FLOAT:
            return hashJoinRightTypeSwitch<NLTYPE,float,OP>(left, right, lhash, buckets, directory, lsize, lpos, rpos, lsel, rsel, currentMatch);
        case ConstantType::DOUBLE:
            return hashJoinRightTypeSwitch<NLTYPE,double,OP>(left, right, lhash, buckets, directory, lsize, lpos, rpos, lsel, rsel, currentMatch);
        default:
            ErrorHandler::errorNotImplemented("Unimplemented type for select operation!");
    }
    return 0;
}



template <class NLTYPE, class OP>
idx_t hashJoinTypeSwitch(Vector &left, Vector &right, Vector &lhash, Vector &buckets, directory_t& directory,
                                         idx_t lsize, idx_t &lpos, idx_t &rpos, SelectionVector &lsel, SelectionVector &rsel,
                                         idx_t currentMatch) {
    if (left.getType() == right.getType())
        return hashJoinEqualTypeSwitch<NLTYPE, OP>(left, right, lhash, buckets, directory, lsize, lpos, rpos, lsel, rsel, currentMatch);
    return hashJoinLeftTypeSwitch<NLTYPE, OP>(left, right, lhash, buckets, directory, lsize, lpos, rpos, lsel, rsel, currentMatch);
}

template <class NLTYPE>
idx_t hashJoinComparisonSwitch(Vector &left, Vector &right, Vector &lhash, Vector &buckets, directory_t& directory,
                                     idx_t lsize, idx_t &lpos, idx_t &rpos, SelectionVector &lsel, SelectionVector &rsel,
                                     idx_t currentMatch, Binop op) {
	switch (op) {
	    case EQUAL:
	        return hashJoinTypeSwitch<NLTYPE, Equals>(left, right, lhash, buckets, directory, lsize, lpos, rpos, lsel, rsel, currentMatch);
        case LESS:
	        return hashJoinTypeSwitch<NLTYPE, LessThan>(left, right, lhash, buckets, directory, lsize, lpos, rpos, lsel, rsel, currentMatch);
	    case GREATER:
	        return hashJoinTypeSwitch<NLTYPE, GreaterThan>(left, right, lhash, buckets, directory, lsize, lpos, rpos, lsel, rsel, currentMatch);
	    case LESS_OR_EQ:
	        return hashJoinTypeSwitch<NLTYPE, LessThanEquals>(left, right, lhash, buckets, directory, lsize, lpos, rpos, lsel, rsel, currentMatch);
	    case GREATER_OR_EQ:
	        return hashJoinTypeSwitch<NLTYPE, GreaterThanEquals>(left, right, lhash, buckets, directory, lsize, lpos, rpos, lsel, rsel, currentMatch);
	    case UNEQUAL:
	        return hashJoinTypeSwitch<NLTYPE, NotEquals>(left, right, lhash, buckets, directory, lsize, lpos, rpos, lsel, rsel, currentMatch);

	    default:
		    ErrorHandler::errorNotImplemented("Error, join comparison not supported");
	        return 0;
	}
}

uint16_t bloomFilter16BitVector(Vector& hash, SelectionVector& sel, idx_t size) {
    BB_ASSERT(hash.getType() == UBIGINT);
    VectorData vdata;
    hash.orrify(size, vdata);
    auto * data = (uint64_t*) vdata.data_;
    uint16_t bloom = 0;
    for (idx_t i = 0; i < size; i++) {
        auto idx = sel.getIndex(i);
        bloom16Add(bloom, data[idx]);
    }
    return bloom;
}

uint64_t bloomFilterBitVector(idx_t bloomSize, Vector& hash, SelectionVector& sel, idx_t size) {
    BB_ASSERT(hash.getType() == UBIGINT);
    uint64_t bloom ;
    switch (bloomSize) {
        case 16:
            return bloomFilter16BitVector(hash, sel, size);
        case 0:
            return 0;
        default:
            ErrorHandler::errorNotImplemented("Error, bloom filter size not supported");
    }
    return 0;
}




JoinHashTable::JoinHashTableStats::DataChunkSel::DataChunkSel(DataChunk &chunk, Vector& hash, SelectionVector &sel, idx_t size):
    sel_(sel),
    hash_(hash.getType()),
    size_(size){
    chunk_.initializeEmpty(chunk.getTypes());
    chunk_.reference(chunk);
    hash_.reference(hash);
}

JoinHashTable::JoinHashTable(Predicate *predicate, const vector<idx_t> &keys, idx_t buckets): predicate_(predicate),
    keys_(keys), buckets_(nextPowerOfTwo(buckets)), stats_(nextPowerOfTwo(buckets)), mutex_(predicate->getArity()) {
    // check that buckets is power of 2
    BB_ASSERT(buckets_ != 0 && (buckets_ & (buckets_ - 1)) == 0);
    int bits = 64 - BLOOM_SIZE;
    auto maxValue = (1ULL << bits) - 1ULL;
    // check the buckets can fit in a uint of 64 - BLOOM_SIZE bits (as BLOOM_SIZE bits are used for bloom)
    BB_ASSERT(buckets < maxValue);
}


JoinHashTable::JoinHashTableStats::JoinHashTableStats(idx_t buckets):bucketMutex_(buckets), bucketChunks_(buckets), bucketSize_(buckets) {
    for (idx_t i = 0; i < buckets; i++) {
        bucketSize_[i].store(0);
    }
}

Vector JoinHashTable::calculateBucketVector(Vector &hash, idx_t size) {
    Vector bucket(hash.getType(), size);
    Value maskValue(buckets_-1);
    Vector mask(maskValue);
    BB_ASSERT(mask.getVectorType() == VectorType::CONSTANT_VECTOR);
    VectorOperations::lAnd(hash, mask, bucket, size);
    return bucket;
}

void JoinHashTable::addDataChunkSel(Vector& hash, DataChunk &chunk) {
    BB_ASSERT(hash.getType() == UBIGINT);

    // calculate the bucket vector
    Vector buckets = calculateBucketVector(hash, chunk.getSize());
    BB_ASSERT(buckets.getVectorType() == VectorType::FLAT_VECTOR);
    BB_ASSERT(buckets.getType() == UBIGINT);
    std::unordered_map<idx_t, vector<idx_t>> bucketSelection;

    // collect the idx for each bucket
    auto data = FlatVector::getData<uint64_t>(buckets);
    for (idx_t i = 0; i < chunk.getSize(); i++) {
        bucketSelection[data[i]].push_back(i);
    }

    // store the stats
    for (auto& [bucket, idxs] : bucketSelection) {
        SelectionVector sel(idxs.size());
        for (idx_t i = 0; i < idxs.size(); i++)
            sel.setIndex(i, idxs[i]);

        incrementBucketSize(bucket, idxs.size());

        std::lock_guard lock(stats_.bucketMutex_[bucket]);
        stats_.bucketChunks_[bucket].emplace_back(chunk, hash, sel, idxs.size());
    }
}

void JoinHashTable::initDirectory() {
    directory_ = directory_t(new uint64_t[buckets_]);
    // init the values of the directory based on the size of the buckets
    auto& bs = stats_.bucketSize_;
    idx_t totalSize = 0;
    directory_[0] = bs[0].load();
    for (idx_t i = 1; i < buckets_; i++) {
        directory_[i] = directory_[i-1] + bs[i].load();
    }

    // init the big data chunk
    for (idx_t i = 0; i < stats_.bucketChunks_.size(); i++) {
        //find first chunk and init the data types
        if (stats_.bucketChunks_[i].size() == 0)continue;
        auto types = stats_.bucketChunks_[i][0].chunk_.getTypes();
        chunkone_.initialize(types);
        break;
    }
    auto size = directory_[buckets_-1];
    chunkone_.resize(size);
    chunkone_.setCapacity(size);
    chunkone_.setCardinality(size);
}


void JoinHashTable::build(idx_t bucket, vector<vector_data_mngr_ptr_t>& stringVectorsDataMngr) {
    BB_ASSERT(bucket < stats_.bucketChunks_.size());
    auto bucketOffset = dirBegin(directory_.get(), bucket);
    uint64_t accBloom = 0; // bloom across all chunks
    for (idx_t i = 0; i < stats_.bucketChunks_[bucket].size(); i++) {
        auto& stat = stats_.bucketChunks_[bucket][i];
        auto& dc = stat.chunk_;
        auto& sel = stat.sel_;
        auto size = stat.size_;
        auto& hash = stat.hash_;

        for (idx_t j = 0; j< dc.columnCount(); j++) {
            auto& sourceVector = dc.data_[j];
            Vector targetVector(chunkone_.data_[j]);
            if (targetVector.getType() == STRING) {
                // change the data aux vector to the input vector to avoid collision
                BB_ASSERT(j < stringVectorsDataMngr.size());
                BB_ASSERT(stringVectorsDataMngr[j]);
                BB_ASSERT(stringVectorsDataMngr[j]->getType() ==VectorDataMngrType::STRING_BUFFER);
                targetVector.setAuxiliary(stringVectorsDataMngr[j]);
            }
            if (sourceVector.getVectorType() != VectorType::CONSTANT_VECTOR)
                VectorOperations::copy(sourceVector,targetVector,sel,size, 0, bucketOffset);
            else
                // because is constant vector pass the zero selection vector
                VectorOperations::copy(sourceVector,targetVector,ConstantVector::ZERO_SELECTION_VECTOR,size, 0, bucketOffset);

        }
        // calculate the bloom filter and add it in the directory
        accBloom |= bloomFilterBitVector(BLOOM_SIZE, hash, sel, size);
        bucketOffset += size;
    }
    directory_[bucket] = dirEnd(directory_.get(), bucket) | (accBloom << BLOOM_SHIFT);
}

void JoinHashTable::build(idx_t start, idx_t end) {
    // create the string data manager to be merged at the end of the build phase
    vector<vector_data_mngr_ptr_t> stringVectorsDataMngr;
    for (idx_t i=0;i<chunkone_.columnCount();++i) {
        if (chunkone_.data_[i].getType() == STRING) {
            if (stringVectorsDataMngr.empty())
                stringVectorsDataMngr.resize(chunkone_.columnCount());
            stringVectorsDataMngr[i] = vector_data_mngr_ptr_t(new StringDataMngr());
        }
    }

    BB_ASSERT(start < stats_.bucketSize_.size());
    BB_ASSERT(end < stats_.bucketSize_.size());
    BB_ASSERT(start <= end);

    for (idx_t bucket = start; bucket <= end; bucket++) {
        build(bucket, stringVectorsDataMngr);
    }

    mergeStringDataMngr(stringVectorsDataMngr);
}

void JoinHashTable::mergeStringDataMngr(vector<vector_data_mngr_ptr_t>& stringDataMngrs) {
    for (idx_t i = 0; i < chunkone_.columnCount(); i++) {
        if (chunkone_.data_[i].getType() != STRING) continue;
        lock_guard lock(mutex_[i]);
        BB_ASSERT(i < stringDataMngrs.size());
        BB_ASSERT(stringDataMngrs[i]);
        BB_ASSERT(stringDataMngrs[i]->getType() ==VectorDataMngrType::STRING_BUFFER);
        if (!chunkone_.data_[i].getAuxiliary()) {
            chunkone_.data_[i].setAuxiliary(stringDataMngrs[i]);
            continue;
        }
        auto& strDataMngr = (StringDataMngr&)*chunkone_.data_[i].getAuxiliary();
        strDataMngr.addHeapReference(stringDataMngrs[i]);
    }
}

void JoinHashTable::clearStats() {
    stats_.bucketSize_.clear();
    stats_.bucketChunks_.clear();
    stats_.bucketMutex_.clear();
}

DataChunk & JoinHashTable::getDataChunk() {
    return chunkone_;
}

string JoinHashTable::toString() {
    string result = "HT "+predicate_->toString() + ":\n\tdirectory: ";
    for (idx_t i = 0; i < buckets_; i++) {
        result += std::to_string(directory_[i]) + ", ";
    }
    result += "\nchunkone: ";
    result += chunkone_.toString();
    return result;
}

directory_t & JoinHashTable::getDirectory() {
    return directory_;
}

void JoinHashTable::setReady() {
    ready_ = true;
}

bool JoinHashTable::isReady() {
    return ready_;
}

idx_t JoinHashTable::getBuckets() {
    return buckets_;
}

bool JoinHashTable::checkKeys(const vector<idx_t>& keys) {
    return compareVectors<idx_t>(keys, keys_);
}

idx_t JoinHashTable::probe(idx_t &lpos, idx_t &rpos, DataChunk &lchunk, Vector &lhash, SelectionVector &lsel,
                           SelectionVector &rsel, const vector<Expression> &conditions) {
    BB_ASSERT(!conditions.empty());
    // accept only simplified conditions with 2 columns
    for (auto& condition : conditions)
        BB_ASSERT(condition.left_.cols_.size() == 1
            && condition.left_.cols_.size() == condition.right_.cols_.size());

    auto& rchunk = chunkone_;
    // calculate the bucket vector
    Vector buckets = calculateBucketVector(lhash, lchunk.getSize());
    BB_ASSERT(buckets.getType() == UBIGINT);


    if (lpos >= lchunk.getSize() || rpos >= rchunk.getSize()) {
        // set the lpos at the end
        lpos = lchunk.getSize();
        return 0;
    };
    // for first tuple we execute the InitHashLoopJoin to init the selection vectors
    auto& icondition = conditions[0];
    auto lidx = icondition.left_.cols_[0];
    auto ridx = icondition.right_.cols_[0];

    BB_ASSERT(rchunk.columnCount() > ridx);
    auto matchCount = hashJoinComparisonSwitch<InitHashJoin>( lchunk.data_[lidx], rchunk.data_[ridx], lhash, buckets, directory_,
        lchunk.getSize(),lpos, rpos, lsel, rsel, 0, icondition.op_);


    for (idx_t i = 1; i < conditions.size(); i++) {
        if (matchCount == 0)
            return 0;
        auto& condition = conditions[i];
        lidx = condition.left_.cols_[0];
        ridx = condition.right_.cols_[0];

        // now refine the join with the condition
        matchCount = hashJoinComparisonSwitch<RefineHashJoin>( lchunk.data_[lidx], rchunk.data_[ridx], lhash, buckets, directory_,
            lchunk.getSize(),lpos, rpos, lsel, rsel, matchCount, condition.op_);

    }
    return matchCount;
}


}
