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

 * @file   Table.h
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-12-20
 *********************************************************************/
#pragma once

#include "Expr.h"
#include <stdio.h>
#include <vector>

struct SelectStatement;
struct JoinDefinition;
struct TableRef;

// Possible table reference types.
enum TableRefType {
	kTableName,
	kTableSelect,
	kTableJoin,
	kTableCrossProduct
};

struct TableName {
	wchar_t* schema;
	wchar_t* name;
};

// Holds reference to tables. Can be either table names or a select statement.
struct TableRef {
	TableRef(TableRefType type);
	virtual ~TableRef();

	TableRefType type;

	wchar_t* schema;
	wchar_t* name;
	wchar_t* alias;

	SelectStatement* select;
	std::vector<TableRef*>* list;
	JoinDefinition* join;

	// Returns true if a schema is set.
	bool hasSchema() const;

	// Returns the alias, if it is set. Otherwise the name.
	const wchar_t* getName() const;
};

// Possible types of joins.
enum JoinType {
	kJoinInner,
	kJoinFull,
	kJoinLeft,
	kJoinRight,
	kJoinCross,
	kJoinNatural
};

// Definition of a join construct.
struct JoinDefinition {
	JoinDefinition();
	virtual ~JoinDefinition();

	TableRef* left;
	TableRef* right;
	Expr* condition;

	JoinType type;
};
