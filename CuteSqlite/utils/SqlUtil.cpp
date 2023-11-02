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

std::wregex SqlUtil::selectPat(L"select\\s+(.*)\\s+from\\s+(.*)\\s*(where .*)?", std::wregex::icase);

std::wregex SqlUtil::whereClausePat1(L"((where)\\s+.*)\\s+(order|group|limit|having|window)+(.*)?", std::wregex::icase);

std::wregex SqlUtil::whereClausePat2(L"(where .*)+", std::wregex::icase);

std::wregex SqlUtil::limitClausePat(L"(limit .*)+", std::wregex::icase);

std::wregex SqlUtil::fourthClausePat(L"((order|group|limit|having|window)\\s+.*)+", std::wregex::icase);

std::wregex SqlUtil::primaryKeyPat(L"\\s+primary\\s+key\\s?\\(?(\\\"?([^(\\s|\\\"|\\s)]+)\\\"?\\s+)", std::wregex::icase);

std::wregex SqlUtil::columnPat(L"(.*)\\s+\\[(.*)\\]+");

std::wregex SqlUtil::tblNumSuffixPat(L"([0-9]+)$");



std::vector<std::wstring> SqlUtil::tableTags({ L"as", L"left", L"right",L"inner", L"cross", L"full", L"outer", L"natural", L"join" });

bool SqlUtil::isSelectSql(std::wstring & sql)
{
	if (sql.empty()) {
		return false;
	}

	if (std::regex_search(sql, SqlUtil::selectPat)) {
		return true;
	}
	return false;
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
std::vector<std::wstring> SqlUtil::getTablesFromSelectSql(std::wstring & selectSql, std::vector<std::wstring> allTables)
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
 * Fetch the where clause such as "select * from [tbl clause] [where clause] [fourth clause : order|having|window|group...]"...
 * 
 * @param sql
 * @return The string of whereClause
 */
std::wstring SqlUtil::getWhereClause(std::wstring & sql)
{
	std::wsmatch results;
	if (!std::regex_search(sql, results, SqlUtil::selectPat)) {
		return L"";
	}
	if (results.size() < 3 || results[2].matched == false) {
		return L"";
	}
	std::wsmatch whereResults;
	std::wstring whereStmt = results[2];
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
 * Fetch the fourth clause such as "select * from [tbl clause] [where clause] [fourth clause : order|group|limit|having|window...]"...
 * 
 * @param sql
 * @return The string of fourthClause
 */
std::wstring SqlUtil::getFourthClause(std::wstring & sql)
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
 * Make sql where clause from columns and vals.
 * @condition:
 *  1.columns and rowItem are not empty.
 *  2.columns.size must equals rowItem.size
 * 
 * @param runtimeColumns colums
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
				auto nSubItem = subItem.iSubItem - 1;
				return nSubItem == i;
			});

			val = iter == rowChangeVals.end() ? 
				StringUtil::escapeSql(rowItem.at(i)) 
				: StringUtil::escapeSql((*iter).origVal);
		}
		
			
		whereClause.append(L" \"").append(columns.at(i)).append(L"\"")
			.append(L"='").append(val).append(L"'");
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
		whereClause.append(L" \"").append(columns.at(i)).append(L"\"")
			.append(L"='").append(val).append(L"'");
		break;
	}

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
	for (int i = 0; i < n; i++) {
		std::wstring column = columns.at(i);
		if (i > 0) {
			columnsClause.append(L", ");
		}
		columnsClause.append(L"\"").append(column).append(L"\"");
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
	for (int i = 0; i < n; i++) {
		std::wstring val = StringUtil::escapeSql(rowItem.at(i));
		if (i > 0) {
			valuesClause.append(L", ");
		}
		if (val == L"(Auto)" || val == L"(NULL)") {
			valuesClause.append(L"NULL");
		} else {
			valuesClause.append(L"'").append(val).append(L"'");
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
		result.push_back(indexInfo);
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
			return SqlUtil::parseLineToPrimaryKey(line);
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
		return SqlUtil::parseLineToPrimaryKey(line);
	}
	if (upline.find(L"UNIQUE") != std::wstring::npos) {
		return SqlUtil::parseLineToUnique(line);
	}

	if (upline.find(L"CHECK") != std::wstring::npos) {
		return SqlUtil::parseLineToCheck(line);
	}
	return result;
}

IndexInfo SqlUtil::parseLineToPrimaryKey(const std::wstring& line)
{
	if (line.empty()) {
		return IndexInfo();
	}
	std::vector<std::wstring> words = StringUtil::split(line, L" ", true);
	if (words.empty()) {
		return IndexInfo();
	}

	IndexInfo result;
	std::wstring first = StringUtil::toupper(words.at(0));
	if (first == L"CONSTRAINT") {
		// such as : CONSTRAINT "idx_name1" Primary Key("id" AUTOINCREMENT)
		result.name = StringUtil::cutParensAndQuotes(words.at(1));
	} 
	
	bool isFound = false;
	if ((first == L"PRIMARY") // such as : Primary Key("id" AUTOINCREMENT)
		|| (words.size() > 2 && StringUtil::toupper(words.at(2)) == L"PRIMARY")){ // such as : CONSTRAINT "idx_name1" Primary Key("id" AUTOINCREMENT)
		result.type = L"Primary Key";
		result.pk = 1;
		auto pair = getColumnAndAiByPrimaryKeyLine(line);
		if (pair.first.empty()) {
			return IndexInfo();
		}
		result.columns = pair.first;
		result.ai = pair.second;
		isFound = true;
	} else {
		return IndexInfo();
	}

	result.sql = line;
	result.seq = std::chrono::system_clock::now();
	return result;

}


IndexInfo SqlUtil::parseLineToUnique(const std::wstring& line)
{
	if (line.empty()) {
		return IndexInfo();
	}
	std::vector<std::wstring> words = StringUtil::split(line, L" ", true);
	if (words.empty()) {
		return IndexInfo();
	}

	IndexInfo result;
	std::wstring first = StringUtil::toupper(words.at(0));
	if (first == L"CONSTRAINT") {
		// such as : CONSTRAINT "idx_name2"  UNIQUE("id","name"...)
		result.name = StringUtil::cutParensAndQuotes(words.at(1));
	}
	
	if ( (first == L"UNIQUE") // such as : UNIQUE("id","name"...)
		|| (words.size() > 2 && StringUtil::toupper(words.at(2)) == L"UNIQUE") ) { // such as : CONSTRAINT "idx_name2"  UNIQUE("id","name"...)
		result.type = L"Unique";
		result.un = 1;
		auto columns = getColumnsByUniqueLine(line);
		if (columns.empty()) {
			return IndexInfo();
		}
		result.columns = columns;
	} else {
		return IndexInfo();
	}

	result.sql = line;
	result.seq = std::chrono::system_clock::now();
	return result;

}


IndexInfo SqlUtil::parseLineToCheck(const std::wstring& line)
{
	if (line.empty()) {
		return IndexInfo();
	}
	std::vector<std::wstring> words = StringUtil::split(line, L" ", true);
	if (words.empty()) {
		return IndexInfo();
	}

	IndexInfo result;
	std::wstring first = StringUtil::toupper(words.at(0));
	if (first == L"CONSTRAINT") {
		// such as : CONSTRAINT "idx_chk"  CHECK("id" > 8)
		result.name = StringUtil::cutParensAndQuotes(words.at(1));
	}
	
	if ( (first == L"CHECK") // such as : UNIQUE("id","name"...)
		|| (words.size() > 2 && StringUtil::toupper(words.at(2)) == L"CHECK") ) { // such as : CONSTRAINT "idx_check"  CHECK("id" > 8)
		result.type = L"Check";
		result.un = 1;
		auto expression = getExpressionByCheckLine(line);
		if (expression.empty()) {
			return IndexInfo();
		}
		result.columns = expression;
	} else {
		return IndexInfo();
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
std::pair<std::wstring, uint8_t> SqlUtil::getColumnAndAiByPrimaryKeyLine(const std::wstring &line)
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
	std::vector<std::wstring> words = StringUtil::split(parenClause, L" ", true);
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
 * Fetch the column names in the unique line. 
 * such as : CONSTRAINT "idx_name2"  UNIQUE("id","name"...)
 * 
 * @param line
 * @return columns
 */
std::wstring SqlUtil::getColumnsByUniqueLine(const std::wstring &line)
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
	std::vector<std::wstring> words = StringUtil::split(parenClause, L",", true);
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
	auto words = StringUtil::split(line, L" ");
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
	auto words = StringUtil::split(line, L" ");
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

	result.seq = std::chrono::system_clock::now();
	return result;
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
