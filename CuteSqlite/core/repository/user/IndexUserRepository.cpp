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

 * @file   IndexUserRepository.cpp
 * @brief  Operations on views in the user database
 * 
 * @author Xuehan Qin
 * @date   2023-05-20
 *********************************************************************/
#include "stdafx.h"
#include "IndexUserRepository.h"
#include "core/common/exception/QRuntimeException.h"
#include "core/common/repository/QSqlColumn.h"

UserIndexList IndexUserRepository::getListByTblName(uint64_t userDbId, const std::wstring &tblName, const std::wstring & schema)
{
	UserIndexList result;
	//std::wstring sql = L"SELECT * FROM sqlite_master WHERE type='index' and tbl_name=:tbl_name ORDER BY name ASC";
	std::wstring sql = L"SELECT * FROM ";
	if (!schema.empty() && schema != L"main") {
		sql.append(L"\"").append(schema).append(L"\".").append(L"\"sqlite_master\"");
	} else {
		sql.append(L"\"sqlite_master\"");
	}
	sql.append(L" WHERE type='index' and tbl_name=:tbl_name ORDER BY name ASC");
	try {
		QSqlStatement query(getUserConnect(userDbId), sql.c_str());
		query.bind(L":tbl_name", tblName);

		while (query.executeStep()) {
			UserIndex item = toUserIndex(query);
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

IndexInfoList IndexUserRepository::getInfoListByTblName(uint64_t userDbId, const std::wstring &tblName, const std::wstring & schema /*= std::wstring()*/)
{
	IndexInfoList result;
	//std::wstring sql = L"SELECT * FROM sqlite_master WHERE type='index' and tbl_name=:tbl_name ORDER BY name ASC";
	std::wstring sql = L"PRAGMA ";
	if (!schema.empty() && schema != L"main") {
		sql.append(schema).append(L".");
	} 
	sql.append(L"index_list(").append(L"\"").append(tblName).append(L"\")");
	try {
		QSqlStatement query(getUserConnect(userDbId), sql.c_str());
		query.bind(L":tbl_name", tblName);

		while (query.executeStep()) {
			IndexInfo item = toIndexInfo(query);
			result.push_back(item);
		}
		return result;
	}
	catch (SQLite::QSqlException &e) {
		std::wstring _err = e.getErrorStr();
		Q_ERROR(L"query db has error:{}, msg:{}", e.getErrorCode(), _err);
		throw QRuntimeException(L"200021", L"sorry, system has error when loading databases.");
	}
}

UserIndex IndexUserRepository::toUserIndex(QSqlStatement &query)
{
	UserIndex item;
	item.name = query.getColumn(L"name").isNull() ? L"" : query.getColumn(L"name").getText();
	item.sql = query.getColumn(L"sql").isNull() ? L"" : query.getColumn(L"sql").getText();
	return item;
}

IndexInfo IndexUserRepository::toIndexInfo(QSqlStatement &query)
{
	IndexInfo item;
	item.name = query.getColumn(L"name").isNull() ? L"" : query.getColumn(L"name").getText();
	item.sql = query.getColumn(L"sql").isNull() ? L"" : query.getColumn(L"sql").getText();
	return item;
}
