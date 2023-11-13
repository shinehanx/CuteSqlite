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

 * @file   QueryPageEditorAdapter.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-11-13
 *********************************************************************/
#include "stdafx.h"
#include "QueryPageEditorAdapter.h"
#include "utils\SqlUtil.h"
#include "core\common\Lang.h"


QueryPageEditorAdapter::QueryPageEditorAdapter(HWND parentHwnd, QueryPageSupplier * supplier)
{
	this->supplier = supplier;
	this->parentHwnd = parentHwnd;
	
}


QueryPageEditorAdapter::~QueryPageEditorAdapter()
{
	if (templatesMenu.IsMenu()) templatesMenu.DestroyMenu();
	if (pragmasMenu.IsMenu()) pragmasMenu.DestroyMenu();
	if (sqlLogMenu.IsMenu()) sqlLogMenu.DestroyMenu();
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
std::vector<std::wstring> QueryPageEditorAdapter::getTags(const std::wstring & line, const std::wstring & preline, const std::wstring & word, size_t curPosInLine)
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
std::vector<std::wstring> QueryPageEditorAdapter::getSelectTags(const std::wstring & upline, const std::wstring & upPreline, const std::wstring & upword, size_t curPosInLine)
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
std::vector<std::wstring> QueryPageEditorAdapter::getUpdateTags(const std::wstring & upline, const std::wstring & upPreline, const std::wstring & upword, size_t curPosInLine)
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

std::vector<std::wstring> & QueryPageEditorAdapter::getCacheUserTableStrings(uint64_t userDbId)
{
	auto & tblStrs = supplier->getCacheUserTableStrings(userDbId);
	if (tblStrs.empty()) {
		// Cache to supplier
		tblStrs = tableService->getUserTableStrings(userDbId);
		// supplier->setCacheUserTableStrings(userDbId, tblStrs);
	}
	return tblStrs;
}


uint64_t QueryPageEditorAdapter::getCurrentUserDbId()
{
	return supplier->getRuntimeUserDbId();
}

Columns & QueryPageEditorAdapter::getCacheTableColumns(uint64_t userDbId, const std::wstring & tblName)
{
	auto & columns = supplier->getCacheTableColumns(userDbId, tblName);
	if (columns.empty()) {
		columns = tableService->getUserColumnStrings(userDbId, tblName);
		// supplier->setCacheTableColumns(userDbId, tblName, tags);
	}
	return columns;
}


/**
 * Create toolbar menus.
 * 
 */
void QueryPageEditorAdapter::createMenus()
{
	createTemplatesMenu();
	createPragmasMenu();
	createSqlLogMenu();
}


void QueryPageEditorAdapter::createTemplatesMenu()
{
	if (!templatesMenu.IsNull() && templatesMenu.IsMenu()) {
		return ;
	}

	templatesMenu.CreatePopupMenu();
	templatesMenu.AppendMenu(MF_STRING, Config::TEMPLATES_SELECT_STMT_MEMU_ID, L"SELECT <columns> FROM ...");
	templatesMenu.AppendMenu(MF_STRING, Config::TEMPLATES_INSERT_STMT_MEMU_ID,  L"INSERT INTO ...");
	templatesMenu.AppendMenu(MF_STRING, Config::TEMPLATES_UPDATE_STMT_MEMU_ID,  L"UPDATE <table> SET ...");
	templatesMenu.AppendMenu(MF_STRING, Config::TEMPLATES_DELETE_STMT_MEMU_ID, L"DELETE FROM ...");
	templatesMenu.AppendMenu(MF_STRING, Config::TEMPLATES_REPLACE_STMT_MEMU_ID, L"REPLACE INTO ...");
	templatesMenu.AppendMenu(MF_SEPARATOR);
	templatesMenu.AppendMenu(MF_STRING, Config::TEMPLATES_CREATE_TABLE_STMT_MEMU_ID,  L"CREATE TABLE ...");
	templatesMenu.AppendMenu(MF_STRING, Config::TEMPLATES_DROP_TABLE_STMT_MEMU_ID,  L"DROP TABLE ...");
	templatesMenu.AppendMenu(MF_STRING, Config::TEMPLATES_ALTER_TABLE_STMT_MEMU_ID, L"ALTER TABLE...");
	templatesMenu.AppendMenu(MF_STRING, Config::TEMPLATES_RENAME_TABLE_STMT_MEMU_ID,  L"RENAME TABLE...");
	
	templatesMenu.AppendMenu(MF_SEPARATOR);
	templatesMenu.AppendMenu(MF_STRING, Config::TEMPLATES_CREATE_INDEX_STMT_MEMU_ID, L"CREATE INDEX ...");
	templatesMenu.AppendMenu(MF_STRING, Config::TEMPLATES_DROP_INDEX_STMT_MEMU_ID, L"DROP INDEX ...");
	templatesMenu.AppendMenu(MF_SEPARATOR);
	templatesMenu.AppendMenu(MF_STRING, Config::TEMPLATES_CREATE_VIEW_STMT_MEMU_ID, L"CREATE VIEW ...");
	templatesMenu.AppendMenu(MF_STRING, Config::TEMPLATES_DROP_VIEW_STMT_MEMU_ID, L"DROP VIEW ...");
	templatesMenu.AppendMenu(MF_SEPARATOR);
	templatesMenu.AppendMenu(MF_STRING, Config::TEMPLATES_CREATE_TRIGGER_STMT_MEMU_ID, L"CREATE TRIGGER ...");
	templatesMenu.AppendMenu(MF_STRING, Config::TEMPLATES_DROP_TRIGGER_STMT_MEMU_ID, L"DROP TRIGGER ...");
	templatesMenu.AppendMenu(MF_SEPARATOR);
	templatesMenu.AppendMenu(MF_STRING, Config::TEMPLATES_WITH_STMT_MEMU_ID, L"WITH ... AS ...");
}

void QueryPageEditorAdapter::createPragmasMenu()
{
	if (pragmasMenu.IsMenu()) {
		return ;
	}

	pragmasMenu.CreatePopupMenu();
	for (auto & item : supplier->pragmas) {
		pragmasMenu.AppendMenu(MF_STRING, WM_USER + std::get<0>(item), std::get<1>(item).c_str());
	}
	
}

void QueryPageEditorAdapter::createSqlLogMenu()
{

}

void QueryPageEditorAdapter::popupTemplatesMenu(CPoint & pt)
{
	if (!templatesMenu.IsMenu()) {
		return;
	}
	templatesMenu.TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, parentHwnd);
}

void QueryPageEditorAdapter::popupPragmasMenu(CPoint & pt)
{
	if (!pragmasMenu.IsMenu()) {
		return;
	}
	pragmasMenu.TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, parentHwnd);
}
