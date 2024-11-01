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

int SqlExecutorUserRepository::execSql(uint64_t userDbId, const std::wstring &sql)
{
	try {
		return getUserConnect(userDbId)->tryExec(sql.c_str());
	} catch (SQLite::QSqlException &e) {
		std::wstring _err = e.getErrorStr();
		Q_ERROR(L"create table has error:{}, msg:{}", e.getErrorCode(), _err);
		QSqlExecuteException ex(std::to_wstring( e.getErrorCode()), _err, sql);
		throw ex;
	}
	return 0;
}

DataList SqlExecutorUserRepository::explainSql(uint64_t userDbId, const std::wstring &sql)
{
	DataList result;
	std::wstring explainSql;
	if (StringUtil::startWith(sql, L"EXPLAIN QUERY PLAN")) {
		explainSql = StringUtil::replace(sql, L"EXPLAIN QUERY PLAN", L"EXPLAIN", true);
	}else if (StringUtil::startWith(sql, L"EXPLAIN")) {
		explainSql = sql;
	} else {
		explainSql = L"EXPLAIN ";
		explainSql.append(sql);
	}
	
	try {
		QSqlStatement query(getUserConnect(userDbId), explainSql.c_str());

		while (query.executeStep()) {
			RowItem rowItem = toRowItem(query);
			result.push_back(rowItem);
		}
		return result;
	} catch (SQLite::QSqlException &ex) {
		std::wstring _err = ex.getErrorStr();
		Q_ERROR(L"query db has error:{}, msg:{}", ex.getErrorCode(), _err);
		throw QSqlExecuteException(std::to_wstring(ex.getErrorCode()), ex.getErrorStr(), sql);
	}
}

ExplainQueryPlans SqlExecutorUserRepository::explainQueryPlanSql(uint64_t userDbId, const std::wstring &sql)
{
	ExplainQueryPlans results;
	std::wstring explainSql;
	if (StringUtil::startWith(sql, L"EXPLAIN QUERY PLAN")) {
		explainSql = sql;		
	} else if (StringUtil::startWith(sql, L"EXPLAIN")) {
		explainSql = StringUtil::replace(sql, L"EXPLAIN", L"EXPLAIN QUERY PLAN", true);
	} else {
		explainSql = L"EXPLAIN QUERY PLAN ";
		explainSql.append(sql);
	}
	
	try {
		QSqlStatement query(getUserConnect(userDbId), explainSql.c_str());

		while (query.executeStep()) {
			auto item = toExplainQueryPlan(query);
			results.push_back(item);
		}
		return results;
	} catch (SQLite::QSqlException &ex) {
		std::wstring _err = ex.getErrorStr();
		Q_ERROR(L"query db has error:{}, msg:{}", ex.getErrorCode(), _err);
		throw QSqlExecuteException(std::to_wstring(ex.getErrorCode()), ex.getErrorStr(), sql);
	}
}

ExplainQueryPlan SqlExecutorUserRepository::toExplainQueryPlan(QSqlStatement &query)
{
	ExplainQueryPlan item;
	item.id = query.getColumn(L"id").isNull() ? 0 : query.getColumn(L"id").getInt();
	item.parent = query.getColumn(L"parent").isNull() ? 0 : query.getColumn(L"parent").getInt();
	item.notused = query.getColumn(L"notused").isNull() ? 0 : query.getColumn(L"notused").getInt();
	item.detail = query.getColumn(L"detail").isNull() ? L"" : query.getColumn(L"detail").getText();
	return item;
}

