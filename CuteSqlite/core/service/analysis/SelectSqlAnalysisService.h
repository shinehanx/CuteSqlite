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
#include "core/repository/report/PerfAnalysisReportRepository.h"
#include "core/common/repository/QSqlStatement.h"

class SelectSqlAnalysisService : public BaseService<SelectSqlAnalysisService, SqlExecutorUserRepository>
{
public:
	SelectSqlAnalysisService() {};
	~SelectSqlAnalysisService() {};

	DataList explainSql(uint64_t userDbId, const std::wstring & sql);
	ExplainQueryPlans explainQueryPlanSql(uint64_t userDbId, const std::wstring & sql);
	ByteCodeResults explainReadByteCodeToResults(uint64_t userDbId, const DataList & byteCodeList, const std::wstring &sql);
	SelectColumns explainReadByteCodeToSelectColumns(uint64_t userDbId, const DataList & byteCodeList, const std::wstring &sql);

	
	uint64_t savePerfAnalysisReport(uint64_t userDbId, uint64_t sqlLogId);
	PerfAnalysisReport getPerfAnalysisReport(uint64_t sqlLogId);
	void dropPerfAnalysisReport(uint64_t sqlLogId);
	PerfAnalysisReportList getPerfAnalysisReportList();
private:
	
	TableUserRepository * tableUserRepository = TableUserRepository::getInstance();
	IndexUserRepository * indexUserRepository = IndexUserRepository::getInstance();
	ColumnUserRepository * columnUserRepository = ColumnUserRepository::getInstance();
	PerfAnalysisReportRepository * perfAnalysisReportRepository = PerfAnalysisReportRepository::getInstance();

	void doConvertByteCodeForWhereColumns(uint64_t userDbId, const DataList &byteCodeList, const std::wstring & sql, ByteCodeResults & results);
	void doConvertByteCodeForOrderColumns(uint64_t userDbId, const DataList &byteCodeList, const std::wstring & sql, ByteCodeResults & results);
	void doMergeColumnsToResults(uint64_t userDbId, ByteCodeResults & results);
	void doConvertByteCodeForSelectColumns(uint64_t userDbId, const DataList &byteCodeList, const std::wstring & sql, SelectColumns & selectColumns);

	bool parseOrderOrIndexColumnFromLastAndPrev(uint64_t userDbId, const DataList &byteCodeList, const std::wstring & sql, ByteCodeResults &results);
	void parseOrderOrIndexColumnFromIdxInsertAndSort(uint64_t userDbId, const DataList &byteCodeList, const std::wstring & sql, ByteCodeResults &results);
	void parseOrderOrIndexColumnFromSorter(uint64_t userDbId, const DataList &byteCodeList, const std::wstring & sql, ByteCodeResults &results);

	void parseOrderColumnsBySubSelectClause(uint64_t userDbId, const std::wstring & sql,  ByteCodeResults & results);

	void parseOrderOrIndexColumnFromOpSorter(uint64_t userDbId, const DataList &byteCodeList, const std::wstring & sql, ByteCodeResults &results);
	bool parseOrderColumnFromSql(ByteCodeResults::iterator tblIter, uint64_t userDbId, const std::wstring & sql, ByteCodeResults &results);
	void parseOrderColumnFromByteCodeUseColumnAndSql(
		uint64_t userDbId, 
		const ByteCodeUseColumns & byteCodeUseColumns, 
		const std::wstring & sql, 
		ByteCodeResults & results);

	void parseTableAndIndexFromOpenRead(uint64_t userDbId, const RowItem &rowItem, ByteCodeResults &results);
	void parseWhereIdxColumnsFromExplainRow(uint64_t userDbId, const RowItem &rowItem, ByteCodeResults &results);
	void parseWhereExpressesFromSeekOpsRow(uint64_t userDbId, DataList::const_iterator rowIter, const DataList &byteCodeList, ByteCodeResults & results);
	void parseWhereOrIndexColumnFromOpColumn(uint64_t userDbId, const RowItem &rowItem, ByteCodeResults &results);
	void parseWhereOrIndexColumnFromOpRowid(uint64_t userDbId, const RowItem &rowItem, ByteCodeResults &results);
	std::wstring getWhereOrIndexColumnFromOpColumn(uint64_t userDbId, const RowItem &rowItem, ByteCodeResults &results);
	void parseWhereExpressFromOpColumn(uint64_t userDbId, DataList::const_iterator compareRowIter,
		DataList::const_iterator columnRowIter, const DataList &byteCodeList, ByteCodeResults &results);
	
	void parseWhereOrIndexColumnFromOpCompare(uint64_t userDbId, DataList::const_iterator iter,  const DataList & byteCodeList, ByteCodeResults & results);
	void parseWhereExpressesFromOpCompare(uint64_t userDbId, DataList::const_iterator iter, const DataList & byteCodeList, ByteCodeResults & results);
	void parseWhereOrIndexColumnFromSeekRowid(uint64_t userDbId, DataList::const_iterator iter, ByteCodeResults &results, const DataList & byteCodeList);
	void parseWhereExpressesFromSeekRowid(uint64_t userDbId, DataList::const_iterator iter, ByteCodeResults &results, const DataList & byteCodeList);

	Columns getUserColumnStrings(uint64_t userDbId, const std::wstring & tblName, const std::wstring & schema = std::wstring());
	std::wstring getPrimaryKeyColumn(uint64_t userDbId, const std::wstring & tblName, Columns & columns, const std::wstring & schema = std::wstring());

	void parseWhereOrIndexNullColumnFromSql(uint64_t userDbId, const RowItem & rowItem, ByteCodeResults & results, const std::wstring & sql);

	void parseSelectColumnsPrevFromResultRow(uint64_t userDbId, DataList::const_iterator resultRowIter, const DataList & byteCodeList, SelectColumns & selectColumns);
	
	UserTableStrings getUserTableStrings(uint64_t userDbId, const std::wstring & schema = std::wstring());
	std::vector<std::tuple<std::wstring, std::wstring, std::wstring>> parseWhereNullColumnsFromSelectSqlUpWords(
		std::vector<std::wstring>& words, const std::vector<std::pair<std::wstring, std::wstring>> & allAliases);
	
	Columns getOrderColumns(uint64_t userDbId, const std::wstring & tblName, const std::wstring & sql);
	std::wstring parseColumnByAliasFromSql(uint64_t userDbId, const std::wstring & tblName, 
		const TableAliasVector & tblAliasVector, const std::wstring & upColOrAlias, const std::wstring & upsql);
	

	bool isMatchIndexColumnSortNo(PragmaIndexColumns & idxColumns, const std::wstring orderExpColumn, int sortNo);
	void mergeByteCodeResults(ByteCodeResults &results, ByteCodeResults & subResults);
	
	// match the specified columns in all indexes of specified table
	std::wstring matchColumnsInAllIndexesOfTable(const Columns & columns, const std::vector<std::pair<int, std::wstring>> & useIndexes, uint64_t userDbId, const std::wstring & tblName);
	std::vector<std::pair<int, std::wstring>> getIndexColumnsByCoveringIndexName(uint64_t userDbId, int no, const std::wstring & coveringIndexName);
	
	
	std::wstring getWhereExpressValByOpColumn(uint64_t userDbId, int regNo, DataList::const_iterator rowIter, const DataList & byteCodeList);
	std::wstring convertByteCodeIterToWhereExpValue(DataList::const_iterator iter, int compareRegNo);

	std::wstring getOpcodeByByteCodeAddr(const std::wstring & compareJumpAddr, const DataList & byteCodeList);
};
