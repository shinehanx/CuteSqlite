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
#include "core/common/repository/QSqlColumn.h"

UserTableList TableUserRepository::getListByUserDbId(uint64_t userDbId)
{
	UserTableList result;
	std::wstring sql = L"SELECT * FROM sqlite_master WHERE type='table' ORDER BY name ASC";
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

UserTable TableUserRepository::getTable(uint64_t userDbId, std::wstring & tblName)
{
	UserTable result;
	std::wstring sql = L"SELECT * FROM sqlite_master WHERE type='table' and name=:name ORDER BY name ASC";
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
		Q_ERROR(L"query db has error:{}, msg:{}", e.getErrorCode(), _err);
		throw QRuntimeException(L"200021", L"sorry, system has error when loading databases.");
	}
}

UserTable TableUserRepository::toUserTable(QSqlStatement &query)
{
	UserTable item;
	item.name = query.getColumn(L"name").getText();
	item.sql = query.getColumn(L"sql").getText();
	return item;
}
