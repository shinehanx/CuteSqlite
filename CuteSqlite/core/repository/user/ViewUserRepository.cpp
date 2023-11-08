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

 * @file   ViewUserRepository.cpp
 * @brief  Operations on views in the user database
 * 
 * @author Xuehan Qin
 * @date   2023-05-20
 *********************************************************************/
#include "stdafx.h"
#include "ViewUserRepository.h"
#include "core/common/exception/QRuntimeException.h"
#include "core/common/repository/QSqlColumn.h"
#include "core/common/exception/QSqlExecuteException.h"

UserViewList ViewUserRepository::getListByUserDbId(uint64_t userDbId, const std::wstring & schema)
{
	UserViewList result;
	// std::wstring sql = L"SELECT * FROM sqlite_master WHERE type='view' ORDER BY name ASC";
	std::wstring sql = L"SELECT * FROM ";
	if (!schema.empty() && schema != L"main") {
		sql.append(L"\"").append(schema).append(L"\".").append(L"\"sqlite_master\"");
	} else {
		sql.append(L"\"sqlite_master\"");
	}
	sql.append(L" WHERE type='view' ORDER BY name ASC");
	try {
		QSqlStatement query(getUserConnect(userDbId), sql.c_str());

		while (query.executeStep()) {
			UserView item = toUserView(query);
			result.push_back(item);
		}
		return result;
	}
	catch (SQLite::QSqlException &e) {
		std::wstring _err = e.getErrorStr();
		Q_ERROR(L"query db has error:{}, msg:{}", e.getErrorCode(), _err);
		throw QRuntimeException(L"200100", L"sorry, system has error when loading databases.");
	}
}

UserView ViewUserRepository::getView(uint64_t userDbId, const std::wstring & viewName, const std::wstring & schema)
{
	UserTrigger result;
	//std::wstring sql = L"SELECT * FROM sqlite_master WHERE type='view' and name=:name ORDER BY name ASC";
	std::wstring sql = L"SELECT * FROM ";
	if (!schema.empty() && schema != L"main") {
		sql.append(L"\"").append(schema).append(L"\".").append(L"\"sqlite_master\"");
	} else {
		sql.append(L"\"sqlite_master\"");
	}
	sql.append(L" WHERE type='view' and name=:name ORDER BY name ASC");
	try {
		QSqlStatement query(getUserConnect(userDbId), sql.c_str());
		query.bind(L":name", viewName);

		if (query.executeStep()) {
			result = toUserView(query);
		}
		return result;
	}
	catch (SQLite::QSqlException &e) {
		std::wstring _err = e.getErrorStr();
		Q_ERROR(L"query db has error:{}, msg:{}", e.getErrorCode(), _err);
		throw QRuntimeException(L"200101", L"sorry, system has error when loading databases.");
	}
}

void ViewUserRepository::drop(uint64_t userDbId, const std::wstring & viewName, const std::wstring & schema /*= std::wstring()*/)
{
	std::wstring sql = L"DROP VIEW IF EXISTS ";
	if (!schema.empty() && schema != L"main") {
		sql.append(L"\"").append(schema).append(L"\".");
	}
	sql.append(L"\"").append(viewName).append(L"\"");
	
	try {
		QSqlStatement query(getUserConnect(userDbId), sql.c_str());
		query.exec();
	} catch (SQLite::QSqlException &e) {
		std::wstring _err = e.getErrorStr();
		Q_ERROR(L"Execute sql has error:{}, msg:{}, SQL:{}", e.getErrorCode(), _err, sql);
		QSqlExecuteException ex(std::to_wstring(e.getErrorCode()), _err, sql);
		
		throw ex;
	}
}

UserView ViewUserRepository::toUserView(QSqlStatement &query)
{
	UserView item;
	item.name = query.getColumn(L"name").isNull() ? L"" : query.getColumn(L"name").getText();
	item.sql = query.getColumn(L"sql").isNull() ? L"" : query.getColumn(L"sql").getText();
	item.tblName = query.getColumn(L"tbl_name").isNull() ? L"" : query.getColumn(L"tbl_name").getText();
	return item;
}
