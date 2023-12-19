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

 * @file   Expr.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-12-19
 *********************************************************************/
#include "stdafx.h"
#include "Expr.h"

Expr::Expr(ExprType type)
  : type(type),
    expr(nullptr),
    expr2(nullptr),
    exprList(nullptr),
    select(nullptr),
    name(nullptr),
    table(nullptr),
    alias(nullptr),
    fval(0),
    ival(0),
    ival2(0),
    opType(kOpNone),
    distinct(false) {};

Expr::~Expr() {
	delete expr;
	delete expr2;
	delete select;
	free(name);
	free(table);
	free(alias);

	if (exprList != nullptr) {
		for (Expr* e : *exprList) {
		delete e;
		}
		delete exprList;
	}
}

Expr* Expr::make(ExprType type) 
{
	Expr* e = new Expr(type);
	return e;
}

Expr* Expr::makeOpUnary(OperatorType op, Expr* expr) 
{
	Expr* e = new Expr(kExprOperator);
	e->opType = op;
	e->expr = expr;
	e->expr2 = nullptr;
	return e;
}

Expr* Expr::makeOpBinary(Expr* expr1, OperatorType op, Expr* expr2) 
{
	Expr* e = new Expr(kExprOperator);
	e->opType = op;
	e->expr = expr1;
	e->expr2 = expr2;
	return e;
}

Expr* Expr::makeBetween(Expr* expr, Expr* left, Expr* right) 
{
	Expr* e = new Expr(kExprOperator);
	e->expr = expr;
	e->opType = kOpBetween;
	e->exprList = new std::vector<Expr*>();
	e->exprList->push_back(left);
	e->exprList->push_back(right);
	return e;
}

Expr* Expr::makeCaseList(Expr* caseListElement) 
{
	Expr* e = new Expr(kExprOperator);
	e->opType = kOpCaseList;
	e->exprList = new std::vector<Expr*>();
	e->exprList->push_back(caseListElement);
	return e;
}

Expr* Expr::makeCaseListElement(Expr* when, Expr* then) 
{
	Expr* e = new Expr(kExprOperator);
	e->opType = kOpCaseListElement;
	e->expr = when;
	e->expr2 = then;
	return e;
}

Expr* Expr::caseListAppend(Expr* caseList, Expr* caseListElement) 
{
	caseList->exprList->push_back(caseListElement);
	return caseList;
}

Expr* Expr::makeCase(Expr* expr, Expr* caseList, Expr* elseExpr) 
{
	Expr* e = new Expr(kExprOperator);
	e->opType = kOpCase;
	e->expr = expr;
	e->expr2 = elseExpr;
	e->exprList = caseList->exprList;
	caseList->exprList = nullptr;
	delete caseList;
	return e;
}

Expr* Expr::makeLiteral(int64_t val) 
{
	Expr* e = new Expr(kExprLiteralInt);
	e->ival = val;
	return e;
}

Expr* Expr::makeLiteral(double value) {
	Expr* e = new Expr(kExprLiteralFloat);
	e->fval = value;
	return e;
}

Expr* Expr::makeLiteral(wchar_t* string) {
	Expr* e = new Expr(kExprLiteralString);
	e->name = string;
	return e;
}

Expr* Expr::makeNullLiteral() {
	Expr* e = new Expr(kExprLiteralNull);
	return e;
}

Expr* Expr::makeColumnRef(wchar_t* name) {
	Expr* e = new Expr(kExprColumnRef);
	e->name = name;
	return e;
}

Expr* Expr::makeColumnRef(wchar_t* table, wchar_t* name) {
	Expr* e = new Expr(kExprColumnRef);
	e->name = name;
	e->table = table;
	return e;
}

Expr* Expr::makeStar(void) {
	Expr* e = new Expr(kExprStar);
	return e;
}

Expr* Expr::makeStar(wchar_t* table) {
	Expr* e = new Expr(kExprStar);
	e->table = table;
	return e;
}

Expr* Expr::makeFunctionRef(wchar_t* func_name, std::vector<Expr*>* exprList,
                            bool distinct) {
	Expr* e = new Expr(kExprFunctionRef);
	e->name = func_name;
	e->exprList = exprList;
	e->distinct = distinct;
	return e;
}

Expr* Expr::makeArray(std::vector<Expr*>* exprList) {
	Expr* e = new Expr(kExprArray);
	e->exprList = exprList;
	return e;
}

Expr* Expr::makeArrayIndex(Expr* expr, int64_t index) {
	Expr* e = new Expr(kExprArrayIndex);
	e->expr = expr;
	e->ival = index;
	return e;
}

Expr* Expr::makeParameter(int id) {
	Expr* e = new Expr(kExprParameter);
	e->ival = id;
	return e;
}

Expr* Expr::makeSelect(SelectStatement* select) {
	Expr* e = new Expr(kExprSelect);
	e->select = select;
	return e;
}

Expr* Expr::makeExists(SelectStatement* select) {
	Expr* e = new Expr(kExprOperator);
	e->opType = kOpExists;
	e->select = select;
	return e;
}

Expr* Expr::makeInOperator(Expr* expr, std::vector<Expr*>* exprList) {
	Expr* e = new Expr(kExprOperator);
	e->opType = kOpIn;
	e->expr = expr;
	e->exprList = exprList;

	return e;
}

Expr* Expr::makeInOperator(Expr* expr, SelectStatement* select) {
	Expr* e = new Expr(kExprOperator);
	e->opType = kOpIn;
	e->expr = expr;
	e->select = select;

	return e;
}

bool Expr::isType(ExprType exprType) const {
	return exprType == type;
}

bool Expr::isLiteral() const {
	return isType(kExprLiteralInt) || isType(kExprLiteralFloat) ||
		   isType(kExprLiteralString) || isType(kExprParameter) ||
		   isType(kExprLiteralNull);
}

bool Expr::hasAlias() const {
	return alias != nullptr;
}

bool Expr::hasTable() const {
	return table != nullptr;
}

const wchar_t* Expr::getName() const {
	if (alias != nullptr)
		return alias;
	else
		return name;
}

wchar_t* substr(const wchar_t* source, int from, int to) {
	int len = to - from;
	wchar_t* copy = (wchar_t*)malloc((len + 1) * sizeof(wchar_t));
	;
	wcsncpy_s(copy, from, source, len);
	copy[len] = '\0';
	return copy;
}
