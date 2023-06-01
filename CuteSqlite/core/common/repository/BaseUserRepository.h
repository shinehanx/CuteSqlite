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

 * @file   BaseUserRepository.h
 * @brief  BaseUserRepository is a repository class of user database table handle 
 * 
 * @author Xuehan Qin
 * @date   2023-05-20
 *********************************************************************/
#pragma once
#include "BaseRepository.h"
#include "core/entity/Entity.h"
#include "utils/FileUtil.h"

template <typename T>
class BaseUserRepository : public BaseRepository<T>
{
public:
	QSqlDatabase * getUserConnect(uint64_t userDbId);
	void closeUserConnect(uint64_t userDbId);
private:
	static std::unordered_map<uint64_t, QSqlDatabase *> connects; //singleton
	UserDb getUserDbById(uint64_t userDbId);
	std::wstring initUserDbFile(uint64_t userDbId);
	UserDb toUserDb(QSqlStatement &query);
};



template <typename T>
std::unordered_map<uint64_t, QSqlDatabase *> BaseUserRepository<T>::connects;

/**
* Read dbName and dbPath from the table CuteSqlite.user_db
*
* @param userDbId The param of CuteSqlite.user_db.id
* @return QSqlDatabase pointer
*/
template <typename T>
QSqlDatabase * BaseUserRepository<T>::getUserConnect(uint64_t userDbId)
{
	if (connects.empty() || connects.find(userDbId) == connects.end()) {
		std::wstring dbName = L"sqlite3_user_db_";
		dbName.append(std::to_wstring(userDbId));
		connects[userDbId] = new QSqlDatabase(dbName);
	}

	Q_INFO(L"BaseUserRepository::getConnect(dbId), dbId:{} connect...", userDbId);
	Q_INFO(L"BaseUserRepository::getConnect(dbId), dbId:{}, connect.isValid():{}, connect.isOpen():{}",
		userDbId, connects[userDbId]->isValid(), connects[userDbId]->isOpen());

	if (!connects[userDbId]->isValid() || !connects[userDbId]->isOpen()) {
		std::wstring dbPath = initUserDbFile(userDbId);
		Q_INFO(L"db path:{}", dbPath.c_str());
		connects[userDbId]->setDatabaseName(dbPath);

		if (!connects[userDbId]->open()) {
			Q_INFO(L"db connect.open Error:{}", connects[userDbId]->lastError());
			setErrorMsg(connects[userDbId]->lastError());
			ATLASSERT(connects[userDbId]->isValid() && connects[userDbId]->isOpen());
		}
	}

	return connects[userDbId];
}

template <typename T>
void BaseUserRepository<T>::closeUserConnect(uint64_t userDbId)
{
	if (connects.empty() || connects.find(userDbId) == connects.end()) {
		return;
	}
	
	QSqlDatabase * tmpConnect = connects.at(userDbId);	
	if (tmpConnect) {
		// 1) close the connect
		if (tmpConnect->isValid() && tmpConnect->isOpen()) {
			tmpConnect->close();
		}
		// 2) delete the ptr
		delete tmpConnect;
	}
	// 3) erase from map
	connects.erase(userDbId);
}

template <typename T>
UserDb BaseUserRepository<T>::getUserDbById(uint64_t userDbId)
{
	std::wstring sql = L"SELECT * FROM user_db WHERE id=:id";

	try {
		QSqlStatement query(getConnect(), sql.c_str());
		query.bind(L":id", userDbId);

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
		throw QRuntimeException(L"10022", L"sorry, system has error.");
	}
}

template <typename T>
std::wstring BaseUserRepository<T>::initUserDbFile(uint64_t userDbId)
{
	UserDb userDb = getUserDbById(userDbId);
	if (userDb.id == 0) {
		throw QRuntimeException(L"10023", L"sorry, user db init error, userDbId:" + userDbId);
		return L"";
	}
	std::wstring userDbDir = FileUtil::getFileDir(userDb.path);
	ATLASSERT(!userDbDir.empty());
	
	char * ansiDbDir = StringUtil::unicodeToUtf8(userDbDir);
	if (_waccess(userDbDir.c_str(), 0) != 0) { //文件目录不存在
		Q_INFO(L"mkpath:{}", userDbDir);
		//创建DB目录，子目录不存在，则创建
		FileUtil::createDirectory(ansiDbDir);
	}
	free(ansiDbDir);


	std::wstring path = userDb.path;
	char * ansiPath = StringUtil::unicodeToUtf8(path);
	if (_access(ansiPath, 0) != 0) { //文件不存在
		std::wstring origPath = localDir + L"res\\db\\UserDb.s3db";
		char * ansiOrigPath = StringUtil::unicodeToUtf8(origPath.c_str());
		ATLASSERT(_access(ansiOrigPath, 0) == 0);

		errno_t _err;
		char _err_buf[80] = { 0 };
		FILE * origFile, *destFile;
		_err = fopen_s(&origFile, ansiOrigPath, "rb"); //原文件
		if (_err != 0 || origFile == NULL) {
			_strerror_s(_err_buf, 80, NULL);
			std::wstring _err_msg = StringUtil::utf82Unicode(_err_buf);
			Q_ERROR(L"orgin db file open error,error:{},path:{}", _err_msg, origPath);
			ATLASSERT(_err == 0);
		}
		_err = fopen_s(&destFile, ansiPath, "wb"); //目标文件
		if (_err != 0 || destFile == NULL) {
			_strerror_s(_err_buf, 80, NULL);
			std::wstring _err_msg = StringUtil::utf82Unicode(_err_buf);
			Q_ERROR(L"dest db file open error,error:{},path:{}", _err_msg, path);
			ATLASSERT(_err == 0);
		}
		char ch = fgetc(origFile);
		while (!feof(origFile)) {
			_err = fputc(ch, destFile);
			ch = fgetc(origFile);
		}

		fclose(destFile);
		fclose(origFile);
		free(ansiOrigPath);
	}

	free(ansiPath);
	return path;
}

template <typename T>
UserDb BaseUserRepository<T>::toUserDb(QSqlStatement &query)
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