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

 * @file   FieldUserRepository.cpp
 * @brief  Operations on fields in the user database
 * 
 * @author Xuehan Qin
 * @date   2023-05-20
 *********************************************************************/
#include "stdafx.h"
#include "ColumnUserRepository.h"
#include <chrono>
#include "core/common/exception/QRuntimeException.h"
#include "core/common/repository/QSqlColumn.h"

ColumnInfoList ColumnUserRepository::getListByTblName(uint64_t userDbId, const std::wstring &tblName, const std::wstring & schema)
{
	ColumnInfoList result;
	std::wstring sql = L"PRAGMA ";
	if (!schema.empty() && schema != L"main") {
		sql.append(schema).append(L".");
	} 
	sql.append(L"table_info(\"").append(tblName).append(L"\")");
	try {
		QSqlStatement query(getUserConnect(userDbId), sql.c_str());

		while (query.executeStep()) {
			ColumnInfo item = toColumnInfo(query);
			result.push_back(item);
		}
		return result;
	} catch (SQLite::QSqlException &e) {
		std::wstring _err = e.getErrorStr();
		Q_ERROR(L"query db has error:{}, msg:{}", e.getErrorCode(), _err);
		throw QRuntimeException(L"200070", L"sorry, system has error when loading databases.");
	}
}

ColumnInfo ColumnUserRepository::toColumnInfo(QSqlStatement &query)
{
	ColumnInfo item;
	item.cid = query.getColumn(L"cid").isNull() ? 0
		: query.getColumn(L"cid").getUInt();
	item.name = query.getColumn(L"name").isNull() ? L""
		: query.getColumn(L"name").getText();
	item.type = query.getColumn(L"type").isNull() ? L"" 
		: query.getColumn(L"type").getText();
	item.notnull = query.getColumn(L"notnull").getInt();
	item.defVal = query.getColumn(L"dflt_value").isNull() ? L"" 
		: query.getColumn(L"dflt_value").getText();
	item.pk = query.getColumn(L"pk").getInt();
	item.seq = std::chrono::system_clock::now(); // seq = current time 
	return item;
}
