%{
//////////////////////////////////////////////////////////////////////////////
// aspcore2.y

//////////////////////////////////////////////////////////////////////////////
/*
This file is part of the ASPCOMP2013 ASP-Core-2 validator (validator in the following)

    The validator is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    The validator is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with the validator.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "bumblebee/parser/ParserInputDirector.h"
#include <iostream>

bool queryFound=false;


%}
%lex-param {bumblebee::ParserInputDirector& director}
%parse-param {bumblebee::ParserInputDirector& director}
%error-verbose
%union {
    char* string;
    char single_char;
    int integer;
}


%type <integer> terms
%type <single_char> arithop1
%type <single_char> arithop2
%type <string> identifier
%type <integer> list_terms_term

%token <string> SYMBOLIC_CONSTANT NUMBER VARIABLE STRING DIRECTIVE_NAME DIRECTIVE_VALUE
%token <string> AGGR_COUNT AGGR_MAX AGGR_MIN AGGR_SUM

%token ERROR NEWLINE

%token DOT DDOT SEMICOLON COLON CONS QUERY_MARK

%token PLUS TIMES SLASH BACK_SLASH

%token ANON_VAR

%token PARAM_OPEN PARAM_CLOSE
%token SQUARE_OPEN SQUARE_CLOSE
%token CURLY_OPEN CURLY_CLOSE

%token EQUAL UNEQUAL LESS GREATER LESS_OR_EQ GREATER_OR_EQ

%token DASH COMMA NAF AT WCONS AMPERSAND

%token VEL EXISTS

%left PLUS DASH
%left TIMES SLASH

%start program
%%

HEAD_SEPARATOR  : VEL;

program
    :
    | rules { }
    | error { yyerror(director,"Generic error"); }
    ;


rules
    : rule
    | rules rule
    ;

rule
    : simple_rule
    | directive
    ;

simple_rule
    : head DOT
        {
            director.getBuilder()->onRule();
        }
    | head CONS DOT
        {
            director.getBuilder()->onRule();
        }
    | head CONS body DOT
        {
            director.getBuilder()->onRule();
        }
    | CONS body DOT /*constraint*/
        {
            director.getBuilder()->onConstraint();
        }
    | WCONS body DOT weight_at_levels
        {
            director.getBuilder()->onWeakConstraint();
        }
  	| query
    	{
			if(!queryFound)
				{ queryFound=true; director.getBuilder()->onQuery(); }
			else
				{ yyerror(director,"A query has been already found"); }
		}
    ;

head
    : disjunction
        {
            director.getBuilder()->onHead();
        }
    | choice_atom
        {
            director.getBuilder()->onChoiceAtom();
            director.getBuilder()->onHead();
        }
    ;

body
    : conjunction
        {
            director.getBuilder()->onBody();
        }
    ;

weight_at_levels
    : SQUARE_OPEN term SQUARE_CLOSE
        {
            // There is only the weight. No level and terms.
            director.getBuilder()->onWeightAtLevels(1,0,0);
        }
    | SQUARE_OPEN term levels_and_terms SQUARE_CLOSE
        {
            // There are also a level and/or some terms.
            // The finalization has been postponed to "level_and_terms".
        }
    ;

levels_and_terms
    : AT term
        {
            // There is no terms following the level.
            director.getBuilder()->onWeightAtLevels(1,1,0);
        }
    | AT term COMMA terms
        {
            director.getBuilder()->onWeightAtLevels(1,1,$4);
        }
    | COMMA terms
        {
            // The level is omitted.
            // Thus, the first term, recognized as the
            // weight, should be a term of this list.
            director.getBuilder()->onWeightAtLevels(0,0,$2+1);
        }
    ;

disjunction
    : classic_literal
        {
            director.getBuilder()->onHeadAtom();
        }
    | disjunction HEAD_SEPARATOR classic_literal
        {
            director.getBuilder()->onHeadAtom();
        }
    | existential_atom
        {
            director.getBuilder()->onHeadAtom();
        }
    | disjunction HEAD_SEPARATOR existential_atom
        {
            director.getBuilder()->onHeadAtom();
        }
    ;

conjunction
    : naf_literal_aggregate
        {
            director.getBuilder()->onBodyLiteral();
        }
    | conjunction COMMA naf_literal_aggregate
        {
            director.getBuilder()->onBodyLiteral();
        }
    ;

choice_atom
    : lower_guard CURLY_OPEN choice_elements CURLY_CLOSE upper_guard
    | lower_guard CURLY_OPEN choice_elements CURLY_CLOSE
    | CURLY_OPEN choice_elements CURLY_CLOSE
    | CURLY_OPEN choice_elements CURLY_CLOSE upper_guard
    ;

lower_guard
    : term binop
        {
            director.getBuilder()->onChoiceLowerGuard();
        }
    ;

upper_guard
    : binop term
        {
            director.getBuilder()->onChoiceUpperGuard();
        }
    ;

choice_elements
    : choice_elements SEMICOLON choice_element
    | choice_element
    ;

choice_element
    : choice_element_atom
        {
            director.getBuilder()->onChoiceElement();
        }
    | choice_element_atom COLON choice_elements_literals
        {
            director.getBuilder()->onChoiceElement();
        }
    ;

choice_element_atom
    : atom
        {
            director.getBuilder()->onChoiceElementAtom();
        }
    ;

choice_elements_literals
    : naf_literal
        {
            director.getBuilder()->onChoiceElementLiteral();
        }
    | choice_elements_literals COMMA naf_literal
        {
            director.getBuilder()->onChoiceElementLiteral();
        }
    ;

naf_literals
    : naf_literal
        {
            director.getBuilder()->onAggregateNafLiteral();
        }
    | naf_literals COMMA naf_literal
        {
            director.getBuilder()->onAggregateNafLiteral();
        }
    ;

naf_literal
    : classic_literal
        {
            director.getBuilder()->onNafLiteral();
        }
    | NAF classic_literal
        {
            director.getBuilder()->onNafLiteral(true);
        }
    | builtin_atom
        {
            director.getBuilder()->onNafLiteral();
        }
    ;

naf_literal_aggregate
    : naf_literal
    | aggregate_atom
        {
            director.getBuilder()->onAggregate();
        }
    | NAF aggregate_atom
        {
            director.getBuilder()->onAggregate(true);
        }
    ;

existential_atom
    : EXISTS vars atom
        {
            director.getBuilder()->onExistentialAtom();
        }

classic_literal
    : atom { director.getBuilder()->onAtom(); }
    | DASH atom { director.getBuilder()->onAtom(true); }
    ;

atom
    : identifier
        {
            director.getBuilder()->onPredicateName($1);
            delete[] $1;
        }
    | identifier PARAM_OPEN terms PARAM_CLOSE
        {
            director.getBuilder()->onPredicateName($1);
            delete[] $1;
        }
    | identifier PARAM_OPEN PARAM_CLOSE
        {
            director.getBuilder()->onPredicateName($1);
            delete[] $1;
        }
    ;


terms
    : term { $$ = 1; }
    | terms COMMA term { $$ = $1 + 1; }
    ;

basic_terms
    : basic_term
    | basic_terms COMMA basic_term
    ;

builtin_atom
    : term binop term
        {
            director.getBuilder()->onBuiltinAtom();
        }
    ;

compareop
    : EQUAL
        {
            director.getBuilder()->onEqualOperator();
        }
    | UNEQUAL
        {
            director.getBuilder()->onUnequalOperator();
        }
    ;

binop
    : compareop
    | leftwardop
    | rightwardop
    ;


arithop1
    : PLUS  { $$ = '+'; }
    | DASH  { $$ = '-'; }
    ;

arithop2
    : TIMES { $$ = '*'; }
    | SLASH { $$ = '/'; }
    | BACK_SLASH { $$ = '\\'; }
    ;

term_
    :  ANON_VAR
        {
            director.getBuilder()->onUnknownVariable();
        }
    | identifier PARAM_OPEN terms PARAM_CLOSE
        {
            director.getBuilder()->onFunction($1, $3);
            delete[] $1;
        }
    | NUMBER DDOT NUMBER
        {
            director.getBuilder()->onTermRange($1, $3);
            delete[] $1;
            delete[] $3;
        }
    | DASH term
        {
            director.getBuilder()->onTermDash();
        }
    | list_term
    ;

term__
    :
    identifier
        {
            director.getBuilder()->onTerm($1);
            delete[] $1;
        }
    | NUMBER
        {
            director.getBuilder()->onTerm($1);
            delete[] $1;
        }
    | PARAM_OPEN term PARAM_CLOSE
        {
            director.getBuilder()->onTermParams();
        }
    ;

term
    : term_ {}
	| expr
	;


expr
	:
	factor
	| expr arithop1 factor
        {
            director.getBuilder()->onArithmeticOperation($2);
        }
    ;

factor
	:
	term__ {}
	| factor arithop2 term__
	    {
            director.getBuilder()->onArithmeticOperation($2);
        }
 	;

basic_term : ground_term
       | variable_term
       | functional_term
       ;

ground_term
    : SYMBOLIC_CONSTANT
        {
            director.getBuilder()->onAggregateGroundTerm($1);
            delete[] $1;
        }
    | STRING
        {
            director.getBuilder()->onAggregateGroundTerm($1);
            delete[] $1;
        }
    | NUMBER
        {
            director.getBuilder()->onAggregateGroundTerm($1);
            delete[] $1;
        }
    | DASH NUMBER
        {
            director.getBuilder()->onAggregateGroundTerm($2,true);
            delete[] $2;
        }
    ;

variable_term
    : VARIABLE
        {
            director.getBuilder()->onAggregateVariableTerm($1);
            delete[] $1;
        }
    | ANON_VAR
        {
            director.getBuilder()->onAggregateUnknownVariable();
        }
    ;

functional_term
	:  identifier PARAM_OPEN terms PARAM_CLOSE
       {
            director.getBuilder()->onAggregateFunctionalTerm($1,$3);
            delete[] $1;
       }

list_term
:
       SQUARE_OPEN SQUARE_CLOSE
       {
               director.getBuilder()->onListTerm(0);
       }
    |  SQUARE_OPEN list_terms_term SQUARE_CLOSE
       {
               director.getBuilder()->onListTerm($2);
       }
    ;

list_terms_term
    :   terms
        {
                $$ = $1;
        }
      | list_head_tail_term
        {
                $$ = 2;
        }
    ;

list_head_tail_term
    :   term VEL list_tail_term
        {

                director.getBuilder()->onHeadTailList();

        }
    ;

list_tail_term
    :   VARIABLE
        {
                director.getBuilder()->onTerm($1);
                delete[] $1;
        }
      | list_term
    ;

vars
    : VARIABLE
        {
            director.getBuilder()->onExistentialVariable($1);
            delete[] $1;
        }
    | vars COMMA VARIABLE
        {
            director.getBuilder()->onExistentialVariable($3);
            delete[] $3;
        }
    ;

identifier
    : SYMBOLIC_CONSTANT { $$ = $1; }
    | STRING { $$ = $1; }
    | VARIABLE { $$ = $1; }
    ;

directive
    : DIRECTIVE_NAME DIRECTIVE_VALUE
        {
            director.getBuilder()->onDirective($1,$2);
            delete[] $1;
            delete[] $2;
        }
    ;

query
    : atom QUERY_MARK
        {
            director.getBuilder()->onAtom();
        }
    ;

lower_guard_compare_aggregate
    : term compareop
        {
            director.getBuilder()->onAggregateLowerGuard();
        }
    ;

upper_guard_compare_aggregate
    : compareop term
        {
            director.getBuilder()->onAggregateUpperGuard();
        }
    ;

compare_aggregate
    : lower_guard_compare_aggregate aggregate
    | aggregate upper_guard_compare_aggregate
    ;

lower_guard_leftward_left_aggregate
    : term leftwardop
        {
            director.getBuilder()->onAggregateLowerGuard();
        }
    ;

leftward_left_aggregate
    : lower_guard_leftward_left_aggregate aggregate
    ;

left_aggregate
    : leftward_left_aggregate
    | rightward_left_aggregate
    ;

lower_guard_rightward_left_aggregate
    : term rightwardop
        {
            director.getBuilder()->onAggregateLowerGuard();
        }
    ;

rightward_left_aggregate
    : lower_guard_rightward_left_aggregate aggregate
    ;

upper_guard_leftward_right_aggregate
    : leftwardop term
        {
            director.getBuilder()->onAggregateUpperGuard();
        }
    ;

upper_guard_rightward_right_aggregate
    : rightwardop term
        {
            director.getBuilder()->onAggregateUpperGuard();
        }
    ;

right_aggregate
    : aggregate upper_guard_leftward_right_aggregate
    | aggregate upper_guard_rightward_right_aggregate
    ;

aggregate_atom
    : left_aggregate
    | right_aggregate
    | compare_aggregate
    | leftward_left_aggregate upper_guard_leftward_right_aggregate
    | rightward_left_aggregate upper_guard_rightward_right_aggregate
    ;

leftwardop
    : LESS
        {
            director.getBuilder()->onLessOperator();
        }
    | LESS_OR_EQ
        {
            director.getBuilder()->onLessOrEqualOperator();
        }
    ;

rightwardop
    : GREATER
        {
            director.getBuilder()->onGreaterOperator();
        }
    | GREATER_OR_EQ
        {
            director.getBuilder()->onGreaterOrEqualOperator();
        }
    ;

aggregate
    : aggregate_function CURLY_OPEN aggregate_elements CURLY_CLOSE
    | aggregate_function CURLY_OPEN CURLY_CLOSE
    ;

aggregate_elements
    : aggregate_elements SEMICOLON aggregate_element
    | aggregate_element
    ;


aggregate_element
    : basic_terms COLON naf_literals
        {
            director.getBuilder()->onAggregateElement();
        }
    ;

aggregate_function
    : AGGR_COUNT
        {
            director.getBuilder()->onAggregateFunction($1);
            delete[] $1;
        }
    | AGGR_MAX
        {
            director.getBuilder()->onAggregateFunction($1);
            delete[] $1;
        }
    | AGGR_MIN
        {
            director.getBuilder()->onAggregateFunction($1);
            delete[] $1;
        }
    | AGGR_SUM
        {
            director.getBuilder()->onAggregateFunction($1);
            delete[] $1;
        }
    ;

