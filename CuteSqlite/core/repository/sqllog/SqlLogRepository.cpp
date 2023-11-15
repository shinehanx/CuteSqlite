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

 * @file   SqlLogRepository.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-11-15
 *********************************************************************/
#include "stdafx.h"
#include "SqlLogRepository.h"


uint64_t SqlLogRepository::create(SqlLog & item)
{
	//sql
	std::wstring sql = L"INSERT INTO sql_log (user_db_id, sql, effect_rows, code, msg, exec_time, transfer_time, total_time, top, created_at, updated_at) \
						VALUES (:user_db_id, :sql, :effect_rows, :code, :msg, :exec_time, :transfer_time, :total_time, :top, :created_at, :updated_at)";
	try {
		QSqlStatement query(getSysConnect(), sql.c_str());
		queryBind(query, item);

		query.exec();
		Q_DEBUG(L"create sql_log success.");
		return getSysConnect()->getLastInsertRowid();
	} catch (SQLite::QSqlException &e) {
		std::wstring _err = e.getErrorStr(); 
		Q_ERROR(L"exec sql has error, code:{}, msg:{}, sql:{}", e.getErrorCode(), _err, sql);
		throw QRuntimeException(L"10031", L"sorry, system has error.");
	}
}


bool SqlLogRepository::remove(uint64_t id)
{
	if (id <= 0) {
		return false;
	}
	//sql
	std::wstring sql = L"DELETE FROM sql_log WHERE id=:id ";
	try {
		QSqlStatement query(getSysConnect(), sql.c_str());
		query.bind(L":id", id);

		query.exec();
		Q_DEBUG(L"delete sql_log success.");
		return true;
	} catch (SQLite::QSqlException &e) {
		std::wstring _err = e.getErrorStr();
		Q_ERROR(L"exec sql has error, code:{}, msg:{}, sql:{}", e.getErrorCode(), _err, sql);
		throw QRuntimeException(L"10032", L"sorry, system has error.");
	}
}


SqlLog SqlLogRepository::getById(uint64_t id)
{
	if (id <= 0) {
		return SqlLog();
	}

	std::wstring sql = L"SELECT * FROM user_log WHERE id=:id";

	try {
		QSqlStatement query(getSysConnect(), sql.c_str());
		query.bind(L":id", id);

		//Ö´ÐÐÓï¾ä
		if (!query.executeStep()) {
			return SqlLog();
		}

		Q_DEBUG(L"Get sql_log detail success");
		SqlLog item = toSqlLog(query);
		return item;
	} catch (SQLite::QSqlException &e) {
		std::wstring _err = e.getErrorStr();
		Q_ERROR(L"exec sql has error, code:{}, msg:{}, sql:{}", e.getErrorCode(), _err, sql);
		throw QRuntimeException(L"10033", L"sorry, system has error.");
	}
}


SqlLogList SqlLogRepository::getAll(uint64_t limit)
{
	SqlLogList result;
	std::wstring sql = L"SELECT * FROM sql_log ORDER BY id DESC LIMIT :limit";
	try {
		QSqlStatement query(getSysConnect(), sql.c_str());
		query.bind(L":limit", limit);

		while (query.executeStep()) {
			SqlLog item = toSqlLog(query);
			result.push_back(item);
		}
		return result;
	} catch (SQLite::QSqlException &e) {
		std::wstring _err = e.getErrorStr();
		Q_ERROR(L"query sql_log has error:{}, msg:{}", e.getErrorCode(), _err);
		throw QRuntimeException(L"000024", L"sorry, system has error when getting sql log.");
	}
}


uint64_t SqlLogRepository::getCount()
{
	std::wstring sql = L"SELECT count(id) as total FROM sql_log ";

	try {
		QSqlStatement query(getSysConnect(), sql.c_str());

		if (query.executeStep()) {
			uint64_t total = query.getColumn(L"total").getInt64();
			return total;
		}
		return 0;
	} catch (SQLite::QSqlException &e) {
		std::wstring _err = e.getErrorStr();
		Q_ERROR(L"query sql_log count has error:{}, msg:{}", e.getErrorCode(), _err);
		throw QRuntimeException(L"000025", L"sorry, system has error when getting total from sql_log");
	}
}


std::vector<uint64_t> SqlLogRepository::getFrontIds(uint64_t limit)
{
	std::vector<uint64_t> result;
	std::wstring sql = L"SELECT id FROM sql_log ORDER BY id ASC LIMIT :limit";
	try {
		QSqlStatement query(getSysConnect(), sql.c_str());

		while (query.executeStep()) {
			uint64_t id = static_cast<uint64_t>(query.getColumn(L"id").getInt64());
			result.push_back(id);
		}
		return result;
	} catch (SQLite::QSqlException &e) {
		std::wstring _err = e.getErrorStr();
		Q_ERROR(L"query sql_log has error:{}, msg:{}", e.getErrorCode(), _err);
		throw QRuntimeException(L"000026", L"sorry, system has error when getting ids of sql_log .");
	}
}

bool SqlLogRepository::removeByBiggerId(uint64_t id)
{
	if (id <= 0) {
		return false;
	}
	//sql
	std::wstring sql = L"DELETE FROM sql_log WHERE id>:id ";
	try {
		QSqlStatement query(getSysConnect(), sql.c_str());
		query.bind(L":id", id);

		query.exec();
		Q_DEBUG(L"delete sql_log success.");
		return true;
	} catch (SQLite::QSqlException &e) {
		std::wstring _err = e.getErrorStr();
		Q_ERROR(L"exec sql has error, code:{}, msg:{}, sql:{}", e.getErrorCode(), _err, sql);
		throw QRuntimeException(L"000025", L"sorry, system has error when remove by bigger id.");
	}
}

void SqlLogRepository::queryBind(QSqlStatement &query, SqlLog &item, bool isUpdate /*= false*/)
{
	if (isUpdate) {
		query.bind(L":id", item.id);
	}
	query.bind(L":sql", item.sql);
	query.bind(L":user_db_id", item.userDbId);
	query.bind(L":effect_rows", item.effectRows);
	query.bind(L":code", item.code);
	query.bind(L":msg", item.msg);
	query.bind(L":exec_time", item.execTime);
	query.bind(L":transfer_time", item.transferTime);
	query.bind(L":total_time", item.totalTime);
	query.bind(L":top", item.top);
	query.bind(L":created_at", item.createdAt);
	query.bind(L":updated_at", item.createdAt);
}

SqlLog SqlLogRepository::toSqlLog(QSqlStatement &query)
{
	SqlLog item;
	item.id = static_cast<uint64_t>(query.getColumn(L"id").getInt64());
	item.userDbId = static_cast<uint64_t>(query.getColumn(L"user_db_id").getInt64());
	item.sql = query.getColumn(L"sql").getText();
	item.effectRows = query.getColumn(L"effect_rows").getInt();
	item.code = query.getColumn(L"code").getInt();
	item.msg = query.getColumn(L"msg").getText();
	item.execTime = query.getColumn(L"exec_time").getText();
	item.transferTime = query.getColumn(L"transfer_time").getText();
	item.totalTime = query.getColumn(L"total_time").getText();
	item.top = query.getColumn(L"top").getInt();
	item.createdAt = query.getColumn(L"created_at").getText();
	
	return item;
}
