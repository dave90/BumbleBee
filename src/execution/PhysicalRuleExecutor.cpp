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
#include "bumblebee/execution/PhysicalRuleExecutor.hpp"

namespace bumblebee{
PhysicalRuleExecutor::PhysicalRuleExecutor(prule_ptr_t prule, ThreadContext* context): prule_(std::move(prule)), tcontext_(context) {
    atomResults_.resize(prule_->patoms_.size());

    states_.reserve(prule_->patoms_.size());
    chunks_.reserve(prule_->patoms_.size());
    initializeStates();
    initializeChunks();
}

void PhysicalRuleExecutor::initializeStates() {
    for (auto& patom : prule_->patoms_) {
        states_.push_back(patom->getState());
    }
    sourceState_ = prule_->source_->getState();
    sinkState_ = prule_->sink_->getState();
}

void PhysicalRuleExecutor::initializeChunks() {
    chunks_.emplace_back(new DataChunk());
    chunks_.back()->initializeEmpty(prule_->source_->getTypes());

    for (auto& patom : prule_->patoms_) {
        chunks_.emplace_back(new DataChunk());
        chunks_.back()->initializeEmpty(patom->getTypes());
    }

}

void PhysicalRuleExecutor::fetchFromSource(DataChunk &input) {
    sourceResult_ = prule_->source_->getData(*tcontext_, input, *sourceState_, *prule_->sourceGlobalState_);
}

void PhysicalRuleExecutor::finalize() {
    if (prule_->isFinalized()) return;
    prule_->source_->finalize(*tcontext_, *prule_->sourceGlobalState_);
    prule_->sink_->finalize(*tcontext_, *prule_->sinkGlobalState_);
    prule_->incrementCompleted();
    prule_->setFinalized();
}

void PhysicalRuleExecutor::execute() {
    while (!finished_) {

        fetchFromSource(*chunks_[0]);
        if (sourceResult_ == AtomResultType::FINISHED) {
            // no more data to source
            // flush the sink passing an empty data chunk
            chunks_[chunks_.size()-1]->setCardinality(0);
            while (sinkResult_ == AtomResultType::HAVE_MORE_OUTPUT)
                sinkResult_ = prule_->sink_->sink(*tcontext_, *chunks_[chunks_.size()-1], *sinkState_, *prule_->sinkGlobalState_);
            finished_ = true;
            break;
        }
        executePush();

    }
    prule_->incrementCompleted();
}


void PhysicalRuleExecutor::executePush() {
    if (prule_->patoms_.size() == 0) {
        // no intermediate patoms, sink immediately
        sinkResult_ = prule_->sink_->sink(*tcontext_, *chunks_[0], *sinkState_, *prule_->sinkGlobalState_);
        if (sinkResult_ == AtomResultType::FINISHED) {
            // the sink requested to end
            finished_ = true;
        }
        return;
    }

    // current index of the patom to exeecute
    idx_t idx = 0;
    // queue of index of patom that have more output, to execute again with same input
    vector<idx_t> idxMoreOutput;
    while (true) {

        auto result = prule_->patoms_[idx]->execute(*tcontext_, *chunks_[idx], *chunks_[idx + 1], *states_[idx]);
        if (result == AtomResultType::FINISHED) {
            // the patom request to end
            finished_ = true;
            break;
        }
        if (result == AtomResultType::HAVE_MORE_OUTPUT) {
            idxMoreOutput.push_back(idx);
        }
        atomResults_[idx++] = result;

        if (idx == prule_->patoms_.size() ) {
            // we reach the end of the pipeline, call the sink
            sinkResult_ = prule_->sink_->sink(*tcontext_, *chunks_[idx], *sinkState_, *prule_->sinkGlobalState_);
            if (sinkResult_ == AtomResultType::FINISHED) {
                // the sink requested to end
                finished_ = true;
                break;
            }
            if (idxMoreOutput.empty()) {
                // no patom with more output to call, so break the push and ask to the source more input
                break;
            }
            // pop back the index of the last atom that have more output
            idx = idxMoreOutput.back();
            idxMoreOutput.pop_back();
        }
    }
}

}
