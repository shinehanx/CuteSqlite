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

 * @file   SqlLogRepository.h
 * @brief  Store the sql log 
 * 
 * @author Xuehan Qin
 * @date   2023-11-15
 *********************************************************************/
#pragma once
#include <string>
#include "core/entity/Entity.h"
#include "core/common/repository/BaseRepository.h"

#define LIMIT_MAX 5000
class SqlLogRepository : public BaseRepository<SqlLogRepository> {
public:
	SqlLogRepository() {};
	~SqlLogRepository() {};

	uint64_t create(SqlLog & item);
	int remove(uint64_t id);
	SqlLog getById(uint64_t id);
	SqlLogList  getAll(uint64_t limit = LIMIT_MAX);
	SqlLogList  getTopList(uint64_t limit = LIMIT_MAX);
	uint64_t  getCount();
	std::vector<uint64_t>  getFrontIds(uint64_t limit = LIMIT_MAX);
	int removeByBiggerId(uint64_t id);
	int topById(uint64_t id, int topVal);

	SqlLogList getPage(int page, int perPage);
	SqlLogList getTopByKeyword(const std::wstring & keyword);
	SqlLogList getPageByKeyword(const std::wstring & keyword, int page, int perPage);
private:
	void queryBind(QSqlStatement &query, SqlLog &item, bool isUpdate = false);
	SqlLog toSqlLog(QSqlStatement &query);

};
