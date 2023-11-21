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

 * @file   CopyTableAdapter.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-11-01
 *********************************************************************/
#include "stdafx.h"
#include "CopyTableAdapter.h"
#include "core/common/Lang.h"
#include "ui/common/message/QPopAnimate.h"
#include "utils/SavePointUtil.h"
#include "common/AppContext.h"
#include "utils/SqlUtil.h"

CopyTableAdapter::CopyTableAdapter(HWND parentHwnd, CopyTableSupplier * supplier)
{
	QAdapter::parentHwnd = parentHwnd;
	this->supplier = supplier;
}

CopyTableAdapter::CopyTableAdapter(HWND parentHwnd, ATL::CWindow * view)
{

}


CopyTableAdapter::~CopyTableAdapter()
{

}

UserDbList CopyTableAdapter::getDbs()
{
	return databaseService->getAllUserDbs();
}

/**
 * Get the default express for table sharding strategy organism.
 *	for example : if has tbl_1....tbl_64, then default express will be: id % 64 + 1
 * 
 * @brief
 *	Default organism : [sharding_key] % (suffixEnd - suffixBegin + 1) + 1
 * 
 * @param suffixBegin
 * @param suffixEnd
 * @return 
 */
std::wstring CopyTableAdapter::getDefaultShardingStrategyExpress(int suffixBegin, int suffixEnd)
{
	if (!(suffixBegin > 0 && suffixEnd > 0 && suffixBegin < suffixEnd)) {
		return L"";
	}

	Columns columns = 
		tableService->getUserColumnStrings(supplier->getRuntimeUserDbId(),supplier->getRuntimeTblName());

	std::wstring primaryKeyColumn =
		tableService->getPrimaryKeyColumn(supplier->getRuntimeUserDbId(),supplier->getRuntimeTblName(), columns);
	if (primaryKeyColumn.empty()) {
		return L"";
	}

	int capacity = suffixEnd - suffixBegin + 1;
	std::wstring express = primaryKeyColumn;
	express.append(L" % ").append(std::to_wstring(capacity));
	if (suffixBegin > 0) {
		express.append(L" + ").append(std::to_wstring(suffixBegin));
	}
	
	return express;
}

/**
 * Mach if the express is default .
 * Default organism : [sharding_key] % (suffixEnd - suffixBegin + 1) + 1
 * Default express for example : id % 64 + 1
 * 
 * @param str [in] : matched string
 * @param capacity [out] : the 2th param of pattern
 * @param offset [out] : the 3th param of pattern
 * @return 
 */
bool CopyTableAdapter::matchDefaultExpress(const std::wstring & str, int & capacity, int & offset)
{
	if (str.empty()) {
		return false;
	}
	// Express pattern(such as : id % 64 + 1) 
	// Pattern params : 
	//    (capacity) - the 2th param of pattern, 
	//    (offset) - the 3th param of pattern
	std::wregex pattern(L"(\\s?[a-z0-9A-Z_]+\\s?%\\s?([0-9]+)\\s?\\+\\s?([0-9]+)$)");

	std::wsmatch results;
	if (!std::regex_search(str, results, pattern)) {
		return false;
	}
	if (results.size() < 3 || results[2].matched == false || results[3].matched == false) {
		return false;
	}

	std::wstring strCapcity = results[2];
	std::wstring strOffset = results[3];

	capacity = _wtoi(strCapcity.c_str());
	offset = _wtoi(strOffset.c_str());
	return true;
}

/**
 * Get preview sql.
 * 
 * @return 
 */
std::wstring CopyTableAdapter::getPreviewSqlInSameDb()
{
	auto sqls = getSqlsInSameDb();
	if (sqls.empty()) {
		return L"";
	}

	std::wstring previewSql;
	int i = 0;
	for (auto & sql : sqls) {
		if (i++ > 0) {
			previewSql.append(L"\n");
		}
		previewSql.append(sql);
	}

	return previewSql;
}


bool CopyTableAdapter::startCopy()
{
	if (supplier->getRuntimeUserDbId() == supplier->getTargetUserDbId()) {
		return doExecSqlsInSameDb();
	}

	return doExecSqlsInOtherDb();
}

bool CopyTableAdapter::doExecSqlsInSameDb()
{
	uint64_t targetUserDbId = supplier->getTargetUserDbId();
	AppContext * appContext = AppContext::getInstance();
	int percent = 0;

	std::wstring sql = L"BEGIN;";
	tableService->execBySql(targetUserDbId, sql);

	try {
		// 1. begin transit 
		auto sqls = getSqlsInSameDb();
		percent += 5;
		::PostMessage(parentHwnd, Config::MSG_COPY_TABLE_PROCESS_ID, 0, percent);

		int n = static_cast<int>(sqls.size());
		int avgVal = int(round(95.0 / double(n)));
		for (auto & tableSql : sqls) {
			sql = tableSql;
			tableService->execBySql(targetUserDbId, tableSql);
			percent += avgVal;
			::PostMessage(parentHwnd, Config::MSG_COPY_TABLE_PROCESS_ID, 0, percent);
		}
		::PostMessage(parentHwnd, Config::MSG_COPY_TABLE_PROCESS_ID, 1, 100);

		// 4.COMMIT TRANSACTION
		sql = L"COMMIT;";
		tableService->execBySql(targetUserDbId, sql);
		return true;
	} catch (QSqlExecuteException &ex) {
		QPopAnimate::report(ex);
		sql = L"ROLLBACK;";
		tableService->execBySql(targetUserDbId, sql);
		::PostMessage(parentHwnd, Config::MSG_COPY_TABLE_PROCESS_ID, 2, NULL);
		return false;
	} catch (QRuntimeException &ex) {
		QPopAnimate::report(ex);
		sql = L"ROLLBACK;";
		tableService->execBySql(targetUserDbId, sql);
		::PostMessage(parentHwnd, Config::MSG_COPY_TABLE_PROCESS_ID, 2, NULL);
		return false;
	}
}


/**
 * Generate multiple SQL statement end with ";"
 * 
 * @return 
 */
std::list<std::wstring> CopyTableAdapter::getSqlsInSameDb()
{
	std::list<std::wstring> result;
	auto tables = supplier->getShardingTables();
	if (tables.empty()) {
		return result;
	}
	size_t n = tables.size();
	for (auto & table : tables) {
		std::wstring tableDdl = generateCreateDdlForTargetTable(table.first, table.second);
		std::wstring dataSql = generateCopyDataSqlInSameDb(table.first, table.second);

		if (!tableDdl.empty()) result.push_back(tableDdl);
		if (!dataSql.empty()) result.push_back(dataSql);

		if (!supplier->getEnableTableSharding()) {
			return result;
		}
	}

	return result;
}

std::wstring CopyTableAdapter::generateCreateDdlForTargetTable(uint16_t suffix, const std::wstring & targetTblName)
{
	if (supplier->getStructAndDataSetting() == DATA_ONLY || targetTblName.empty()) {
		return L"";
	}
	std::wstring ddl;
	uint64_t userDbId = supplier->getRuntimeUserDbId();
	std::wstring & sourceTblName = supplier->getRuntimeTblName();
	UserTable userTable = tableService->getUserTable(userDbId, sourceTblName);
	if (userTable.name.empty() || userTable.sql.empty()) {
		throw QRuntimeException(L"200001");
	}

	std::wstring & createTblSql = userTable.sql;
	size_t paren_first = createTblSql.find_first_of(L'(');
	size_t paren_end = createTblSql.find_last_of(L')');
	if (paren_first == std::wstring::npos || paren_end == std::wstring::npos) {
		throw QRuntimeException(L"200001");
	}
	// createTableClause is sub string before  character '{', such as "CREATE TABLE tbl_name [IF NOT EXISTS]"
	std::wstring createTableClause = createTblSql.substr(0, paren_first);
	// columns and constrains statement, include character '{' and '}'
	std::wstring columnsAndConstrains = createTblSql.substr(paren_first, paren_end - paren_first + 1);

	createTableClause = StringUtil::replace(createTableClause, sourceTblName, targetTblName);
	ddl.append(createTableClause).append(columnsAndConstrains).append(L";"); // don;t forget end with character";"
	return ddl; 
}


/**
 * Generate copy data sql in same database.
 * 
 * @param suffix
 * @param targetTblName
 * @return 
 */
std::wstring CopyTableAdapter::generateCopyDataSqlInSameDb(uint16_t suffix, const std::wstring & targetTblName)
{
	// SQL : INSERT INTO target_tbl SELECT * FROM source_tbl [WHERE {express} = {suffix}]
	if (supplier->getStructAndDataSetting() == STRUCT_ONLY 
		|| supplier->getStructAndDataSetting() == UNKOWN || targetTblName.empty()) {
		return L"";
	}

	// sql: INSERT INTO "target_tbl" 
	std::wstring sql = L"INSERT INTO ";
	sql.append(quo).append(targetTblName).append(quo).append(blk);

	uint64_t userDbId = supplier->getRuntimeUserDbId();
	std::wstring & sourceTblName = supplier->getRuntimeTblName();
	// sql: SELECT * FROM "source_tbl" 
	sql.append(L"SELECT * FROM ").append(quo).append(sourceTblName).append(quo).append(blk);

	if (supplier->getEnableTableSharding() && supplier->getEnableShardingStrategy() 
		&& !supplier->getShardingStrategyExpress().empty()) {
		// sql : WHERE (express) = suffix
		sql.append(L"WHERE ").append(cmb).append(supplier->getShardingStrategyExpress()).append(cme)
			.append(blk).append(eq).append(blk).append(to_wstring(suffix));
	}
	sql.append(edl);
	return sql;
}

bool CopyTableAdapter::doExecSqlsInOtherDb()
{
	uint64_t targetUserDbId = supplier->getTargetUserDbId();
	int percent = 0;

	std::wstring sql = L"BEGIN;";
	tableService->execBySql(targetUserDbId, sql);

	try {
		auto tableMap = supplier->getShardingTables();
		percent += 5;
		::PostMessage(parentHwnd, Config::MSG_COPY_TABLE_PROCESS_ID, 0, percent);

		int n = static_cast<int>(tableMap.size());
		int avgVal = int(round(90.0 / double(n)));

		for (auto & tblItem : tableMap) {
			// 1. execute create table sql
			sql = generateCreateDdlForTargetTable(tblItem.first, tblItem.second);
			if (!sql.empty()) {
				tableService->execBySql(targetUserDbId, sql);
			}
			
			// 2. execute table data sql
			doExecCopyDataSqlInOtherDb(tblItem.first, tblItem.second);
			percent += avgVal;
			::PostMessage(parentHwnd, Config::MSG_COPY_TABLE_PROCESS_ID, 0, percent);
		}
		
		// 4.COMMIT TRANSACTION
		sql = L"COMMIT;";
		tableService->execBySql(targetUserDbId, sql);
		::PostMessage(parentHwnd, Config::MSG_COPY_TABLE_PROCESS_ID, 1, 100);
		return true;
	}
	catch (QSqlExecuteException &ex) {
		QPopAnimate::report(ex);
		sql = L"ROLLBACK;";
		tableService->execBySql(targetUserDbId, sql);
		::PostMessage(parentHwnd, Config::MSG_COPY_TABLE_PROCESS_ID, 2, NULL);
		return false;
	}
	catch (QRuntimeException &ex) {
		QPopAnimate::report(ex);
		sql = L"ROLLBACK;";
		tableService->execBySql(targetUserDbId, sql);
		::PostMessage(parentHwnd, Config::MSG_COPY_TABLE_PROCESS_ID, 2, NULL);
		return false;
	}
}

bool CopyTableAdapter::doExecCopyDataSqlInOtherDb(uint16_t suffix, const std::wstring & targetTblName)
{
	// SQL : INSERT INTO target_tbl SELECT * FROM source_tbl [WHERE {express} = {suffix}]
	if (supplier->getStructAndDataSetting() == STRUCT_ONLY 
		|| supplier->getStructAndDataSetting() == UNKOWN || targetTblName.empty()) {
		return false;
	}

	int page = 1;
	int perpage = 100;

	// Get data from the source table of source database, 
	std::wstring whereClause;
	bool checked1 = supplier->getStructAndDataSetting() == DATA_ONLY || supplier->getStructAndDataSetting() == STRUCTURE_AND_DATA;
	bool checked2 = supplier->getEnableTableSharding();
	bool checked3 = supplier->getEnableShardingStrategy();
	bool checked4 = !supplier->getShardingStrategyExpress().empty();
	if (checked1 && checked2 && checked3 && checked4) {
		whereClause.append(L"(").append(supplier->getShardingStrategyExpress()).append(L")").append(L"=").append(std::to_wstring(suffix));
	}
	
	int totalPage = tableService->getTableWhereDataPageCount(supplier->getRuntimeUserDbId(), supplier->getRuntimeTblName(), whereClause, perpage);
	for (page = 1; page <= totalPage; page++) {
		// Get data from source table 
		DataList pageDataList = tableService->getTableWhereDataList(supplier->getRuntimeUserDbId(), supplier->getRuntimeTblName(), whereClause, page, perpage);

		// Generate a sql statement for the page data list. and then execute the sql statement.
		std::wstring sql = genderatePageDataSql(pageDataList, targetTblName);
		if (!sql.empty()) {
			tableService->execBySql(supplier->getTargetUserDbId(), sql); 
		}		
	}
	return true;
}

/**
 * Generate a sql statement for the page data list. and then execute the sql statement.
 * 
 * @param pageDataList : list of page data 
 * @param targetTblName : target table name
 * @return 
 */
std::wstring CopyTableAdapter::genderatePageDataSql(const DataList & pageDataList, const std::wstring & targetTblName)
{
	if (pageDataList.empty() || targetTblName.empty()) {
		return L"";
	}
	std::wstring sql = L"INSERT INTO ";
	sql.append(quo).append(targetTblName).append(quo).append(blk);
	sql.append(L"VALUES ");

	int i = 0;
	for (const RowItem & row : pageDataList) {
		if (i++ > 0) sql.append(cma).append(brk); // append ',',will be a new row 		
		
		sql.append(cmb);
		int j = 0;
		for (auto val : row) {
			if (j++ > 0) sql.append(cma) ;
			(val == L"[NULL]") ? sql.append(nil) :
				sql.append(qua).append(StringUtil::escapeSql(val)).append(qua);
		}
		sql.append(cme);
	}
	sql.append(edl);
	return sql;
}

void CopyTableAdapter::loadSqlForPreviewEdit(QHelpEdit * editorPtr)
{
	// copy data in same db
	if (supplier->getRuntimeUserDbId() == supplier->getTargetUserDbId()) {
		std::wstring sql = getPreviewSqlInSameDb();
		editorPtr->setText(sql);
	} else {
		// copy data from different db 
		doLoadTargetTableSqlToEditorForOtherDb(editorPtr);
	}
}


void CopyTableAdapter::doLoadTargetTableSqlToEditorForOtherDb(QHelpEdit * editorPtr)
{
	uint64_t targetUserDbId = supplier->getTargetUserDbId();
	int percent = 0;

	std::wstring sql = L"BEGIN;\n";
	editorPtr->addText(sql);

	try {
		auto tableMap = supplier->getShardingTables();

		for (auto & tblItem : tableMap) {
			// 1. create table sql
			sql = generateCreateDdlForTargetTable(tblItem.first, tblItem.second);
			if (!sql.empty()) {
				editorPtr->addText(sql);
				editorPtr->addText(L"\n");
			}
			
			// 2. append table data sql to editor
			doAppendCopyDataSqlToEditor(editorPtr, tblItem.first, tblItem.second);			
		}
		
		// 4.COMMIT TRANSACTION
		sql = L"COMMIT;\n";
		editorPtr->addText(sql);
	} catch (QSqlExecuteException &ex) {
		QPopAnimate::report(ex);
		return ;
	} catch (QRuntimeException &ex) {
		QPopAnimate::report(ex);
		return ;
	}
}

bool CopyTableAdapter::doAppendCopyDataSqlToEditor(QHelpEdit * editorPtr, uint16_t suffix, const std::wstring & targetTblName)
{
	// SQL : INSERT INTO target_tbl SELECT * FROM source_tbl [WHERE {express} = {suffix}]
	if (supplier->getStructAndDataSetting() == STRUCT_ONLY 
		|| supplier->getStructAndDataSetting() == UNKOWN || targetTblName.empty()) {
		return false;
	}

	int page = 1;
	int perpage = 100;

	// Get data from the source table of source database, 
	// Get data from the source table of source database, 
	std::wstring whereClause;
	bool checked1 = supplier->getStructAndDataSetting() == DATA_ONLY || supplier->getStructAndDataSetting() == STRUCTURE_AND_DATA;
	bool checked2 = supplier->getEnableTableSharding();
	bool checked3 = supplier->getEnableShardingStrategy();
	bool checked4 = !supplier->getShardingStrategyExpress().empty();
	if (checked1 && checked2 && checked3 && checked4) { 
		whereClause.append(L"(").append(supplier->getShardingStrategyExpress()).append(L")").append(L"=").append(std::to_wstring(suffix));
	}
	
	int totalPage = tableService->getTableWhereDataPageCount(supplier->getRuntimeUserDbId(), supplier->getRuntimeTblName(), whereClause, perpage);
	for (page = 1; page <= totalPage; page++) {
		// Get data from source table 
		DataList pageDataList = tableService->getTableWhereDataList(supplier->getRuntimeUserDbId(), supplier->getRuntimeTblName(), whereClause, page, perpage);

		// Generate a sql statement for the page data list. and then execute the sql statement.
		std::wstring sql = genderatePageDataSql(pageDataList, targetTblName);
		if (!sql.empty()) {
			editorPtr->addText(sql);
			editorPtr->addText(L"\n");
		}
	}
	return true;
}