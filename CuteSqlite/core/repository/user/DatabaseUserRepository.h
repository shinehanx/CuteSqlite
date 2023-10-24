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

 * @file   DatabaseUserRepository.h
 * @brief  DatabaseUserRepository is a user repository class to handle user database
 * 
 * @author Xuehan Qin
 * @date   2023-05-20
 *********************************************************************/
#pragma once
#include <string>
#include "core/common/repository/BaseUserRepository.h"

class DatabaseUserRepository : public BaseUserRepository<DatabaseUserRepository>
{
public:
	DatabaseUserRepository() {}
	~DatabaseUserRepository() {}

	void create(uint64_t userDbId, std::wstring & userDbPath);
	void copy(const std::wstring & fromDbPath, const std::wstring & toDbPath);
};
