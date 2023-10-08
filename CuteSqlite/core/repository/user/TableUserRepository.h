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

 * @file   TableUserRepository.h
 * @brief  Operations on tables in the user database
 * 
 * @author Xuehan Qin
 * @date   2023-05-20
 *********************************************************************/
#pragma once
#include "core/common/repository/BaseUserRepository.h"
#include "core/entity/Entity.h"

class TableUserRepository : public BaseUserRepository<TableUserRepository>
{
public:
	TableUserRepository() {}
	~TableUserRepository() {}

	UserTableList getListByUserDbId(uint64_t userDbId);
	UserTable getTable(uint64_t userDbId, std::wstring & tblName);

	uint64_t getDataCount(uint64_t userDbId, std::wstring & tblName);
	DataList getPageDataList(uint64_t userDbId, std::wstring & tblName, int page, int perpage);
private:
	
	UserTable toUserTable(QSqlStatement &query);
	RowItem toRowItem(QSqlStatement &query);
};
