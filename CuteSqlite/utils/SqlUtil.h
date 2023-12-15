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

// special charactor for SQL
// special characters for sql statement
#define quo L"\""
#define lbrk L"\r\n"
#define qua L"'"
#define blk L" "
#define cma L","
#define cmb L"("
#define cme L")"
#define eq L"="
#define edl L";"
#define brk L"\n"
#define nil L"NULL"
#define dot L"."

class SqlUtil {
public:
	// select sql regex pattern
	static std::wregex selectPat;
	static std::wregex selectPat2;
	static std::wregex selectPat3;
	static std::wregex selectPat4;
	static std::wregex explainPat;

	// where clause regex pattern
	static std::wregex whereClausePat1;
	static std::wregex whereClausePat2;

	// order clause regex pattern
	static std::wregex orderClausePat1;
	static std::wregex orderClausePat2;

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

	// number suffix pattern of table name, use for table sharding
	static std::wregex tblNumSuffixPat;

	static std::vector<std::wstring> tableTags;

	// parse sql 
	static bool isSelectSql(std::wstring & sql);
	static bool isPragmaStmt(std::wstring & sql, bool excludeEqual);
	static bool hasLimitClause(std::wstring & sql);
	static std::wstring getColumnName(std::wstring & str);
	static std::vector<std::wstring> getTablesFromSelectSql(const std::wstring & sql, std::vector<std::wstring> allTables);
	static std::vector<std::wstring> parseTablesFromTableClause(std::wstring & tblStmt);
	static std::wstring parseTableAliasFromSelectSql(const std::wstring & sql, const std::wstring & table, const std::vector<std::wstring> & tables);
	static std::wstring parseTableAliasFromSelectSqlUpWords(const std::vector<std::wstring>& upSqlWords, std::wstring & upTable, const UserTableStrings & tables);

	static std::wstring parsePrimaryKey(std::wstring & createTblSql);
	static std::wstring getWhereClause(const std::wstring & sql);
	static std::wstring getOrderClause(const std::wstring & sql);
	static std::vector<std::wstring> getSelectColumnsClause(const std::wstring & sql);
	static std::wstring getOrderExpresses(const std::wstring & sql);
	static std::wstring getFourthClause(const std::wstring & sql);

	// make sql
	static std::wstring makeWhereClause(Columns & columns, RowItem &rowItem , SubItemValues &rowChangeVals);
	static std::wstring makeWhereClauseByPrimaryKey(std::wstring & primaryKey, Columns & columns, RowItem &rowItem, SubItemValues &rowChangeVals);
	static std::wstring makeWhereClauseByRowId(Columns & columns, RowItem &rowItem);
	static std::wstring makeInsertColumsClause(Columns & columns);
	static std::wstring makeInsertValuesClause(RowItem & rowItem);

	// make table name
	static std::wstring makeTmpTableName(const std::wstring & tblName, int number = 1, const std::wstring & prefix = std::wstring(L"ctsqlite_tmp_"));
	
	// parse columns by create table DDL
	static ColumnInfoList parseColumnsByTableDDL(const std::wstring & createTblSql);

	// parse constraints by create table DDL exclude foreign key
	static IndexInfoList parseConstraints(const std::wstring & createTblSql);
	static IndexInfo parseConstraintsForPrimaryKey(const std::wstring & createTblSql);
	static std::vector<std::wstring> splitTableDDLColumnClausesToLines(std::wstring str, bool bTrim = true);

	// parse foreign keys by create table DDL
	static ForeignKeyList parseForeignKeysByTableDDL(const std::wstring & createTblSql);

	static std::wstring replaceNumOfSuffixInTblName(const std::wstring & tblName, const std::wstring & after);

	// parse table clause from sql, such as "SELECT * FROM [tbl_1 as name_1, tbl_2 as name_2] WHERE ..."
	static TableAliasVector parseTableClauseFromSelectSql(const std::wstring & upSelectSql);
	static TableAliasVector parseTableClauseFromUpdateSql(const std::wstring & upSelectSql);

	static IndexInfo parseCreateIndex(const std::wstring & createIndexSql);
	
	
	static std::vector<std::pair<std::wstring, std::wstring>> parseWhereNullColumnsFromSelectSqlUpWords(
		const std::vector<std::wstring>& upSqlWords, 
		const std::vector<std::pair<std::wstring, std::wstring>> & allAliases);
private:
	static IndexInfo parseConstraintFromLine(const std::wstring& line);
	static IndexInfo parseLineToPrimaryKey(const std::wstring& line, bool isConstaintLine = true);
	static IndexInfo parseLineToUnique(const std::wstring& line, bool isConstaintLine = true);
	static IndexInfo parseLineToCheck(const std::wstring& line, bool isConstaintLine = true);
	static std::pair<std::wstring, uint8_t> getColumnAndAiFromPrimaryKeyLine(const std::wstring &line);
	static std::wstring getConflictClauseFromConstraintLine(const std::wstring& line);
	static std::wstring getColumnsFromUniqueLine(const std::wstring &line);
	static std::wstring getExpressionByCheckLine(const std::wstring &line);

	static ColumnInfo parseColumnFromLine(const std::wstring& line);
	static ForeignKey parseForeignKeyFromLine(const std::wstring& line);
	static std::wstring getPartialClauseFromForeignKeyLine(const std::wstring& line);
};
