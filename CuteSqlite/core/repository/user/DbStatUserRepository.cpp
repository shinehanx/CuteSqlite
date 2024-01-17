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

 * @file   DbstatUserRepository.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2024-01-16
 *********************************************************************/
#include "stdafx.h"
#include "DbStatUserRepository.h"
#include "core\common\exception\QSqlExecuteException.h"

int DbStatUserRepository::getPageSize(uint64_t userDbId)
{
	// sql : SELECT * FROM sqlite_master WHERE type='table' and name=:name ORDER BY name ASC
	int result = 0;
	std::wstring sql = L"PRAGMA page_size";
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

uint64_t DbStatUserRepository::getPageCount(uint64_t userDbId)
{
	// sql : SELECT * FROM sqlite_master WHERE type='table' and name=:name ORDER BY name ASC
	uint64_t result = 0;
	std::wstring sql = L"PRAGMA page_count";
	try {
		QSqlStatement query(getUserConnect(userDbId), sql.c_str());
		
		if (query.executeStep()) {
			result = query.getColumn(0).getInt64();
		}
		return result;
	} catch (SQLite::QSqlException &ex) {
		std::wstring _err = ex.getErrorStr();
		Q_ERROR(L"query db has error:{}, msg:{}", ex.getErrorCode(), _err);
		throw QSqlExecuteException(std::to_wstring(ex.getErrorCode()), ex.getErrorStr(), sql);
	}
}

uint64_t DbStatUserRepository::getAutovacuum(uint64_t userDbId)
{
	// sql : SELECT * FROM sqlite_master WHERE type='table' and name=:name ORDER BY name ASC
	uint64_t result = 0;
	std::wstring sql = L"PRAGMA auto_vacuum";
	try {
		QSqlStatement query(getUserConnect(userDbId), sql.c_str());
		
		if (query.executeStep()) {
			result = query.getColumn(0).getInt64();
		}
		return result;
	} catch (SQLite::QSqlException &ex) {
		std::wstring _err = ex.getErrorStr();
		Q_ERROR(L"query db has error:{}, msg:{}", ex.getErrorCode(), _err);
		throw QSqlExecuteException(std::to_wstring(ex.getErrorCode()), ex.getErrorStr(), sql);
	}
}

uint64_t DbStatUserRepository::getFreePageCount(uint64_t userDbId)
{
	// sql : SELECT * FROM sqlite_master WHERE type='table' and name=:name ORDER BY name ASC
	uint64_t result = 0;
	std::wstring sql = L"PRAGMA freelist_count";
	try {
		QSqlStatement query(getUserConnect(userDbId), sql.c_str());
		
		if (query.executeStep()) {
			result = query.getColumn(0).getInt64();
		}
		return result;
	} catch (SQLite::QSqlException &ex) {
		std::wstring _err = ex.getErrorStr();
		Q_ERROR(L"query db has error:{}, msg:{}", ex.getErrorCode(), _err);
		throw QSqlExecuteException(std::to_wstring(ex.getErrorCode()), ex.getErrorStr(), sql);
	}
}

TblIdxSpaceUsed DbStatUserRepository::getTblIdxSpaceUsedByName(uint64_t userDbId, const std::wstring & name)
{
	TblIdxSpaceUsed result;
	std::wstring sql = L"SELECT \
      sum(ncell) AS nentry, \
      sum((pagetype=='leaf')*ncell) AS leaf_entries,\
      sum(payload) AS payload, \
      sum((pagetype=='overflow') * payload) AS ovfl_payload, \
      sum(path LIKE '%+000000') AS ovfl_cnt, \
      max(mx_payload) AS mx_payload, \
      sum(pagetype=='internal') AS int_pages, \
      sum(pagetype=='leaf') AS leaf_pages, \
      sum(pagetype=='overflow') AS ovfl_pages, \
      sum((pagetype=='internal') * unused) AS int_unused,\
      sum((pagetype=='leaf') * unused) AS leaf_unused, \
      sum((pagetype=='overflow') * unused) AS ovfl_unused, \
      sum(pgsize) AS compressed_size, \
      max((length(CASE WHEN path LIKE '%+%' THEN '' ELSE path END)+3)/4) \
        AS depth \
    FROM temp.dbstat WHERE name = :name";

	try {
		QSqlStatement query(getUserConnect(userDbId), sql.c_str());
		query.bind(L":name", name);
		if (query.executeStep()) {
			result = toTblIdxSpaceUsed(query);
		}
		return result;
	} catch (SQLite::QSqlException &ex) {
		std::wstring _err = ex.getErrorStr();
		Q_ERROR(L"query db has error:{}, msg:{}", ex.getErrorCode(), _err);
		throw QSqlExecuteException(std::to_wstring(ex.getErrorCode()), ex.getErrorStr(), sql);
	}
}

TblIdxSpaceUsed DbStatUserRepository::toTblIdxSpaceUsed(QSqlStatement & query)
{
	TblIdxSpaceUsed item;
	item.nentry = query.getColumn(L"nentry").isNull() ? 0 : query.getColumn(L"nentry").getInt64();
	item.leafEntries = query.getColumn(L"leaf_entries").isNull() ? 0 : query.getColumn(L"leaf_entries").getInt64();
	item.payload = query.getColumn(L"payload").isNull() ? 0 : query.getColumn(L"payload").getInt64();
	item.ovflPayload = query.getColumn(L"ovfl_payload").isNull() ? 0 : query.getColumn(L"ovfl_payload").getInt64();
	item.ovflCnt = query.getColumn(L"ovfl_cnt").isNull() ? 0 : query.getColumn(L"ovfl_cnt").getInt64();
	item.mxPayload = query.getColumn(L"mx_payload").isNull() ? 0 : query.getColumn(L"mx_payload").getInt64();
	item.intPages = query.getColumn(L"int_pages").isNull() ? 0 : query.getColumn(L"int_pages").getInt64();
	item.leafPages = query.getColumn(L"leaf_pages").isNull() ? 0 : query.getColumn(L"leaf_pages").getInt64();
	item.ovflPages = query.getColumn(L"ovfl_pages").isNull() ? 0 : query.getColumn(L"ovfl_pages").getInt64();
	item.intUnused = query.getColumn(L"int_unused").isNull() ? 0 : query.getColumn(L"int_unused").getInt64();
	item.leafUnused = query.getColumn(L"leaf_unused").isNull() ? 0 : query.getColumn(L"leaf_unused").getInt64();
	item.ovflUnused = query.getColumn(L"ovfl_unused").isNull() ? 0 : query.getColumn(L"ovfl_unused").getInt64();
	item.compressedSize = query.getColumn(L"compressed_size").isNull() ? 0 : query.getColumn(L"compressed_size").getInt64();
	item.depth = query.getColumn(L"depth").isNull() ? 0 : query.getColumn(L"depth").getInt();
	return item;
}
