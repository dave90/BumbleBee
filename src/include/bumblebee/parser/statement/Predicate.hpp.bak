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
#pragma once
#include <unordered_set>

#include "bumblebee/common/Hash.h"
#include "bumblebee/common/TypeDefs.h"
#include "bumblebee/common/types/BumbleString.h"

namespace bumblebee {

class Predicate {
public:

    // internal new predicates
    static constexpr string INTERNAL_VARS_PREFIX = "#ARITH";
    constexpr static string INTERNAL_PREDICATE_AGG_PREFIX = "#AGG";
    constexpr static string INTERNAL_SOURCE_ONE_ROW = "#FASO";


    Predicate() = default;
    explicit Predicate(const char* name, unsigned arity);
    Predicate(Predicate &&other) = delete;
    ~Predicate() = default;

    Predicate(const Predicate &other) = delete;
    Predicate & operator=(const Predicate &other) = delete;
    Predicate & operator=(Predicate &&other) noexcept = delete;

    const char* getName() const;

    unsigned getArity() const;
    void setArity(unsigned arity);
    bool isEdb() const;
    void setEdb(bool edb);
    bool isInternal() const;
    void setInternal(bool internal);
    bool isSolved() const;
    void setSolved(bool solved);
    string getLabel() const;

    friend bool operator==(const Predicate &lhs, const Predicate &rhs);
    friend bool operator!=(const Predicate &lhs, const Predicate &rhs);

    hash_t  hash() const;
    std::string toString();

private:
    // name of the rpedicate
    std::unique_ptr<char[]> name_;
    // arity of the predicate
    unsigned arity_;
    // if is EDB (facts) or IDB
    bool edb_{false};
    // if is used only for internal computation (no printing or stored)
    bool internal_{true};
    // If the predicated is solved or undefined
    bool solved_{true};

public:
    //static functions
    static string buildAggregateInternalPredicate(idx_t suffixCounter, const vector<idx_t>& groups,const vector<idx_t>& payloads, const vector<string>& funcNames);
    static void parseAggregateInternalPredicate(const string& predName, vector<idx_t>& groups,vector<idx_t>& payloads, vector<string>& funcNames);
};


struct PredicateMapEntry {
    const char* name_;
    unsigned arity_;

    bool operator==(const PredicateMapEntry& other) const;

    // Functor adapter for std::unordered_map
    struct PEHash {
        hash_t operator()(const PredicateMapEntry& entry) const;
    };
};

using predicates_ptr_set_t = std::unordered_set<Predicate*>;
using predicate_ptr_t = std::unique_ptr<Predicate>;
using predicates_ptr_map_t = std::unordered_map<PredicateMapEntry, predicate_ptr_t, PredicateMapEntry::PEHash>;

}
