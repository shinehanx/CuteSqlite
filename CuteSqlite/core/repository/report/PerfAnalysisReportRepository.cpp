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

 * @file   PerfAnalysisReportRepository.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-11-15
 *********************************************************************/
#include "stdafx.h"
#include "PerfAnalysisReportRepository.h"


uint64_t PerfAnalysisReportRepository::create(PerfAnalysisReport & item)
{
	//sql
	std::wstring sql = L"INSERT INTO perf_analysis_report (user_db_id, sql_log_id, created_at, updated_at) \
						VALUES (:user_db_id, :sql_log_id, :created_at, :updated_at)";
	try {
		QSqlStatement query(getSysConnect(), sql.c_str());
		queryBind(query, item);

		query.exec();
		Q_DEBUG(L"create sql_log success.");
		return getSysConnect()->getLastInsertRowid();
	} catch (SQLite::QSqlException &e) {
		std::wstring _err = e.getErrorStr(); 
		Q_ERROR(L"exec sql has error, code:{}, msg:{}, sql:{}", e.getErrorCode(), _err, sql);
		throw QRuntimeException(L"10041", L"sorry, system has error.");
	}
}


int PerfAnalysisReportRepository::remove(uint64_t id)
{
	if (id <= 0) {
		return false;
	}
	//sql
	std::wstring sql = L"DELETE FROM perf_analysis_report WHERE id=:id ";
	try {
		QSqlStatement query(getSysConnect(), sql.c_str());
		query.bind(L":id", id);

		Q_DEBUG(L"delete perf_analysis_report success.");
		return query.exec();
	} catch (SQLite::QSqlException &e) {
		std::wstring _err = e.getErrorStr();
		Q_ERROR(L"exec sql has error, code:{}, msg:{}, sql:{}", e.getErrorCode(), _err, sql);
		throw QRuntimeException(L"10042", L"sorry, system has error.");
	}
}


PerfAnalysisReport PerfAnalysisReportRepository::getById(uint64_t id)
{
	if (id <= 0) {
		return PerfAnalysisReport();
	}

	std::wstring sql = L"SELECT * FROM perf_analysis_report WHERE id=:id";

	try {
		QSqlStatement query(getSysConnect(), sql.c_str());
		query.bind(L":id", id);

		//Ö´ÐÐÓï¾ä
		if (!query.executeStep()) {
			return PerfAnalysisReport();
		}

		Q_DEBUG(L"Get perf_analysis_report detail success");
		PerfAnalysisReport item = toPerfAnalysisReport(query);
		return item;
	} catch (SQLite::QSqlException &e) {
		std::wstring _err = e.getErrorStr();
		Q_ERROR(L"exec sql has error, code:{}, msg:{}, sql:{}", e.getErrorCode(), _err, sql);
		throw QRuntimeException(L"10043", L"sorry, system has error.");
	}
}


PerfAnalysisReport PerfAnalysisReportRepository::getBySqlLogId(uint64_t sqlLogId)
{
	if (sqlLogId <= 0) {
		return PerfAnalysisReport();
	}

	std::wstring sql = L"SELECT * FROM perf_analysis_report WHERE sql_log_id=:sql_log_id";

	try {
		QSqlStatement query(getSysConnect(), sql.c_str());
		query.bind(L":sql_log_id", sqlLogId);

		//Ö´ÐÐÓï¾ä
		if (!query.executeStep()) {
			return PerfAnalysisReport();
		}

		Q_DEBUG(L"Get perf_analysis_report detail success");
		PerfAnalysisReport item = toPerfAnalysisReport(query);
		return item;
	} catch (SQLite::QSqlException &e) {
		std::wstring _err = e.getErrorStr();
		Q_ERROR(L"exec sql has error, code:{}, msg:{}, sql:{}", e.getErrorCode(), _err, sql);
		throw QRuntimeException(L"10044", L"sorry, system has error.");
	}
}

PerfAnalysisReportList PerfAnalysisReportRepository::getAll(uint64_t limit)
{
	PerfAnalysisReportList result;
	std::wstring sql = L"SELECT * FROM perf_analysis_report ORDER BY id ASC LIMIT :limit";
	try {
		QSqlStatement query(getSysConnect(), sql.c_str());
		query.bind(L":limit", limit);

		while (query.executeStep()) {
			PerfAnalysisReport item = toPerfAnalysisReport(query);
			result.push_back(item);
		}
		return result;
	} catch (SQLite::QSqlException &e) {
		std::wstring _err = e.getErrorStr();
		Q_ERROR(L"query perf_analysis_report has error:{}, msg:{}", e.getErrorCode(), _err);
		throw QRuntimeException(L"10045", L"sorry, system has error when getting sql log.");
	}
}

uint64_t PerfAnalysisReportRepository::getCount()
{
	std::wstring sql = L"SELECT count(id) as total FROM perf_analysis_report ";

	try {
		QSqlStatement query(getSysConnect(), sql.c_str());

		if (query.executeStep()) {
			uint64_t total = query.getColumn(L"total").isNull() ? 0 
				: query.getColumn(L"total").getInt64();
			return total;
		}
		return 0;
	} catch (SQLite::QSqlException &e) {
		std::wstring _err = e.getErrorStr();
		Q_ERROR(L"query perf_analysis_report count has error:{}, msg:{}", e.getErrorCode(), _err);
		throw QRuntimeException(L"10046", L"sorry, system has error when getting total from sql_log");
	}
}


bool PerfAnalysisReportRepository::removeBySqlLogId(uint64_t sqlLogId)
{
	if (sqlLogId <= 0) {
		return false;
	}
	//sql
	std::wstring sql = L"DELETE FROM perf_analysis_report WHERE sql_log_id=:sql_log_id ";
	try {
		QSqlStatement query(getSysConnect(), sql.c_str());
		query.bind(L":sql_log_id", sqlLogId);

		Q_DEBUG(L"delete perf_analysis_report success.");
		return query.exec();
	} catch (SQLite::QSqlException &e) {
		std::wstring _err = e.getErrorStr();
		Q_ERROR(L"exec sql has error, code:{}, msg:{}, sql:{}", e.getErrorCode(), _err, sql);
		throw QRuntimeException(L"10047", L"sorry, system has error.");
	}
}


void PerfAnalysisReportRepository::queryBind(QSqlStatement &query, PerfAnalysisReport &item, bool isUpdate /*= false*/)
{
	if (isUpdate) {
		query.bind(L":id", item.id);
	}
	
	query.bind(L":user_db_id", item.userDbId);
	query.bind(L":sql_log_id", item.sqlLogId);
	
	query.bind(L":created_at", item.createdAt);
	query.bind(L":updated_at", item.createdAt);
}

PerfAnalysisReport PerfAnalysisReportRepository::toPerfAnalysisReport(QSqlStatement &query)
{
	PerfAnalysisReport item;
	item.id = static_cast<uint64_t>(query.getColumn(L"id").getInt64());
	item.userDbId = static_cast<uint64_t>(query.getColumn(L"user_db_id").getInt64());
	item.sqlLogId = static_cast<uint64_t>(query.getColumn(L"sql_log_id").getInt64());
	
	item.createdAt = query.getColumn(L"created_at").getText();
	item.updatedAt = query.getColumn(L"updated_at").getText();
	
	return item;
}
