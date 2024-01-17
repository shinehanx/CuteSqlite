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

 * @file   SqliteSchemaUserRepository.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2024-01-16
 *********************************************************************/
#include "stdafx.h"
#include "SqliteSchemaUserRepository.h"
#include "core/common/exception/QSqlExecuteException.h"

SqliteSchemaList SqliteSchemaUserRepository::getListGtRootpage(uint64_t userDbId)
{
	SqliteSchemaList result;
	// std::wstring sql = L"SELECT * FROM sqlite_master WHERE type='table' ORDER BY name ASC";
	std::wstring sql = L"SELECT * FROM sqlite_schema WHERE rootpage>0";
	
	try {
		QSqlStatement query(getUserConnect(userDbId), sql.c_str());

		while (query.executeStep()) {
			SqliteSchema item = toSqliteSchema(query);
			result.push_back(item);
		}
		return result;
	} catch (SQLite::QSqlException &ex) {
		std::wstring _err = ex.getErrorStr();
		Q_ERROR(L"query db has error:{}, msg:{}", ex.getErrorCode(), _err);
		throw QSqlExecuteException(std::to_wstring(ex.getErrorCode()), ex.getErrorStr(), sql);
	}
}


int SqliteSchemaUserRepository::getTableCount(uint64_t userDbId)
{
	// sql : SELECT * FROM sqlite_master WHERE type='table' and name=:name ORDER BY name ASC
	int result = 0;
	std::wstring sql = L"SELECT count(*)+1 FROM sqlite_schema WHERE type='table'";
	try {
		QSqlStatement query(getUserConnect(userDbId), sql.c_str());
		
		if (query.executeStep()) {
			result = query.getColumn(0).getInt();
		}
		return result;
	} catch (SQLite::QSqlException &ex) {
		std::wstring _err = ex.getErrorStr();
		Q_ERROR(L"query db has error:{}, msg:{}", ex.getErrorCode(), _err);
		throw QSqlExecuteException(std::to_wstring(ex.getErrorCode()), ex.getErrorStr(), sql);
	}
}

int SqliteSchemaUserRepository::getIndexCount(uint64_t userDbId)
{
	// sql : SELECT * FROM sqlite_master WHERE type='table' and name=:name ORDER BY name ASC
	int result = 0;
	std::wstring sql = L"SELECT count(*)+1 FROM sqlite_schema WHERE type='index'";
	try {
		QSqlStatement query(getUserConnect(userDbId), sql.c_str());
		
		if (query.executeStep()) {
			result = query.getColumn(0).getInt();
		}
		return result;
	} catch (SQLite::QSqlException &ex) {
		std::wstring _err = ex.getErrorStr();
		Q_ERROR(L"query db has error:{}, msg:{}", ex.getErrorCode(), _err);
		throw QSqlExecuteException(std::to_wstring(ex.getErrorCode()), ex.getErrorStr(), sql);
	}
}


int SqliteSchemaUserRepository::getAutoIndexCount(uint64_t userDbId)
{
	// sql : SELECT * FROM sqlite_master WHERE type='table' and name=:name ORDER BY name ASC
	int result = 0;
	std::wstring sql = L"SELECT count(*)+1 FROM sqlite_schema WHERE name LIKE 'sqlite_autoindex%'";
	try {
		QSqlStatement query(getUserConnect(userDbId), sql.c_str());
		
		if (query.executeStep()) {
			result = query.getColumn(0).getInt();
		}
		return result;
	} catch (SQLite::QSqlException &ex) {
		std::wstring _err = ex.getErrorStr();
		Q_ERROR(L"query db has error:{}, msg:{}", ex.getErrorCode(), _err);
		throw QSqlExecuteException(std::to_wstring(ex.getErrorCode()), ex.getErrorStr(), sql);
	}
}

SqliteSchema SqliteSchemaUserRepository::toSqliteSchema(QSqlStatement & query)
{
	SqliteSchema item;
	item.type = query.getColumn(L"type").isNull() ? L"" : query.getColumn(L"type").getText();
	item.name = query.getColumn(L"name").isNull() ? L"" : query.getColumn(L"name").getText();
	item.tblName = query.getColumn(L"tbl_name").isNull() ? L"" : query.getColumn(L"tbl_name").getText();
	item.rootpage = query.getColumn(L"rootpage").isNull() ? 0 : query.getColumn(L"rootpage").getInt64();
	item.sql = query.getColumn(L"sql").isNull() ? L"" : query.getColumn(L"sql").getText();
	return item;
}
