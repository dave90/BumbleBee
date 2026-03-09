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

#include "Predicate.hpp"
#include "Term.hpp"


namespace bumblebee {

enum AggregateFunctionType : uint8_t {
    NONE = 0,
    COUNT = 1,
    MAX = 2,
    MIN = 3,
    SUM = 4,
    AVG = 5
};


enum AtomType {
    CLASSICAL = 0,
    BUILTIN = 1,
    AGGREGATE = 2,
    EXTERNAL = 3
};

struct BuiltinTerms {
    BuiltinTerms(Term &left, Term &right)
        : left(left),
          right(right) {
    }

    Term& left;
    Term& right;
};

using terms_vector_t = vector<Term>;
//TODO Remove the && as function parameter
class Atom {
public:

    Atom() = default;
    Atom(Predicate* predicate, terms_vector_t &&terms, AtomType type);
    Atom(terms_vector_t &&terms, Binop binop);
    // Single aggregate constructor (with guards)
    Atom(AggregateFunctionType aggFunction, Binop firstBinop, Binop secondBinop, Term& lowerGuard, Term& upperGuard, terms_vector_t&& aggTerms, vector<Atom>&& aggAtoms, terms_vector_t&& aggGroupTerms = {});
    // Multi-aggregate constructor: assignment terms, aggregate terms, body atoms, explicit groups
    Atom(vector<AggregateFunctionType>&& aggFunctions, terms_vector_t&& assignTerms, terms_vector_t&& aggTerms, vector<Atom>&& aggAtoms, terms_vector_t&& aggGroupTerms = {});
    Atom(AtomType type, bool negative);
    Atom(std::unordered_map<string, Value> & namedParams, vector<Value>& inputValues,string& externalFunctionName, terms_vector_t&& terms, bool negative = false);

    Atom(const Atom &other) = delete;
    Atom(Atom &&other) noexcept;
    ~Atom() = default;

    Atom & operator=(const Atom &other) = delete;
    Atom & operator=(Atom &&other) noexcept;
    Binop getBinop() const;
    Binop getSecondBinop() const;
    Binop getBinop(idx_t i) const;

    hash_t hash();
    void getVariables(set_term_variable_t &variables);
    void getVariablesList(vector<string> &variables);
    void getAggAtomsVariables(set_term_variable_t &variables);
    void getAggSharedVariables(const set_term_variable_t& globalVariables,set_term_variable_t& sharedVariables);
    bool isGround();
    std::string toString() const;
    Atom clone() const;


    friend bool operator==(const Atom &lhs, const Atom &rhs);
    friend bool operator!=(const Atom &lhs, const Atom &rhs);
    inline Term& operator[](unsigned i);

    terms_vector_t& getTerms();
    terms_vector_t& getAggTerms();
    terms_vector_t getAggGroupTerms();
    bool hasExplicitGroups() const;
    vector<Atom>& getAggsAtoms();
    const terms_vector_t& getTerms()const;
    Predicate* getPredicate();
    void getPredicates(predicates_ptr_set_t &predicates);
    AtomType getType() const;
    void setType(AtomType type);
    bool isNegative() const;
    void setNegative(bool negative);
    void setBinop(Binop binop);
    void setSecondBinop(Binop binop);
    bool containsAnonymous() const;
    bool containsRange() const;
    bool containsArith() const;
    bool containsConstant() const;
    bool containsVariables(set_term_variable_t& vars);
    // return true if is constant assignment , i.e X == 10
    bool isOrBuiltin() const;
    // construct list of builtin OR
    void addBuiltin(Atom& atom);
    idx_t getBuiltinsSize() const;
    vector<BuiltinTerms> getBuiltinTerms();
    void setTerms(terms_vector_t& terms);
    void setBinops(vector<Binop> &ops);

    bool isConstantAssignment();
    bool isAggregateAssignment();
    vector<PhysicalType> getTermsPhysicalTypes();
    void replaceVariable(const string& var, const string& newVar);
    string getAggregateFunctionName();
    vector<string> getAggregateFunctionNames();
    vector<AggregateFunctionType>& getAggregateFunctions();
    idx_t getNumAggregateFunctions() const;
    bool isMultiAggregate() const;
    terms_vector_t getAssignmentTerms();

    string getExternalFunctionName();
    std::unordered_map<string, Value>& getNamedParamters();
    vector<Value>& getInputValues();
    vector<LogicalType> getInputValuesType() const;
    void setPredicte(Predicate* predicate);

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
    // Binops for list or builtins and aggregate atoms
    vector<Binop> binops_;
    // True if it is ground
    bool ground_{false};

    // Aggregate fields
    vector<AggregateFunctionType> aggregates_;
    // Atoms to aggregate
    vector<Atom> aggAtoms_;
    // Aggregation terms
    terms_vector_t aggTerms_;
    // Note: For aggregates, terms_ stores [lower_guard, upper_guard, group_terms...]
    // If terms_.size() > 2, the extra terms are explicit group terms

    // External atoms
    std::unordered_map<string, Value> namedParameters_;
    vector<Value> inputValues_;
    string externalFunctionName_;

public:
    // static functions
    static Atom createClassicalAtom(Predicate* p, terms_vector_t&& t);
    static Atom createBuiltinAtom(terms_vector_t&& t, Binop binop);
    // Single aggregate factory (with guards)
    static Atom createAggregateAtom(AggregateFunctionType aggFunction, Binop firstBinop, Binop secondBinop, Term& lowerGuard, Term& upperGuard, terms_vector_t&& aggTerms, vector<Atom>&& aggAtoms, terms_vector_t&& aggGroupTerms = {});
    // Multi-aggregate factory
    static Atom createMultiAggregateAtom(vector<AggregateFunctionType>&& aggFunctions, terms_vector_t&& assignTerms, terms_vector_t&& aggTerms, vector<Atom>&& aggAtoms, terms_vector_t&& aggGroupTerms = {});
    static string getAggFunction(AggregateFunctionType agg);
    static AggregateFunctionType getAggFunction(const char* agg);
    static Atom createExternalAtom(std::unordered_map<string, Value> & namedParams, vector<Value>& inputValues,string& externalFunctionName, terms_vector_t&& t);
    static Atom createOrBuiltinAtom(vector<Atom>& builtins);
    static bool isEqualBuiltins(Term& lhs1, Term& rhs1, Binop op1,Term& lhs2, Term& rhs2, Binop op2 );

};

}
