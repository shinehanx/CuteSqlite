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

 * @file   PerfAnalysisReportRepository.h
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
class PerfAnalysisReportRepository : public BaseRepository<PerfAnalysisReportRepository> {
public:
	PerfAnalysisReportRepository() {};
	~PerfAnalysisReportRepository() {};

	uint64_t create(PerfAnalysisReport & item);
	int remove(uint64_t id);
	bool removeBySqlLogId(uint64_t sqlLogId);
	PerfAnalysisReport getById(uint64_t id);
	PerfAnalysisReport getBySqlLogId(uint64_t sqlLogId);
	PerfAnalysisReportList getAll(uint64_t limit = LIMIT_MAX);
	uint64_t  getCount();
	
private:
	void queryBind(QSqlStatement &query, PerfAnalysisReport &item, bool isUpdate = false);
	PerfAnalysisReport toPerfAnalysisReport(QSqlStatement &query);
};
