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

#include <climits>

#include "bumblebee/parser/ParserInputBuilder.hpp"

#include "CLI11.hpp"
#include "bumblebee/ClientContext.hpp"
#include "bumblebee/common/Helper.hpp"
#include "bumblebee/common/Log.hpp"
#include "bumblebee/common/StringUtils.hpp"

namespace bumblebee {


ParserInputBuilder::ParserInputBuilder(OutputType type, ClientContext& context):
    currentSchema_(Catalog::instance().getDefaultSchema()),
    hiddenNewPredicate(!context.printAll_),
    distinctNewPredicate_(context.distinct_),
    bufferManager_(*context.bufferManager_),
    clientContext_(context),
    output_builder_(type) {
}

ParserInputBuilder::~ParserInputBuilder() {
}

void ParserInputBuilder::onDirective(char *directiveName, char *directiveValue) {
    if (foundASafetyError_) return;

    string dName = directiveName;
    if (dName == "#") {
        foundASafetyError_ = true;
        safetyErrorMessage = "Error: empty directive. Please provide a directive name immediately after '#', without spaces.";
        return;
    }

    auto lower = StringUtils::lower(dName);

    if (lower == "#limit")
        parseLimitDirective(directiveValue);
    else if (lower== "#order_by")
        parseOrderByDirective(directiveValue);
    else {
        foundASafetyError_ = true;
        safetyErrorMessage = "Error: not supported directive: "+dName;
    }

}

void ParserInputBuilder::parseLimitDirective(const string &value) {
    try {
        size_t idx;
        int v = std::stoi(value, &idx);

        if (idx != value.size()) {
            foundASafetyError_ = true;
            safetyErrorMessage = "invalid value for limit directive: '" + value + "'. Please specify a positive number.";
            return;
        }

        if (v > STANDARD_VECTOR_SIZE) {
            foundASafetyError_ = true;
            safetyErrorMessage = "limit > "+std::to_string(STANDARD_VECTOR_SIZE)+" is not currently supported :(";
            return;
        }
        limit_ = v;
    }
    catch (const std::exception &) {
        foundASafetyError_ = true;
        safetyErrorMessage = "invalid value for limit directive: '" + value + ". Please specify a positive number.";
    }
}

void ParserInputBuilder::parseOrderByDirective(const string &value) {
    string trimValue = StringUtils::trim(value);
    std::stringstream ss(trimValue);
    string token;

    while (getline(ss, token, ',')) { // Split by ';'
        if (token.empty()) continue;
        string tokenTrimmed = StringUtils::trim(token);
        string col;
        string modifier;
        auto pos = tokenTrimmed.find(' ');
        if (pos != string::npos) {
            col = tokenTrimmed.substr(0, pos);
            modifier = tokenTrimmed.substr(pos + 1);

        }else {
            modifier = "asc";
            col = tokenTrimmed;
        }
        orderModifiers_.push_back(OrderModifiers::parse(modifier));
        colsOrderModifiers_.push_back(col);
    }
}

bool ParserInputBuilder::checkRuleSafety() {
    return checkRuleSafety(currentRule);
}

bool ParserInputBuilder::checkRuleSafety(Rule& currentRule) {
    // check if rule is safe
    set_term_variable_t toCheckVars, bodyVars;
    for (auto&a : currentRule.getHead())a.getVariables(toCheckVars);
    for (auto&a : currentRule.getBody())
        if (a.isNegative())
            a.getVariables(toCheckVars);
    for (auto&a : currentRule.getBody())
        if (!a.isNegative())
            a.getVariables(bodyVars);
    // Check all the vars in head and in negative atoms are present in body
    for (auto& v : toCheckVars)
        if (bodyVars.find(v) == bodyVars.end()) {
            currentRuleIsUnsafe_ = true;
            break;
        }
    // check that range are not in the rule
    for (auto&a : currentRule.getHead())
        if (a.containsRange())
            currentRuleIsUnsafe_ = true;
    for (auto&a : currentRule.getBody())
        if (a.containsRange())
            currentRuleIsUnsafe_ = true;

    if(currentRuleIsUnsafe_){
        safetyErrorMessage="--> Safety Error: "+currentRule.toString();
        foundASafetyError_=true;
        return false;
    }
    return true;
}

void ParserInputBuilder::onRule() {
    if(foundASafetyError_) return;
    if (currentRule.isFact()) {
        Atom fact = std::move(currentRule.getHead()[0]);
        // print if is not internal atom
        auto& pt = currentSchema_.get().getPredicateTable(fact.getPredicate());
        pt->addFact(fact);
        currentRule = {};
        return;
    }
    if (!checkRuleSafety()) return;
    for (auto& a: currentRule.getBody())
        if (a.getType() == AGGREGATE) {
            rulesWithAggregates_.push_back(program_.size());
            break;
        }

    setRuleDirective();

    program_.push_back(std::move(currentRule));
    currentRule = {};
}


void ParserInputBuilder::setRuleDirective() {
    if (limit_ > 0) {
        currentRule.setLimit(limit_);
        limit_ = 0;
    }
    if (!colsOrderModifiers_.empty()) {
        BB_ASSERT(colsOrderModifiers_.size() == orderModifiers_.size());
        std::unordered_map<string, idx_t> varColIndex;
        BB_ASSERT(currentRule.getHead().size() == 1);
        auto& head = currentRule.getHead()[0];
        if (head.getType() != CLASSICAL) {
            foundASafetyError_ = true;
            safetyErrorMessage = "order by directive support only classical atom.";
            return;
        }
        for (idx_t i=0;i < head.getTerms().size();++i) {
            if (head.getTerms()[i].getType() != VARIABLE)continue;
            varColIndex[head.getTerms()[i].getVariable()] = i;
        }


        vector<ColModifier> colModifiers;
        for (idx_t i = 0; i < colsOrderModifiers_.size(); ++i) {
            auto var = colsOrderModifiers_[i];
            if (!varColIndex.contains(var)) {
                foundASafetyError_ = true;
                safetyErrorMessage = "order by variable: '"+var+"' not found in the head. Please specify a variable in the head.";
                return;
            }

            // decrement as 0 is not found
            colModifiers.push_back({.col_ = varColIndex[var], .modifier_ = orderModifiers_[i]});
        }

        currentRule.setModifiers(colModifiers);
        colModifiers.clear();
    }

    if (!currentRule.getModifiers().empty() && !currentRule.getLimit()) {
        foundASafetyError_ = true;
        safetyErrorMessage = "unsupported order by directive without a limit. Please add a #limit directive.";
    }
}

void ParserInputBuilder::onConstraint() {
    if (!checkRuleSafety()) return;
    program_.push_back(std::move(currentRule));
    currentRule = {};
}

void ParserInputBuilder::onWeakConstraint() {
}

void ParserInputBuilder::onQuery() {
    currentAtom.getPredicate()->setInternal(false);
}

void ParserInputBuilder::onHeadAtom() {
    if(foundASafetyError_) return;
    if (currentAtom.containsAnonymous() || currentAtom.getType() == AGGREGATE || currentAtom.getType() == BUILTIN)
        currentRuleIsUnsafe_=true;
    if (currentAtom.getType() == EXTERNAL) {
        // create a predicate to get processed by statement dependency
        string name =  currentAtom.getExternalFunctionName() + "_" + std::to_string(extAtomCounter_++);
        Predicate *predicate = currentSchema_.get().createPredicate(&clientContext_ ,name.c_str(), terms_parsered.size());
        predicate->setInternal(true);
        currentAtom.setPredicte(predicate);
    }
    currentRule.addAtomInHead(std::move(currentAtom));
}

void ParserInputBuilder::onHead() {
}

void ParserInputBuilder::onBodyLiteral() {
    if(foundASafetyError_) return;
    currentRule.addAtomInBody(std::move(currentAtom));
}

void ParserInputBuilder::onBody() {
}

void ParserInputBuilder::onNafLiteral(bool naf) {
    if(foundASafetyError_) return;
    currentAtom.setNegative(naf);
    if (naf && currentAtom.containsAnonymous())currentRuleIsUnsafe_=true;
}

void ParserInputBuilder::onAtom(bool isStrongNeg) {
}

void ParserInputBuilder::onExistentialAtom() {
}

void ParserInputBuilder::onPredicateName(char *name) {
    if(foundASafetyError_) return;

    Predicate *predicate = currentSchema_.get().createPredicate(&clientContext_ ,name, terms_parsered.size());
    if (!hiddenNewPredicate) {
        predicate->setInternal(false);
    }
    if (distinctNewPredicate_) {
        predicate->setDistinct();
    }

    currentAtom = Atom::createClassicalAtom(predicate, std::move(terms_parsered));
    terms_parsered.clear();
}

void ParserInputBuilder::onExistentialVariable(char *var) {
}

void ParserInputBuilder::onEqualOperator() {
    binop_ = Binop::EQUAL;
}

void ParserInputBuilder::onUnequalOperator() {
    binop_ = Binop::UNEQUAL;
}

void ParserInputBuilder::onLessOperator() {
    binop_ = Binop::LESS;
}

void ParserInputBuilder::onLessOrEqualOperator() {
    binop_ = Binop::LESS_OR_EQ;
}

void ParserInputBuilder::onGreaterOperator() {
    binop_ = Binop::GREATER;
}

void ParserInputBuilder::onGreaterOrEqualOperator() {
    binop_ = Binop::GREATER_OR_EQ;
}

void ParserInputBuilder::onTerm(char *value) {
    if(foundASafetyError_) return;
    newTerm(value);
}


void ParserInputBuilder::onUnknownVariable() {
    if(foundASafetyError_) return;
    std::string s("_");
    Term term = Term::createVariable(std::move(s));
    terms_parsered.push_back(std::move(term));
}

void ParserInputBuilder::onFunction(char *functionSymbol, int nTerms) {
}

void ParserInputBuilder::onHeadTailList() {
}

void ParserInputBuilder::onListTerm(int nTerms) {
}

void ParserInputBuilder::onTermDash() {
    if(foundASafetyError_) return;

    Term& term = terms_parsered.back();
    term.setNegative(true);
    if (term.getType() == CONSTANT && term.getPhysicalType() != PhysicalType::STRING) {
        // multiply the constant numeric value to -1
        auto newValue = term.getValue().cast(PhysicalType::BIGINT).getNumericValue<int64_t>() * -1;
        Term::setConstantNumericTerm(term, newValue);
        return;
    }
    if (term.getType() == ARITH) {
        // multiply the arith term by -1
        int8_t mone = -1;
        Term moneTerm = Term(mone);
        term.addInArithTermBegin(std::move(moneTerm), Operator::TIMES);
        return;
    }
    if (term.getType() == VARIABLE) {
        // transform the variable term in arith and multiply by -1
        int8_t mone = -1;
        Term moneTerm = Term(mone);
        Term newArith = Term::createArith(std::move(term), std::move(moneTerm), '*');
        terms_parsered.pop_back();
        terms_parsered.push_back(std::move(newArith));
    }
    if (term.getType() == RANGE) {
        auto& interval = term.getInterval();
        interval.from = interval.from*-1;
    }
}

void ParserInputBuilder::onTermParams() {
    if (foundASafetyError_) return;
    if (!terms_parsered.empty() && terms_parsered.back().getType() == ARITH)
        terms_parsered.back().setParenthesized(true);
}

void ParserInputBuilder::onTermRange(char *lowerBound, char *upperBound) {
    if(foundASafetyError_) return;

    Term t = Term::createRange(atoi(lowerBound), atoi(upperBound));
    terms_parsered.push_back(std::move(t));
}

void ParserInputBuilder::onArithmeticOperation(char arithOperator) {
    if(foundASafetyError_) return;

    auto lt = std::move(terms_parsered.back());
    terms_parsered.pop_back();
    auto slt = std::move(terms_parsered.back());
    terms_parsered.pop_back();

    bool sltIsArith = slt.getType() == ARITH;
    bool ltIsArith = lt.getType() == ARITH;

    // Case : neither is ARITH - create new ARITH term
    if (!sltIsArith && !ltIsArith) {
        auto t = Term::createArith(std::move(slt), std::move(lt), arithOperator);
        terms_parsered.push_back(std::move(t));
        return;
    }

    // Case : if either side is parenthesized or both are ARITH, keep nested
    if ((sltIsArith && slt.isParenthesized()) ||
        (ltIsArith && lt.isParenthesized()) ||
        (sltIsArith && ltIsArith)) {
        auto t = Term::createArith(std::move(slt), std::move(lt), arithOperator);
        terms_parsered.push_back(std::move(t));
        return;
    }

    // Case : left (slt) is ARITH (not parenthesized), right (lt) is not ARITH
    if (sltIsArith && !ltIsArith) {
        slt.addInArithTerm(std::move(lt), arithOperator);
        terms_parsered.push_back(std::move(slt));
        return;
    }

    // Case : right (lt) is ARITH (not parenthesized), left (slt) is not ARITH
    // Fix ordering: prepend slt at the beginning
    Operator op = Term::getOperator(arithOperator);
    lt.addInArithTermBegin(std::move(slt), op);
    terms_parsered.push_back(std::move(lt));
}

void ParserInputBuilder::onWeightAtLevels(int nWeight, int nLevel, int nTerm) {
}

void ParserInputBuilder::onChoiceLowerGuard() {
}

void ParserInputBuilder::onChoiceUpperGuard() {
}

void ParserInputBuilder::onChoiceElementAtom() {
}

void ParserInputBuilder::onChoiceElementLiteral() {
}

void ParserInputBuilder::onChoiceElement() {
}

void ParserInputBuilder::onChoiceAtom() {
}

void ParserInputBuilder::onBuiltinAtom() {
    if(foundASafetyError_) return;

    auto atom = Atom::createBuiltinAtom(std::move(terms_parsered), binop_);
    builtin_atoms.push_back(std::move(atom));
    terms_parsered.clear();
}

void ParserInputBuilder::onBuiltinOrList() {
    if(foundASafetyError_) return;
    BB_ASSERT(builtin_atoms.size() > 0);
    if (builtin_atoms.size() == 1) {
        currentAtom = std::move(builtin_atoms[0]);
    }else {
        currentAtom = Atom::createOrBuiltinAtom(builtin_atoms);
    }
    builtin_atoms.clear();
}

void ParserInputBuilder::onAggregateLowerGuard() {
    if(foundASafetyError_) return;
    if (guard_terms.size() != 2)
        guard_terms.resize(2);
    if (terms_parsered.back().containsAnonymous())
        currentRuleIsUnsafe_ = true;
    guard_terms[0] = std::move(terms_parsered.back());
    terms_parsered.pop_back();
    aggBinop_ = binop_;
}

void ParserInputBuilder::onAggregateUpperGuard() {
    if(foundASafetyError_) return;
    if (guard_terms.size() != 2)
        guard_terms.resize(2);
    if (terms_parsered.back().containsAnonymous())
        currentRuleIsUnsafe_ = true;
    secondBinop_ = binop_;
    binop_ = NONE_OP;
    guard_terms[1] = std::move(terms_parsered.back());
    terms_parsered.pop_back();
    aggSecondBinop_ = secondBinop_;
}

void ParserInputBuilder::onAggregateFunction(char *functionSymbol) {
    AggregateFunctionType func;
    if (functionSymbol[0] == '#')
        func = Atom::getAggFunction(++functionSymbol); // skip #
    else
        func = Atom::getAggFunction(functionSymbol);
    aggregateFunctions_.push_back(func);
}

void ParserInputBuilder::onAggregateGroundTerm(char *value, bool dash) {
    if(foundASafetyError_) return;
    newTerm(value);
    agg_terms_parsered.push_back(std::move(terms_parsered.back()));
    terms_parsered.pop_back();
}

void ParserInputBuilder::onAggregateVariableTerm(char *value) {
    if(foundASafetyError_) return;
    string var(value);
    agg_terms_parsered.emplace_back(std::move(var), true);
}

void ParserInputBuilder::onAggregateUnknownVariable() {
    currentRuleIsUnsafe_ = true;
}

void ParserInputBuilder::onAggregateFunctionalTerm(char *value, int nTerms) {
}

void ParserInputBuilder::onAggregateNafLiteral() {
    if(foundASafetyError_) return;
    agg_atoms.push_back(std::move(currentAtom));
}

void ParserInputBuilder::onAggregateElement() {
    if(foundASafetyError_) return;
}

void ParserInputBuilder::onAggregateGroupSemicolon() {
    if(foundASafetyError_) return;
    // When we see ';', agg_terms_parsered contains the agg terms (before ';')
    // Save them temporarily to agg_group_terms_parsered
    // After this, the parser will parse group terms into agg_terms_parsered
    agg_group_terms_parsered = std::move(agg_terms_parsered);
    agg_terms_parsered.clear();
}

// If agg_terms contains #ID, keep only the first numToKeep non-#ID terms and #ID.
// #ID disables distinct calculation, making extra terms unnecessary overhead.
static void filterAggTermsWithID(vector<Term>& agg_terms, size_t numToKeep) {
    bool hasID = false;
    for (const auto& t : agg_terms) {
        if (t.getType() == VARIABLE && t.getVariable() == Predicate::INTERNAL_ID_VAR) {
            hasID = true;
            break;
        }
    }
    if (!hasID) return;

    vector<Term> filtered;
    filtered.reserve(numToKeep + 1);
    size_t kept = 0;
    for (auto& t : agg_terms) {
        if (t.getType() == VARIABLE && t.getVariable() == Predicate::INTERNAL_ID_VAR)
            continue;
        if (kept < numToKeep) {
            filtered.push_back(std::move(t));
            ++kept;
        }
    }
    filtered.emplace_back(string(Predicate::INTERNAL_ID_VAR), true);
    agg_terms = std::move(filtered);
}

void ParserInputBuilder::onAggregate(bool naf) {
    if(foundASafetyError_) return;

    // If we had explicit groups (saw ';'), then:
    // - agg_terms_parsered contains group terms (parsed after ';')
    // - agg_group_terms_parsered contains agg terms (parsed before ';')
    // We need to swap them for correct semantics
    if (!agg_group_terms_parsered.empty()) {
        std::swap(agg_terms_parsered, agg_group_terms_parsered);
    }


    BB_ASSERT(aggregateFunctions_.size() == 1);
    filterAggTermsWithID(agg_terms_parsered, 1);

    currentAtom = Atom::createAggregateAtom(aggregateFunctions_[0], aggBinop_, aggSecondBinop_, guard_terms[0], guard_terms[1], std::move(agg_terms_parsered), std::move(agg_atoms), std::move(agg_group_terms_parsered));
    aggBinop_ = NONE_OP;
    aggSecondBinop_ = NONE_OP;
    guard_terms.clear();
    agg_atoms.clear();
    agg_terms_parsered.clear();
    agg_group_terms_parsered.clear();
    aggregateFunctions_.clear();
}

void ParserInputBuilder::onMultiAssignVariable(char* var) {
    if(foundASafetyError_) return;
    // Create a variable term and add to multi-assignment terms list
    string varStr(var);
    multi_assign_terms_.emplace_back(std::move(varStr), true);
}

void ParserInputBuilder::onMultiAggregateAssignment() {
    if(foundASafetyError_) return;

    // If we had explicit groups (saw ';'), then swap terms
    if (!agg_group_terms_parsered.empty()) {
        std::swap(agg_terms_parsered, agg_group_terms_parsered);
    }

    filterAggTermsWithID(agg_terms_parsered, aggregateFunctions_.size());

    // Validate: number of assignment terms must match number of aggregate functions
    if (multi_assign_terms_.size() != aggregateFunctions_.size()) {
        foundASafetyError_ = true;
        safetyErrorMessage = "Number of assignment variables (" + std::to_string(multi_assign_terms_.size()) +
            ") must match number of aggregate functions (" + std::to_string(aggregateFunctions_.size()) + ").";
        return;
    }

    currentAtom = Atom::createMultiAggregateAtom(std::move(aggregateFunctions_), std::move(multi_assign_terms_),
        std::move(agg_terms_parsered), std::move(agg_atoms), std::move(agg_group_terms_parsered));

    aggBinop_ = NONE_OP;
    aggSecondBinop_ = NONE_OP;
    guard_terms.clear();
    agg_atoms.clear();
    agg_terms_parsered.clear();
    agg_group_terms_parsered.clear();
    aggregateFunctions_.clear();
    multi_assign_terms_.clear();
}

void ParserInputBuilder::onEnd() {
}

rules_vector_t & ParserInputBuilder::getProgram() {
    return program_;
}

void ParserInputBuilder::newTerm(char * value) {
    if(foundASafetyError_) return;

    if( value[0] >= 'A' && value[0] <='Z' ) // Variable
    {
        std::string s(value);
        Term term = Term::createVariable(std::move(s));
        terms_parsered.push_back(std::move(term));
        return;
    }
    if( (value[0] == '\"' && value[strlen(value)-1] == '\"') ||
            (value[0] == '\'' && value[strlen(value)-1] == '\'') ||
            (value[0] >= 'a' && value[0] <='z') )   // String constant
    {
        std::string s(value);
        if (value[0] == '\"' || value[0] == '\'')
            // remove the quote
            s = s.substr(1, s.size() - 2);

        Term term = Term(std::move(s));
        terms_parsered.push_back(std::move(term));
        return;
    }

    if (strchr(value, '.') != nullptr) {
        // contains dot is a decimal
        double num = strtod(value, nullptr);
        Term term(num);
        terms_parsered.push_back(std::move(term));
        return;
    }

    // Numeric constant
    long long num = atoll(value);
    if (num >= 0) {
        // Positive number
        unsigned long long unum = strtoull(value, nullptr, 10);
        Term term  = Term::createSmallestConstantNumericTerm(unum);
        terms_parsered.push_back(std::move(term));
        return;
    }
    Term term  = Term::createSmallestConstantNumericTerm(num);
    terms_parsered.push_back(std::move(term));
}

bool ParserInputBuilder::isFoundASafetyError() {
    return foundASafetyError_;
}

const std::string & ParserInputBuilder::getSafetyErrorMessage() {
    return safetyErrorMessage;
}


void ParserInputBuilder::onExtAtom(bool naf) {
    currentAtom = Atom::createExternalAtom(namedParameters_, inputValues_, externalFunctionName_, std::move(terms_parsered));
    // check if the external pred exist
    if (!clientContext_.functionRegister_.getFunction(currentAtom.getExternalFunctionName(), currentAtom.getInputValuesType())) {
        foundASafetyError_ = true;
        safetyErrorMessage = "Error, external function "+currentAtom.getExternalFunctionName()+" with parameters [ ";
        for (auto t: currentAtom.getInputValuesType())
            safetyErrorMessage += t.toString()+" ";
        safetyErrorMessage += "] does not exist.";
        return;
    }

    namedParameters_.clear();
    inputValues_.clear();
    externalFunctionName_.clear();
    terms_parsered.clear();
    externalSemicolumnCount_ = 0;
}

void ParserInputBuilder::onSemicolon() {
    BB_ASSERT(externalSemicolumnCount_ < 3);
    switch (externalSemicolumnCount_) {
        case 0: {
            // input values
            for (auto& t: terms_parsered) {
                auto& val = t.getValue();
                inputValues_.push_back(std::move(val));
            }
            terms_parsered.clear();
        }case 1: {
            // named paramters
            for (idx_t i=0;i<terms_parsered.size();i=i+2) {
                auto& key = terms_parsered[i].getValue();
                auto& value = terms_parsered[i+1].getValue();
                if (key.getPhysicalType() != PhysicalType::STRING) {
                    safetyErrorMessage = "Error, key named parameter should be a string, received: "+key.toString();
                    foundASafetyError_ = true;
                    return;
                }
                namedParameters_.insert({key.toString(), std::move(value)});
            }
            terms_parsered.clear();
        }
    }
    externalSemicolumnCount_++;
}

void ParserInputBuilder::onExternalPredicateName(char* name) {
    externalFunctionName_ = name;
    externalFunctionName_ = "&"+externalFunctionName_;
}

void ParserInputBuilder::onNamedParameter() {

}


/* ------------------------------------------------------------------------
* SQL PARSER
*------------------------------------------------------------------------
*/

sql::SQLStatement& ParserInputBuilder::getSqlStatement() {
    BB_ASSERT(!sqlStatements_.empty());
    return sqlStatements_[0];
}

bool ParserInputBuilder::isSQL() {
    return isSql_;
}

void ParserInputBuilder::onSQLValue(char *value) {
    if(foundASafetyError_) return;

    if(strcmp(value, "*") == 0 || (value[0] >= 'A' && value[0] <='Z') ) // Column name
    {
        std::string s(value);
        Value v(std::move(s));
        sqlValuePrimary_.emplace_back(v);
        return;
    }
    if( (value[0] == '\"' && value[strlen(value)-1] == '\"') ||
            (value[0] == '\'' && value[strlen(value)-1] == '\'') ||
            (value[0] >= 'a' && value[0] <='z') )   // String constant
    {
        std::string s(value);
        if (value[0] == '\"' || value[0] == '\'')
            // remove the quote
            s = s.substr(1, s.size() - 2);
        Value v(std::move(s));
        sqlValuePrimary_.emplace_back(v);
        return;
    }
    if (strchr(value, '.') != nullptr) {
        // contains dot is a decimal
        double num = strtod(value, nullptr);
        Value v(num);
        sqlValuePrimary_.emplace_back(v);
        return;
    }
    // Numeric constant
    long long num = atoll(value);
    if (num >= 0) {
        // Positive number
        unsigned long long unum = strtoull(value, nullptr, 10);
        Value v(unum);
        sqlValuePrimary_.emplace_back(v);
        return;
    }
    Value v(num);
    sqlValuePrimary_.emplace_back(v);

}

void ParserInputBuilder::onSQLQualifiedName(char *name, char *table) {
    if (strcmp(name, "*") == 0) {
        Value v("*");
        sqlValuePrimary_.emplace_back(v);
        return;
    }
    sql::QualifiedName qName{.name_ = name};
    if (table)
        qName.table_ = table;

    sqlValuePrimary_.emplace_back(qName);
}

void ParserInputBuilder::onSQLMulTerm(char op) {
    if (mulExpr_.getOperators().empty()) {
        auto vp1 = std::move(sqlValuePrimary_.back());
        sqlValuePrimary_.pop_back();
        auto vp2 = std::move(sqlValuePrimary_.back());
        sqlValuePrimary_.pop_back();
        mulExpr_.addValuePrimary(vp2);
        mulExpr_.addValuePrimary(vp1);
        mulExpr_.addOperator(getCharOperator(op));
        return;
    }
    mulExpr_.addValuePrimary(sqlValuePrimary_.back());
    sqlValuePrimary_.pop_back();
    mulExpr_.addOperator(getCharOperator(op));
}

void ParserInputBuilder::onSQLFinalizeMulExpr() {
    if (!mulExpr_.getOperators().empty()) {
        sqlValuePrimary_.push_back(sql::ValuePrimary(std::move(mulExpr_)));
        mulExpr_.clear();
    }
}

void ParserInputBuilder::onSQLAddTerm(char op) {
    if (valueExpr_.getOperators().empty()) {
        auto vp1 = std::move(sqlValuePrimary_.back());
        sqlValuePrimary_.pop_back();
        auto vp2 = std::move(sqlValuePrimary_.back());
        sqlValuePrimary_.pop_back();
        valueExpr_.addValuePrimary(vp2);
        valueExpr_.addValuePrimary(vp1);
        valueExpr_.addOperator(getCharOperator(op));
        return;
    }
    valueExpr_.addValuePrimary(sqlValuePrimary_.back());
    sqlValuePrimary_.pop_back();
    valueExpr_.addOperator(getCharOperator(op));
}

void ParserInputBuilder::onSQLParenOpen() {
    sqlExprStack_.push_back({std::move(valueExpr_), std::move(mulExpr_)});
    valueExpr_.clear();
    mulExpr_.clear();
}

void ParserInputBuilder::onSQLParenClose() {
    if (!valueExpr_.getOperators().empty()) {
        sqlValuePrimary_.push_back(sql::ValuePrimary(std::move(valueExpr_)));
    }
    auto& saved = sqlExprStack_.back();
    valueExpr_ = std::move(saved.addExpr);
    mulExpr_ = std::move(saved.mulExpr);
    sqlExprStack_.pop_back();
}

void ParserInputBuilder::onSQLSelectItem() {

    if (!alias_.empty())
        valueExpr_.setAlias(alias_);
    if (!sqlValuePrimary_.empty()) {
        BB_ASSERT(sqlValuePrimary_.size() == 1);
        valueExpr_.addValuePrimary(sqlValuePrimary_.back());
        sqlValuePrimary_.pop_back();
    }
    alias_.clear();
    sqlStatements_.back().getSelect().addItem(valueExpr_);
    valueExpr_.clear();
}

void ParserInputBuilder::onSQLAlias(char *alias) {
    alias_ = alias;
}

void ParserInputBuilder::onSQLTableRef(char *table) {
    fromItems_.emplace_back(table);
}

void ParserInputBuilder::onSQLFromItem() {
    if (!alias_.empty())
        fromItems_.back().setAlias(alias_);
    alias_.clear();
    for (auto& fi: fromItems_)
        sqlStatements_.back().getFrom().addItem(fi);
    fromItems_.clear();
}

void ParserInputBuilder::onSQLPredicateValueExpr() {
    if (valueExpr_.getValues().empty()) {
        BB_ASSERT(!sqlValuePrimary_.empty());
        // no operations take from sqlValuePrimary_
        valueExpr_.addValuePrimary(sqlValuePrimary_.back());
        sqlValuePrimary_.pop_back();
    }
    sqlPredicate_.setValue1(valueExpr_);
    valueExpr_.clear();
}

void ParserInputBuilder::onSQLPredicateValueExprOp() {
    if (foundASafetyError_) return;
    if (valueExpr_.getValues().empty()) {
        BB_ASSERT(!sqlValuePrimary_.empty());

        // no operations take from sqlValuePrimary_
        valueExpr_.addValuePrimary(sqlValuePrimary_.back());
        sqlValuePrimary_.pop_back();
    }
    sqlPredicate_.setValue2(valueExpr_);
    valueExpr_.clear();
    sqlPredicate_.setOp(sql::toSQLBinop(binop_));
}

void ParserInputBuilder::onSQLLikePredicate() {
    if (foundASafetyError_) return;
    if (valueExpr_.getValues().empty()) {
        BB_ASSERT(!sqlValuePrimary_.empty());
        valueExpr_.addValuePrimary(sqlValuePrimary_.back());
        sqlValuePrimary_.pop_back();
    }
    sqlPredicate_.setValue2(valueExpr_);
    valueExpr_.clear();
    sqlPredicate_.setOp(sql::SQLBinop::SQL_LIKE);
}

void ParserInputBuilder::onSQLOperatorCondition(const char * op) {
    string sop = op;
    sqlStatements_.back().getWhere().addOperator(sql::Where::getOp(sop));
}

void ParserInputBuilder::onSQLPredicate() {
    sqlStatements_.back().getWhere().addItem(sqlPredicate_);
}

void ParserInputBuilder::onSQLWhereGroupBegin() {
    // Save the current (outer) WHERE context and start a fresh one for the inner group
    whereContextStack_.push_back(std::move(sqlStatements_.back().getWhere()));
    sqlStatements_.back().getWhere().clear();
}

void ParserInputBuilder::onSQLWhereGroupEnd() {
    // Package the completed inner WHERE as a WhereGroup, then restore the outer context
    auto innerWhere = std::move(sqlStatements_.back().getWhere());
    sqlStatements_.back().getWhere() = std::move(whereContextStack_.back());
    whereContextStack_.pop_back();
    sql::WhereGroup group(std::move(innerWhere));
    sqlStatements_.back().getWhere().addGroup(std::move(group));
}

void ParserInputBuilder::onSQLWhere() {

}

void ParserInputBuilder::onSQLFrom() {

}

void ParserInputBuilder::onSQLSelect() {

}

void ParserInputBuilder::onSQLStart() {
    isSql_ = true;
    sqlStatements_.emplace_back();
}

void ParserInputBuilder::onSQLSubQuery() {
    BB_ASSERT(sqlStatements_.size() > 1);
    auto& last = sqlStatements_.back();
    sqlStatements_[0].getFrom().addSubqueries(last, alias_);
    alias_.clear();
    sqlStatements_.pop_back();
}

void ParserInputBuilder::onSQLExtTableName(char* name) {
    externalFunctionName_ = name;
    externalFunctionName_ = "&"+externalFunctionName_;
    // parse the named paramters
    for (idx_t i=0;i<terms_parsered.size();i=i+2) {
        auto& key = terms_parsered[i].getValue();
        auto& value = terms_parsered[i+1].getValue();
        if (key.getPhysicalType() != PhysicalType::STRING) {
            safetyErrorMessage = "Error, key named parameter should be a string, received: "+key.toString();
            foundASafetyError_ = true;
            return;
        }
        namedParameters_.insert({key.toString(), std::move(value)});
    }
    terms_parsered.clear();
    fromItems_.emplace_back(inputValues_, externalFunctionName_, namedParameters_);
    inputValues_.clear();
    externalFunctionName_.clear();
    namedParameters_.clear();
    externalSemicolumnCount_ = 0;
}



void ParserInputBuilder::onSQLExtTableNameString(char* name) {
    string table = name;
    StringUtils::removeQuote(table);
    if (FileSystem::isCSVFile(table)) {
        externalFunctionName_ = "&read_csv";
    } else if (FileSystem::isParquetFile(table))
        externalFunctionName_ = "&read_parquet";
    else {
        foundASafetyError_ = true;
        safetyErrorMessage = "Error, external table: "+table+" not supported. Please specify a supported table type (csv or parquet).";
        return;
    }
    inputValues_.emplace_back(table);

    terms_parsered.clear();
    namedParameters_.clear();
    fromItems_.emplace_back(inputValues_, externalFunctionName_, namedParameters_);
    inputValues_.clear();
    externalFunctionName_.clear();
    namedParameters_.clear();
    externalSemicolumnCount_ = 0;
}

void ParserInputBuilder::onSQLExtTable() {
    if (!alias_.empty())
        fromItems_.back().setAlias(alias_);
    alias_.clear();
    for (auto& fi: fromItems_)
        sqlStatements_.back().getFrom().addItem(fi);
    fromItems_.clear();
}

void ParserInputBuilder::onSQLAggregateFunction(char *str) {
    string lower = StringUtils::lower(str);
    auto agg = Atom::getAggFunction(lower.c_str());
    sqlStatements_.back().getSelect().addAggFunction(agg);
}

void ParserInputBuilder::onSQLGroupByItem() {
    if (!sqlValuePrimary_.empty()) {
        BB_ASSERT(sqlValuePrimary_.size() == 1);
        valueExpr_.addValuePrimary(sqlValuePrimary_.back());
        sqlValuePrimary_.pop_back();
    }
    alias_.clear();
    sqlStatements_.back().getGroupby().addItem(valueExpr_);
    valueExpr_.clear();
}

void ParserInputBuilder::onSQLCopyTo(char *path) {
    // remove first and last char as contains double quote
    sqlExportFilePath_ = path;
    StringUtils::removeQuote(sqlExportFilePath_);
}



void ParserInputBuilder::onSQLCopy() {
    for (idx_t i=0;i<terms_parsered.size();i=i+2) {
        auto& key = terms_parsered[i].getValue();
        auto& value = terms_parsered[i+1].getValue();
        if (key.getPhysicalType() != PhysicalType::STRING) {
            safetyErrorMessage = "Error, key named parameter should be a string, received: "+key.toString();
            foundASafetyError_ = true;
            return;
        }
        namedParameters_.insert({key.toString(), std::move(value)});
    }
    terms_parsered.clear();

    sqlStatements_.back().setExportPath(sqlExportFilePath_);
    sqlStatements_.back().setExportNamedParameters(namedParameters_);
    string x = "";
}

void ParserInputBuilder::onSqlOrderModifier(char *modifier) {
    if (!modifier)
        orderModifiers_.emplace_back(OrderType::ASCENDING);
    else
        orderModifiers_.push_back(OrderModifiers::parse(modifier));
}

void ParserInputBuilder::onSqlOrderCol() {
    if (foundASafetyError_)return;
    auto vp = sqlValuePrimary_.back();
    auto q = vp.getQualifier();
    string colVar = (!q.table_.empty())?q.table_+"."+q.name_:q.name_;
    bool couldBeAlias = q.table_.empty();
    sqlValuePrimary_.pop_back();
    BB_ASSERT(!q.name_.empty());
    // let's find the qualifier in the select items
    bool find = false;
    idx_t idx = 0;
    auto& items = sqlStatements_.back().getSelect().getItems();
    for (idx_t i=0;i<items.size() && !find;i++) {
        auto& ve = items[i];
        if (ve.toString() == "*") {
            find = true; // we have a select start set find as true
        }
        if ((!ve.getAlias().empty() && ve.getAlias() == colVar) || (ve.toString() == colVar)) {
                find = true;
                idx = i;
        }

    }
    if (!find) {
        foundASafetyError_ = true;
        safetyErrorMessage = "order by variable: '"+colVar+"' not found in the select. Please specify a variable in the head.";
    }
    BB_ASSERT(!orderModifiers_.empty());
    ColModifier colModifier = {.col_ = idx, .modifier_ = orderModifiers_.back()};
    orderModifiers_.pop_back();
    sqlStatements_.back().getOrderby().addColModifier(colModifier);
}

void ParserInputBuilder::onSQLLimit(char *number) {
    if (foundASafetyError_)return;
    parseLimitDirective(number);
    if (limit_ == 0)return;
    sqlStatements_.back().setLimit(limit_);
    limit_ = 0;
    if (sqlStatements_.back().getOrderby().empty()) {
        // order by is empty because we have a limit create it
        ColModifier cm = {.col_ = 0, .modifier_ = OrderType::ASCENDING};
        sqlStatements_.back().getOrderby().addColModifier(cm);
    }
}
}

