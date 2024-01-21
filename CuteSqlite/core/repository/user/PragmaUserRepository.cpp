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

 * @file   PragmaUserRepository.cpp
 * @brief  Exeute pragma sql from user db
 * 
 * @author Xuehan Qin
 * @date   2024-01-20
 *********************************************************************/
#include "stdafx.h"
#include "PragmaUserRepository.h"
#include <xutility>
#include "core/common/exception/QRuntimeException.h"
#include "core/common/exception/QSqlExecuteException.h"

QSqlStatement PragmaUserRepository::tryExec(uint64_t userDbId, const std::wstring &pragmaSql)
{
	try {
		QSqlStatement query(getUserConnect(userDbId), pragmaSql.c_str());
		return std::move(query);
	}
	catch (SQLite::QSqlException &ex) {
		std::wstring _err = ex.getErrorStr();
		Q_ERROR(L"query db has error:{}, msg:{}", ex.getErrorCode(), _err);
		throw ex;
	}
}

int PragmaUserRepository::exec(uint64_t userDbId, const std::wstring &pragmaSql)
{
	try {
		return getUserConnect(userDbId)->tryExec(pragmaSql.c_str());
	} catch (SQLite::QSqlException &e) {
		std::wstring _err = e.getErrorStr();
		Q_ERROR(L"create table has error:{}, msg:{}", e.getErrorCode(), _err);
		QSqlExecuteException ex(std::to_wstring( e.getErrorCode()), _err, pragmaSql);
		throw ex;
	}
	return 0;
}

std::wstring PragmaUserRepository::execForOne(uint64_t userDbId, const std::wstring &pragmaSql)
{
	try {
		QSqlStatement query(getUserConnect(userDbId), pragmaSql.c_str());
		if (query.executeStep()) {
			if (!query.getColumn(0).isNull()) {
				return query.getColumn(0).getText();
			}
		}
		return L"";
	} catch (SQLite::QSqlException &e) {
		std::wstring _err = e.getErrorStr();
		Q_ERROR(L"create table has error:{}, msg:{}", e.getErrorCode(), _err);
		QSqlExecuteException ex(std::to_wstring( e.getErrorCode()), _err, pragmaSql);
		throw ex;
	}
	return 0;
}

