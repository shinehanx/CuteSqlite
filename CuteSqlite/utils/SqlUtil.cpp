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

 * @file   SqlUtil.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-05-28
 *********************************************************************/
#include "stdafx.h"
#include "SqlUtil.h"
#include <chrono>
#include "StringUtil.h"

std::wregex SqlUtil::selectPat(L"^select\\s+(.*)\\s+from\\s+(.*)\\s*(where .*)?", std::wregex::icase);
std::wregex SqlUtil::selectPat2(L"^with\\s(.*)?\\s?select\\s+(.*)\\s+from\\s+(.*)\\s*(where .*)?", std::wregex::icase);
std::wregex SqlUtil::selectPat3(L"select\\s+(.*)\\s+from\\s+(.*)\\s*(order .*)+", std::wregex::icase);
std::wregex SqlUtil::selectPat4(L"select\\s+(.*)\\s+from(.*)", std::wregex::icase);
std::wregex SqlUtil::explainPat(L"^explain(query|\\s|plan)+(.*)+", std::wregex::icase);

std::wregex SqlUtil::whereClausePat1(L"((where)\\s+.*)\\s+(order|group|limit|having|window)+(.*)?", std::wregex::icase);
std::wregex SqlUtil::whereClausePat2(L"(where .*\\)?)+", std::wregex::icase);

std::wregex SqlUtil::orderClausePat1(L"((order\\sby)\\s+.*)\\s+(limit|offset)+(.*)?", std::wregex::icase);
std::wregex SqlUtil::orderClausePat2(L"(order\\sby\\s.*\\)?)+", std::wregex::icase);

std::wregex SqlUtil::limitClausePat(L"(limit .*)+", std::wregex::icase);

std::wregex SqlUtil::fourthClausePat(L"((order|group|limit|having|window)\\s+.*)+", std::wregex::icase);

std::wregex SqlUtil::primaryKeyPat(L"\\s+primary\\s+key\\s?\\(?(\\\"?([^(\\s|\\\"|\\s)]+)\\\"?\\s?)", std::wregex::icase);

std::wregex SqlUtil::columnPat(L"(.*)\\s+\\[(.*)\\]+");

std::wregex SqlUtil::tblNumSuffixPat(L"([0-9]+)$");



std::vector<std::wstring> SqlUtil::tableTags({ L"as", L"left", L"right",L"inner", L"cross", L"full", L"outer", L"natural", L"join" });

bool SqlUtil::isSelectSql(std::wstring & sql)
{
	if (sql.empty()) {
		return false;
	}

	if (std::regex_search(sql, SqlUtil::selectPat) 
		|| std::regex_search(sql, SqlUtil::explainPat)
		|| std::regex_search(sql, SqlUtil::selectPat2)) {
		return true;
	}
	return false;
}

bool SqlUtil::isPragmaStmt(std::wstring & sql, bool excludeEqual)
{
	if (sql.empty()) {
		return false;
	}
	std::wstring upsql = StringUtil::toupper(sql);
	if (excludeEqual) {
		return upsql.find(L"PRAGMA") == 0 && upsql.find_last_of(L'=') == std::wstring::npos;
	}
	return upsql.find(L"PRAGMA") == 0;
}

bool SqlUtil::hasLimitClause(std::wstring & sql)
{
	if (sql.empty()) {
		return false;
	}

	if (std::regex_search(sql, SqlUtil::limitClausePat)) {
		return true;
	}
	return false;
}

/**
 * Fetch the column name from str such as "name [hello world]".
 * 
 * @param str
 * @return 
 */
std::wstring SqlUtil::getColumnName(std::wstring & str)
{
	if (str.empty()) {
		return L"";
	}

	std::wsmatch results;
	if (std::regex_search(str, results,  SqlUtil::columnPat)) {
		return results[1];
	}
	return L"";
}

/**
 * Fetch the table(s) name from select query sql statement.
 * 
 * @param selectSql - The select statement
 * @param tables - All tables of databases 
 * @return the vector of table(s)
 */
std::vector<std::wstring> SqlUtil::getTablesFromSelectSql(const std::wstring & selectSql, std::vector<std::wstring> allTables)
{
	std::vector<std::wstring> tbls;
	if (selectSql.empty()) {
		return tbls;
	}
	
	std::wsmatch results;
	if (!std::regex_search(selectSql, results, SqlUtil::selectPat)) {
		return tbls;
	}
	// The second param from match results is the tables statement,
	//   such as : table1 as tbl1, table2 as tbl2 or table1 t1 left join table2 t2 
	std::wstring tblClause = results[2];
	if (tblClause.empty()) {
		return tbls;
	}
	
	auto vec = parseTablesFromTableClause(tblClause);
	for (auto tbl : allTables) {
		auto iterator = std::find_if(vec.begin(), vec.end(), [&tbl](std::wstring & str) {
			std::wstring lowtbl = StringUtil::tolower(tbl);
			std::wstring lowstr = StringUtil::tolower(str);
			return lowtbl == str;
		});

		if (iterator != vec.end()) {
			tbls.push_back(tbl);
		}
	}

	return tbls;
}

/**
 * Fetch the table name(s) from table clause such as "select * from [tblClause]"..
 * 
 * @param tblStmt
 * @return The vector of table name(s) such as {tbl1,tbl2}
 */
std::vector<std::wstring> SqlUtil::parseTablesFromTableClause(std::wstring & tblClause)
{
	std::vector<std::wstring> tbls;
	if (tblClause.empty()) {
		return tbls;
	}
	std::wstring str(tblClause);
	str = StringUtil::replace(str, std::wstring(L","), std::wstring(L" "));
	auto vec = StringUtil::split(str, L" ");
	for (auto tag : vec) {
		if (tag.empty()) {
			continue;
		}

		std::wstring lowtag = StringUtil::tolower(tag);
		auto iterator = std::find_if(tableTags.begin(), tableTags.end(), [&lowtag](std::wstring & str) {
			return lowtag == str;
		});

		if (iterator != tableTags.end()) {
			continue;
		}
		tag = StringUtil::cutParensAndQuotes(tag);
		tbls.push_back(tag);
	}
	return tbls;
}

/**
 * Fetch the primary key clause such as 'PRIMARY KEY("id" AUTOINCREMENT)'.
 * 
 * @param createTblSql
 * @return std::wstring such as id
 */
std::wstring SqlUtil::parsePrimaryKey(std::wstring & createTblSql)
{
	std::wsmatch results;
	if (!std::regex_search(createTblSql, results, SqlUtil::primaryKeyPat)) {
		return L"";
	}
	std::wstring primaryKey = results[2];
	return primaryKey;
}

/**
 * Fetch the WHERE clause such as "select * from [tbl clause] [where clause] [fourth clause : order|having|window|group...]"...
 * 
 * @param sql
 * @return The string of whereClause
 */
std::wstring SqlUtil::getWhereClause(const std::wstring & sql)
{
	if (sql.empty()) {
		return L"";
	}
	std::wsmatch results;
	if (!std::regex_search(sql, results, SqlUtil::selectPat)) {
		return L"";
	}
	if (results.size() < 3 || results[3].matched == false) {
		return L"";
	}
	std::wsmatch whereResults;
	std::wstring whereStmt = results[3];
	std::wstring whereClause;

	while (!whereStmt.empty() && std::regex_search(whereStmt, whereResults, SqlUtil::whereClausePat1)) {
		if (whereResults[1].matched == false) {
			return L"";
		}
		whereClause = whereResults[1];
		if (std::regex_search(whereClause, SqlUtil::fourthClausePat)) {
			whereStmt = whereClause;
			continue;
		}
		break;
	}

	if (whereClause.empty() && std::regex_search(whereStmt, whereResults, SqlUtil::whereClausePat2)) {
		if (whereResults[1].matched == false) {
			return L"";
		}
		whereClause = whereResults[1];
	}

	if (whereClause.empty() || StringUtil::toupper(whereClause.substr(0, 5)) != L"WHERE") {
		return L"";
	}
	return whereClause;
}

/**
 * Fetch the ORDER clause such as "select * from [tbl clause] [where clause] [order clause] [fourth clause : limit...]"...
 * 
 * @param sql
 * @return The string of whereClause
 */
std::wstring SqlUtil::getOrderClause(const std::wstring & sql)
{
	if (sql.empty()) {
		return L"";
	}
	std::wsmatch results;
	if (!std::regex_search(sql, results, SqlUtil::selectPat3)) {
		return L"";
	}
	if (results.size() < 3 || results[3].matched == false) {
		return L"";
	}
	std::wsmatch orderResults;
	std::wstring orderStmt = results[3];
	std::wstring orderClause;

	while (!orderStmt.empty() && std::regex_search(orderStmt, orderResults, SqlUtil::orderClausePat1)) {
		if (orderResults[1].matched == false) {
			return L"";
		}
		orderClause = orderResults[1];
		if (std::regex_search(orderClause, SqlUtil::fourthClausePat)) {
			orderStmt = orderClause;
			continue;
		}
		break;
	}

	if (orderClause.empty() && std::regex_search(orderStmt, orderResults, SqlUtil::orderClausePat2)) {
		if (orderResults[1].matched == false) {
			return L"";
		}
		orderClause = orderResults[1];
	}

	if (orderClause.empty() || StringUtil::toupper(orderClause.substr(0, 5)) != L"ORDER") {
		return L"";
	}
	return orderClause;
}

/**
 * Fetch the ORDER clause such as "select * from [tbl clause] [where clause] [order clause] [fourth clause : limit...]"...
 * 
 * @param sql
 * @return The string of whereClause
 */
std::vector<std::wstring> SqlUtil::getSelectColumnsClause(const std::wstring & sql)
{
	std::vector<std::wstring> result;
	if (sql.empty()) {
		return result;
	}
	std::wsmatch columnResults;
	std::wstring selectColumnClause;
	while (std::regex_search(sql, columnResults, SqlUtil::selectPat4)) {
		if (columnResults[1].matched == false) {
			return result;
		}
		selectColumnClause = columnResults[1];
		if (selectColumnClause.empty()) {
			continue;
		}
		result.push_back(selectColumnClause);
	}

	return result;
}

/**
 * .
 * 
 * @param sql
 * @return 
 */
std::wstring SqlUtil::getOrderExpresses(const std::wstring & sql)
{
	std::wstring orderClause = getOrderClause(sql);
	if (orderClause.empty()) {
		return L"";
	}
	std::wstring upOrderClause = StringUtil::toupper(orderClause);
	size_t byPos = upOrderClause.find(L"BY");
	if (byPos == std::wstring::npos) {
		return L"";
	}
	std::wstring result = orderClause.substr(byPos + 2);
	return result;
}

/**
 * Fetch the fourth clause such as "select * from [tbl clause] [where clause] [fourth clause : order|group|limit|having|window...]"...
 * 
 * @param sql
 * @return The string of fourthClause
 */
std::wstring SqlUtil::getFourthClause(const std::wstring & sql)
{
	std::wsmatch results;
	if (!std::regex_search(sql, results, SqlUtil::fourthClausePat)) {
		return L"";
	}
	if (results.size() == 0 || results[1].matched == false) {
		return L"";
	}

	std::wstring fouthClause = results[1];
	if (fouthClause.empty()  || (StringUtil::toupper(fouthClause.substr(0, 5)) != L"ORDER"
		&& StringUtil::toupper(fouthClause.substr(0, 5)) != L"GROUP"
		&& StringUtil::toupper(fouthClause.substr(0, 5)) != L"LIMIT"
		&& StringUtil::toupper(fouthClause.substr(0, 6)) != L"HAVING"
		&& StringUtil::toupper(fouthClause.substr(0, 6)) != L"WINDOW")) {
		return L"";
	}
	return fouthClause;
}

/**
 * Make sql where clause from columns and values.
 * @condition:
 *  1.columns and rowItem are not empty.
 *  2.columns.size must equals rowItem.size
 * 
 * @param runtimeColumns columns
 * @param rowItem row 
 * @param rowChangeVals The change subitem index in only this selected row
 * @return 
 */
std::wstring SqlUtil::makeWhereClause(Columns & columns, RowItem &rowItem, SubItemValues &rowChangeVals)
{
	if (columns.empty() || rowItem.empty() 
		|| columns.size() != rowItem.size()) {
		return L"";
	}

	std::wstring whereClause(L" WHERE ");
	int n = static_cast<int>(columns.size());
	for (int i = 0; i < n; i++) {
		if (i > 0) {
			whereClause.append(L" AND ");
		}

		std::wstring val;
		if (rowChangeVals.empty()) {
			val = StringUtil::escapeSql(rowItem.at(i));
		} else {
			auto iter = std::find_if(rowChangeVals.begin(), rowChangeVals.end(), [&i](SubItemValue & subItem) {
				auto nSubItem = subItem.iSubItem ;
				return nSubItem == i;
			});

			val = iter == rowChangeVals.end() ? 
				StringUtil::escapeSql(rowItem.at(i)) 
				: StringUtil::escapeSql((*iter).origVal);
		}
		
			
		whereClause.append(blk).append(quo).append(columns.at(i)).append(quo)
			.append(L"=").append(qua).append(val).append(qua);
	}

	return whereClause;
}

/**
 * make where clause only through primary key.
 * 
 * @param primaryKey 
 * @param columns
 * @param rowItem
 * @param nChangeSubItem
 * @param subItemOrigVal
 * @return 
 */
std::wstring SqlUtil::makeWhereClauseByPrimaryKey(std::wstring & primaryKey, Columns & columns, RowItem &rowItem, SubItemValues &rowChangeVals)
{
	if (columns.empty() || rowItem.empty() 
		|| columns.size() != rowItem.size()) {
		return L"";
	}

	std::wstring whereClause(L" WHERE ");
	int n = static_cast<int>(columns.size());
	for (int i = 0; i < n; i++) {
		std::wstring column = columns.at(i);
		if (column != primaryKey) {
			continue;
		}
		
		std::wstring val;
		if (rowChangeVals.empty()) {
			val = StringUtil::escapeSql(rowItem.at(i));
		} else {
			auto iter = std::find_if(rowChangeVals.begin(), rowChangeVals.end(), [&i](SubItemValue & subItem) {
				auto nSubItem = subItem.iSubItem - 1;
				return nSubItem == i;
			});

			val = iter == rowChangeVals.end() ? 
				StringUtil::escapeSql(rowItem.at(i)) 
				: StringUtil::escapeSql((*iter).origVal);
		}
		whereClause.append(blk).append(quo).append(columns.at(i)).append(quo)
			.append(L"=").append(qua).append(val).append(qua);
		break;
	}

	return whereClause;
}

std::wstring SqlUtil::makeWhereClauseByRowId(Columns & columns, RowItem &rowItem)
{
	if (columns.empty() || rowItem.empty() 
		|| columns.size() != rowItem.size()) {
		return L"";
	}
	std::wstring rowIdColumnName = columns.at(0);
	std::wstring rowIdVal = rowItem.at(0);
	ATLASSERT(rowIdColumnName == L"_ct_sqlite_rowid" && !rowIdVal.empty());
	std::wstring whereClause(L" WHERE ");
	whereClause.append(quo).append(L"ROWID").append(quo) // such as "ct_sqlite_row_id = 11"
		.append(eq).append(qua).append(rowIdVal).append(qua);
	return whereClause;
}

/**
 * make insert sql columns clause such as id, name1, name2, email...
 * 
 * @param columns The columns vector
 * @return 
 */
std::wstring SqlUtil::makeInsertColumsClause(Columns & columns)
{
	if (columns.empty()) {
		return L"";
	}

	std::wstring columnsClause(L" (");
	int n = static_cast<int>(columns.size());
	for (int i = 1; i < n; i++) {
		std::wstring column = columns.at(i);
		if (i > 1) {
			columnsClause.append(L", ");
		}
		columnsClause.append(quo).append(column).append(quo);
	}
	columnsClause.append(L") ");
	return columnsClause;
}

/**
 * make insert sql columns clause such as id, name1, name2, email...
 * 
 * @param rowItem The row vector
 * @return 
 */
std::wstring SqlUtil::makeInsertValuesClause(RowItem & rowItem)
{
	if (rowItem.empty()) {
		return L"";
	}

	std::wstring valuesClause(L" VALUES (");
	int n = static_cast<int>(rowItem.size());
	for (int i = 1; i < n; i++) {
		std::wstring val = StringUtil::escapeSql(rowItem.at(i));
		if (i > 1) {
			valuesClause.append(L", ");
		}
		if (val == L"< AUTO >" || val == L"< NULL >") {
			valuesClause.append(L"NULL");
		} else {
			valuesClause.append(qua).append(val).append(qua);
		}
		
	}
	valuesClause.append(L")");

	return valuesClause;
}

std::wstring SqlUtil::makeTmpTableName(const std::wstring & tblName, int number,  const std::wstring & prefix /*= std::wstring(L"ctsqlite_tmp_")*/)
{
	std::wstring result = prefix;
	result.append(tblName).append(L"_").append(std::to_wstring(number));
	return result;
}

/**
 * Parse all columns from create table DDL.
 * 
 * @param createTblSql
 * @return IndexInfoList
 */
ColumnInfoList SqlUtil::parseColumnsByTableDDL(const std::wstring & createTblSql)
{
	if (createTblSql.empty()) {
		return ColumnInfoList();
	}

	size_t paren_first = createTblSql.find_first_of(L'(');
	size_t paren_end = createTblSql.find_last_of(L')');
	if (paren_first == std::wstring::npos || paren_end == std::wstring::npos) {
		return ColumnInfoList();
	}

	std::wstring columnsAndConstrains = createTblSql.substr(paren_first + 1, paren_end - paren_first - 1);
	std::vector<std::wstring> lineList = splitTableDDLColumnClausesToLines(columnsAndConstrains,true);

	ColumnInfoList result;
	for (auto & line : lineList) {		
		ColumnInfo columnInfo = SqlUtil::parseColumnFromLine(line);
		if (columnInfo.name.empty()) {
			continue;
		}
		result.push_back(columnInfo);
	}
	return result;
}

/**
 * Parse all constraints from create table DDL.
 * 
 * @param createTblSql
 * @return IndexInfoList
 */
IndexInfoList SqlUtil::parseConstraints(const std::wstring & createTblSql)
{
	if (createTblSql.empty()) {
		return IndexInfoList();
	}

	size_t paren_first = createTblSql.find_first_of(L'(');
	size_t paren_end = createTblSql.find_last_of(L')');
	if (paren_first == std::wstring::npos || paren_end == std::wstring::npos) {
		return IndexInfoList();
	}

	std::wstring columnsAndConstrains = createTblSql.substr(paren_first + 1, paren_end - paren_first - 1);
	std::vector<std::wstring> lineList = splitTableDDLColumnClausesToLines(columnsAndConstrains,true);

	IndexInfoList result;
	for (auto & line : lineList) {		
		IndexInfo indexInfo = SqlUtil::parseConstraintFromLine(line);
		if (indexInfo.columns.empty()) {
			continue;
		}
		// Get rid of the duplicated indexes
		auto iter = std::find_if(result.begin(), result.end(), [&indexInfo](IndexInfo & item) {
			return item.type == indexInfo.type && item.columns == indexInfo.columns;
		});
		if (iter == result.end()) {
			result.push_back(indexInfo);
		}		
	}
	return result;
}

/**
 * Parse primary key constraints only from create table sql.
 * 
 * @param createTblSql
 * @return IndexInfoList
 */
IndexInfo SqlUtil::parseConstraintsForPrimaryKey(const std::wstring & createTblSql)
{
	if (createTblSql.empty()) {
		return IndexInfo();
	}

	size_t paren_first = createTblSql.find_first_of(L'(');
	size_t paren_end = createTblSql.find_last_of(L')');
	if (paren_first == std::wstring::npos || paren_end == std::wstring::npos) {
		return IndexInfo();
	}

	std::wstring columnsAndConstrains = createTblSql.substr(paren_first + 1, paren_end - paren_first - 1);
	// std::vector<std::wstring> lineList = StringUtil::split(columnsAndConstrains, L",", true);
	std::vector<std::wstring> lineList = splitTableDDLColumnClausesToLines(columnsAndConstrains,true);
		
	for (auto & line : lineList) {
		std::wstring upline = StringUtil::toupper(line);
		if (upline.find(L"PRIMARY") != std::wstring::npos) {
			return SqlUtil::parseLineToPrimaryKey(line, true);
		}		
	}
	return IndexInfo();
}

/**
 * Split columns and index to text lines in the create table DDL.
 * Such as :
*      "is_delete" INTEGER NOT NULL DEFAULT (0), // will be a line
*      "created_at" DATETIME NOT NULL DEFAULT (''), // will be a line
*      "updated_at" DATETIME NOT NULL DEFAULT (''), // will be a line
*       PRIMARY KEY(id,analysis_type,sample_lib_id) // will be a line
 * @param str
 * @param bTrim
 * @return string vector 
 */
std::vector<std::wstring> SqlUtil::splitTableDDLColumnClausesToLines(std::wstring str, bool bTrim /*= true*/)
{
	std::wstring pattern = L","; // column or index clause line split character
	
	std::wstring::size_type pos;
	std::vector<std::wstring> result;
	str.append(pattern); // 扩展字符串以方便操作

	// not found the pattern character between ignore_begin and ignore_end
	wchar_t ignore_begin = L'('; 
	wchar_t ignore_end = L')'; 
	
	size_t size = static_cast<int>(str.size());
	size_t from = 0;
	for (size_t i = 0; i < size; i++) {
		size_t ignore_begin_pos = str.find_first_of(ignore_begin, i);
		size_t ignore_end_pos = str.find_first_of(ignore_end, i);
		pos = str.find(pattern, i);
		if (pos < size && (pos < ignore_begin_pos || pos > ignore_end_pos)) {
			std::wstring s = str.substr(from, pos - from);
			if (s.empty()) {
				i = pos + pattern.size() - 1;
				continue;
			}
			if (bTrim && !s.empty()) {
				StringUtil::trim(s);
			}
			result.push_back(s);
			i = pos + pattern.size() - 1;
			from = pos + pattern.size();
		} else {
			i = ignore_end_pos ;
		}
	}
	return result;
}

ForeignKeyList SqlUtil::parseForeignKeysByTableDDL(const std::wstring & createTblSql)
{
	if (createTblSql.empty()) {
		return ForeignKeyList();
	}

	size_t paren_first = createTblSql.find_first_of(L'(');
	size_t paren_end = createTblSql.find_last_of(L')');
	if (paren_first == std::wstring::npos || paren_end == std::wstring::npos) {
		return ForeignKeyList();
	}

	std::wstring columnsAndConstrains = createTblSql.substr(paren_first + 1, paren_end - paren_first - 1);
	std::vector<std::wstring> lineList = splitTableDDLColumnClausesToLines(columnsAndConstrains, true);

	ForeignKeyList result;
	for (auto & line : lineList) {
		ForeignKey foreignKey = SqlUtil::parseForeignKeyFromLine(line);
		if (foreignKey.columns.empty()) {
			continue;
		}
		result.push_back(foreignKey);
	}
	return result;
}

/**
 * Parse one line to IndexInfo object, line text such as "PRIMARY KEY ("uid")"
 * 
 * @param line
 * @return 
 */
IndexInfo SqlUtil::parseConstraintFromLine(const std::wstring& line)
{
	IndexInfo result;
	std::wstring upline = StringUtil::toupper(line);
	if (upline.find(L"PRIMARY") != std::wstring::npos) {
		IndexInfo indexInfo = SqlUtil::parseLineToPrimaryKey(line, true); // from constrain only
		if (indexInfo.name.empty() && indexInfo.type.empty()) {
			indexInfo = SqlUtil::parseLineToPrimaryKey(line, false); // from column only
		}
		return indexInfo;
	}
	if (upline.find(L"UNIQUE") != std::wstring::npos) {
		return SqlUtil::parseLineToUnique(line, true);
	}

	if (upline.find(L"CHECK") != std::wstring::npos) {
		return SqlUtil::parseLineToCheck(line, true);
	}
	return result;
}

/**
 * Parse the primary key line, you can parse the column line or the constraint line through specified isConstainLine.
 * 
 * @param line
 * @param isConstaintLine - true : parse the constraint line only, false : parse the column line only
 * @return 
 */
IndexInfo SqlUtil::parseLineToPrimaryKey(const std::wstring& line, bool isConstaintLine)
{
	if (line.empty()) {
		return IndexInfo();
	}
	std::vector<std::wstring> words = StringUtil::splitByBlank(line, true);
	if (words.empty()) {
		return IndexInfo();
	}

	IndexInfo result;
	std::wstring first = StringUtil::toupper(words.at(0));
	if (isConstaintLine && first == L"CONSTRAINT") {
		// such as : CONSTRAINT "idx_name1" Primary Key("id" AUTOINCREMENT)
		result.name = StringUtil::cutParensAndQuotes(words.at(1));
	} 
	
	bool isFound = false;
	if (isConstaintLine) { // parse the constraint line only
		if ((first == L"PRIMARY" || first.find(L"PRIMARY") == 0) // such as : Primary Key("id" AUTOINCREMENT)
			|| (words.size() > 2 && StringUtil::toupper(words.at(2)) == L"PRIMARY")){ // such as : CONSTRAINT "idx_name1" Primary Key("id" AUTOINCREMENT)
			result.type = L"Primary Key";
			result.pk = 1;
			auto pair = getColumnAndAiFromPrimaryKeyLine(line);
			if (pair.first.empty()) {
				return IndexInfo();
			}
			result.columns = pair.first;
			result.ai = pair.second;
			// return conflict-clause
			result.partialClause = getConflictClauseFromConstraintLine(line);
			isFound = true;
		} else {
			return IndexInfo();
		}
	} else { // parse the column line only
		std::wstring upline = StringUtil::toupper(line);		
		// the column line contains PRIMARY KEY, such as : "id" INTEGER NOT NULL PRIMARY KEY
		result.columns = StringUtil::cutParensAndQuotes(words.at(0));
		result.type = L"Primary Key";
		result.pk = 1;
		if (upline.find(L"AUTOINCREMENT") != std::wstring::npos) {
			result.ai = 1;
		}
	}
	

	result.sql = line;
	result.seq = std::chrono::system_clock::now();
	return result;

}

/**
 * Parse the Unique line, you can parse the column line or the constraint line with specified isConstainLine=true/false.
 * 
 * @param line
 * @param isConstaintLine - true : parse the constraint line only, false : parse the column line only
 * @return 
 */
IndexInfo SqlUtil::parseLineToUnique(const std::wstring& line, bool isConstaintLine)
{
	if (line.empty()) {
		return IndexInfo();
	}
	std::vector<std::wstring> words = StringUtil::splitByBlank(line, true);
	if (words.empty()) {
		return IndexInfo();
	}

	IndexInfo result;
	std::wstring first = StringUtil::toupper(words.at(0));
	if (isConstaintLine && first == L"CONSTRAINT") {
		// such as : CONSTRAINT "idx_name2"  UNIQUE("id","name"...)
		result.name = StringUtil::cutParensAndQuotes(words.at(1));	}
	
	if (isConstaintLine) { // parse the constraint line only
		if ( (first == L"UNIQUE" || first.find(L"UNIQUE") == 0) // such as : UNIQUE("id","name"...)
			|| (words.size() > 2  && StringUtil::toupper(words.at(2)).find(L"UNIQUE") == 0)) { // such as : CONSTRAINT "idx_name2"  UNIQUE("id","name"...)
			result.type = L"Unique";
			result.un = 1;
			auto columns = getColumnsFromUniqueLine(line);
			if (columns.empty()) {
				return IndexInfo();
			}
			result.columns = columns;
			// return conflict-clause
			result.partialClause = getConflictClauseFromConstraintLine(line);
		} else {
			return IndexInfo();
		}
	} else { // parse the column line only
		// the column line contains UNIQUE, such as : "id" INTEGER NOT NULL PRIMARY KEY
		result.columns = StringUtil::cutParensAndQuotes(words.at(0));
		result.type = L"Unique";
		result.un = 1;
		std::wstring upline = StringUtil::toupper(line);
		if (upline.find(L"AUTOINCREMENT") != std::wstring::npos) {
			return IndexInfo();
		} else if (upline.find(L"PRIMARY") != std::wstring::npos) {
			return IndexInfo();
		}			
	}

	result.sql = line;
	result.seq = std::chrono::system_clock::now();
	return result;

}

/**
 * Parse the Check line, you can parse the column line or the constraint line with specified isConstainLine=true/false.
 * 
 * @param line
 * @param isConstaintLine - true : parse the constraint line only, false : parse the column line only
 * @return 
 */
IndexInfo SqlUtil::parseLineToCheck(const std::wstring& line, bool isConstaintLine)
{
	if (line.empty()) {
		return IndexInfo();
	}
	std::vector<std::wstring> words = StringUtil::splitByBlank(line, true);
	if (words.empty()) {
		return IndexInfo();
	}

	IndexInfo result;
	std::wstring first = StringUtil::toupper(words.at(0));
	if (isConstaintLine && first == L"CONSTRAINT") {
		// such as : CONSTRAINT "idx_chk"  CHECK("id" > 8)
		result.name = StringUtil::cutParensAndQuotes(words.at(1));
	}
	
	if (isConstaintLine) { // parse the constraint line only
		if ( (first == L"CHECK") // such as : UNIQUE("id","name"...)
			|| (words.size() > 2 && StringUtil::toupper(words.at(2)) == L"CHECK") ) { // such as : CONSTRAINT "idx_check"  CHECK("id" > 8)
			result.type = L"Check";
			result.ck = 1;
			auto expression = getExpressionByCheckLine(line);
			if (expression.empty()) {
				return IndexInfo();
			}
			result.columns = expression;
		} else {
			return IndexInfo();
		}
	} else { // parse the column line only
		std::wstring upline = StringUtil::toupper(line);		
		// the column line contains PRIMARY KEY, such as : "id" INTEGER NOT NULL PRIMARY KEY
		result.columns = StringUtil::cutParensAndQuotes(words.at(0));
		result.type = L"Check";
		result.ck = 1;
		if (upline.find(L"AUTOINCREMENT") != std::wstring::npos) {
			return IndexInfo();
		} else if (upline.find(L"PRIMARY") != std::wstring::npos) {
			return IndexInfo();
		} else if (upline.find(L"UNIQUE") != std::wstring::npos) {
			return IndexInfo();
		}			
	}

	result.sql = line;
	result.seq = std::chrono::system_clock::now();
	return result;
}

/**
 * Fetch the column name and auto increment in the Primary key line. 
 * such as "Primary Key("id" AUTOINCREMENT)"
 * 
 * @param line
 * @return pair<std::wstring : column, uint_8 : ai> 
 *              - first:primary key column name, 
 *              - second: 1 if AUTOINCREMENT, 0 if not AUTOINCREMENT
 */
std::pair<std::wstring, uint8_t> SqlUtil::getColumnAndAiFromPrimaryKeyLine(const std::wstring &line)
{
	if (line.empty()) {
		return std::pair<std::wstring, int>();
	}

	size_t paren_first = line.find_first_of(L'(');
	size_t paren_end = line.find_last_of(L')');
	if (paren_first == std::wstring::npos || paren_end == std::wstring::npos) {
		return std::pair<std::wstring, int>();
	}

	std::wstring parenClause = line.substr(paren_first + 1, paren_end - paren_first - 1);
	std::vector<std::wstring> words = StringUtil::splitByBlank(parenClause, true);
	if (words.empty()) {
		return std::pair<std::wstring, int>();
	}
	std::wstring columns;
	if (words.at(0).find_first_of(L',') == std::wstring::npos) {
		 // not found ',', single column ,such as "Primary key(id AUTOINCREMENT)"
		columns = StringUtil::cutParensAndQuotes(words.at(0));
	} else { 
		// found ',', multiple columns, such as "Primary key(id,name,...)"
		std::vector<std::wstring> columnVect = StringUtil::split(words.at(0), L",", true);

		// Get gid of the symbol 
		int i = 0;
		for (auto & colItem : columnVect) {
			colItem = StringUtil::cutParensAndQuotes(colItem);
			StringUtil::trim(colItem);
			if (colItem.empty()) {
				continue;
			}
			if (i++ > 0) {
				columns.append(L",");
			}
			columns.append(colItem);
		}
	}
	uint8_t ai = 0;
	if (words.size() > 1 && StringUtil::toupper(words.at(1)) == L"AUTOINCREMENT") {
		ai = 1;
	}
	return { columns, ai };
}

/**
 * return conflict-clause from the primary-key line or a unique-line
 *  conflict-clause : such as "ON CONFLICT ROLLBACK/ABORT/FAIL/IGNORE/REPLACE"
 * 
 * @param line
 * @return 
 */
std::wstring SqlUtil::getConflictClauseFromConstraintLine(const std::wstring& line)
{
	std::wstring partialClause;
	if (line.empty()) {
		return partialClause;
	}
	std::wstring upline = StringUtil::toupper(line);
	if (upline.find(L"PRIMARY") == std::wstring::npos 
		&& upline.find(L"UNIQUE") == std::wstring::npos) {
		return partialClause;
	}

	size_t onPos = line.find(L"ON");
	if (onPos == std::wstring::npos) {
		return partialClause;
	}
	partialClause = line.substr(onPos, -1);
	return partialClause;
}

/**
 * Fetch the column names in the unique line. 
 * such as : CONSTRAINT "idx_name2"  UNIQUE("id","name"...)
 * 
 * @param line
 * @return columns
 */
std::wstring SqlUtil::getColumnsFromUniqueLine(const std::wstring &line)
{
	if (line.empty()) {
		return L"";
	}

	size_t paren_first = line.find_first_of(L'(');
	size_t paren_end = line.find_last_of(L')');
	if (paren_first == std::wstring::npos || paren_end == std::wstring::npos) {
		return L"";
	}

	std::wstring result = line.substr(paren_first + 1, paren_end - paren_first - 1);
	result = StringUtil::cutParensAndQuotes(result);
	return result;
}

/**
 * Fetch the column names in the unique line. 
 * such as : CONSTRAINT "idx_check"  CHECK("id" > 8)
 * 
 * @param line
 * @return columns
 */
std::wstring SqlUtil::getExpressionByCheckLine(const std::wstring &line)
{
	if (line.empty()) {
		return L"";
	}

	size_t paren_first = line.find_first_of(L'(');
	size_t paren_end = line.find_last_of(L')');
	if (paren_first == std::wstring::npos || paren_end == std::wstring::npos) {
		return L"";
	}

	std::wstring parenClause = line.substr(paren_first + 1, paren_end - paren_first - 1);
	std::vector<std::wstring> words = StringUtil::splitByBlank(parenClause, true);
	if (words.empty()) {
		return L"";
	}
	std::wstring result;
	int i = 0;
	for (auto & word : words) {
		auto column = StringUtil::cutParensAndQuotes(word);
		StringUtil::trim(column);
		if (column.empty()) {
			continue;
		}
		if (i++ > 0) {
			result.append(L",");
		}
		result.append(column);
	}
	return result;
}


ColumnInfo SqlUtil::parseColumnFromLine(const std::wstring& line)
{
	ColumnInfo result;
	std::wstring upline = StringUtil::toupper(line);
	auto words = StringUtil::splitByBlank(line, true);
	if (words.empty() || words.at(0).empty()) {
		return result;
	}

	// exclude the constrains
	std::wstring & first = words.at(0);
	std::wstring upfirst = StringUtil::toupper(first);
	if (upfirst == L"CONSTRAINT"
		|| upfirst == L"PRIMARY"
		|| upfirst == L"UNIQUE" 
		|| upfirst == L"CHECK"
		|| upfirst == L"FOREIGN") {
		return result;
	}

	// first word must be column name
	std::wstring columnName = StringUtil::cutParensAndQuotes(first);
	if (columnName.empty()) {
		return result;
	}
	result.name = columnName;

	if (words.size() > 1 && !words.at(1).empty()) {
		result.type = words.at(1);
	}
	if (upline.find(L"NOT NULL") != std::wstring::npos) {
		result.notnull = 1;
	}

	size_t defPos = -1;
	if ((defPos = upline.find(L"DEFAULT")) != std::wstring::npos) {
		wchar_t ch = StringUtil::nextNotBlankChar(line, L"DEFAULT", defPos);
		if (ch == L'(') {
			result.defVal = StringUtil::inSymbolString(line, L'(', L')', defPos);
		} else {
			auto iter = std::find_if(words.begin(), words.end(), [](std::wstring &str) {
				return StringUtil::toupper(str) == L"DEFAULT";
			});
			while (iter != words.end() && ++iter != words.end()) {
				if ((*iter).empty()) {
					continue;
				}
				result.defVal = (*iter);
				break;
			}
		}		
	}

	if (upline.find(L"PRIMARY") != std::wstring::npos) {
		result.pk = 1;
	}

	if (upline.find(L"UNIQUE") != std::wstring::npos) {
		result.un = 1;
	}

	if (upline.find(L"AUTOINCREMENT") != std::wstring::npos) {
		result.ai = 1;
	}

	if ((defPos = upline.find(L"CHECK")) != std::wstring::npos) {
		size_t paren_first = line.find_first_of(L'(', defPos);
		size_t paren_end = line.find_first_of(L')', defPos);
		if (paren_first != std::wstring::npos && paren_end != std::wstring::npos) {
			result.checks = StringUtil::inSymbolString(line, L'(', L')', defPos);
		} else {
			auto iter = std::find_if(words.begin(), words.end(), [](std::wstring &str) {
				return StringUtil::toupper(str) == L"DEFAULT";
			});
			while (iter != words.end()) {
				if ((*iter).empty()) {
					iter++;
				}
				result.checks = (*iter);
				break;
			}
		}
	}

	return result;
}

/**
 * parse ddl line to a ForeignKey object
 * foreig key supported such as :
 *  // CONSTRAINT `name` FOREIGN KEY (`dog_id`) REFERENCES `asia_dog` (`id`) ON DELETE SET NULL ON UPDATE RESTRICT
 * 
 * @param line
 * @return 
 */
ForeignKey SqlUtil::parseForeignKeyFromLine(const std::wstring& line)
{
	ForeignKey result;
	std::wstring upline = StringUtil::toupper(line);
	auto words = StringUtil::splitByBlank(line, true);
	if (words.empty() || words.at(0).empty()) {
		return result;
	}

	// exclude the constrains
	std::wstring first = StringUtil::toupper(words.at(0));
	if (first == L"PRIMARY"
		|| first == L"UNIQUE" 
		|| first == L"CHECK") {
		return result;
	}

	// 1. fetch name
	if (first == L"CONSTRAINT") {
		// such as : CONSTRAINT "foreign_key_idx1"  FOREIGN KEY("uid")
		result.name = StringUtil::cutParensAndQuotes(words.at(1));
	}
	
	size_t defPos = -1;
	if (words.size() <= 2 || 
		((first != L"FOREIGN") // such as : FOREIGN KEY("id","name"...)
		&& (words.size() > 2 && StringUtil::toupper(words.at(2)) != L"FOREIGN"))) { // such as : CONSTRAINT "idx_check"  FOREIGN KEY("uid")...
		return result;
	}
		
	// 2. fetch columns
	defPos = upline.find(L"FOREIGN");
	wchar_t ch = StringUtil::nextNotBlankChar(upline, L"FOREIGN KEY", defPos);
	if (ch == L'(') {
		result.columns = StringUtil::inSymbolString(line, L'(', L')', defPos);
		StringUtil::trim(result.columns);
		result.columns = StringUtil::cutParensAndQuotes(result.columns);
	}

	// 2. fetch referenced table and columns
	auto iter = std::find_if(words.begin(), words.end(), [](const std::wstring & word) {
		return StringUtil::toupper(word) == L"REFERENCES";
	});
	if (iter != words.end() && ++iter != words.end()) {
		result.referencedTable = StringUtil::cutParensAndQuotes((*iter));

		defPos = upline.find(L"REFERENCES");
		result.referencedColumns = StringUtil::inSymbolString(line, L'(', L')', defPos);
		StringUtil::trim(result.referencedColumns);
		result.referencedColumns = StringUtil::cutParensAndQuotes(result.referencedColumns);
	}
		
	// 3. fetch on update / on delete
	size_t n = words.size();
	for (size_t i = 0; i < n - 2; i++) {
		auto word = StringUtil::toupper(words.at(i));
		std::wstring next1 = (i + 1) < n ? StringUtil::toupper(words.at(i + 1)) : L"";
		std::wstring next2 = (i + 2) < n ? StringUtil::toupper(words.at(i + 2)) : L"";
		std::wstring next3 = (i + 3) < n ? StringUtil::toupper(words.at(i + 3)) : L"";
		if (word == L"ON" && next1 == L"UPDATE") {				
			if (next2 == L"SET" || next2 == L"ON") {
				result.onUpdate = next2 + L" " + next3;
			} else {
				result.onUpdate = next2;
			}
		} else if (word == L"ON" && next1 == L"DELETE") {
			if (next2 == L"SET" || next2 == L"ON") {
				result.onDelete = next2 + L" " + next3;
			} else {
				result.onDelete = next2;
			}
		}
	}
	result.partialClause = getPartialClauseFromForeignKeyLine(line);
	result.seq = std::chrono::system_clock::now();
	return result;
}

std::wstring SqlUtil::getPartialClauseFromForeignKeyLine(const std::wstring& line)
{
	std::wstring partialClause;
	if (line.empty()) {
		return partialClause;
	}
	std::wstring upline = StringUtil::toupper(line);
	if (upline.find(L"FOREIGN") == std::wstring::npos ) {
		return partialClause;
	}

	size_t notDefferPos = upline.find(L"NOT DEFERRABLE");
	size_t fromPos = -1;
	if (notDefferPos == std::wstring::npos) {
		size_t defferPos = upline.find(L"DEFERRABLE");
		if (defferPos == std::wstring::npos) {
			return partialClause;
		}
		fromPos = defferPos;
	} else {
		fromPos = notDefferPos;
	}
	
	partialClause = line.substr(fromPos, -1);
	return partialClause;
}

/**
 * Replace the number of suffix in the table name. use for table sharding
 * such as : "analysis_23" => "analysis_{n}"; 
 * 
 * @param tblName
 * @param after
 * @return 
 */
std::wstring SqlUtil::replaceNumOfSuffixInTblName(const std::wstring & tblName, const std::wstring & after)
{
	if (tblName.empty() || after.empty()) {
		return tblName;
	}
	if (tblName.find(after) != std::wstring::npos) {
		return tblName;
	}

	std::regex_constants::match_flag_type fonly =
        std::regex_constants::format_first_only;
	
	std::wstring str = std::regex_replace(tblName, SqlUtil::tblNumSuffixPat, after, fonly);
	if (str.find(after) != std::wstring::npos) {
		return str;
	}
	str.append(L"_").append(after);
	return str;
}

/**
 * parse table clause from select sql, such as "SELECT * FROM [tbl_1 as alias_1, tbl_2 as alias_2] WHERE ..."
 * 
 * @param selectSql
 * @param endWord - substring begin FROM to [endWord]
 * @return  - up case vector ,such as : [{tbl:"ANALYSIS", alis:"A"},{...}]
 */
TableAliasVector SqlUtil::parseTableClauseFromSelectSql(const std::wstring & upSql)
{
	TableAliasVector result;
	if (upSql.empty()) {
		return result;
	}
	size_t npos = std::wstring::npos;
	size_t fromPos = upSql.find(L" FROM ");
	size_t onPos, wherePos, groupPos, orderPos, limitPos;
	if (fromPos == npos) {
		return result;
	}

	std::wstring tblClause;
	size_t endPos = upSql.size() -1;
	if ((onPos = upSql.rfind(L" ON ")) != npos && onPos > fromPos) {
		endPos = onPos;
	}else if ((wherePos = upSql.rfind(L" WHERE ")) != npos && wherePos > fromPos) {
		endPos = wherePos;
	} else if ((groupPos = upSql.rfind(L" GROUP ")) != npos && groupPos > fromPos) {
		endPos = groupPos;
	} else if ((orderPos = upSql.rfind(L" ORDER ")) != npos && orderPos > fromPos) {
		endPos = orderPos;
	} else if ((limitPos = upSql.rfind(L" LIMIT ")) != npos && limitPos > fromPos) {
		endPos = limitPos;
	}

	tblClause = upSql.substr(fromPos + 6, endPos - fromPos - 6);  // 4 - size of " FROM "
	StringUtil::trim(tblClause);

	if (tblClause.empty()) {
		return result;
	}

	std::wstring pattern;
	if (tblClause.find(L",") != std::wstring::npos ) {
		pattern = L",";
	} else if (tblClause.find(L" JOIN ") != std::wstring::npos ) {
		pattern = L" JOIN ";
	}

	std::vector<std::wstring> tbls;
	if (!pattern.empty()) {
		// [tbl_1 as alias_1, tbl_2 as alias_2]; [tbl_1 as alias_1 left join tbl_2 as alias_2]
		tbls = StringUtil::split(tblClause, pattern); 
	} else {
		tbls.push_back(tblClause);
	}
	
	for (auto & tbl : tbls) {
		TableAlias item;
		auto tblWords = StringUtil::splitByBlank(tbl); // tbl_1
		item.tbl = StringUtil::cutParensAndQuotes(tblWords.at(0));
		if (tblWords.size() > 2 && tblWords.at(1) == L"AS") { // tbl_1 as alias_1
			item.alias = StringUtil::cutParensAndQuotes(tblWords.at(2));
		} else if (tblWords.size() > 1) { // tbl_1 alias_1
			item.alias = StringUtil::cutParensAndQuotes(tblWords.at(1));
		}
		result.push_back(item);
	}
	return result;
}

/**
 * parse table clause from update sql, such as "SELECT * FROM [tbl_1 as alias_1, tbl_2 as alias_2] WHERE ..."
 * 
 * @param selectSql
 * @param endWord - substring begin FROM to [endWord]
 * @return 
 */
TableAliasVector SqlUtil::parseTableClauseFromUpdateSql(const std::wstring & upSql)
{
	TableAliasVector result;
	if (upSql.empty()) {
		return result;
	}
	size_t npos = std::wstring::npos;
	size_t updatePos = upSql.find(L"UPDATE");
	if (updatePos == npos) {
		return result;
	}

	size_t onPos, setPos;
	std::wstring tblClause;
	size_t endPos = upSql.size() -1;
	if ((onPos = upSql.rfind(L" ON ")) != npos && onPos > updatePos) {
		endPos = onPos;
	}else if ((setPos = upSql.rfind(L" SET ")) != npos && setPos > updatePos) {
		endPos = setPos;
	}

	tblClause = upSql.substr(updatePos + 6, endPos - updatePos - 6); // 6 - size of "UPDATE"
	StringUtil::trim(tblClause);

	if (tblClause.empty()) {
		return result;
	}

	std::wstring pattern;
	if (tblClause.find(L",") != std::wstring::npos ) {
		pattern = L",";
	} 

	std::vector<std::wstring> tbls;
	if (!pattern.empty()) {
		// [tbl_1 as alias_1, tbl_2 as alias_2];
		tbls = StringUtil::split(tblClause, pattern); 
	} else {
		tbls.push_back(tblClause);
	}
	for (auto & tbl : tbls) {
		TableAlias item;
		auto tblWords = StringUtil::splitByBlank(tbl); // tbl_1
		item.tbl = StringUtil::cutParensAndQuotes(tblWords.at(0));
		if (tblWords.size() > 2 && tblWords.at(1) == L"AS") { // tbl_1 as alias_1
			item.alias = StringUtil::cutParensAndQuotes(tblWords.at(2));
		} else if (tblWords.size() > 1) { // tbl_1 alias_1
			item.alias = StringUtil::cutParensAndQuotes(tblWords.at(1));
		} 
		result.push_back(item);
	}
	return result;
}


/**
 * Pars the create index DDL statement to IndexInfo.
 * For example : CREATE UNIQUE INDEX "name" ON "analysis_sample_class_16" ("sample_lib_id","inspection_id");
 * 
 * @param createIndexSql
 * @return 
 */
IndexInfo SqlUtil::parseCreateIndex(const std::wstring & createIndexSql)
{	
	if (createIndexSql.empty()) {
		return IndexInfo();
	}

	std::wstring upline = StringUtil::toupper(createIndexSql);
	std::vector<std::wstring> upwords = StringUtil::splitByBlank(upline);
	if (upwords.empty() || upwords.at(0) != L"CREATE" || (upwords.at(1) != L"INDEX" && upwords.at(2) != L"INDEX")) {
		return IndexInfo();
	}
	IndexInfo result;
	result.type = L"Index";
	// Find the index name and table name and unique
	size_t n = upwords.size();
	for (size_t i = 0; i < n; i++) {
		auto & word = upwords.at(i);
		if (word == L"ON") {
			if (upwords.at(i - 1) != L"INDEX" && upwords.at(i - 1) != L"EXISTS") {
				std::vector<std::wstring> words = StringUtil::splitByBlank(createIndexSql);
				result.name = StringUtil::cutParensAndQuotes(words.at(i - 1));
			}
		}
		if (word == L"UNIQUE") {
			result.un = 1;
		}
	}
	// Find the columns, it must be between in the first ()
	size_t paren_first = upline.find_first_of(L'(');
	size_t paren_end = upline.find_first_of(L')');
	if (paren_first == std::wstring::npos || paren_end == std::wstring::npos) {
		return IndexInfo();
	}	
	auto columns = createIndexSql.substr(paren_first + 1, paren_end - paren_first - 1);
	columns = StringUtil::cutParensAndQuotes(columns);
	auto columnVector = StringUtil::split(columns, L",");
	columns = StringUtil::implode(columnVector, L",");
	result.columns = StringUtil::cutParensAndQuotes(columns);

	// Find the partial clause, it must be start with "WHERE"
	auto wherePos = upline.find(L"WHERE");
	if (wherePos != std::wstring::npos) {
		result.partialClause = createIndexSql.substr(wherePos);
	}

	return result;
}

/**
 * Parse the sql, get the alias of specified table.
 * 
 * @param sql - sql statement
 * @param table - specified table
 * @param tables - all table names of sql
 * @return UPCASE alias/table
 */
std::wstring SqlUtil::parseTableAliasFromSelectSql(const std::wstring & sql, const std::wstring & table, const std::vector<std::wstring> & tables)
{
	std::wstring uptable = StringUtil::toupper(table);
	size_t tblLen = tables.size();
	if (tables.empty()) {
		return uptable;
	}
	std::wstring alias;	
	std::wstring upsql = StringUtil::toupper(sql);
	upsql = StringUtil::replace(upsql, L",", L" ");

	std::vector<std::wstring> words = StringUtil::splitByBlank(upsql);
	return parseTableAliasFromSelectSqlUpWords(words, uptable, tables);
}

/**
 * Parse the sql up case words vector, get the alias of specified table..
 * 
 * @param upWords - Up case word vector of sql statement split by blank
 * @param upTable - Table name of up case
 * @param tables - All of table names for sql
 * @return table alias such as : "customer as c" return "c", "customer c" return "c"
 */
std::wstring SqlUtil::parseTableAliasFromSelectSqlUpWords(const std::vector<std::wstring>& upWords, std::wstring & upTable, const UserTableStrings & tables)
{
	if (upWords.empty() || upTable.empty() || tables.empty()) {
		return upTable;
	}
	size_t n = upWords.size();
	int i = 0;
	for (int i = 0; i < n - 1; i++) {
		auto & word = upWords.at(i);
		if (word != upTable) {
			continue;
		}
		
		if (i < n - 2 && upWords.at(i + 1) == L"AS" && !(upWords.at(i + 2).empty())) {
			return upWords.at(i + 2);
		}
		auto & nextWord = upWords.at(i + 1);
		if (nextWord == L"NATURAL" || nextWord == L"JOIN"
			|| nextWord == L"LEFT" || nextWord == L"RIGHT"
			|| nextWord == L"FULL" || nextWord == L"INNER"
			|| nextWord == L"CROSS" || nextWord == L"OUTER"
			|| nextWord == L"WHERE" || nextWord == L"ORDER" 
			|| nextWord == L"ON" || nextWord == L"GROUP"
			|| nextWord == L"HAVING" || nextWord == L"LIMIT"
			|| nextWord == L"OFFSET" || nextWord == L"WINDOW"
			|| nextWord == L"(" || nextWord == L"(SELECT") {
			return upTable;
		}

		auto iter = std::find_if(tables.begin(), tables.end(), [&nextWord] (auto & tblName) {
			return nextWord == StringUtil::toupper(tblName);
		});
		if (iter == tables.end()) {
			return nextWord;
		}
		
	}
	return L"";
}

/**
 * Parse columns of "is null" or "is not null" in the sql statement
 * 
 * @param sqlWords - word vector of be splitted sql statement with blank
 * @param allAliases - pair vector, params: pair.first - table, pair.second - table alias
 * @return columns tuple, tuple params: pair.first - table name, pair.second - columns or is not null columns
 */
std::vector<std::pair<std::wstring, std::wstring>> SqlUtil::parseWhereNullColumnsFromSelectSqlUpWords(
	const std::vector<std::wstring>& upSqlWords, 
	const std::vector<std::pair<std::wstring, std::wstring>> & allAliases)
{
	std::vector<std::pair<std::wstring, std::wstring>> results;
	int n = static_cast<int>(upSqlWords.size());
	for (int i = 0; i < n - 1; i++) {
		auto & word = upSqlWords.at(i);
		auto & nextWord = upSqlWords.at(i + 1);
		if ((word == L"IS" && (nextWord == L"NOT" || nextWord == L"NULL"))
			|| word == L"ISNULL" || word == L"NOTNULL") {
			auto & columnWord = upSqlWords.at(i - 1);
			auto pos = columnWord.find_first_of(L'.');
			if (pos != std::wstring::npos) {
				// columnWord, such as "t.name" or "tbl.name"
				std::wstring tblAlias = columnWord.substr(0, pos); // table alias or table name, "t.name"--get->"t", "tbl.name"--get->"tbl"
				std::wstring columnName = columnWord.substr(pos + 1); // column name, "tbl.name"--get->"name"
				for (auto & pair : allAliases) {
					// pair.first - alias, pair.second - table 
					if (StringUtil::toupper(pair.first) == tblAlias || StringUtil::toupper(pair.second) == tblAlias) {
						 // found, add to results, {tableName, columnName}
						results.push_back({ pair.second, columnName });
					}
				}
			} else {
				const std::wstring & columnName = columnWord;
				if (i == 0) {
					continue;
				}
				// Find backward from i-1
				for (int j = i - 1; j >= 0; j--) {
					auto & prevWord = upSqlWords.at(j);
					for (auto & pair : allAliases) {
						// pair.first - alias, pair.second - table
						if (StringUtil::toupper(pair.first) == prevWord || StringUtil::toupper(pair.second) == prevWord) {	
							 // found, add to results, {tableName, columnName}
							results.push_back({ pair.second, columnName });
						}
					}// for allAliases
				} // for upSqlWords, Find backward from i-1
			}  // if pos != std::wstring::npos
		} // if ((word == L"IS"..
	} // upSqlWords

	return results;
}

