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

 * @file   SqlEditorAdapter.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-11-11
 *********************************************************************/
#include "stdafx.h"
#include "SqlEditorAdapter.h"
#include "utils/StringUtil.h"
#include "utils/SqlUtil.h"



SqlEditorAdapter::SqlEditorAdapter(QueryPageSupplier * supplier)
{
	this->supplier = supplier;
}

/**
 * Get tags for different SQL statement such as select sql statement or delete sql statement or update sql statement.
 * 
 * @param line - the current line string
 * @param preline - the string before current position 
 * @param word - current word
 * @param curPosInLine - The current position in the line
 * @return 
 */
std::vector<std::wstring> SqlEditorAdapter::getTags(const std::wstring & line, const std::wstring & preline, const std::wstring & word, size_t curPosInLine)
{
	if (word.empty() || line.empty() || preline.empty()) {
		return std::vector<std::wstring>();
	}

	auto sqlTags = supplier->sqlTags;
	auto functions = databaseService->getFunctionsStrings(supplier->getRuntimeUserDbId(), true);
	sqlTags.insert(sqlTags.end(), functions.begin(), functions.end());
	std::wstring upline = StringUtil::toupper(line);
	std::wstring upPreline = StringUtil::toupper(preline);
	std::wstring upword = StringUtil::toupper(word);

	std::vector<std::wstring> selTags;
	if (upPreline.find(L"SELECT") != std::wstring::npos 
		|| upPreline.find(L"DELETE") != std::wstring::npos) {
		selTags = getSelectTags(upline, upPreline, upword, curPosInLine);
		if (!selTags.empty()) {
			return selTags;
		}
	} else if (upPreline.find(L"UPDATE") != std::wstring::npos) {
		selTags = getUpdateTags(upline, upPreline, upword, curPosInLine);
		if (!selTags.empty()) {
			return selTags;
		}
	}
	if (word == L" ") {
		return sqlTags;
	}
	
	for (auto & str : sqlTags) {
		if (str == upword || str.find(upword) != 0) {
			continue;
		}

		selTags.push_back(str);
	}
	return selTags;
}

/**
 * Get tags for select SQL statement and delete SQL statement
 * 
 * @param line - the current line string
 * @param preline - the string before current position 
 * @param word - current word
 * @param curPosInLine - The current position in the line
 * @return 
 */
std::vector<std::wstring> SqlEditorAdapter::getSelectTags(const std::wstring & upline, const std::wstring & upPreline, const std::wstring & upword, size_t curPosInLine)
{
	ATLASSERT(!upline.empty() && !upPreline.empty());
	std::vector<std::wstring> tags, selTags;
	auto words = StringUtil::splitByBlank(upPreline);
	if (words.empty()) {
		return selTags;
	}
	int n = static_cast<int>(words.size());
	uint64_t userDbId = supplier->getRuntimeUserDbId();
	std::wstring & lastWord = words.back();
	wchar_t lastCharOfLastWord = upword.back();
	wchar_t lastCharOfPrevWord = 0;
	std::wstring prevWord;
	if (n > 1) {
		prevWord = words.at(n - 2);
		lastCharOfPrevWord = prevWord.back();
	}
	auto tblService = tableService;
	
	size_t fromPos, wherePos, groupPos, orderPos, limitPos, npos;
	npos = std::wstring::npos;
	fromPos = upline.find(L" FROM ");
	wherePos = upline.find(L" WHERE ");
	groupPos = upline.find(L" GROUP ");
	orderPos = upline.find(L" ORDER ");	
	limitPos = upline.find(L" LIMIT ");
	if (lastWord == L"FROM" || prevWord == L"FROM"
		|| (wherePos != npos && curPosInLine < wherePos)
		|| (wherePos == npos && groupPos != npos && curPosInLine < groupPos)
		|| (wherePos == npos && groupPos == npos 
			&& orderPos != npos && curPosInLine < orderPos)
		|| (wherePos == npos && groupPos == npos && orderPos == npos 
			&& limitPos != npos  && curPosInLine < limitPos)
		|| (wherePos == npos && groupPos == npos && orderPos == npos  && limitPos == npos)) { // columns
		tags = getCacheUserTableStrings(userDbId);
	} else if (lastWord == L"ON" || prevWord == L"ON" 
		|| lastWord == L"WHERE" || prevWord == L"WHERE"
		|| lastWord == L"BY" || prevWord == L"BY"
		|| lastWord == L"AND" || prevWord == L"AND"
		|| lastWord == L"OR" || prevWord == L"OR"
		|| lastCharOfLastWord == L'.' || lastCharOfPrevWord == L'.'
		|| lastCharOfLastWord == L'>' || lastCharOfPrevWord == L'>'
		|| lastCharOfLastWord == L'=' || lastCharOfPrevWord == L'='  
		|| lastCharOfLastWord == L'<' || lastCharOfPrevWord == L'<'
		|| lastWord == L">=" || prevWord == L">="
		|| lastWord == L"<=" || prevWord == L"<=") { // fields
		TableAliasVector tblAliasVec = SqlUtil::parseTableClauseFromSelectSql(upline);
		if (tblAliasVec.empty()) {
			return selTags;
		}
		size_t dotPos = lastWord.find_last_of(L".") ;
		if (lastCharOfLastWord == L'.' || lastCharOfPrevWord == L'.'
			|| dotPos != std::wstring::npos) {
			std::wstring tblAlias = lastWord.substr(0, dotPos);
			for(TableAlias & item : tblAliasVec) {
				if (item.tbl == tblAlias || item.alias == tblAlias) {
					tags = getCacheTableColumns(userDbId, item.tbl);
				}
			};
		}else {
			tags = getCacheTableColumns(userDbId, tblAliasVec.at(0).tbl);
		}
	}

	for (auto & str : tags) {
		std::wstring upstr = StringUtil::toupper(str);
		if (upstr == upword || upstr.find(upword) != 0) {
			continue;
		}

		selTags.push_back(str);
	}
	return selTags;
}

/**
 * Get tags for select SQL statement and delete SQL statement.
 * 
 * @param line - the current line string
 * @param preline - the string before current position 
 * @param word - current word
 * @param curPosInLine - The current position in the line
 * @return 
 */
std::vector<std::wstring> SqlEditorAdapter::getUpdateTags(const std::wstring & upline, const std::wstring & upPreline, const std::wstring & upword, size_t curPosInLine)
{
	ATLASSERT(!upline.empty() && !upPreline.empty());
	std::vector<std::wstring> tags, selTags;
	auto words = StringUtil::splitByBlank(upPreline);
	if (words.empty()) {
		return selTags;
	}

	int n = static_cast<int>(words.size());
	uint64_t userDbId = supplier->getRuntimeUserDbId();
	std::wstring & lastWord = words.back();
	wchar_t lastCharOfLastWord = upword.back();
	
	std::wstring prevWord;
	wchar_t lastCharOfPrevWord = 0;
	if (n > 1) {
		prevWord = words.at(n - 2);
		lastCharOfPrevWord = prevWord.back();
	}

	auto tblService = tableService;
	size_t npos = std::wstring::npos;
	size_t setPos = upline.find(L" SET ");
	size_t wherePos = upline.find(L" WHERE ");
	if (lastWord == L"UPDATE" || prevWord == L"UPDATE"  || 
		(setPos != npos && curPosInLine < setPos) || 
		(setPos == npos && wherePos == npos)) {
		tags = getCacheUserTableStrings(userDbId);
	} else if (lastWord == L"ON" || prevWord == L"ON"
		|| lastWord == L"WHERE" || prevWord == L"WHERE"
		|| lastWord == L"BY" || prevWord == L"BY"
		|| lastWord == L"AND" || prevWord == L"AND"
		|| lastWord == L"OR" || prevWord == L"OR"
		|| lastWord == L"SET" || prevWord == L"SET"
		|| lastCharOfLastWord == L',' || lastCharOfPrevWord == L','
		|| lastCharOfLastWord == L'.' || lastCharOfPrevWord == L'.'
		|| lastCharOfLastWord == L'>' || lastCharOfPrevWord == L'>'
		|| lastCharOfLastWord == L'=' || lastCharOfPrevWord == L'='  
		|| lastCharOfLastWord == L'<' || lastCharOfPrevWord == L'<'
		|| lastWord == L">=" || prevWord == L">="
		|| lastWord == L"<=" || prevWord == L"<=") { // fields

		TableAliasVector tblAliasVec = SqlUtil::parseTableClauseFromUpdateSql(upline);
		if (tblAliasVec.empty()) {
			return selTags;
		}
		size_t dotPos = lastWord.find_last_of(L".") ;
		if (lastCharOfLastWord == L'.' || lastCharOfPrevWord == L'.'
			|| dotPos != std::wstring::npos) {			
			std::wstring tblAlias = lastWord.substr(0, dotPos);
			for(TableAlias & item : tblAliasVec) {
				if (item.tbl == tblAlias || item.alias == tblAlias) {
					tags = getCacheTableColumns(userDbId, item.tbl);
				}
			};
		}else {
			tags = getCacheTableColumns(userDbId, tblAliasVec.at(0).tbl);
		}
	}

	for (auto & str : tags) {
		std::wstring upstr = StringUtil::toupper(str);
		if (upstr == upword || upstr.find(upword) != 0) {
			continue;
		}

		selTags.push_back(str);
	}
	return selTags;
}

UserTableStrings & SqlEditorAdapter::getCacheUserTableStrings(uint64_t userDbId)
{
	auto & tblStrs = supplier->getCacheUserTableStrings(userDbId);
	if (tblStrs.empty()) {
		// Cache to supplier
		tblStrs = tableService->getUserTableStrings(userDbId);
		// supplier->setCacheUserTableStrings(userDbId, tblStrs);
	}
	return tblStrs;
}

Columns & SqlEditorAdapter::getCacheTableColumns(uint64_t userDbId, const std::wstring & tblName)
{
	auto & columns = supplier->getCacheTableColumns(userDbId, tblName);
	if (columns.empty()) {
		columns = tableService->getUserColumnStrings(userDbId, tblName);
		// supplier->setCacheTableColumns(userDbId, tblName, tags);
	}
	return columns;
}

