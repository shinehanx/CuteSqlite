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
#include "StringUtil.h"

std::wregex SqlUtil::selectPat(L"select\\s+(.*)\\s+from\\s+(.*)\\s*(where .*)?", std::regex::icase);

std::wregex SqlUtil::columnPat(L"(.*)\\s+\\[(.*)\\]+");


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
	std::wstring tblStmt = results[2];
	if (tblStmt.empty()) {
		return tbls;
	}
	
	auto vec = parseTablesFromTableStatement(tblStmt);
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

std::vector<std::wstring> SqlUtil::parseTablesFromTableStatement(std::wstring & tblStmt)
{
	std::vector<std::wstring> tbls;
	if (tblStmt.empty()) {
		return tbls;
	}
	std::wstring str(tblStmt);
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

		tbls.push_back(tag);
	}
	return tbls;
}
