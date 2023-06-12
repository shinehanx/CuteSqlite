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

 * @file   DatabaseService.cpp
 * @brief  The Sqlite Databases management
 * 
 * @author Xuehan Qin
 * @date   2023-05-19
 *********************************************************************/
#include "stdafx.h"
#include "DatabaseService.h"
#include "utils/DateUtil.h"
#include "utils/Log.h"
#include "core/common/exception/QRuntimeException.h"

DatabaseService::DatabaseService()
{

}


DatabaseService::~DatabaseService()
{

}

UserDbList DatabaseService::getAllUserDbs()
{
	return getRepository()->getAll();
}

uint64_t DatabaseService::hasUserDb(std::wstring & dbPath)
{
	ATLASSERT(!dbPath.empty());
	UserDb userDb = getRepository()->getByPath(dbPath);
	return userDb.id;
}


bool DatabaseService::hasUserDb(uint64_t usersDbId)
{
	ATLASSERT(usersDbId > 0);
	UserDb userDb = getRepository()->getById(usersDbId);
	return userDb.id > 0;
}

uint64_t DatabaseService::createUserDb(std::wstring & dbPath)
{
	ATLASSERT(!dbPath.empty());
	UserDb userDb;
	userDb.name = FileUtil::getFileName(dbPath, false);
	userDb.path = dbPath;
	userDb.isActive = 1;
	userDb.createdAt = DateUtil::getCurrentDateTime();
	userDb.updatedAt = DateUtil::getCurrentDateTime();
	uint64_t userDbId = getRepository()->create(userDb);
	if (!userDbId) {
		Q_ERROR(L"getRepository()->create has error, return userDbId=0");
		throw QRuntimeException(L"11001", L"sorry, system has error.");
	}
	getRepository()->updateIsActiveByNotId(userDbId);
	
	databaseUserRepository->create(userDbId, dbPath);
	return userDbId;
}


uint64_t DatabaseService::openUserDb(std::wstring & dbPath)
{
	ATLASSERT(!dbPath.empty());
	UserDb userDb;
	userDb.name = FileUtil::getFileName(dbPath, false);
	userDb.path = dbPath;
	userDb.isActive = 1;
	userDb.createdAt = DateUtil::getCurrentDateTime();
	userDb.updatedAt = DateUtil::getCurrentDateTime();
	uint64_t userDbId = getRepository()->create(userDb);
	if (!userDbId) {
		Q_ERROR(L"getRepository()->create has error, return userDbId=0");
		throw QRuntimeException(L"11002", L"sorry, system has error.");
	}
	getRepository()->updateIsActiveByNotId(userDbId);
	return userDbId;
}

/**
 * remove user db , and close the connect.
 * 
 * @param userDbId
 * @param isDeleteFile
 */
void DatabaseService::removeUserDb(uint64_t userDbId, bool isDeleteFile)
{
	ATLASSERT(userDbId > 0);
	// 1.Judge the user db exists.if exists, remove the db file first.
	if (isDeleteFile) {
		UserDb userDb = getRepository()->getById(userDbId);
		if (!userDb.id || userDb.path.empty()) {
			return;
		}
		Q_WARN(L"Delete sqlite db file, id:{},path:{}", userDbId, userDb.path);
		::DeleteFile(userDb.path.c_str());
	}
	// 2) Remove from system db.
	getRepository()->remove(userDbId);

	// 3) Close from connects.
	databaseUserRepository->closeUserConnect(userDbId);
}


bool DatabaseService::activeUserDb(uint64_t userDbId)
{
	ATLASSERT(userDbId > 0);
	getRepository()->updateIsActiveById(userDbId, 1);
	getRepository()->updateIsActiveByNotId(userDbId,0);
	return true;
}

UserTableList DatabaseService::getUserTables(uint64_t userDbId)
{
	ATLASSERT(userDbId > 0);
	return tableUserRepository->getListByUserDbId(userDbId);
}

UserTable DatabaseService::getUserTable(uint64_t userDbId, std::wstring & tblName)
{
	ATLASSERT(userDbId > 0 && !tblName.empty());
	return tableUserRepository->getTable(userDbId, tblName);
}

UserTableStrings DatabaseService::getUserTableStrings(uint64_t userDbId)
{
	UserTableStrings result;
	UserTableList userTableList = tableUserRepository->getListByUserDbId(userDbId);
	for (auto userTable : userTableList) {
		result.push_back(userTable.name);
	}
	return result;
}

UserViewList DatabaseService::getUserViews(uint64_t userDbId)
{
	return viewUserRepository->getListByUserDbId(userDbId);
}

UserTriggerList DatabaseService::getUserTriggers(uint64_t userDbId)
{
	return triggerUserRepository->getListByUserDbId(userDbId);
}

UserColumnList DatabaseService::getUserColumns(uint64_t userDbId, std::wstring & tblName)
{
	return fieldUserRepository->getListByTblName(userDbId, tblName);
}

UserIndexList DatabaseService::getUserIndexes(uint64_t userDbId, std::wstring & tblName)
{
	return indexUserRepository->getListByTblName(userDbId, tblName);
}
