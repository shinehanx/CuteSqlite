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

 * @file   statements.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-12-20
 *********************************************************************/
#include "stdafx.h"
#include "statements.h"

// SelectStatement.h

// OrderDescription
OrderDescription::OrderDescription(OrderType type, Expr* expr) :
	type(type),
	expr(expr) {}

OrderDescription::~OrderDescription() {
	delete expr;
}

// LimitDescription
LimitDescription::LimitDescription(int64_t limit, int64_t offset) :
	limit(limit),
	offset(offset) {}

// GroypByDescription
GroupByDescription::GroupByDescription() :
	columns(nullptr),
	having(nullptr) {}

GroupByDescription::~GroupByDescription() {
	delete having;

	if (columns != nullptr) {
		for (Expr* expr : *columns) {
			delete expr;
		}
		delete columns;
	}
}

// SelectStatement
SelectStatement::SelectStatement() :
	SQLStatement(kStmtSelect),
	fromTable(nullptr),
	selectDistinct(false),
	selectList(nullptr),
	whereClause(nullptr),
	groupBy(nullptr),
	unionSelect(nullptr),
	order(nullptr),
	limit(nullptr) {};

SelectStatement::~SelectStatement() {
	delete fromTable;
	delete whereClause;
	delete groupBy;
	delete unionSelect;
	delete limit;

	// Delete each element in the select list.
	if (selectList != nullptr) {
		for (Expr* expr : *selectList) {
			delete expr;
		}
		delete selectList;
	}

	if (order != nullptr) {
		for (OrderDescription* desc : *order) {
			delete desc;
		}
		delete order;
	}
}
