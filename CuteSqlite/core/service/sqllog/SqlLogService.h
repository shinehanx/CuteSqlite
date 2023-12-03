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

 * @file   SqlLogService.h
 * @brief  Store the sql log for executed in QueryPage
 * 
 * @author Xuehan Qin
 * @date   2023-11-15
 *********************************************************************/
#pragma once

#include "core/entity/Entity.h"
#include "core/common/service/BaseService.h"
#include "core/repository/sqllog/SqlLogRepository.h"

class SqlLogService : public BaseService<SqlLogService, SqlLogRepository>
{
public:
	SqlLogService() {};
	~SqlLogService() {};

	uint64_t createSqlLog(SqlLog & sqlLog);
	void clearOldSqlLog();
	std::vector<std::wstring> getDatesFromList(const SqlLogList &list);

	SqlLogList getFilteredListByDate(const SqlLogList &list, const std::wstring & date);

	void topSqlLog(uint64_t id, int topVal);
	void removeSqlLog(uint64_t id);

	SqlLogList getAllSqlLog();
	uint64_t getSqlLogCount();
	SqlLogList getTopSqlLog();
	SqlLogList getPageSqlLog(int page, int perPage);
	SqlLogList getTopSqlLogByKeyword(const std::wstring & keyword);
	SqlLogList getPageSqlLogByKeyword(const std::wstring & keyword, int page, int perPage);
	uint64_t getSqlLogCountByKeyword(const std::wstring & keyword);
	SqlLog getSqlLog(uint64_t sqlLogId);
};

