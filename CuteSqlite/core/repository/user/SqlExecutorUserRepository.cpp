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

 * @file   SqlExecutorUserRepository.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-05-28
 *********************************************************************/
#include "stdafx.h"
#include "SqlExecutorUserRepository.h"
#include <xutility>
#include "core/common/exception/QRuntimeException.h"
#include "core/common/exception/QSqlExecuteException.h"

QSqlStatement SqlExecutorUserRepository::tryExecSql(uint64_t userDbId, const std::wstring &sql)
{
	try {
		QSqlStatement query(getUserConnect(userDbId), sql.c_str());
		return std::move(query);
	}
	catch (SQLite::QSqlException &ex) {
		std::wstring _err = ex.getErrorStr();
		Q_ERROR(L"query db has error:{}, msg:{}", ex.getErrorCode(), _err);
		throw ex;
	}
}

void SqlExecutorUserRepository::execSql(uint64_t userDbId, const std::wstring &sql)
{
	try {
		QSqlStatement query(getUserConnect(userDbId), sql.c_str());
		query.exec();		
	} catch (SQLite::QSqlException &e) {
		std::wstring _err = e.getErrorStr();
		Q_ERROR(L"create table has error:{}, msg:{}", e.getErrorCode(), _err);
		QSqlExecuteException ex(std::to_wstring( e.getErrorCode()), _err);
		ex.setErrRow(1);
		ex.setErrCol(200);
		throw ex;
	}
}
