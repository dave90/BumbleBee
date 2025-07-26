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
#include <vector>

#include "Predicate.h"
#include "Term.h"


namespace bumblebee {

enum AtomType {
    CLASSICAL = 0,
    BUILTIN = 1,
};

using terms_vector_t = std::vector<Term>;

class Atom {
public:

    Atom() = default;
    Atom(Predicate* predicate, terms_vector_t &&terms, AtomType type);
    Atom(terms_vector_t &&terms, Binop binop);
    Atom(AtomType type, bool negative);
    Atom(const Atom &other) = delete;
    Atom(Atom &&other) noexcept;
    ~Atom() = default;

    Atom & operator=(const Atom &other) = delete;
    Atom & operator=(Atom &&other) noexcept;
    Binop getBinop() const;

    hash_t hash();
    void getVariables(set_term_variable_t &variables);
    bool isGround();
    std::string toString() const;


    friend bool operator==(const Atom &lhs, const Atom &rhs);
    friend bool operator!=(const Atom &lhs, const Atom &rhs);
    inline Term& operator[](unsigned i);

    terms_vector_t& getTerms();
    Predicate* getPredicate();
    void getPredicates(predicates_ptr_set_t &predicates);
    AtomType getType() const;
    void setType(AtomType type);
    bool isNegative() const;
    void setNegative(bool negative);
    void setBinop(Binop binop);
    bool containsAnonymous() const;
    bool containsRange() const;
    bool containsArith() const;
    bool containsConstant() const;
    // return true if is constant assignment , i.e X == 10
    bool isConstantAssignment();
    std::vector<ConstantType> getTermsCType();
    void replaceVariable(const string& var, const string& newVar);

    inline const Value& getValue(idx_t idx) const {
        return terms_[idx].getValue();
    }


private:
    void calculateIsGround();

    terms_vector_t terms_;
    // pointer to predicate, do not own the predicate lifetime
    Predicate* predicate_;
    AtomType type_;
    bool negative_{false};
    // If it is a builtin the binop operation
    Binop binop_;
    // True if it is ground
    bool ground_{false};

public:
    // static functions
    static Atom createClassicalAtom(Predicate* p, terms_vector_t&& t);
    static Atom createBuiltinAtom(terms_vector_t&& t, Binop binop);
    static std::string getBinop(Binop binop);
};

}
