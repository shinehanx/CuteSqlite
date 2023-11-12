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

std::vector<std::wstring> SqlEditorAdapter::getTags(const std::wstring & line, const std::wstring & preline, const std::wstring & word)
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
	if (upPreline.find(L"SELECT") != std::wstring::npos || upPreline.find(L"DELETE") != std::wstring::npos) {
		selTags = getSelectTags(upline, upPreline, upword);
		if (!selTags.empty()) {
			return selTags;
		}
	} else if (upPreline.find(L"UPDATE") != std::wstring::npos) {
		selTags = getUpdateTags(upline, upPreline, upword);
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

std::vector<std::wstring> SqlEditorAdapter::getSelectTags(const std::wstring & upline, const std::wstring & upPreline, const std::wstring & upword)
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
	wchar_t lastChar = upword.back();
	wchar_t prevLastChar = 0;
	std::wstring prevLastWord;
	if (n > 1) {
		prevLastWord = words.at(n - 2);
		prevLastChar = prevLastWord.back();
	}
	auto tblService = tableService;
	
	if (lastWord == L"FROM" || prevLastWord == L"FROM") { // columns
		tags = getCacheUserTableStrings(userDbId);
	} else if (lastWord == L"ON" || prevLastWord == L"ON" 
		|| lastWord == L"WHERE" || prevLastWord == L"WHERE"
		|| lastWord == L"BY" || prevLastWord == L"BY"
		|| lastWord == L"AND" || prevLastWord == L"AND"
		|| lastWord == L">" || prevLastWord == L">" || lastChar == L'>' || prevLastChar == L'>'
		|| lastWord == L"=" || prevLastWord == L"=" || lastChar == L'=' || prevLastChar == L'='  
		|| lastWord == L"<" || prevLastWord == L"<" || lastChar == L'<' || prevLastChar == L'<'
		|| lastWord == L">=" || prevLastWord == L">="
		|| lastWord == L"<=" || prevLastWord == L"<="
		|| lastWord == L"OR" || prevLastWord == L"OR") { // fields
		TableAliasVector tblAliasVec = SqlUtil::parseTableClauseFromSelectSql(upline);
		if (tblAliasVec.empty()) {
			return selTags;
		}
		if (lastChar == L'.') {
			std::wstring tblAlias = upword.substr(0, upword.size() - 1);
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


std::vector<std::wstring> SqlEditorAdapter::getUpdateTags(const std::wstring & upline, const std::wstring & upPreline, const std::wstring & upword)
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
	wchar_t lastChar = upword.back();
	
	std::wstring prevLastWord;
	wchar_t prevLastChar = 0;
	if (n > 1) {
		prevLastWord = words.at(n - 2);
		prevLastChar = prevLastWord.back();
	}

	auto tblService = tableService;
	if (lastWord == L"UPDATE" || prevLastWord == L"UPDATE") {
		tags = getCacheUserTableStrings(userDbId);
	} else if (lastWord == L"ON" || prevLastWord == L"ON"
		|| lastWord == L"WHERE" || prevLastWord == L"WHERE"
		|| lastWord == L"BY" || prevLastWord == L"BY"
		|| lastWord == L"AND" || prevLastWord == L"AND"
		|| lastWord == L"SET" || prevLastWord == L"SET"
		|| lastChar == L',' || prevLastChar == L','
		|| lastChar == L'>' || prevLastChar == L'>'
		|| lastChar == L'=' || prevLastChar == L'='  
		|| lastChar == L'<' || prevLastChar == L'<'
		|| lastWord == L">=" || prevLastWord == L">="
		|| lastWord == L"<=" || prevLastWord == L"<="
		|| lastWord == L"OR" || prevLastWord == L"OR") { // fields

		TableAliasVector tblAliasVec = SqlUtil::parseTableClauseFromUpdateSql(upline);
		if (tblAliasVec.empty()) {
			return selTags;
		}
		if (lastChar == L'.') {			
			std::wstring tblAlias = upword.substr(0, upword.size() - 1);
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

