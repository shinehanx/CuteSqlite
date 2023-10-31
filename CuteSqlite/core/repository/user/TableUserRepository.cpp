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

 * @file   TableUserRepository.cpp
 * @brief  Operations on tables in the user database
 * 
 * @author Xuehan Qin
 * @date   2023-05-20
 *********************************************************************/
#include "stdafx.h"
#include "TableUserRepository.h"
#include "core/common/exception/QRuntimeException.h"
#include "core/common/exception/QSqlExecuteException.h"
#include "core/common/repository/QSqlColumn.h"

UserTableList TableUserRepository::getListByUserDbId(uint64_t userDbId, const std::wstring & schema)
{
	UserTableList result;
	// std::wstring sql = L"SELECT * FROM sqlite_master WHERE type='table' ORDER BY name ASC";
	std::wstring sql = L"SELECT * FROM ";
	if (!schema.empty() && schema != L"main") {
		sql.append(L"\"").append(schema).append(L"\".").append(L"\"sqlite_master\"");
	} else {
		sql.append(L"\"sqlite_master\"");
	}
	sql.append(L" WHERE type='table' ORDER BY name ASC");
	try {
		QSqlStatement query(getUserConnect(userDbId), sql.c_str());

		while (query.executeStep()) {
			UserTable item = toUserTable(query);
			result.push_back(item);
		}
		return result;
	}
	catch (SQLite::QSqlException &e) {
		std::wstring _err = e.getErrorStr();
		Q_ERROR(L"query db has error:{}, msg:{}", e.getErrorCode(), _err);
		throw QRuntimeException(L"200020", L"sorry, system has error when loading databases.");
	}
}

UserTable TableUserRepository::getTable(uint64_t userDbId, const std::wstring & tblName, const std::wstring & schema)
{
	UserTable result;
	// sql : SELECT * FROM sqlite_master WHERE type='table' and name=:name ORDER BY name ASC
	std::wstring sql = L"SELECT * FROM ";
	if (!schema.empty() && schema != L"main") {
		sql.append(L"\"").append(schema).append(L"\".").append(L"\"sqlite_master\"");
	} else {
		sql.append(L"\"sqlite_master\"");
	}
	sql.append(L" WHERE type='table' and name=:name ORDER BY name ASC");
	try {
		QSqlStatement query(getUserConnect(userDbId), sql.c_str());
		query.bind(L":name", tblName);

		if (query.executeStep()) {
			result = toUserTable(query);
		}
		return result;
	}
	catch (SQLite::QSqlException &e) {
		std::wstring _err = e.getErrorStr();
		Q_ERROR(L"query table has error:{}, msg:{}", e.getErrorCode(), _err);
		throw QRuntimeException(L"200021", L"sorry, system has error when loading databases.");
	}
}

uint64_t TableUserRepository::getDataCount(uint64_t userDbId, const std::wstring & tblName, const std::wstring & schema)
{
	uint64_t n = 0;
	// std::wstring sql = "SELECT COUNT(*) AS n FROM  tbl_name"
	std::wstring sql = L"SELECT COUNT(*) AS n FROM ";
	if (!schema.empty() && schema != L"main") {
		sql.append(L"\"").append(schema).append(L"\".").append(L"\"").append(tblName).append(L"\"");
	} else {
		sql.append(L"\"").append(tblName).append(L"\"");
	}
	

	try {
		QSqlStatement query(getUserConnect(userDbId), sql.c_str());

		if (query.executeStep()) {
			n = query.getColumn(0).getInt64();
		}
		return n;
	}
	catch (SQLite::QSqlException &e) {
		std::wstring _err = e.getErrorStr();
		Q_ERROR(L"query table has error:{}, msg:{}", e.getErrorCode(), _err);
		throw QRuntimeException(L"200022", L"sorry, system has error when loading databases.");
	}
}

DataList TableUserRepository::getPageDataList(uint64_t userDbId, const std::wstring & tblName, int page, int perpage, const std::wstring & schema)
{
	DataList result;
	QPagePair pagePair = { page, perpage };
	std::wstring sql = L"SELECT * FROM ";
	if (!schema.empty() && schema != L"main") {
		sql.append(L"\"").append(schema).append(L"\".").append(L"\"").append(tblName).append(L"\"");
	} else {
		sql.append(L"\"").append(tblName).append(L"\"");
	}
	std::wstring limitSql = limitClause(pagePair);
	sql.append(limitSql);
	try {
		QSqlStatement query(getUserConnect(userDbId), sql.c_str());

		while (query.executeStep()) {
			RowItem rowItem = toRowItem(query);
			result.push_back(rowItem);
		}
		return result;
	} catch (SQLite::QSqlException &e) {
		std::wstring _err = e.getErrorStr();
		Q_ERROR(L"query table has error:{}, msg:{}", e.getErrorCode(), _err);
		throw QRuntimeException(L"200023", L"sorry, system has error when loading databases.");
	}
}


void TableUserRepository::execBySql(uint64_t userDbId, const std::wstring & sql)
{
	try {
		QSqlStatement query(getUserConnect(userDbId), sql.c_str());
		query.exec();
	} catch (SQLite::QSqlException &e) {
		std::wstring _err = e.getErrorStr();
		Q_ERROR(L"Execute sql has error:{}, msg:{}", e.getErrorCode(), _err);
		QSqlExecuteException ex(std::to_wstring(e.getErrorCode()), _err);
		
		throw ex;
	}
}

void TableUserRepository::renameTable(uint64_t userDbId, const std::wstring & oldTableName, const std::wstring & newTableName, const std::wstring & schema)
{
	std::wstring ddl = L"ALTER TABLE ";
	if (!schema.empty() && schema != L"main") {
		ddl.append(schema).append(L".");
	}
	ddl.append(L"\"").append(oldTableName).append(L"\" RENAME TO ")
		.append(L"\"").append(newTableName).append(L"\"");

	try {
		QSqlStatement query(getUserConnect(userDbId), ddl.c_str());
		query.exec();
	} catch (SQLite::QSqlException &e) {
		std::wstring _err = e.getErrorStr();
		Q_ERROR(L"rename table has error:{}, msg:{}", e.getErrorCode(), _err);
		throw QSqlExecuteException(std::to_wstring(e.getErrorCode()), _err);
	}
}

void TableUserRepository::truncateTable(uint64_t userDbId, const std::wstring & tblName, const std::wstring & schema)
{
	std::wstring ddl = L"DELETE FROM ";
	if (!schema.empty() && schema != L"main") {
		ddl.append(schema).append(L".");
	}
	ddl.append(L"\"").append(tblName).append(L"\"");

	try {
		QSqlStatement query(getUserConnect(userDbId), ddl.c_str());
		query.exec();
	} catch (SQLite::QSqlException &e) {
		std::wstring _err = e.getErrorStr();
		Q_ERROR(L"truncate table has error:{}, msg:{}", e.getErrorCode(), _err);
		throw QSqlExecuteException(std::to_wstring(e.getErrorCode()), _err);
	}
}

void TableUserRepository::dropTable(uint64_t userDbId, const std::wstring & tblName, const std::wstring & schema)
{
	std::wstring ddl = L"DROP TABLE IF EXISTS ";
	if (!schema.empty() && schema != L"main") {
		ddl.append(schema).append(L".");
	}
	ddl.append(L"\"").append(tblName).append(L"\"");

	try {
		QSqlStatement query(getUserConnect(userDbId), ddl.c_str());
		query.exec();
	} catch (SQLite::QSqlException &e) {
		std::wstring _err = e.getErrorStr();
		Q_ERROR(L"truncate table has error:{}, msg:{}", e.getErrorCode(), _err);
		throw QSqlExecuteException(std::to_wstring(e.getErrorCode()), _err);
	}
}

UserTable TableUserRepository::toUserTable(QSqlStatement &query)
{
	UserTable item;
	item.name = query.getColumn(L"name").isNull() ? L"" : query.getColumn(L"name").getText();
	item.sql = query.getColumn(L"sql").isNull() ? L"" : query.getColumn(L"sql").getText();
	item.tblName = query.getColumn(L"tbl_name").isNull() ? L"" : query.getColumn(L"tbl_name").getText();
	return item;
}

RowItem TableUserRepository::toRowItem(QSqlStatement &query)
{
	RowItem rowItem;
	int columnCount = query.getColumnCount();
	for (int i = 0; i < columnCount; i++) {
		std::wstring val = query.getColumn(i).isNull() ? L"" : query.getColumn(i).getText();
		rowItem.push_back(val);
	}
	return rowItem;
}


