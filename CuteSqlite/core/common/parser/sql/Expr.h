/*****************************************************************//**
 * Copyright 2023 Xuehan Qin 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *   http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and

 * limitations under the License.

 * @file   Expr.h
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-12-19
 *********************************************************************/
#pragma once
#include <stdlib.h>
#include <memory>
#include <vector>

struct SelectStatement;
// Helper function used by the lexer.
// TODO: move to more appropriate place.
char* substr(const char* source, int from, int to);

enum ExprType {
	kExprLiteralFloat,
	kExprLiteralString,
	kExprLiteralInt,
	kExprLiteralNull,
	kExprStar,
	kExprParameter,
	kExprColumnRef,
	kExprFunctionRef,
	kExprOperator,
	kExprSelect,
	kExprHint,
	kExprArray,
	kExprArrayIndex,
	kExprCaseList,
	kExprCaseListElement
};

// Operator types. These are important for expressions of type kExprOperator.
enum OperatorType {
	kOpNone,

	// Ternary operator
	kOpBetween,

	// n-nary special case
	kOpCase,
	kOpCaseList, // Contains n >= 1 kExprCaseListElement in its exprList
	kOpCaseListElement, // `WHEN expr THEN expr`

	// Binary operators.
	kOpPlus,
	kOpMinus,
	kOpAsterisk,
	kOpSlash,
	kOpPercentage,
	kOpCaret,

	kOpEquals,
	kOpNotEquals,
	kOpLess,
	kOpLessEq,
	kOpGreater,
	kOpGreaterEq,
	kOpLike,
	kOpNotLike,
	kOpILike,
	kOpAnd,
	kOpOr,
	kOpIn,
	kOpConcat,

	// Unary operators.
	kOpNot,
	kOpUnaryMinus,
	kOpIsNull,
	kOpExists
};

typedef struct Expr Expr;

// Represents SQL expressions (i.e. literals, operators, column_refs).
// TODO: When destructing a placeholder expression, we might need to alter the
// placeholder_list.
struct Expr {
	Expr(ExprType type);
	virtual ~Expr();

	ExprType type;

	// TODO: Replace expressions by list.
	Expr* expr;
	Expr* expr2;
	std::vector<Expr*>* exprList;
	SelectStatement* select;
	wchar_t* name;
	wchar_t* table;
	wchar_t* alias;
	double fval;
	int64_t ival;
	int64_t ival2;

	OperatorType opType;
	bool distinct;

	// Convenience accessor methods.

	bool isType(ExprType exprType) const;

	bool isLiteral() const;

	bool hasAlias() const;

	bool hasTable() const;

	const wchar_t* getName() const;

	// Static constructors.

	static Expr* make(ExprType type);

	static Expr* makeOpUnary(OperatorType op, Expr* expr);

	static Expr* makeOpBinary(Expr* expr1, OperatorType op, Expr* expr2);

	static Expr* makeBetween(Expr* expr, Expr* left, Expr* right);

	static Expr* makeCaseList(Expr* caseListElement);

	static Expr* makeCaseListElement(Expr* when, Expr* then);

	static Expr* caseListAppend(Expr* caseList, Expr* caseListElement);

	static Expr* makeCase(Expr* expr, Expr* when, Expr* elseExpr);

	static Expr* makeLiteral(int64_t val);

	static Expr* makeLiteral(double val);

	static Expr* makeLiteral(wchar_t* val);

	static Expr* makeNullLiteral();

	static Expr* makeColumnRef(wchar_t* name);

	static Expr* makeColumnRef(wchar_t* table, wchar_t* name);

	static Expr* makeStar(void);

	static Expr* makeStar(wchar_t* table);

	static Expr* makeFunctionRef(wchar_t* func_name, std::vector<Expr*>* exprList,
									bool distinct);

	static Expr* makeArray(std::vector<Expr*>* exprList);

	static Expr* makeArrayIndex(Expr* expr, int64_t index);

	static Expr* makeParameter(int id);

	static Expr* makeSelect(SelectStatement* select);

	static Expr* makeExists(SelectStatement* select);

	static Expr* makeInOperator(Expr* expr, std::vector<Expr*>* exprList);

	static Expr* makeInOperator(Expr* expr, SelectStatement* select);
};