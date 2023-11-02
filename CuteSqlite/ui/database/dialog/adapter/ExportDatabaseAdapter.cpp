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

 * @file   ExportDatabaseAdapter.cpp
 * @brief  Export the table(s),view(s),trigger(s) of the selected database in SQL.
 * 
 * @author Xuehan Qin
 * @date   2023-10-04
 *********************************************************************/
#include "stdafx.h"
#include "ExportDatabaseAdapter.h"
#include "utils/FileUtil.h"
#include "utils/StringUtil.h"
#include "common/AppContext.h"
#include "core/common/Lang.h"
#include "core/common/exception/QRuntimeException.h"
#include "ui/common/message/QPopAnimate.h"
#include "ui/common/message/QMessageBox.h"

ExportDatabaseAdapter::ExportDatabaseAdapter(HWND parentHwnd, ATL::CWindow * view)
{
	this->parentHwnd = parentHwnd;
	this->dataView = view;
}

ExportDatabaseAdapter::~ExportDatabaseAdapter()
{

}

UserDbList ExportDatabaseAdapter::getDbs()
{
	loadDbs();
	return dbs;
}

void ExportDatabaseAdapter::loadDbs()
{
	try {
		dbs = databaseService->getAllUserDbs();
	}
	catch (QRuntimeException &ex) {
		Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::error(parentHwnd, S(L"error-text").append(ex.getMsg()).append(L",[code:").append(ex.getCode()).append(L"]"));
	}
}

uint64_t ExportDatabaseAdapter::getSeletedUserDbId()
{
	return databaseSupplier->getSelectedUserDbId();
}


void ExportDatabaseAdapter::exportObjectsToSql(uint64_t userDbId, 
	std::wstring & exportPath, 
	UserTableList & tblList, 
	UserViewList & viewList, 
	UserTriggerList & triggerList, 
	StructAndDataParams & structureAndDataParams, 
	InsertStatementParams & insertStatementParams, 
	TblStatementParams & tblStatementParams)
{
	ATLASSERT(!exportPath.empty() && !tblList.empty() 
		&& !structureAndDataParams.sqlSetting.empty() && !tblStatementParams.param.empty());

	std::wstring dirPath = FileUtil::getFileDir(exportPath);
	FileUtil::createDirectory(dirPath);

	// 1. create and open the output fstream	
	std::wofstream ofs;
	auto codecvt = new std::codecvt_utf8<wchar_t, 0x10ffff, std::codecvt_mode(std::generate_header | std::little_endian)>();
	std::locale utf8(std::locale("C"), codecvt);
	ofs.imbue(utf8);
	ofs.open(exportPath, std::ios::out | std::ios::trunc);

	ofs << L"--" << endl;
	ofs << L"PRAGMA foreign_keys=OFF;" << endl;
	ofs << L"BEGIN TRANSACTION;" << endl;

	// 2.Export tables/views/triggers to sql
	int percent = 0;
	exportTablesToSql(userDbId, ofs, tblList, structureAndDataParams, insertStatementParams, tblStatementParams, percent);
	exportViewsToSql(userDbId, ofs, viewList, tblStatementParams, percent);
	exportTriggersToSql(userDbId, ofs, triggerList, tblStatementParams,  percent);


	ofs << L"COMMIT;" << endl;
	ofs.flush();
	ofs.close();

	// 3. complete the expert task and send message MSG_EXPORT_DB_AS_SQL_PROCESS_ID to dialog with wParam=1
	AppContext::getInstance()->dispatch(Config::MSG_EXPORT_DB_AS_SQL_PROCESS_ID, 1, 100); // 进度
}

/**
 * Export database tables as sql to sql file in append mode
 * 
 * @param userDbId
 * @param exportPath
 * @param tblList
 * @param structureAndDataParams
 * @param insertStatementParams
 * @param tblStatementParams
 * @return 
 */
int ExportDatabaseAdapter::exportTablesToSql(uint64_t userDbId, 
	std::wofstream & ofs, 
	UserTableList & tblList, 
	StructAndDataParams & structureAndDataParams, 
	InsertStatementParams & insertStatementParams, 
	TblStatementParams & tblStatementParams,
	int & percent)
{
	if (tblList.empty()) {
		percent = 90;
		AppContext::getInstance()->dispatch(Config::MSG_EXPORT_DB_AS_SQL_PROCESS_ID, 0, percent); // 进度
		return 0;
	}
	// 1.create table and export table data
	int n = 0;
	// 90%进度 分成N份
	int avgVal = int(round( 80.0 / double(tblList.size())));
	for (auto userTable : tblList) {
		doExportCreateTableStructure(ofs, userDbId, userTable, structureAndDataParams, tblStatementParams);
		doExportTableInsertStatement(ofs, userDbId, userTable, structureAndDataParams, insertStatementParams);

		percent += avgVal;
		AppContext::getInstance()->dispatch(Config::MSG_EXPORT_DB_AS_SQL_PROCESS_ID, 0, percent); // 进度
		n++;
	}
	percent = 80;
	AppContext::getInstance()->dispatch(Config::MSG_EXPORT_DB_AS_SQL_PROCESS_ID, 0, percent); // 进度
	
	return n;
}

void ExportDatabaseAdapter::exportViewsToSql(uint64_t userDbId, std::wofstream & ofs, UserViewList & viewList, 
	TblStatementParams & tblStatementParams, int & percent)
{
	if (viewList.empty()) {
		percent = 90;
		AppContext::getInstance()->dispatch(Config::MSG_EXPORT_DB_AS_SQL_PROCESS_ID, 0, percent); // 进度
		return ;
	}
	// 1.create table and export table data
	int n = 0;
	// 10%进度 分成N份
	int avgVal = int(round( 10.0 / double(viewList.size())));
	for (auto userView : viewList) {
		if (tblStatementParams.param == L"override-table") {
			ofs << L"DROP VIEW IF EXISTS \"" << userView.name << L"\";" << endl;	
			ofs << userView.sql << L";" << endl;
		} else if (tblStatementParams.param == L"retain-table"){
			std::wstring sql = userView.sql;
			replaceCreateViewClause(sql);
			ofs << sql << L";" << endl;
		}

		percent += avgVal;
		AppContext::getInstance()->dispatch(Config::MSG_EXPORT_DB_AS_SQL_PROCESS_ID, 0, percent); // 进度
		n++;
	}
	percent = 90;
	AppContext::getInstance()->dispatch(Config::MSG_EXPORT_DB_AS_SQL_PROCESS_ID, 0, percent); // 进度
}

void ExportDatabaseAdapter::exportTriggersToSql(uint64_t userDbId, std::wofstream & ofs, UserTriggerList & triggerList, TblStatementParams & tblStatementParams, 
	int & percent)
{
	if (triggerList.empty()) {
		percent = 100;
		AppContext::getInstance()->dispatch(Config::MSG_EXPORT_DB_AS_SQL_PROCESS_ID, 0, percent); // 进度
		return ;
	}
	// 1.create table and export table data
	int n = 0;
	// 10%进度 分成N份
	int avgVal = int(round( 10.0 / double(triggerList.size())));
	for (auto userTrigger : triggerList) {
		if (tblStatementParams.param == L"override-table") {
			ofs << L"DROP TRIGGER IF EXISTS \"" << userTrigger.name << L"\";" << endl;
			ofs << userTrigger.sql << L";" << endl;
		} else if (tblStatementParams.param == L"retain-table"){
			std::wstring sql = userTrigger.sql;
			replaceCreateTriggerClause(sql);
			ofs << sql << L";" << endl;
		}

		percent += avgVal;
		AppContext::getInstance()->dispatch(Config::MSG_EXPORT_DB_AS_SQL_PROCESS_ID, 0, percent); // 进度
		n++;
	}
	percent = 100;
	AppContext::getInstance()->dispatch(Config::MSG_EXPORT_DB_AS_SQL_PROCESS_ID, 0, percent); // 进度
}

/**
 * Export the table structure to sql file.
 * 
 * @param ofs - file stream reference
 * @param userdbId - user database id
 * @param tbl - table struct
 * @param structureAndDataParams - StructAndDataParams object reference
 * @return true/false
 */
void ExportDatabaseAdapter::doExportCreateTableStructure(std::wofstream & ofs, uint64_t userDbId, 
	const UserTable tbl, const StructAndDataParams & structureAndDataParams, const TblStatementParams & tblStatementParams)
{
	ATLASSERT(userDbId && !tbl.name.empty() && !structureAndDataParams.sqlSetting.empty());

	if (structureAndDataParams.sqlSetting == L"data-only") {
		return ;
	}

	// Drop tables first ,then create table
	
	if (tblStatementParams.param == L"override-table") {
		ofs << L"DROP TABLE IF EXISTS \"" << tbl.name << L"\";" << endl;	
		ofs << tbl.sql << L";" << endl;
	} else if (tblStatementParams.param == L"retain-table"){
		std::wstring sql = tbl.sql;
		replaceCreateTableClause(sql);
		ofs << sql << L";" << endl;
	}
	

	return ;
}

/**
 * Export insert statement of the table data to sql file.
 * 
 * @param ofs - file stream reference
 * @param userdbId - user database id
 * @param tbl - table struct
 * @param structureAndDataParams - StructAndDataParams object reference
 * @param insertStatementParams - InsertStatementParams object reference
 */
void ExportDatabaseAdapter::doExportTableInsertStatement(std::wofstream & ofs, uint64_t userDbId,
	const UserTable &tbl, const StructAndDataParams & structureAndDataParams, const InsertStatementParams & insertStatementParams)
{
	ATLASSERT(userDbId && !tbl.name.empty() && !structureAndDataParams.sqlSetting.empty());

	if (structureAndDataParams.sqlSetting == L"structure-only") {
		return;
	}

	int pages = tableService->getTableDataPageCount(userDbId, tbl.name, EXPERT_PER_PAGE);
	for (int i = 1; i <= pages; i++) {
		std::pair<Columns, DataList> result = tableService->getTableDataListWithColumns(userDbId, tbl.name, i, EXPERT_PER_PAGE);
		std::wstring sql = L"INSERT INTO ";
		sql.append(L"\"").append(tbl.name).append(L"\" ");
		// retain columns in insert statement, such as "INSERT INTO tbl (column1, column2, ...) VALUES (v1, v2, ...)"
		if (insertStatementParams.retainColumn && result.first.size() > 0) {
			std::wstring columnClause = L"(";
			for (auto column : result.first) {
				columnClause.append(L"\"").append(column).append(L"\"").append(L",");
			}
			columnClause = StringUtil::cutLastChar(columnClause);
			columnClause.append(L")");
			sql.append(columnClause);
		}

		// if enabled multiRows ,then multi rows in values clause, such as "... VALUES (v1, v2, ...),(vv1, vv2, ...) "
		if (insertStatementParams.multiRows) {
			ofs << sql;
			ofs << L" VALUES ";
			int i = 0;
			for (auto rowItem : result.second) {
				if (i > 0) {
					ofs << L",";
				}
				std::wstring valClause = L"(";
				int j = 0;
				for (auto val : rowItem) {
					if (j > 0) {
						valClause.append(L",");
					}
					valClause.append(L"'").append(StringUtil::escapeSql(val)).append(L"'");
					j++;
				}
				valClause.append(L")");
				ofs << valClause << endl;
				i++;
			}
			ofs << L";" << endl;
		}
		else {
			for (auto rowItem : result.second) {
				ofs << sql;
				ofs << L" VALUES ";

				std::wstring valClause = L"(";
				int j = 0;
				for (auto val : rowItem) {
					if (j > 0) {
						valClause.append(L",");
					}
					valClause.append(L"'").append(StringUtil::escapeSql(val)).append(L"'");
					j++;
				}
				valClause.append(L");");
				ofs << valClause << endl;
			}
		}
	}
}

/**
* If found "CREATE TABLE",then replace the string to "CREATE TABLE IF NOT EXISTS".
* 
* @param sql
*/
void ExportDatabaseAdapter::replaceCreateTableClause(std::wstring &sql)
{
	ATLASSERT(!sql.empty());

	if (StringUtil::search(sql, L"CREATE TABLE", true) == false || StringUtil::search(sql, L"IF NOT EXISTS", true) == true) {
		return ;
	}
	sql = StringUtil::replace(sql, L"CREATE TABLE", L"CREATE TABLE IF NOT EXISTS", true);
}

void ExportDatabaseAdapter::replaceCreateViewClause(std::wstring &sql)
{
	ATLASSERT(!sql.empty());

	if (StringUtil::search(sql, L"CREATE VIEW", true) == false || StringUtil::search(sql, L"IF NOT EXISTS", true) == true) {
		return ;
	}
	sql = StringUtil::replace(sql, L"CREATE VIEW", L"CREATE VIEW IF NOT EXISTS", true);
}

void ExportDatabaseAdapter::replaceCreateTriggerClause(std::wstring &sql)
{
	ATLASSERT(!sql.empty());

	if (StringUtil::search(sql, L"CREATE TRIGGER", true) == false || StringUtil::search(sql, L"IF NOT EXISTS", true) == true) {
		return ;
	}
	sql = StringUtil::replace(sql, L"CREATE TRIGGER", L"CREATE TRIGGER IF NOT EXISTS", true);
}
