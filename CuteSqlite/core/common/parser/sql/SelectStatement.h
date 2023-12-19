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

 * @file   SelectStatment.h
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-12-20
 *********************************************************************/
#pragma once

#include "SQLStatement.h"
#include "Expr.h"
#include "Table.h"

enum OrderType {
	kOrderAsc,
	kOrderDesc
};


// Description of the order by clause within a select statement.
struct OrderDescription {
	OrderDescription(OrderType type, Expr* expr);
	virtual ~OrderDescription();

	OrderType type;
	Expr* expr;
};

const int64_t kNoLimit = -1;
const int64_t kNoOffset = -1;

// Description of the limit clause within a select statement.
struct LimitDescription {
	LimitDescription(int64_t limit, int64_t offset);

	int64_t limit;
	int64_t offset;
};

// Description of the group-by clause within a select statement.
struct GroupByDescription {
	GroupByDescription();
	virtual ~GroupByDescription();

	std::vector<Expr*>* columns;
	Expr* having;
};

// Representation of a full SQL select statement.
// TODO: add union_order and union_limit.
struct SelectStatement : SQLStatement {
	SelectStatement();
	virtual ~SelectStatement();

	TableRef* fromTable;
	bool selectDistinct;
	std::vector<Expr*>* selectList;
	Expr* whereClause;
	GroupByDescription* groupBy;

	SelectStatement* unionSelect;
	std::vector<OrderDescription*>* order;
	LimitDescription* limit;
};
