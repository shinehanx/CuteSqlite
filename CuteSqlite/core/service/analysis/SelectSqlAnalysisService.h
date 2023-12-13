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

 * @file   SelectSqlAnalysisService.h
 * @brief  Analysis sql service for select sql statement.
 * 
 * @author Xuehan Qin
 * @date   2023-12-07
 *********************************************************************/
#pragma once
#include "core/common/service/BaseService.h"
#include "core/repository/user/SqlExecutorUserRepository.h"
#include "core/repository/user/TableUserRepository.h"
#include "core/repository/user/IndexUserRepository.h"
#include "core/repository/user/ColumnUserRepository.h"
#include "core/common/repository/QSqlStatement.h"

class SelectSqlAnalysisService : public BaseService<SelectSqlAnalysisService, SqlExecutorUserRepository>
{
public:
	SelectSqlAnalysisService() {};
	~SelectSqlAnalysisService() {};

	DataList explainSql(uint64_t userDbId, const std::wstring & sql);
	ExplainQueryPlans explainQueryPlanSql(uint64_t userDbId, const std::wstring & sql);
	ByteCodeResults explainReadByteCodeToResults(uint64_t userDbId, const DataList & byteCodeList, const std::wstring &sql);

private:
	TableUserRepository * tableUserRepository = TableUserRepository::getInstance();
	IndexUserRepository * indexUserRepository = IndexUserRepository::getInstance();
	ColumnUserRepository * columnUserRepository = ColumnUserRepository::getInstance();


	void parseTblOrIdxFromOpenRead(uint64_t userDbId, const RowItem &rowItem, ByteCodeResults &results);
	void parseIdxColumnsFromExplainRow(uint64_t userDbId, const RowItem &rowItem, ByteCodeResults &results);
	void parseTblOrIdxColumnFromOpColumn(uint64_t userDbId, const RowItem &rowItem, ByteCodeResults &results);

	void parseTblOrIdxColumnFromOpCompare(uint64_t userDbId, const RowItem& rowItem, ByteCodeResults & results, DataList::const_iterator iter, const DataList & byteCodeList);
	void parseTblOrIdxColumnFromSeekRowid(uint64_t userDbId, const RowItem& rowItem, ByteCodeResults &results, DataList::const_iterator iter, const DataList & byteCodeList);

	Columns getUserColumnStrings(uint64_t userDbId, const std::wstring & tblName, const std::wstring & schema = std::wstring());
	std::wstring getPrimaryKeyColumn(uint64_t userDbId, const std::wstring & tblName, Columns & columns, const std::wstring & schema = std::wstring());

	void parseTblOrIdxNullColumnFromSql(uint64_t userDbId, const RowItem & rowItem, ByteCodeResults & results, const std::wstring & sql);
	UserTableStrings getUserTableStrings(uint64_t userDbId, const std::wstring & schema = std::wstring());
	std::vector<std::tuple<std::wstring, std::wstring, std::wstring>> parseWhereNullColumnsFromSelectSqlUpWords(std::vector<std::wstring>& words, const std::vector<std::pair<std::wstring, std::wstring>> & allAliases);
};
