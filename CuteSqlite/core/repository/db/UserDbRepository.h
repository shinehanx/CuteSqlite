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

 * @file   DbRepository.h
 * @brief  The table db repository
 * 
 * @author Xuehan Qin
 * @date   2023-05-19
 *********************************************************************/
#pragma once
#include <string>
#include "core/entity/Entity.h"
#include "core/common/repository/BaseRepository.h"

class UserDbRepository : public BaseRepository<UserDbRepository>
{
public:
	UserDbRepository() {};
	~UserDbRepository() {};

	uint64_t create(UserDb & item);
	bool remove(uint64_t id);
	UserDb getById(uint64_t id);
	UserDb getByPath(std::wstring & path);
	UserDbList  getAll();
	bool updateIsActiveById(uint64_t id, int isActive=1);
	bool updateIsActiveByNotId(uint64_t notId, int isActive = 0);
private:
	void queryBind(QSqlStatement &query, UserDb &item, bool isUpdate = false);
	UserDb toUserDb(QSqlStatement &query);
};
