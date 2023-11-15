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

 * @file   DbRepository.cpp
 * @brief  The table db repository
 * 
 * @author Xuehan Qin
 * @date   2023-05-19
 *********************************************************************/
#include "stdafx.h"
#include "UserDbRepository.h"
#include "core/common/exception/QRuntimeException.h"
#include "core/common/repository/QSqlColumn.h"

uint64_t UserDbRepository::create(UserDb & item)
{
	//sql
	std::wstring sql = L"INSERT INTO user_db (name, path, is_active, created_at, updated_at) \
						VALUES (:name, :path, :is_active, :created_at, :updated_at)";
	try {
		QSqlStatement query(getSysConnect(), sql.c_str());
		queryBind(query, item);

		query.exec();
		Q_INFO(L"create user_db success.");
		return getSysConnect()->getLastInsertRowid();
	}
	catch (SQLite::QSqlException &e) {
		std::wstring _err = e.getErrorStr();
		Q_ERROR(L"exec sql has error, code:{}, msg:{}, sql:{}", e.getErrorCode(), _err, sql);
		throw QRuntimeException(L"10020", L"sorry, system has error.");
	}
}


bool UserDbRepository::remove(uint64_t id)
{
	if (id <= 0) {
		return false;
	}
	//sql
	std::wstring sql = L"DELETE FROM user_db WHERE id=:id ";
	try {
		QSqlStatement query(getSysConnect(), sql.c_str());
		query.bind(L":id", id);

		query.exec();
		Q_INFO(L"delete user_db success.");
		return true;
	}
	catch (SQLite::QSqlException &e) {
		std::wstring _err = e.getErrorStr();
		Q_ERROR(L"exec sql has error, code:{}, msg:{}, sql:{}", e.getErrorCode(), _err, sql);
		throw QRuntimeException(L"10021", L"sorry, system has error.");
	}
}

UserDb UserDbRepository::getById(uint64_t id)
{
	if (id <= 0) {
		return UserDb();
	}

	std::wstring sql = L"SELECT * FROM user_db WHERE id=:id";

	try {
		QSqlStatement query(getSysConnect(), sql.c_str());
		query.bind(L":id", id);

		//Ö´ÐÐÓï¾ä
		if (!query.executeStep()) {
			return UserDb();
		}

		Q_INFO(L"Get user_db detail success");
		UserDb item = toUserDb(query);
		return item;
	}
	catch (SQLite::QSqlException &e) {
		std::wstring _err = e.getErrorStr();
		Q_ERROR(L"exec sql has error, code:{}, msg:{}, sql:{}", e.getErrorCode(), _err, sql);
		throw QRuntimeException(L"10022", L"sorry, system has error.");
	}
}


UserDb UserDbRepository::getByPath(std::wstring & path)
{
	if (path.empty()) {
		return UserDb();
	}
	std::wstring sql = L"SELECT * FROM user_db WHERE path=:path limit 1";

	try {
		QSqlStatement query(getSysConnect(), sql.c_str());
		query.bind(L":path", path);

		//Ö´ÐÐÓï¾ä
		if (!query.executeStep()) {
			return UserDb();
		}

		Q_INFO(L"Get db detail success");
		UserDb item = toUserDb(query);
		return item;
	}
	catch (SQLite::QSqlException &e) {
		std::wstring _err = e.getErrorStr();
		Q_ERROR(L"exec sql has error, code:{}, msg:{}, sql:{}", e.getErrorCode(), _err, sql);
		throw QRuntimeException(L"10023", L"sorry, system has error.");
	}
}

UserDbList UserDbRepository::getAll()
{
	UserDbList result;
	std::wstring sql = L"SELECT * FROM user_db ORDER BY id ASC";
	try {
		QSqlStatement query(getSysConnect(), sql.c_str());

		while (query.executeStep()) {
			UserDb item = toUserDb(query);
			result.push_back(item);
		}
		return result;
	}
	catch (SQLite::QSqlException &e) {
		std::wstring _err = e.getErrorStr();
		Q_ERROR(L"query db has error:{}, msg:{}", e.getErrorCode(), _err);
		throw QRuntimeException(L"000024", L"sorry, system has error when loading databases.");
	}
}


bool UserDbRepository::updateIsActiveById(uint64_t id, int isActive)
{
	if (id <= 0) {
		return false;
	}
	//sql
	std::wstring sql = L"UPDATE user_db SET is_active=:is_active WHERE id=:id ";
	try {
		QSqlStatement query(getSysConnect(), sql.c_str());
		query.bind(L":is_active", isActive);
		query.bind(L":id", id);

		query.exec();
		Q_INFO(L"updateIsActiveById in user_db success.");
		return true;
	}
	catch (SQLite::QSqlException &e) {
		std::wstring _err = e.getErrorStr();
		Q_ERROR(L"exec sql has error, code:{}, msg:{}, sql:{}", e.getErrorCode(), _err, sql);
		throw QRuntimeException(L"10025", L"sorry, system has error.");
	}
}

bool UserDbRepository::updateIsActiveByNotId(uint64_t notId, int isActive)
{
	if (notId <= 0) {
		return false;
	}
	//sql
	std::wstring sql = L"UPDATE user_db SET is_active=:is_active WHERE id<>:id ";
	try {
		QSqlStatement query(getSysConnect(), sql.c_str());
		query.bind(L":is_active", isActive);
		query.bind(L":id", notId);

		query.exec();
		Q_INFO(L"updateIsActiveByNotId in user_db success.");
		return true;
	} catch (SQLite::QSqlException &e) {
		std::wstring _err = e.getErrorStr();
		Q_ERROR(L"exec sql has error, code:{}, msg:{}, sql:{}", e.getErrorCode(), _err, sql);
		throw QRuntimeException(L"10026", L"sorry, system has error.");
	}
}

/**
* bind the fields
*
* @param query
* @param item
*/
void UserDbRepository::queryBind(QSqlStatement &query, UserDb &item, bool isUpdate)
{
	if (isUpdate) {
		query.bind(L":id", item.id);
	}
	query.bind(L":name", item.name);
	query.bind(L":path", item.path);
	query.bind(L":is_active", item.isActive);
	query.bind(L":created_at", item.createdAt);
	query.bind(L":updated_at", item.updatedAt);
}

/**
 * convert to entity.
 * 
 * @param query
 * @return 
 */
UserDb UserDbRepository::toUserDb(QSqlStatement &query)
{
	UserDb item;
	item.id = query.getColumn(L"id").getInt64();
	item.name = query.getColumn(L"name").getText();
	item.path = query.getColumn(L"path").getText();
	item.isActive = query.getColumn(L"is_active").getInt();
	item.createdAt = query.getColumn(L"created_at").getText();
	item.updatedAt = query.getColumn(L"updated_at").getText();
	return item;
}
