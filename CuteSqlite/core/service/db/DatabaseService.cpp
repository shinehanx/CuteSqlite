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
#include "utils/SqlUtil.h"
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


bool DatabaseService::hasUserDb(uint64_t userDbId)
{
	ATLASSERT(userDbId > 0);
	UserDb userDb = getRepository()->getById(userDbId);
	return userDb.id > 0;
}

UserDb DatabaseService::getUserDb(uint64_t userDbId)
{
	ATLASSERT(userDbId > 0);
	return getRepository()->getById(userDbId);
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

/**
 * Copy database from fromUserDbId to toDbPath.
 * 
 * @param fromUserDbId - source database dbId
 * @param toDbPath - target database path
 * @return target database db id 
 */
uint64_t DatabaseService::copyUserDb(uint64_t fromUserDbId, const std::wstring & toDbPath)
{
	ATLASSERT(!toDbPath.empty());
	UserDb fromUserDb = getRepository()->getById(fromUserDbId);
	if (!fromUserDb.id) {
		Q_ERROR(L"From user db not exists, fromUserDbId={}", fromUserDbId);
		throw QRuntimeException(L"11003", L"sorry, system has error.");
	}

	UserDb userDb;
	userDb.name = FileUtil::getFileName(toDbPath, false);
	userDb.path = toDbPath;
	userDb.isActive = 1;
	userDb.createdAt = DateUtil::getCurrentDateTime();
	userDb.updatedAt = DateUtil::getCurrentDateTime();
	uint64_t toUserDbId = getRepository()->create(userDb);
	if (!toUserDbId) {
		Q_ERROR(L"getRepository()->create has error, return userDbId=0");
		throw QRuntimeException(L"11002", L"sorry, system has error.");
	}
	getRepository()->updateIsActiveByNotId(toUserDbId);
	
	// close the database connection first, release the FILE handler
	databaseUserRepository->closeUserConnect(fromUserDbId);
	databaseUserRepository->copy(fromUserDb.path, toDbPath);
	return toUserDbId;
}


UserViewList DatabaseService::getUserViews(uint64_t userDbId, const std::wstring & schema)
{
	ATLASSERT(userDbId > 0);
	return viewUserRepository->getListByUserDbId(userDbId, schema);
}

UserView DatabaseService::getUserView(uint64_t userDbId, const std::wstring & viewName, const std::wstring & schema)
{
	ATLASSERT(userDbId > 0 && !viewName.empty());
	return viewUserRepository->getView(userDbId, viewName, schema);
}

UserTriggerList DatabaseService::getUserTriggers(uint64_t userDbId, const std::wstring & schema)
{
	ATLASSERT(userDbId > 0);
	return triggerUserRepository->getListByUserDbId(userDbId, schema);
}

UserTrigger DatabaseService::getUserTrigger(uint64_t userDbId, const std::wstring & triggerName, const std::wstring & schema)
{
	ATLASSERT(userDbId > 0 && !triggerName.empty());
	return triggerUserRepository->getTrigger(userDbId, triggerName, schema);
}

Functions DatabaseService::getFunctionsStrings(uint64_t userDbId)
{
	ATLASSERT(userDbId > 0);
	return databaseUserRepository->getFunctions(userDbId);
}
