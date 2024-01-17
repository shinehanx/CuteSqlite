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

 * @file   DbstatUserRepository.h
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2024-01-16
 *********************************************************************/
#pragma once
#include "core/common/repository/BaseUserRepository.h"
#include "core/entity/Entity.h"

class DbStatUserRepository : public BaseUserRepository<DbStatUserRepository> {
public:
	DbStatUserRepository() {};
	~DbStatUserRepository() {};

	int getPageSize(uint64_t userDbId);
	uint64_t getPageCount(uint64_t userDbId);
	uint64_t getAutovacuum(uint64_t userDbId);
	uint64_t getFreePageCount(uint64_t userDbId);
	TblIdxSpaceUsed getTblIdxSpaceUsedByName(uint64_t userDbId, const std::wstring & name);
private:
	TblIdxSpaceUsed toTblIdxSpaceUsed(QSqlStatement & query);
};
