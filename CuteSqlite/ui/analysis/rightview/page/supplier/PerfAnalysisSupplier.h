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

 * @file   PerAnalysisSupplier.h
 * @brief  Store the runtime data for PerAnalysisPage,
 *         Every one instance of PerAnalysisPage have only one instance of PerAnalysisSupplier
 * 
 * @author Xuehan Qin
 * @date   2023-12-07
 *********************************************************************/
#pragma once
#include "ui/database/rightview/common/QPageSupplier.h"
class PerfAnalysisSupplier : public QPageSupplier {
public:
	

	PerfAnalysisSupplier() {};
	~PerfAnalysisSupplier() {};

	uint64_t getSqlLogId() const { return sqlLogId; }
	void setSqlLogId(uint64_t val) { sqlLogId = val; }

	const SqlLog & getSqlLog() { return sqlLog; }
	void setSqlLog(const SqlLog & val) { sqlLog = val; }

	const DataList & getRuntimeExplainDataList()  { return runtimeExplainDataList; }
	void setRuntimeExplainDataList(const DataList & val) { runtimeExplainDataList = val; }

	const TableIndexAnalysisVector & getTableIndexAnalysisVector() { return tableIndexAnalysisVector; }
	void setTableIndexAnalysisVector(const TableIndexAnalysisVector & val) { tableIndexAnalysisVector = val; }
	void addTableIndexAnalysis(TableIndexAnalysis & item); 
private:
	uint64_t sqlLogId = 0;
	SqlLog sqlLog;
	TableIndexAnalysisVector tableIndexAnalysisVector;

	DataList runtimeExplainDataList;
};

