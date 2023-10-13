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

 * @file   DatabaseService.h
 * @brief  The Sqlite Databases management class
 * 
 * @author Xuehan Qin
 * @date   2023-05-19
 *********************************************************************/
#pragma once
#include "core/common/service/BaseService.h"
#include "core/repository/db/UserDbRepository.h"
#include "core/repository/user/DatabaseUserRepository.h"
#include "core/repository/user/TableUserRepository.h"
#include "core/repository/user/ViewUserRepository.h"
#include "core/repository/user/TriggerUserRepository.h"
#include "core/repository/user/ColumnUserRepository.h"
#include "core/repository/user/IndexUserRepository.h"

class DatabaseService : public BaseService<DatabaseService, UserDbRepository>
{
public:
	DatabaseService();
	~DatabaseService();

	// user db operations
	UserDbList getAllUserDbs();
	uint64_t hasUserDb(std::wstring & dbPath);
	bool hasUserDb(uint64_t userDbId);
	UserDb getUserDb(uint64_t userDbId);
	uint64_t createUserDb(std::wstring & dbPath);
	uint64_t openUserDb(std::wstring & dbPath);
	void removeUserDb(uint64_t userDbId, bool isRealDelete=false);
	bool activeUserDb(uint64_t userDbId);

	// user table operations
	UserTableList getUserTables(uint64_t userDbId);
	UserTable getUserTable(uint64_t userDbId, const std::wstring & tblName);

	// user table operations
	UserTableStrings getUserTableStrings(uint64_t userDbId);

	// user views operations
	UserViewList getUserViews(uint64_t userDbId);
	UserView getUserView(uint64_t userDbId, const std::wstring & viewName);

	// user triggers operations
	UserTriggerList getUserTriggers(uint64_t userDbId);
	UserTrigger getUserTrigger(uint64_t userDbId, const std::wstring & triggerName);

	// user indexes operations
	ColumnInfoList getUserColumns(uint64_t userDbId, const std::wstring & tblName);

	// user indexes operations
	UserIndexList getUserIndexes(uint64_t userDbId, const std::wstring & tblName);
	
	// get runtime user unique or index columns
	std::wstring getPrimaryKeyColumn(uint64_t userDbId, const std::wstring & tblName, Columns & columns);

private:
	DatabaseUserRepository * databaseUserRepository = DatabaseUserRepository::getInstance();
	TableUserRepository * tableUserRepository = TableUserRepository::getInstance();
	ViewUserRepository * viewUserRepository = ViewUserRepository::getInstance();
	TriggerUserRepository * triggerUserRepository = TriggerUserRepository::getInstance();
	ColumnUserRepository * columnUserRepository = ColumnUserRepository::getInstance();
	IndexUserRepository * indexUserRepository = IndexUserRepository::getInstance();
};
