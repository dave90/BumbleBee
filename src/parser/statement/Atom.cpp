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

#include "bumblebee/parser/statement/Atom.hpp"

#include "bumblebee/common/Hash.hpp"
#include "bumblebee/common/Log.hpp"

namespace bumblebee {

Atom::Atom(Predicate* predicate, terms_vector_t &&terms, AtomType type): terms_(std::move(terms)) , negative_(false), predicate_(predicate), type_(type) {
    calculateIsGround();
}

Atom::Atom(AtomType type, bool negative): type_(type), negative_(negative), ground_(true) {
}

Atom::Atom(std::unordered_map<string, Value> &namedParams, vector<Value> &inputValues, string &externalFunctionName, terms_vector_t&& terms, bool negative):
    externalFunctionName_(std::move(externalFunctionName)),
    namedParameters_(std::move(namedParams)),
    inputValues_(std::move(inputValues)),
    terms_(std::move(terms)),
    type_(EXTERNAL),  predicate_(nullptr), negative_(negative){
}

Atom::Atom(Atom &&other) noexcept: terms_(std::move(other.terms_)),
                                   predicate_(other.predicate_),
                                   type_(other.type_),
                                   negative_(other.negative_),
                                   binops_(other.binops_),
                                   aggregates_(std::move(other.aggregates_)),
                                   aggAtoms_(std::move(other.aggAtoms_)),
                                   aggTerms_(std::move(other.aggTerms_)),
                                   externalFunctionName_(std::move(other.externalFunctionName_)),
                                   namedParameters_(std::move(other.namedParameters_)),
                                   inputValues_(std::move(other.inputValues_)){
    calculateIsGround();
}

Atom::Atom(terms_vector_t &&terms, Binop binop) : terms_(std::move(terms)), type_(AtomType::BUILTIN) {
    binops_.push_back(binop);
    calculateIsGround();
}

// Single aggregate constructor (with guards)
Atom::Atom(AggregateFunctionType aggFunction, Binop firstBinop, Binop secondBinop, Term &lowerGuard, Term &upperGuard,
    terms_vector_t &&aggTerms, vector<Atom> &&aggAtoms, terms_vector_t &&aggGroupTerms):
    aggTerms_(std::move(aggTerms)),
    aggAtoms_(std::move(aggAtoms)), predicate_(nullptr), type_(AGGREGATE){
    aggregates_.push_back(aggFunction);
    // terms_ stores: [lower_guard, upper_guard, group_terms...]
    terms_.push_back(std::move(lowerGuard));
    terms_.push_back(std::move(upperGuard));
    for (auto& term : aggGroupTerms)
        terms_.push_back(std::move(term));
    binops_.push_back(firstBinop);
    binops_.push_back(secondBinop);
}

// Multi-aggregate constructor
Atom::Atom(vector<AggregateFunctionType>&& aggFunctions, terms_vector_t&& assignTerms, terms_vector_t &&aggTerms,
    vector<Atom> &&aggAtoms, terms_vector_t &&aggGroupTerms): aggregates_(std::move(aggFunctions)),
    aggTerms_(std::move(aggTerms)),
    aggAtoms_(std::move(aggAtoms)), predicate_(nullptr), type_(AGGREGATE){
    // terms_ stores: [assign_term_0, assign_term_1, ..., group_terms...]
    for (auto& term : assignTerms)
        terms_.push_back(std::move(term));
    for (auto& term : aggGroupTerms)
        terms_.push_back(std::move(term));
    binops_.push_back(ASSIGNMENT);
    binops_.push_back(NONE_OP);
}

terms_vector_t& Atom::getTerms() {
    return terms_;
}

terms_vector_t & Atom::getAggTerms() {
    return aggTerms_;
}

terms_vector_t Atom::getAggGroupTerms() {
    terms_vector_t groupTerms;
    if (type_ != AGGREGATE) return groupTerms;

    if (isMultiAggregate()) {
        // Multi-aggregate: terms_[0..n-1] are assignments, terms_[n..] are groups
        idx_t numAssign = aggregates_.size();
        for (idx_t i = numAssign; i < terms_.size(); ++i)
            groupTerms.push_back(terms_[i]);
    } else {
        // Single aggregate: terms_[2..] are explicit group terms
        for (idx_t i = 2; i < terms_.size(); ++i)
            groupTerms.push_back(terms_[i]);
    }
    return groupTerms;
}

bool Atom::hasExplicitGroups() const {
    if (type_ != AGGREGATE) return false;
    if (isMultiAggregate()) {
        // Multi-aggregate: has groups if terms_.size() > aggregates_.size()
        return terms_.size() > aggregates_.size();
    }
    // Single aggregate: has groups if terms_.size() > 2
    return terms_.size() > 2;
}

vector<Atom> & Atom::getAggsAtoms() {
    return aggAtoms_;
}

const terms_vector_t& Atom::getTerms() const {
    return terms_;
}
Predicate* Atom::getPredicate() {
    return predicate_;
}

void Atom::getPredicates(predicates_ptr_set_t &predicates) {
    if (type_ == AtomType::CLASSICAL) {
        predicates.insert(predicate_);
    }else if (type_ == AGGREGATE) {
        for (auto& atom : aggAtoms_)
            atom.getPredicates(predicates);
    } else if (type_ == EXTERNAL && predicate_) {
        predicates.insert(predicate_);
    }
}


AtomType Atom::getType() const {
    return type_;
}

void Atom::setType(AtomType type) {
    type_ = type;
}

bool Atom::isNegative() const {
    return negative_;
}

void Atom::setNegative(bool negative) {
    negative_ = negative;
}

Binop Atom::getBinop() const {
    if (binops_.empty())
        return NONE_OP;
    return binops_[0];
}

void Atom::setBinop(Binop binop) {
    if (binops_.empty())
        binops_.push_back(NONE_OP);
    binops_[0] = binop;
}

Binop Atom::getSecondBinop() const {
    if (binops_.size() < 1) return NONE_OP;
    return binops_[1];
}

Binop Atom::getBinop(idx_t i) const {
    if (i >= binops_.size()) return NONE_OP;
    return binops_[i];
}

void Atom::setSecondBinop(Binop binop) {
    while (binops_.size() < 2)
        binops_.push_back(NONE_OP);
    binops_[1] = binop;
}

bool Atom::containsAnonymous() const {
    for (auto& term : terms_)
        if (term.containsAnonymous())
            return true;
    return false;
}

bool Atom::containsRange() const {
    for (auto& term : terms_)
        if (term.getType() == TermType::RANGE)
            return true;
    return false;
}

bool Atom::containsArith() const {
    for (auto& term : terms_)
        if (term.getType() == TermType::ARITH)
            return true;
    return false;
}

bool Atom::containsConstant() const {
    for (auto& term : terms_)
        if (term.getType() == TermType::CONSTANT)
            return true;
    return false;
}

bool Atom::containsVariables(set_term_variable_t &vars) {
    set_term_variable_t atomVars, result;
    getVariables(atomVars);
    Term::intersetVariables(atomVars, vars, result);
    return result.size();
}

bool Atom::isOrBuiltin() const{
    return (type_ == BUILTIN && binops_.size() > 1);
}

void Atom::addBuiltin(Atom &atom) {
    BB_ASSERT(atom.getType() == BUILTIN);
    BB_ASSERT(!atom.isOrBuiltin());
    binops_.push_back(atom.getBinop());
    for (auto& term: atom.terms_) {
        terms_.push_back(std::move(term));
    }
}

idx_t Atom::getBuiltinsSize() const {
    if (type_ != BUILTIN) return 0;
    return binops_.size();
}

vector<BuiltinTerms> Atom::getBuiltinTerms() {
    vector<BuiltinTerms> terms;
    for (idx_t i = 0; i < getBuiltinsSize(); ++i) {
        terms.emplace_back(terms_[i*2], terms_[i*2+1]);
    }
    return terms;
}

void Atom::setTerms(terms_vector_t &terms) {
    terms_ = std::move(terms);
}

void Atom::setBinops(vector<Binop> &ops) {
    binops_ = std::move(ops);
}

bool Atom::isConstantAssignment() {
    if (type_ != BUILTIN) return false;
    if (isOrBuiltin()) return false;
    if (getBinop() != EQUAL && getBinop() != ASSIGNMENT) return false;
    auto lType = terms_[0].getType();
    auto rType = terms_[1].getType();

    return (lType == CONSTANT && rType == VARIABLE) ||
           (lType == VARIABLE && rType == CONSTANT);

}

bool Atom::isAggregateAssignment() {
    if (getType() != AGGREGATE) return false;
    if (getBinop() == ASSIGNMENT || getSecondBinop() == ASSIGNMENT) return true;
    return false;
}

vector<PhysicalType> Atom::getTermsPhysicalTypes() {
    vector<PhysicalType> types;
    types.reserve(terms_.size());
    for (auto& term : terms_) {
        if (term.getType() == TermType::NONE_TERM) continue;
        types.push_back(term.getPhysicalType());
    }
    return types;
}

void Atom::replaceVariable(const string &var,const string &newVar) {
    for (auto& term : terms_) {
        term.replaceVariable(var, newVar);
    }
}

string Atom::getAggregateFunctionName() {
    BB_ASSERT(getType() == AGGREGATE);
    BB_ASSERT(!aggregates_.empty());
    BB_ASSERT(aggregates_.size() == 1);
    return Atom::getAggFunction(aggregates_[0]);
}

vector<string> Atom::getAggregateFunctionNames() {
    BB_ASSERT(getType() == AGGREGATE);
    vector<string> names;
    for (auto& agg : aggregates_)
        names.push_back(Atom::getAggFunction(agg));
    return names;
}

vector<AggregateFunctionType>& Atom::getAggregateFunctions() {
    return aggregates_;
}

idx_t Atom::getNumAggregateFunctions() const {
    return aggregates_.size();
}

bool Atom::isMultiAggregate() const {
    return type_ == AGGREGATE && aggregates_.size() > 1;
}

terms_vector_t Atom::getAssignmentTerms() {
    terms_vector_t assignTerms;
    if (type_ != AGGREGATE) return assignTerms;

    if (isMultiAggregate()) {
        // Multi-aggregate: terms_[0..n-1] are assignment terms
        for (idx_t i = 0; i < aggregates_.size(); ++i)
            assignTerms.push_back(terms_[i]);
    } else {
        // Single aggregate: terms_[0] is the assignment term (lower_guard)
        if (getBinop() == ASSIGNMENT && !terms_.empty())
            assignTerms.push_back(terms_[0]);
    }
    return assignTerms;
}

string Atom::getExternalFunctionName() {
    return externalFunctionName_;
}

std::unordered_map<string, Value> & Atom::getNamedParamters() {
    return namedParameters_;
}

vector<Value> & Atom::getInputValues() {
    return inputValues_;
}

vector<LogicalType> Atom::getInputValuesType() const{
    vector<LogicalType> types;
    for (auto& v:inputValues_)
        types.push_back(v.getPhysicalType());
    return types;
}

Atom & Atom::operator=(Atom &&other) noexcept {
    if (this == &other)
        return *this;
    terms_ = std::move(other.terms_);
    predicate_ = std::move(other.predicate_);
    type_ = other.type_;
    negative_ = other.negative_;
    binops_ = other.binops_;
    ground_ = other.ground_;
    aggregates_ = std::move(other.aggregates_);
    aggAtoms_ = std::move(other.aggAtoms_);
    aggTerms_ = std::move(other.aggTerms_);
    namedParameters_ = std::move(other.namedParameters_);
    inputValues_ = std::move(other.inputValues_);
    externalFunctionName_ = std::move(other.externalFunctionName_);
    return *this;
}



bool operator==(const Atom &lhs, const Atom &rhs) {
    return  lhs.terms_ == rhs.terms_
           && lhs.predicate_ == rhs.predicate_
           && lhs.type_ == rhs.type_
           && lhs.negative_ == rhs.negative_
           && lhs.binops_ == rhs.binops_;
}

bool operator!=(const Atom &lhs, const Atom &rhs) {
    return !(lhs == rhs);
}

hash_t Atom::hash() {
    hash_t hash = predicate_->hash();
    for (auto& term : terms_) {
        CombineHash(hash, term.hash());
    }
    return hash;
}

void Atom::getVariables(set_term_variable_t &variables) {
    if (getType() == AGGREGATE) {
        if (isMultiAggregate()) {
            // Multi-aggregate: terms_[0..n-1] are assignments, terms_[n..] are groups
            // Include all assignment terms
            for (idx_t i = 0; i < aggregates_.size(); ++i)
                terms_[i].getVariables(variables);
            // Include explicit group terms
            for (idx_t i = aggregates_.size(); i < terms_.size(); ++i)
                terms_[i].getVariables(variables);
        } else {
            // Single aggregate: terms_[0] = lower_guard, terms_[1] = upper_guard, terms_[2..] = groups
            if (getBinop() != NONE_OP)
                terms_[0].getVariables(variables);
            if (getSecondBinop() != NONE_OP)
                terms_[1].getVariables(variables);
            // Include explicit group terms
            for (idx_t i = 2; i < terms_.size(); ++i)
                terms_[i].getVariables(variables);
        }
        return;
    }
    for (auto& term : terms_) {
        term.getVariables(variables);
    }
}

void Atom::getVariablesList(vector<string> &variables) {
    if (getType() == AGGREGATE) {
        if (isMultiAggregate()) {
            // Multi-aggregate: include all assignment terms
            for (idx_t i = 0; i < aggregates_.size(); ++i)
                terms_[i].getVariablesList(variables);
        } else {
            // Single aggregate
            if (getBinop() != NONE_OP)
                terms_[0].getVariablesList(variables);
            if (getSecondBinop() != NONE_OP)
                terms_[1].getVariablesList(variables);
        }
        return;
    }
    for (auto& term : terms_) {
        term.getVariablesList(variables);
    }
}

void Atom::getAggAtomsVariables(set_term_variable_t &variables) {
    for (auto& atom: aggAtoms_) {
        atom.getVariables(variables);
    }
}

void Atom::getAggSharedVariables(const set_term_variable_t &globalVariables, set_term_variable_t &sharedVariables) {
    set_term_variable_t internals;
    getAggAtomsVariables(internals);
    Term::intersetVariables(internals, globalVariables, sharedVariables);
    // Remove explicit group terms - they are bound by the aggregate body, not external atoms
    if (hasExplicitGroups()) {
        auto groupTerms = getAggGroupTerms();
        for (auto& term : groupTerms) {
            if (term.getType() == TermType::VARIABLE)
                sharedVariables.erase(term.getVariable());
        }
    }
}

void Atom::setPredicte(Predicate *predicate) {
    BB_ASSERT(type_ == EXTERNAL); // only external atoms can set predicate after the creation
    predicate_ = predicate;
}

bool Atom::isGround() {
    return ground_;
}

Term & Atom::operator[](unsigned i) {
    return terms_[i];
}

std::string Atom::toString() const {
    if (type_ == AtomType::CLASSICAL) {
        std::string s = negative_ ? "not " : "";
        s += predicate_->toString() + "(";
        if (!terms_.empty()) s += terms_[0].toString();
        for (unsigned i = 1; i < terms_.size(); ++i) {
            s += ","+terms_[i].toString();
        }
        return s+")";
    }
    if (type_ == AtomType::BUILTIN ) {
        string result = "";
        idx_t idx = 0;
        BB_ASSERT(terms_.size() == binops_.size() * 2);
        for (idx_t i = 0; i < getBuiltinsSize(); ++i) {
            if (idx++ > 0) result += " OR ";
            result += terms_[i*2].toString() + " " + getBinopStr(binops_[i]) + " " + terms_[i*2+1].toString();
        }
        return result;
    }
    if (type_ == AGGREGATE) {
        string s ="";

        // Handle multi-aggregate: "Min,Sum = #[min,sum]{...}"
        if (aggregates_.size() > 1) {
            // Show all assignment terms on the left side
            for (idx_t i = 0; i < aggregates_.size(); ++i) {
                if (i > 0) s += ",";
                s += terms_[i].toString();
            }
            s += " = #[";
            for (idx_t i = 0; i < aggregates_.size(); ++i) {
                if (i > 0) s += ",";
                s += getAggFunction(aggregates_[i]).substr(1); // remove leading '#'
            }
            s += "]";
        } else {
            // Single aggregate
            if (getBinop() != NONE_OP)
                s += terms_[0].toString() + " " + getBinopStr(getBinop()) + " ";
            s += getAggFunction(aggregates_[0]);
        }

        s += "{";
        for (idx_t i = 0; i < aggTerms_.size(); ++i) {
            if (i > 0) s += ",";
            s += aggTerms_[i].toString();
        }

        // Add explicit groups if present
        auto groupTerms = const_cast<Atom*>(this)->getAggGroupTerms();
        if (!groupTerms.empty()) {
            s += ";";
            for (idx_t i = 0; i < groupTerms.size(); ++i) {
                if (i > 0) s += ",";
                s += groupTerms[i].toString();
            }
        }

        s += ":";
        for (idx_t i = 0; i < aggAtoms_.size(); ++i) {
            if (i > 0) s += ",";
            s += aggAtoms_[i].toString();
        }
        s += "}";

        // For single aggregate with upper guard
        if (aggregates_.size() == 1 && getSecondBinop() != NONE_OP)
            s += " " + getBinopStr(getSecondBinop()) + " " + terms_[1].toString();

        return s;
    }
    if (type_ == EXTERNAL) {
        std::string s = negative_ ? "not " : "";
        s += externalFunctionName_ + "(";
        for (auto& v: inputValues_) {
            s += v.toString() + ",";
        }
        if (!inputValues_.empty())s.pop_back(); // remove last ,
        s += ";";
        for (auto& [k,v]:namedParameters_) {
             s += k + "="+v.toString() + ",";
        }
        if (!namedParameters_.empty())s.pop_back(); // remove last ,
        s += ";";
        if (!terms_.empty()) s += terms_[0].toString();
        for (unsigned i = 1; i < terms_.size(); ++i) {
            s += ","+terms_[i].toString();
        }
        return s+")";
    }
    ErrorHandler::errorNotImplemented("Atom type not implemented");
    return "";
}

void Atom::calculateIsGround() {
    if (type_ == AGGREGATE) {
        ground_ = false;
        return;
    }
    for (auto &term : terms_) {
        if (!term.isGround()) {
            ground_ = false;
            return;
        }
    }
    ground_ = true;
}

Atom Atom::clone() const {
    terms_vector_t terms;
    for (auto& term : terms_)
        terms.emplace_back(term);

    switch (type_) {
        case AGGREGATE: {
            vector<Atom> aggAtoms;
            for (auto& atom : aggAtoms_) {
                Atom cloned = atom.clone();
                aggAtoms.push_back(std::move(cloned));
            }
            terms_vector_t aggTerms;
            for (auto& term : aggTerms_)
                aggTerms.emplace_back(term);

            if (isMultiAggregate()) {
                // Multi-aggregate: terms_[0..n-1] are assignment, terms_[n..] are groups
                idx_t numAssign = aggregates_.size();
                terms_vector_t assignTerms;
                for (idx_t i = 0; i < numAssign; ++i)
                    assignTerms.emplace_back(terms_[i]);
                terms_vector_t aggGroupTerms;
                for (idx_t i = numAssign; i < terms_.size(); ++i)
                    aggGroupTerms.emplace_back(terms_[i]);
                vector<AggregateFunctionType> aggFunctions(aggregates_);
                return createMultiAggregateAtom(std::move(aggFunctions), std::move(assignTerms), std::move(aggTerms), std::move(aggAtoms), std::move(aggGroupTerms));
            } else {
                // Single aggregate: terms_[0]=lower, terms_[1]=upper, terms_[2..]=groups
                terms_vector_t aggGroupTerms;
                for (idx_t i = 2; i < terms_.size(); ++i)
                    aggGroupTerms.emplace_back(terms_[i]);
                BB_ASSERT(terms.size() >= 2);
                return createAggregateAtom(aggregates_[0], getBinop(), getSecondBinop(), terms[0], terms[1], std::move(aggTerms), std::move(aggAtoms), std::move(aggGroupTerms));
            }
        }case BUILTIN: {
            Atom cloned;
            cloned.setType(BUILTIN);
            cloned.terms_ = std::move(terms);
            cloned.binops_ = binops_;
            return cloned;
        }case CLASSICAL: {
            Atom newAtom = createClassicalAtom(predicate_, std::move(terms));
            newAtom.setNegative(negative_);
            return newAtom;
        }
        case EXTERNAL: {
            std::unordered_map<string, Value> namedParameters;
            for (auto& param : namedParameters_)
                namedParameters.insert(std::make_pair(param.first, param.second.cast(param.second.getPhysicalType())));
            vector<Value> inputValues;
            for (auto& val: inputValues_)
                inputValues.emplace_back(std::move(val.cast(val.getPhysicalType())))
            ;
            auto externalFunctionName = externalFunctionName_;
            return createExternalAtom(namedParameters, inputValues, externalFunctionName, std::move(terms));
        }
    }
    return {};
}

Atom Atom::createClassicalAtom(Predicate*p, terms_vector_t &&t) {
    return Atom(p, std::move(t), AtomType::CLASSICAL);
}

Atom Atom::createBuiltinAtom(terms_vector_t &&t, Binop binop) {
    return Atom(std::move(t), binop);
}

Atom Atom::createAggregateAtom(AggregateFunctionType aggFunction, Binop firstBinop, Binop secondBinop, Term &lowerGuard,
    Term &upperGuard, terms_vector_t &&aggTerms, vector<Atom> &&aggAtoms, terms_vector_t &&aggGroupTerms) {
    return Atom(aggFunction, firstBinop, secondBinop, lowerGuard, upperGuard, std::move(aggTerms), std::move(aggAtoms), std::move(aggGroupTerms));
}

Atom Atom::createMultiAggregateAtom(vector<AggregateFunctionType>&& aggFunctions, terms_vector_t&& assignTerms,
    terms_vector_t &&aggTerms, vector<Atom> &&aggAtoms, terms_vector_t &&aggGroupTerms) {
    return Atom(std::move(aggFunctions), std::move(assignTerms), std::move(aggTerms), std::move(aggAtoms), std::move(aggGroupTerms));
}

Atom Atom::createExternalAtom(std::unordered_map<string, Value> &namedParams, vector<Value> &inputValues,
    string &externalFunctionName, terms_vector_t&& terms) {
    return Atom(namedParams, inputValues, externalFunctionName, std::move(terms));
}

Atom Atom::createOrBuiltinAtom(vector<Atom>& builtins) {
    Atom newAtom;
    newAtom.setType(BUILTIN);
    for (auto& builtin: builtins)
        newAtom.addBuiltin(builtin);
    return newAtom;
}

string Atom::getAggFunction(AggregateFunctionType agg) {
    switch (agg) {
        case NONE:
            return "";
        case MIN:
            return "#min";
        case MAX:
            return "#max";
        case AVG:
            return "#avg";
        case SUM:
            return "#sum";
        case COUNT:
            return "#count";
    }
}


AggregateFunctionType Atom::getAggFunction(const char* aggFunction) {
    if (aggFunction == nullptr) return NONE;

    std::string s(aggFunction);
    if (s == "min")   return MIN;
    if (s == "max")   return MAX;
    if (s == "avg")   return AVG;
    if (s == "sum")   return SUM;
    if (s == "count") return COUNT;

    return NONE; // default if no match
}

bool Atom::isEqualBuiltins(Term &lhs1, Term &rhs1, Binop op1, Term &lhs2, Term &rhs2, Binop op2) {
    if (lhs1 == lhs2 && rhs1 == rhs2 && op1 == op2) return true;
    if (lhs1 == rhs2 && rhs1 == lhs2 && op1 == getFlippedBinop(op2)) return true;
    return false;
}
}
