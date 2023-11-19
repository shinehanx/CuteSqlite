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

 * @file   SqlLogService.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-11-15
 *********************************************************************/
#include "stdafx.h"
#include "SqlLogService.h"

uint64_t SqlLogService::createSqlLog(SqlLog & sqlLog)
{
	return getRepository()->create(sqlLog);
}

void SqlLogService::clearOldSqlLog()
{
	uint64_t totalNums = getRepository()->getCount();
	if (totalNums < LIMIT_MAX) { // limit max 1000 records for sql log
		return;
	}

	auto ids = getRepository()->getFrontIds(LIMIT_MAX);
	if (ids.empty()) {
		return;
	}
	auto maxId = ids.back();
	getRepository()->removeByBiggerId(maxId);
}

std::vector<std::wstring> SqlLogService::getDatesFromList(const SqlLogList &list)
{
	std::vector<std::wstring> result;
	if (list.empty()) {
		return result;
	}

	for (auto & item : list) {
		std::wstring date = DateUtil::getDateFromDateTime(item.createdAt);
		if (std::find(result.begin(), result.end(), date) == result.end()) {
			result.push_back(date);
		}
	}
	return result;
}

SqlLogList SqlLogService::getFilteredListByDate(const SqlLogList &list, const std::wstring & date)
{
	SqlLogList result;
	if (list.empty() || date.empty()) {
		return result;
	}
	for (auto & item : list) {
		if (date == DateUtil::getDateFromDateTime(item.createdAt)) {
			result.push_back(item);
		}
	}
	return result;
}

void SqlLogService::topSqlLog(uint64_t id, int topVal)
{
	getRepository()->topById(id, topVal);
}

void SqlLogService::removeSqlLog(uint64_t id)
{
	getRepository()->remove(id);
}


SqlLogList SqlLogService::getAllSqlLog()
{
	return getRepository()->getAll();
}


SqlLogList SqlLogService::getTopSqlLog()
{
	return getRepository()->getTopList();
}

SqlLogList SqlLogService::getPageSqlLog(int page, int perPage)
{
	return getRepository()->getPage(page, perPage);
}

SqlLogList SqlLogService::getTopSqlLogByKeyword(const std::wstring & keyword)
{
	return getRepository()->getTopByKeyword(keyword);
}

SqlLogList SqlLogService::getPageSqlLogByKeyword(const std::wstring & keyword, int page, int perPage)
{
	return getRepository()->getPageByKeyword(keyword, page, perPage);
}


