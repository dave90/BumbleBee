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
#include "bumblebee/parser/statement/Predicate.hpp"
#include "bumblebee/common/Hash.hpp"
#include <sstream>

namespace bumblebee {

Predicate::Predicate(const char* name, unsigned arity): arity_(arity){
    auto len = strlen(name) + 1;
    name_ = std::make_unique<char[]>(len);
    std::memcpy(name_.get(), name, len);
    name_.get()[len-1] = '\0';
}

const char* Predicate::getName() const {
    return name_.get();
}

unsigned Predicate::getArity() const {
    return arity_;
}

void Predicate::setArity(unsigned arity) {
    arity_ = arity;
}

bool Predicate::isEdb() const {
    return edb_;
}

void Predicate::setEdb(bool edb) {
    edb_ = edb;
}

bool Predicate::isInternal() const {
    return internal_;
}

void Predicate::setInternal(bool internal) {
    internal_ = internal;
}

bool Predicate::isSolved() const {
    return solved_;
}

void Predicate::setSolved(bool solved) {
    solved_ = solved;
}

string Predicate::getLabel() const {
    string label = name_.get();
    return label + "::"+std::to_string(arity_);
}

bool operator==(const Predicate &lhs, const Predicate &rhs) {
    return lhs.name_ == rhs.name_
           && lhs.arity_ == rhs.arity_;
}

bool operator!=(const Predicate &lhs, const Predicate &rhs) {
    return !(lhs == rhs);
}

hash_t Predicate::hash() const{
    hash_t h1 = Hash(name_.get());
    hash_t h2 = Hash<u_int64_t>(arity_);
    return CombineHash(h1,h2);
}

std::string Predicate::toString() {
    return name_.get();
}

bool PredicateMapEntry::operator==(const PredicateMapEntry &other) const {
    return arity_ == other.arity_ && strcmp(name_, other.name_) == 0;
}

hash_t PredicateMapEntry::PEHash::operator()(const PredicateMapEntry &entry) const {
    return CombineHash(Hash(entry.name_), Hash<unsigned>(entry.arity_));
}


string Predicate::buildAggregateInternalPredicate(idx_t suffixCounter, const vector<idx_t> &groups, const vector<idx_t> &payloads,
    const vector<string> &funcNames) {
    std::ostringstream oss;
    oss << INTERNAL_PREDICATE_AGG_PREFIX<<suffixCounter;

    // Append function names
    for (const auto& fn : funcNames) {
        oss << "_" << fn;
    }

    // Append groups
    if (!groups.empty()) {
        oss << "_GROUPS";
        for (auto g : groups) {
            oss << "_" << g;
        }
    }

    // Append payloads
    if (!payloads.empty()) {
        oss << "_PAYLOADS";
        for (auto p : payloads) {
            oss << "_" << p;
        }
    }

    return oss.str();
}

void Predicate::parseAggregateInternalPredicate(const string &predName, vector<idx_t> &groups, vector<idx_t> &payloads,
    vector<string> &funcNames) {
    std::istringstream iss(predName);
    std::string token;

    // Split by '_'
    std::vector<std::string> parts;
    while (std::getline(iss, token, '_')) {
        parts.push_back(token);
    }

    enum Section { FUNCS, GROUPS, PAYLOADS };
    Section section = FUNCS;

    for (size_t i = 1; i < parts.size(); ++i) { // skip prefix (#AGG)
        const auto& part = parts[i];

        if (part == "GROUPS") {
            section = GROUPS;
            continue;
        } else if (part == "PAYLOADS") {
            section = PAYLOADS;
            continue;
        }

        switch (section) {
            case FUNCS:
                funcNames.push_back(part);
                break;
            case GROUPS:
                groups.push_back(std::stoi(part));
                break;
            case PAYLOADS:
                payloads.push_back(std::stoi(part));
                break;
        }
    }
}
}
