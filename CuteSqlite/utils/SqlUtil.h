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

 * @file   SqlUtil.h
 * @brief  sql statement utils
 * 
 * @author Xuehan Qin
 * @date   2023-05-28
 *********************************************************************/
#pragma once
#include <regex>
#include <string>
#include <vector>
#include "core/entity/Entity.h"

class SqlUtil {
public:
	// select sql regex pattern
	static std::wregex selectPat;

	// where clause regex pattern
	static std::wregex whereClausePat1;
	static std::wregex whereClausePat2;

	// limit clause regex patter
	static std::wregex limitClausePat;

	// fourth clause regex pattern 
	static std::wregex fourthClausePat;

	// table statement of select sql regex pattern
	static std::wregex selectTablesPat;

	//primary key pattern of create table statement
	static std::wregex primaryKeyPat;

	// field regex pattern
	static std::wregex columnPat;

	static std::vector<std::wstring> tableTags;

	// parse sql 
	static bool isSelectSql(std::wstring & sql);
	static bool hasLimitClause(std::wstring & sql);
	static std::wstring getColumnName(std::wstring & str);
	static std::vector<std::wstring> getTablesFromSelectSql(std::wstring & sql, std::vector<std::wstring> allTables);
	static std::vector<std::wstring> parseTablesFromTableClause(std::wstring & tblStmt);
	static std::wstring parsePrimaryKeyFromCreateTableSql(std::wstring & createTblSql);
	static std::wstring getWhereClause(std::wstring & sql);
	static std::wstring getFourthClause(std::wstring & sql);

	// make sql
	static std::wstring makeWhereClause(Columns & columns, RowItem &rowItem , SubItemValues &rowChangeVals);
	static std::wstring makeWhereClauseByPrimaryKey(std::wstring & primaryKey, Columns & columns, RowItem &rowItem, SubItemValues &rowChangeVals);
	static std::wstring makeInsertColumsClause(Columns & columns);
	static std::wstring makeInsertValuesClause(RowItem & rowItem);

	// make table name
	static std::wstring makeTmpTableName(const std::wstring & tblName, int number = 1, const std::wstring & prefix = std::wstring(L"ctsqlite_tmp_"));
};
