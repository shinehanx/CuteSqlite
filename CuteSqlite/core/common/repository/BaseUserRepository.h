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
#include "QConnect.h"

template <typename T>
class BaseUserRepository : public BaseRepository<T>
{
public:
	QSqlDatabase * getUserConnect(uint64_t userDbId);
	void closeUserConnect(uint64_t userDbId);
protected:
	
	UserDb getUserDbById(uint64_t userDbId);
	std::wstring initUserDbFile(uint64_t userDbId);
	UserDb toUserDb(QSqlStatement &query);
	
	RowItem toRowItem(QSqlStatement &query);
};

template <typename T>
RowItem BaseUserRepository<T>::toRowItem(QSqlStatement &query)
{
	RowItem rowItem;
	int columnCount = query.getColumnCount();
	for (int i = 0; i < columnCount; i++) {
		std::wstring val = query.getColumn(i).isNull() ? L"< NULL >" : query.getColumn(i).getText();
		rowItem.push_back(val);
	}
	return rowItem;
}

/**
* Read dbName and dbPath from the table CuteSqlite.user_db
*
* @param userDbId The param of CuteSqlite.user_db.id
* @return QSqlDatabase pointer
*/
template <typename T>
QSqlDatabase * BaseUserRepository<T>::getUserConnect(uint64_t userDbId)
{
	if (QConnect::userConnectPool.empty() || QConnect::userConnectPool.find(userDbId) == QConnect::userConnectPool.end()) {
		std::wstring dbName = L"sqlite3_user_db_";
		dbName.append(std::to_wstring(userDbId));
		QConnect::userConnectPool[userDbId] = new QSqlDatabase(dbName);
	}

	Q_INFO(L"BaseUserRepository::getConnect(dbId), dbId:{} connect...", userDbId);
	Q_INFO(L"BaseUserRepository::getConnect(dbId), dbId:{}, connect.isValid():{}, connect.isOpen():{}",
		userDbId, QConnect::userConnectPool[userDbId]->isValid(), QConnect::userConnectPool[userDbId]->isOpen());

	if (!QConnect::userConnectPool[userDbId]->isValid() || !QConnect::userConnectPool[userDbId]->isOpen()) {
		std::wstring dbPath = initUserDbFile(userDbId);
		Q_INFO(L"db path:{}", dbPath.c_str());
		QConnect::userConnectPool[userDbId]->setDatabaseName(dbPath);

		if (!QConnect::userConnectPool[userDbId]->open()) {
			Q_INFO(L"db connect.open Error:{}", QConnect::userConnectPool[userDbId]->lastError());
			setErrorMsg(QConnect::userConnectPool[userDbId]->lastError());
			ATLASSERT(QConnect::userConnectPool[userDbId]->isValid() && QConnect::userConnectPool[userDbId]->isOpen());
		}
	}

	return QConnect::userConnectPool[userDbId];
}

template <typename T>
void BaseUserRepository<T>::closeUserConnect(uint64_t userDbId)
{
	if (QConnect::userConnectPool.empty() || QConnect::userConnectPool.find(userDbId) == QConnect::userConnectPool.end()) {
		return;
	}
	
	QSqlDatabase * tmpConnect = QConnect::userConnectPool.at(userDbId);	
	if (tmpConnect) {
		// 1) close the connect
		if (tmpConnect->isValid() && tmpConnect->isOpen()) {
			tmpConnect->close();
		}
		// 2) delete the ptr
		delete tmpConnect;
		tmpConnect = nullptr;
	}
	// 3) erase from userConnectPool (map)
	QConnect::userConnectPool.erase(userDbId);
}

template <typename T>
UserDb BaseUserRepository<T>::getUserDbById(uint64_t userDbId)
{
	std::wstring sql = L"SELECT * FROM user_db WHERE id=:id";

	try {
		QSqlStatement query(getSysConnect(), sql.c_str());
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
		throw QRuntimeException(L"10023", L"sorry, user db init error, userDbId:" + std::to_wstring(userDbId));
		return L"";
	}
	std::wstring userDbDir = FileUtil::getFileDir(userDb.path);
	ATLASSERT(!userDbDir.empty());

	std::wstring destPath = userDb.path;
	if (_waccess(destPath.c_str(), 0) != 0) { //文件不存在
		localDir = localDir = ResourceUtil::getProductBinDir();
		std::wstring origPath = localDir + L"res\\db\\UserDb.s3db";
		ATLASSERT(_waccess(origPath.c_str(), 0) == 0);

		FileUtil::copy(origPath, destPath);
	}

	return destPath;
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