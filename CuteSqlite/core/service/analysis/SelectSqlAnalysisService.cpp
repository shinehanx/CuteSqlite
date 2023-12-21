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

 * @file   SelectSqlAnalysisServic.cpp
 * @brief  Analysis sql service for select sql statement.
 * 
 * @author Xuehan Qin
 * @date   2023-12-07
 *********************************************************************/
#include "stdafx.h"
#include "SelectSqlAnalysisService.h"
#include "utils/SqlUtil.h"
#include "core/common/exception/QRuntimeException.h"
#include "core/common/exception/QSqlExecuteException.h"

DataList SelectSqlAnalysisService::explainSql(uint64_t userDbId, const std::wstring & sql)
{
	return getRepository()->explainSql(userDbId, sql);
}


ExplainQueryPlans SelectSqlAnalysisService::explainQueryPlanSql(uint64_t userDbId, const std::wstring & sql)
{
	return getRepository()->explainQueryPlanSql(userDbId, sql);
}

/**
 * Explain bycodeList to ByteCodeResults.
 * 
 * @param byteCodeList - Generate a byte code list from Explain a sql statement
 * @return - ByteCodeResults
 */
ByteCodeResults SelectSqlAnalysisService::explainReadByteCodeToResults(uint64_t userDbId, const DataList & byteCodeList, const std::wstring &sql)
{
	ByteCodeResults results;
	// For results.whereColumns
	doConvertByteCodeForWhereColumns(userDbId, byteCodeList, sql, results);
	// For results.orderColumns
	doConvertByteCodeForOrderColumns(userDbId, byteCodeList, sql, results);
	return results;
}

/**
 * Parse the explain byteCodeList to results.
 * includes : 
 * 1) Fill in the table or index params(op, name, type, rootpage) of ByteCodeResult from the row that it is OP_OpenRead
 * 2) Fill in the useIndex of ByteCodeResult from the row.
 * 3) Fill in the whereColumns of ByteCodeResult from the row.
 * 4) Fill in the indexColumns of ByteCodeResult from the row.
 * 
 * @param [in]userDbId
 * @param [in]byteCodeList
 * @param [in]sql
 * @param [out]results
 */
void SelectSqlAnalysisService::doConvertByteCodeForWhereColumns(uint64_t userDbId, const DataList &byteCodeList, const std::wstring & sql,  ByteCodeResults & results)
{
	for (auto iter = byteCodeList.begin(); iter != byteCodeList.end(); iter++) {
		auto & rowItem = *iter;
		auto & opcode = rowItem.at(EXP_OPCODE);
		if (opcode == L"OpenRead") {
			parseTblOrIdxFromOpenRead(userDbId, rowItem, results);
		}
		else if (opcode == L"SeekGT" || opcode == L"SeekGE" || opcode == L"SeekLT" || opcode == L"SeekLE") { // index column
			parseWhereIdxColumnsFromExplainRow(userDbId, rowItem, results);
		}
		else if (opcode == L"IdxGE" || opcode == L"IdxGT" || opcode == L"IdxLE" || opcode == L"IdxLE") { // index column
			parseWhereIdxColumnsFromExplainRow(userDbId, rowItem, results);
		}
		else if (opcode == L"Eq" || opcode == L"Ne" || opcode == L"Lt" || opcode == L"Le" || opcode == L"Gt" || opcode == L"Ge") { // compare opcode
			parseWhereOrIndexColumnFromOpCompare(userDbId, rowItem, results, iter, byteCodeList);
		}
		else if (opcode == L"SeekRowid" || opcode == L"NotExists") {
			parseWhereOrIndexColumnFromSeekRowid(userDbId, rowItem, results, iter, byteCodeList);
		}
		else if (opcode == L"Column") { // where column 
			auto nextIter = std::next(iter);
			auto & nextOpcode = (*nextIter).at(EXP_OPCODE);
			if (nextOpcode == L"IfPos" || nextOpcode == L"Rewind"
				|| nextOpcode == L"Column" || nextOpcode == L"ResultRow"
				|| nextOpcode == L"Rowid" || nextOpcode == L"MakeRecord") {
				continue;
			}
			parseWhereOrIndexColumnFromOpColumn(userDbId, rowItem, results);
		}
		else if (opcode == L"IfNot" || opcode == L"If") {
			parseWhereOrIndexNullColumnFromSql(userDbId, rowItem, results, sql);
		}
	}
}

/**
 * Parse the explain byteCodeList to results.
 * includes : 
 * 1) Fill in the useIndex of ByteCodeResult from the row.
 * 2) Fill in the indexColumns of ByteCodeResult from the row.
 * 
 * @param [in]userDbId
 * @param [in]byteCodeList
 * @param [in]sql
 * @param [out]results
 */
void SelectSqlAnalysisService::doConvertByteCodeForOrderColumns(uint64_t userDbId, const DataList & byteCodeList, const std::wstring & sql, ByteCodeResults & results)
{

	// opcode Last and opcode Prev exists in byteCodeList at the same time
	parseOrderOrIndexColumnFromLastAndPrev(userDbId, byteCodeList, sql, results);

	// opcode IdxInsert and opcode Sort exists in byteCodeList at the same time
	parseOrderOrIndexColumnFromIdxInsertAndSort(userDbId, byteCodeList, sql, results);

	// opcode SorterInsert and opcode Sort exists in byteCodeList at the same time
	parseOrderOrIndexColumnFromSorter(userDbId, byteCodeList, sql, results);

// 	for (auto tblIter = results.begin(); tblIter != results.end(); tblIter++) {
// 		parseOrderClauseColumnFromSql(tblIter, userDbId, sql, results);
// 	}
}


bool SelectSqlAnalysisService::parseOrderOrIndexColumnFromLastAndPrev(uint64_t userDbId, const DataList &byteCodeList, const std::wstring & sql, ByteCodeResults &results)
{
	int hasLast = false, hasPrev = false;
	int lastP1 = -1, prevP1 = -1;
	auto lastIter = byteCodeList.end(), prevIter = byteCodeList.end();

	for (auto iter = byteCodeList.begin(); iter != byteCodeList.end(); iter++) {
		auto & rowItem = *iter;
		auto & opcode = rowItem.at(EXP_OPCODE);
		if (opcode == L"Last" || opcode == L"SeekEnd") {
			hasLast = true;
			lastP1 = rowItem.at(EXP_P1) != L"< NULL >" ? std::stoi(rowItem.at(EXP_P1)) : -1;
			lastIter = iter;
		} else if (opcode == L"Prev") {
			hasPrev = true;
			prevP1 = rowItem.at(EXP_P1) != L"< NULL >" ? std::stoi(rowItem.at(EXP_P1)) : -1;
			prevIter = iter;
		}
	}

	if (!hasLast || !hasPrev || lastP1 == -1 || prevP1 == -1 || lastP1 != prevP1) return false;

	auto tblIter = std::find_if(results.begin(), results.end(), [&lastP1](auto & result) {
		return result.no == lastP1;
	});
	if (tblIter == results.end()) {
		return false;
	}

	// Get gid of all sub-select statements that between '(' and ')'
	std::wstring mainSelectClause = StringUtil::notInSymbolString(sql, L'(', L')', 0, true);

	parseOrderColumnFromSql(tblIter, userDbId, mainSelectClause, results);

	// parse the order columns by sub-select clause that between '(' and ')'
	parseOrderColumnsBySubSelectClause(userDbId, sql,  results);
}

/**
 * Parse Order Clause's Column From Sql.
 * 
 * @param tblIter
 * @param userDbId
 * @param sql
 * @param results
 * @return 
 */
bool SelectSqlAnalysisService::parseOrderColumnFromSql(ByteCodeResults::iterator tblIter, uint64_t userDbId, const std::wstring & sql, ByteCodeResults &results)
{
	int no = (*tblIter).no;
	auto & orderColumns = (*tblIter).orderColumns;
	auto & orderIndexColumns = (*tblIter).orderIndexColumns;


	if ((*tblIter).type == L"table") {
		auto & tblName = (*tblIter).name;
		ColumnInfoList columnInfoList = columnUserRepository->getListByTblName(userDbId, tblName);
		Columns orderExpColumns = getOrderColumns(userDbId, tblName, sql);

		for (auto & tblColumnName : orderExpColumns) {
			auto foundIter = std::find_if(orderColumns.begin(), orderColumns.end(), [&tblColumnName](const auto & colName) {
				return tblColumnName == colName;
			});

			if (foundIter == orderColumns.end()) {
				orderColumns.push_back(tblColumnName);
			}
		}
	} else if ((*tblIter).type == L"index") {
		PragmaIndexColumns idxColumns = indexUserRepository->getPragmaIndexColumns(userDbId, (*tblIter).name);

		// parent table item in the results
		UserIndex userIndex = indexUserRepository->getByIndexName(userDbId, (*tblIter).name);
		auto pTblIter = std::find_if(results.begin(), results.end(), [&userIndex](auto & resItem) {
			return resItem.name == userIndex.tblName;
		});
		if (pTblIter == results.end()) {
			return false;
		}
		auto & tblName = (*pTblIter).name;
		auto & parentTblResult = *pTblIter;
		auto & tblOrderColumns = parentTblResult.orderColumns;
		auto & tblOrderIndexColumns = parentTblResult.orderIndexColumns;

		ColumnInfoList columnInfoList = columnUserRepository->getListByTblName(userDbId, tblName);
		Columns orderExpColumns = getOrderColumns(userDbId, tblName, sql);

		int sortNo = 0;
		for (auto & orderExpColumn : orderExpColumns) {
			bool matchIndexColumnSortNo = isMatchIndexColumnSortNo(idxColumns, orderExpColumn, sortNo);
			// Add index columns to index result
			auto foundIter = std::find_if(orderColumns.begin(), orderColumns.end(), [&orderExpColumn](const auto & colName) {
				return orderExpColumn == colName;
			});
			if (foundIter == orderColumns.end()) {
				orderColumns.push_back(orderExpColumn);
			}

			if (matchIndexColumnSortNo) {
				auto foundIter2 = std::find_if(orderIndexColumns.begin(), orderIndexColumns.end(), [&no, &orderExpColumn](const auto & idx) {
					return no == idx.first && orderExpColumn == idx.second;
				});
				if (foundIter2 == orderIndexColumns.end()) {
					orderIndexColumns.push_back({ no, orderExpColumn });
				}
			}


			// Add index columns to parent table result
			auto foundIter3 = std::find_if(tblOrderColumns.begin(), tblOrderColumns.end(), [&orderExpColumn](const auto & colName) {
				return orderExpColumn == colName;
			});
			if (foundIter3 == tblOrderColumns.end()) {
				tblOrderColumns.push_back(orderExpColumn);
			}

			if (matchIndexColumnSortNo) {
				auto foundIter4 = std::find_if(tblOrderIndexColumns.begin(), tblOrderIndexColumns.end(), [&no, &orderExpColumn](const auto & idx) {
					return no == idx.first && orderExpColumn == idx.second;
				});
				if (foundIter4 == tblOrderIndexColumns.end()) {
					tblOrderIndexColumns.push_back({ no, orderExpColumn });
				}
			}
			sortNo++;
		}
	}
	return true;
}

void SelectSqlAnalysisService::parseOrderOrIndexColumnFromIdxInsertAndSort(uint64_t userDbId, const DataList &byteCodeList, const std::wstring & sql, ByteCodeResults &results)
{
	// 1.parse using columns in sorter(opcode order: OpenEphemeral--...-->MakeRecord---->IdxInsert---...--->Sort)
	bool hasOpenEphemeral = false, hasMakeRecord = false, hasIdxInsert = false, hasSort = false;
	int openEphemeralP1 = -1, idxInsertP1 = -1, sortP1 = -1,
		idxInsertP2 = -1,
		makeRecordP1 = -1, makeRecordP2 = -1, makeRecordP3 = -1;
	DataList::const_iterator makeRecordIter = byteCodeList.end();
	
	for (auto iter = byteCodeList.begin(); iter != byteCodeList.end(); iter++) {
		auto & rowItem = *iter;
		auto & opcode = rowItem.at(EXP_OPCODE);
		if (opcode == L"OpenEphemeral") {
			hasOpenEphemeral = true;
			openEphemeralP1 = rowItem.at(EXP_P1) != L"< NULL >" ? std::stoi(rowItem.at(EXP_P1)) : -1;
		} else if (opcode == L"MakeRecord" && (*std::next(iter)).at(EXP_OPCODE) == L"IdxInsert") {
			hasMakeRecord = true;
			makeRecordP1 = rowItem.at(EXP_P1) != L"< NULL >" ? std::stoi(rowItem.at(EXP_P1)) : -1;
			makeRecordP2 = rowItem.at(EXP_P2) != L"< NULL >" ? std::stoi(rowItem.at(EXP_P2)) : -1;
			makeRecordP3 = rowItem.at(EXP_P3) != L"< NULL >" ? std::stoi(rowItem.at(EXP_P3)) : -1;
			makeRecordIter = iter;
		} else if (opcode == L"IdxInsert") {
			hasIdxInsert = true;
			idxInsertP1 = rowItem.at(EXP_P1) != L"< NULL >" ? std::stoi(rowItem.at(EXP_P1)) : -1;
			idxInsertP2 = rowItem.at(EXP_P2) != L"< NULL >" ? std::stoi(rowItem.at(EXP_P2)) : -1;
		} else if (opcode == L"Sort") {
			hasSort = true;
			sortP1 = rowItem.at(EXP_P1) != L"< NULL >" ? std::stoi(rowItem.at(EXP_P1)) : -1;
		}
	}

	if (!hasOpenEphemeral || !hasMakeRecord || !hasIdxInsert || !hasSort
		|| openEphemeralP1 == -1 || idxInsertP1 == -1 || sortP1 == -1
		|| makeRecordP1 == -1 || makeRecordP2 == -1 || makeRecordP3 == -1
		|| idxInsertP2 != makeRecordP3
		|| idxInsertP1 != sortP1) return;

	std::vector<int> makeRecordColumnRegNos;
	for (int i = 0; i < makeRecordP2; i++) {
		makeRecordColumnRegNos.push_back(makeRecordP1 + i);
	}

	// pair : first(std::wstring) - tbl name, second(std::wstring) - column name
	ByteCodeUseColumns byteCodeUseColumns;
	// 2. travel the column opcode row item from MakeRecord to Rewind
	for (int regNo : makeRecordColumnRegNos) {		
		for (auto iter = std::prev(makeRecordIter); iter != byteCodeList.begin(); iter = std::prev(iter)) {
			auto & rowItem = *iter;
			auto rowItemP3 = rowItem.at(EXP_P3) != L"< NULL >" ? std::stoi(rowItem.at(EXP_P3)) : -1;
			if (rowItem.at(EXP_OPCODE) == L"Rewind") {
				break;
			}
			if (rowItem.at(EXP_OPCODE) != L"Column" || rowItemP3 != regNo) { // Column p3 - register number,must equals each other
				continue;
			}
			auto rowItemP1 = rowItem.at(EXP_P1) != L"< NULL >" ? std::stoi(rowItem.at(EXP_P1)) : -1; // Column p1 - OpenRead p1
			auto rowItemP2 = rowItem.at(EXP_P2) != L"< NULL >" ? std::stoi(rowItem.at(EXP_P2)) : -1; // Column p2 - Table / Index index

			auto tblIter = std::find_if(results.begin(), results.end(), [&rowItemP1](auto & tbl) {
				return rowItemP1 == tbl.no;
			});
			if (tblIter == results.end()) {
				return ;
			}
			int tblNo = (*tblIter).no;
			std::wstring tblName = (*tblIter).name;
			std::wstring tblType = (*tblIter).type;
			if (tblType == L"table") {
				ColumnInfoList columnInfoList = columnUserRepository->getListByTblName(userDbId, tblName);

				//ByteCodeUseColumn params: 0-table no, 1-table name, 2-index no, 3-index name, 4-column name
				ByteCodeUseColumn sortColumn{tblNo, tblName, -1, L"", columnInfoList.at(rowItemP2).name};
				byteCodeUseColumns.push_back(sortColumn);
			} else if (tblType == L"index") {
				PragmaIndexColumns idxColumns = indexUserRepository->getPragmaIndexColumns(userDbId, (*tblIter).name);	
				const auto & colomnName = idxColumns.at(rowItemP2).name;

				// parent table item in the results
				UserIndex userIndex = indexUserRepository->getByIndexName(userDbId, (*tblIter).name);
				auto pTblIter = std::find_if(results.begin(), results.end(), [&userIndex](auto & resItem) {
					return resItem.name == userIndex.tblName;
				});
				if (pTblIter == results.end()) {
					return ;
				}
				//ByteCodeUseColumn params: 0-table no, 1-table name, 2-index no, 3-index name, 4-column name
				ByteCodeUseColumn sortColumn{(*pTblIter).no, (*pTblIter).name, tblNo, tblName, colomnName};
				byteCodeUseColumns.push_back(sortColumn);
			}
		}		
	}
	if (byteCodeUseColumns.empty()) {
		return;
	}

	// Get gid of all sub-select statements that between '(' and ')'
	std::wstring mainSelectClause = StringUtil::notInSymbolString(sql, L'(', L')', 0, true);
	parseOrderColumnFromByteCodeUseColumnAndSql(userDbId, byteCodeUseColumns,  mainSelectClause, results);

	// parse the order columns by sub-select clause that between '(' and ')'
	parseOrderColumnsBySubSelectClause(userDbId, sql,  results);
}


void SelectSqlAnalysisService::parseOrderOrIndexColumnFromSorter(uint64_t userDbId, const DataList &byteCodeList, const std::wstring & sql, ByteCodeResults &results)
{
	// 1.parse using columns in sorter(opcode order: OpenEphemeral--...-->MakeRecord---->IdxInsert---...--->Sort)
	bool hasSorterOpen = false, hasMakeRecord = false, hasSorterInsert = false, hasSorterSort = false;
	int sorterOpenP1 = -1, sorterInsertP1 = -1, sorterSortP1 = -1,
		sorterInsertP2 = -1,
		makeRecordP1 = -1, makeRecordP2 = -1, makeRecordP3 = -1;
	DataList::const_iterator makeRecordIter = byteCodeList.end();
	
	for (auto iter = byteCodeList.begin(); iter != byteCodeList.end(); iter++) {
		auto & rowItem = *iter;
		auto & opcode = rowItem.at(EXP_OPCODE);
		if (opcode == L"SorterOpen") {
			hasSorterOpen = true;
			sorterOpenP1 = rowItem.at(EXP_P1) != L"< NULL >" ? std::stoi(rowItem.at(EXP_P1)) : -1;
		} else if (opcode == L"MakeRecord" && (*std::next(iter)).at(EXP_OPCODE) == L"SorterInsert") {
			hasMakeRecord = true;
			makeRecordP1 = rowItem.at(EXP_P1) != L"< NULL >" ? std::stoi(rowItem.at(EXP_P1)) : -1; // MakeRecord P1 = Column opcode start index 
			makeRecordP2 = rowItem.at(EXP_P2) != L"< NULL >" ? std::stoi(rowItem.at(EXP_P2)) : -1;
			makeRecordP3 = rowItem.at(EXP_P3) != L"< NULL >" ? std::stoi(rowItem.at(EXP_P3)) : -1;
			makeRecordIter = iter;
		} else if (opcode == L"SorterInsert") {
			hasSorterInsert = true;
			sorterInsertP1 = rowItem.at(EXP_P1) != L"< NULL >" ? std::stoi(rowItem.at(EXP_P1)) : -1;
			sorterInsertP2 = rowItem.at(EXP_P2) != L"< NULL >" ? std::stoi(rowItem.at(EXP_P2)) : -1;
		} else if (opcode == L"SorterSort") {
			hasSorterSort = true;
			sorterSortP1 = rowItem.at(EXP_P1) != L"< NULL >" ? std::stoi(rowItem.at(EXP_P1)) : -1;
		}
	}

	if (!hasSorterOpen || !hasMakeRecord || !hasSorterInsert || !hasSorterSort
		|| sorterOpenP1 == -1 || sorterInsertP1 == -1 || sorterSortP1 == -1
		|| makeRecordP1 == -1 || makeRecordP2 == -1 || makeRecordP3 == -1
		|| sorterInsertP1 != sorterOpenP1
		|| sorterInsertP2 != makeRecordP3 
		|| sorterInsertP1 != sorterSortP1) return;

	std::vector<int> makeRecordColumnRegNos;
	for (int i = 0; i < makeRecordP2; i++) {
		makeRecordColumnRegNos.push_back(makeRecordP1 + i);
	}

	// pair : first(std::wstring) - tbl name, second(std::wstring) - column name
	ByteCodeUseColumns byteCodeUseColumns;
	// 2. travel the column opcode row item from MakeRecord to Rewind
	for (int regNo : makeRecordColumnRegNos) {		
		for (auto iter = std::prev(makeRecordIter); iter != byteCodeList.begin(); iter = std::prev(iter)) {
			auto & rowItem = *iter;
			auto rowItemP3 = rowItem.at(EXP_P3) != L"< NULL >" ? std::stoi(rowItem.at(EXP_P3)) : -1;
			if (rowItem.at(EXP_OPCODE) == L"Rewind") {
				break;
			}
			if (rowItem.at(EXP_OPCODE) != L"Column" || rowItemP3 != regNo) { // Column p3 - register number,must equals each other
				continue;
			}
			auto rowItemP1 = rowItem.at(EXP_P1) != L"< NULL >" ? std::stoi(rowItem.at(EXP_P1)) : -1; // Column p1 - OpenRead p1
			auto rowItemP2 = rowItem.at(EXP_P2) != L"< NULL >" ? std::stoi(rowItem.at(EXP_P2)) : -1; // Column p2 - Table / Index index

			auto tblIter = std::find_if(results.begin(), results.end(), [&rowItemP1](auto & tbl) {
				return rowItemP1 == tbl.no;
			});
			if (tblIter == results.end()) {
				return ;
			}
			int tblNo = (*tblIter).no;
			std::wstring tblName = (*tblIter).name;
			std::wstring tblType = (*tblIter).type;
			if (tblType == L"table") {
				ColumnInfoList columnInfoList = columnUserRepository->getListByTblName(userDbId, tblName);

				//ByteCodeUseColumn params: 0-table no, 1-table name, 2-index no, 3-index name, 4-column name
				ByteCodeUseColumn sortColumn{tblNo, tblName, -1, L"", columnInfoList.at(rowItemP2).name};
				byteCodeUseColumns.push_back(sortColumn);
			} else if (tblType == L"index") {
				PragmaIndexColumns idxColumns = indexUserRepository->getPragmaIndexColumns(userDbId, (*tblIter).name);	
				const auto & colomnName = idxColumns.at(rowItemP2).name;

				// parent table item in the results
				UserIndex userIndex = indexUserRepository->getByIndexName(userDbId, (*tblIter).name);
				auto pTblIter = std::find_if(results.begin(), results.end(), [&userIndex](auto & resItem) {
					return resItem.name == userIndex.tblName;
				});
				if (pTblIter == results.end()) {
					return ;
				}
				//ByteCodeUseColumn params: 0-table no, 1-table name, 2-index no, 3-index name, 4-column name
				ByteCodeUseColumn sortColumn{(*pTblIter).no, (*pTblIter).name, tblNo, tblName, colomnName};
				byteCodeUseColumns.push_back(sortColumn);
			}
		}		
	}
	if (byteCodeUseColumns.empty()) {
		return;
	}

	// Get gid of all sub-select clause that between '(' and ')'
	std::wstring mainSelectClause = StringUtil::notInSymbolString(sql, L'(', L')', 0, true);
	parseOrderColumnFromByteCodeUseColumnAndSql(userDbId, byteCodeUseColumns,  mainSelectClause, results);

	// parse the order columns by sub-select clause that between '(' and ')'
	parseOrderColumnsBySubSelectClause(userDbId, sql,  results);

}

/**
 * parse the order columns by sub-select clause 
 * 
 * @param userDbId
 * @param sql
 * @param results
 */
void SelectSqlAnalysisService::parseOrderColumnsBySubSelectClause(uint64_t userDbId, const std::wstring & sql,  ByteCodeResults & results)
{
	// Get all sub-select clause that between '(' and ')'
	auto subSelectClauses = StringUtil::inSymbolStrings(sql, L'(', L')', 0, true);
	for (auto subSelectClause : subSelectClauses) {
		if (subSelectClause == sql || !SqlUtil::isSelectSql(subSelectClause)) {
			continue;
		}

		try {
			DataList subByteCodeList = explainSql(userDbId, subSelectClause);
			ByteCodeResults subResults;

			// For results.whereColumns, recursion calling doConvertByteCodeForWhereColumns
			doConvertByteCodeForWhereColumns(userDbId, subByteCodeList, subSelectClause, subResults);
			// For results.orderColumns, recursion calling doConvertByteCodeForWhereColumns
			doConvertByteCodeForOrderColumns(userDbId, subByteCodeList, subSelectClause, subResults);

			if (subResults.empty()) {
				continue;
			}
			mergeByteCodeResults(results, subResults);
		}
		catch (QSqlExecuteException & ex) {
			Q_ERROR(L"Parse sql to order columns has QSqlExecuteException, code:{},msg:{}", ex.getCode(), ex.getMsg());
			continue;
		}
		catch (QRuntimeException & ex) {
			Q_ERROR(L"Parse sql to order columns has QRuntimeException, code:{},msg:{}", ex.getCode(), ex.getMsg());
			continue;
		}
	}
}

/**
 * .
 * 
 * @param userDbId
 * @param byteCodeSortColumns - parse all sorter columns 
 * @param mainSelectClause
 * @param results
 */
void SelectSqlAnalysisService::parseOrderColumnFromByteCodeUseColumnAndSql(
	uint64_t userDbId, 
	const ByteCodeUseColumns & byteCodeUseColumns, 
	const std::wstring & sql, 
	ByteCodeResults & results)
{
	if (byteCodeUseColumns.empty()) {
		return;
	}
	auto & orderClauses = SqlUtil::getOrderExpresses(sql);
	if (orderClauses.empty() || orderClauses.size() > 1) {
		return;
	}
	auto orderExpresses = StringUtil::split(orderClauses.at(0), L",");
	size_t n = orderExpresses.size();
	if (n > byteCodeUseColumns.size()) {
		return;
	}

	for (size_t i = 0; i < n; i++) {
		auto & byteCodeUseColumn = byteCodeUseColumns.at(i);
		
		auto tblIter = std::find_if(results.begin(), results.end(), [&byteCodeUseColumn](auto & result) {
			return result.no == byteCodeUseColumn.tblNo;
		});

		if (tblIter != results.end()) {
			auto & useIndexes = (*tblIter).useIndexes;
			auto & orderColumns = (*tblIter).orderColumns;
			auto & orderIndexColumns = (*tblIter).orderIndexColumns;

			auto foundIter1 = std::find_if(orderColumns.begin(), orderColumns.end(), [&byteCodeUseColumn] (auto & item) {
				return byteCodeUseColumn.columnName == item;
			});
			if (foundIter1 == orderColumns.end()) {
				orderColumns.push_back(byteCodeUseColumn.columnName);
			}
			
			if (byteCodeUseColumn.idxNo != -1 && byteCodeUseColumn.idxName.empty() == false) {
				auto foundIter2 = std::find_if(useIndexes.begin(), useIndexes.end(), [&byteCodeUseColumn] (auto & item) {
					return byteCodeUseColumn.idxNo == item.first && byteCodeUseColumn.idxName == item.second;
				});
				if (foundIter2 == useIndexes.end()) {
					useIndexes.push_back({ byteCodeUseColumn.idxNo, byteCodeUseColumn.idxName });
				}

				auto foundIter3 = std::find_if(orderIndexColumns.begin(), orderIndexColumns.end(), [&byteCodeUseColumn] (auto & item) {
					return byteCodeUseColumn.idxNo == item.first && byteCodeUseColumn.columnName == item.second;
				});

				if (foundIter3 == orderIndexColumns.end()) {
					orderIndexColumns.push_back({ byteCodeUseColumn.idxNo, byteCodeUseColumn.columnName });
				}
			}
		}
		
		auto tblIter2 = std::find_if(results.begin(), results.end(), [&byteCodeUseColumn](auto & result) {
			return result.no == byteCodeUseColumn.idxNo;
		});

		if (tblIter2 != results.end()) {
			auto & useIndexes = (*tblIter2).useIndexes;
			auto & orderColumns = (*tblIter2).orderColumns;
			auto & orderIndexColumns = (*tblIter2).orderIndexColumns;

			auto foundIter1 = std::find_if(orderColumns.begin(), orderColumns.end(), [&byteCodeUseColumn] (auto & item) {
				return byteCodeUseColumn.columnName == item;
			});
			if (foundIter1 == orderColumns.end()) {
				orderColumns.push_back(byteCodeUseColumn.columnName);
			}
			
			if (byteCodeUseColumn.idxNo != -1 && byteCodeUseColumn.idxName.empty() == false) {
				auto foundIter2 = std::find_if(useIndexes.begin(), useIndexes.end(), [&byteCodeUseColumn] (auto & item) {
					return byteCodeUseColumn.idxNo == item.first && byteCodeUseColumn.idxName == item.second;
				});
				if (foundIter2 == useIndexes.end()) {
					useIndexes.push_back({ byteCodeUseColumn.idxNo, byteCodeUseColumn.idxName });
				}

				auto foundIter3 = std::find_if(orderIndexColumns.begin(), orderIndexColumns.end(), [&byteCodeUseColumn] (auto & item) {
					return byteCodeUseColumn.idxNo == item.first && byteCodeUseColumn.columnName == item.second;
				});

				if (foundIter3 == orderIndexColumns.end()) {
					orderIndexColumns.push_back({ byteCodeUseColumn.idxNo, byteCodeUseColumn.columnName });
				}
			}
		}
	}
}

void SelectSqlAnalysisService::parseOrderOrIndexColumnFromOpSorter(uint64_t userDbId, const DataList &byteCodeList, const std::wstring & sql, ByteCodeResults &results)
{
	bool hasSorterOpen = false, hasSorterSort = false;
	int sorterOpenP1 = -1, sorterSortP1 = -1;

	for (auto iter = byteCodeList.begin(); iter != byteCodeList.end(); iter++) {
		auto & rowItem = *iter;
		auto & opcode = rowItem.at(EXP_OPCODE);
		if (opcode == L"IdxInsert") {
			hasSorterOpen = true;
			sorterOpenP1 = rowItem.at(EXP_P1) != L"< NULL >" ? std::stoi(rowItem.at(EXP_P1)) : -1;			
		} else if (opcode == L"Sort") {
			hasSorterSort = true;
			sorterSortP1 = rowItem.at(EXP_P1) != L"< NULL >" ? std::stoi(rowItem.at(EXP_P1)) : -1;
		}
	}

	if (!hasSorterOpen || !hasSorterSort || sorterOpenP1 == -1 || sorterSortP1 == -1 || sorterOpenP1 != sorterSortP1) return;

	for (auto tblIter = results.begin(); tblIter != results.end(); tblIter++) {
		parseOrderColumnFromSql(tblIter, userDbId, sql, results);
	}
}

void SelectSqlAnalysisService::parseTblOrIdxFromOpenRead(uint64_t userDbId, const RowItem &rowItem, ByteCodeResults &results)
{
	ByteCodeResult result;
	result.no = std::stoi(rowItem.at(EXP_P1)); // P1 - Table No. in the VDBE
	result.rootPage = std::stoull(rowItem.at(EXP_P2)); // p2 - root page in the VDBE
	UserTable userTable = tableUserRepository->getByRootPage(userDbId, result.rootPage);
	result.name = userTable.name;
	result.type = userTable.type;

	if (userTable.type == L"index") {
		auto iter = std::find_if(results.begin(), results.end(), [&userTable](auto & resItem) {
			return resItem.name == userTable.tblName;
		});
		if (iter == results.end()) {
			return;
		}
		(*iter).useIndexes.push_back({result.no, userTable.name });
	}
	results.push_back(result);
}

/**
 * From the row that row.opcode == SeekXX/IdxXX, explain the row params to ByteCodeResults.whereColumns and  ByteCodeResults.indexColumns for ByteCodeResults
 * support SeekXX £º SeekGT/SeekGE/SeekLT/SeekLE
 * support IdxXX £º IdxGE/IdxGT/IdxLE/IdxLE
 * 
 * @param userDbId - user db
 * @param rowItem - Opcode row
 * @param results - For ByteCodeResults
 */
void SelectSqlAnalysisService::parseWhereIdxColumnsFromExplainRow(uint64_t userDbId, const RowItem &rowItem, ByteCodeResults &results)
{
	int no = std::stoi(rowItem.at(EXP_P1)); // P1 - Table No. in the VDBE
	
	auto iter = std::find_if(results.begin(), results.end(), [&no](auto & result) {
		return result.no == no;
	});
	if (iter == results.end()) {
		return;
	}
	
	auto & whereColumns = (*iter).whereColumns;
	auto & whereIndexColumns = (*iter).whereIndexColumns;

	// If cursor P1 refers to an SQL table (B-Tree that uses integer keys), use the value in register P3 as a key. 
	// If cursor P1 refers to an SQL index, then P3 is the first in an array of P4 registers that are used as an unpacked index key.
	if ((*iter).type == L"table") {
		int columnIdx = std::stoi(rowItem.at(EXP_P3));
		auto & tblName = (*iter).name;
		ColumnInfoList columnInfoList = columnUserRepository->getListByTblName(userDbId, tblName);
		auto tblColumnName = columnInfoList.at(columnIdx).name;
		auto foundIter = std::find_if(whereColumns.begin(), whereColumns.end(), [&tblColumnName](const auto & colName) {
			return tblColumnName == colName;
		});
		if (foundIter == whereColumns.end()) {
			whereColumns.push_back(tblColumnName);
		}
	} else if ((*iter).type == L"index") {
		PragmaIndexColumns idxColumns = indexUserRepository->getPragmaIndexColumns(userDbId, (*iter).name);	
		
		// parent table item in the results
		UserIndex userIndex = indexUserRepository->getByIndexName(userDbId, (*iter).name);
		auto pTblIter = std::find_if(results.begin(), results.end(), [&userIndex](auto & resItem) {
			return resItem.name == userIndex.tblName;
		});
		if (pTblIter == results.end()) {
			return ;
		}
		auto & parentTblResult = *pTblIter;
		auto & tblWhereColumns = parentTblResult.whereColumns;
		auto & tblWhereIndexColumns = parentTblResult.whereIndexColumns;

		int useIdxColLen = std::stoi(rowItem.at(EXP_P4)); // P4 - use index column length with by sort
		for (int i = 0; i < useIdxColLen; i++) {
			auto & idxColumn = idxColumns.at(i);
			// Add index columns to index result
			auto foundIter = std::find_if(whereColumns.begin(), whereColumns.end(), [&idxColumn](const auto & colName) {
				return idxColumn.name == colName;
			});
			if (foundIter == whereColumns.end()) {
				whereColumns.push_back(idxColumn.name);
			}

			auto foundIter2 = std::find_if(whereIndexColumns.begin(), whereIndexColumns.end(), [&no, &idxColumn](const auto & idx) {
				return no == idx.first && idxColumn.name == idx.second;
			});
			if (foundIter2 == whereIndexColumns.end()) {
				whereIndexColumns.push_back({ no, idxColumn.name });
			}

			// Add index columns to parent table result
			auto foundIter3 = std::find_if(tblWhereColumns.begin(), tblWhereColumns.end(), [&idxColumn](const auto & colName) {
				return idxColumn.name == colName;
			});
			if (foundIter3 == tblWhereColumns.end()) {
				tblWhereColumns.push_back(idxColumn.name);
			}

			auto foundIter4 = std::find_if(tblWhereIndexColumns.begin(), tblWhereIndexColumns.end(), [&no, &idxColumn](const auto & idx) {
				return no == idx.first && idxColumn.name == idx.second;
			});
			if (foundIter4 == tblWhereIndexColumns.end()) {
				tblWhereIndexColumns.push_back({ no, idxColumn.name });
			}			
		}
	}
	
}

/**
 * From the row that row.opcode == Column, explain the row params to ByteCodeResults.whereColumns and  ByteCodeResults.indexColumns for ByteCodeResults
 * 
 * @param userDbId - user db
 * @param rowItem - Opcode row
 * @param results - For ByteCodeResults
 */
void SelectSqlAnalysisService::parseWhereOrIndexColumnFromOpColumn(uint64_t userDbId, const RowItem &rowItem, ByteCodeResults &results)
{
	int no = std::stoi(rowItem.at(EXP_P1)); // P1 - Table No. in the VDBE
	
	auto iter = std::find_if(results.begin(), results.end(), [&no](auto & result) {
		return result.no == no;
	});
	if (iter == results.end()) {
		return;
	}
	auto & whereColumns = (*iter).whereColumns;
	auto & whereIndexColumns = (*iter).whereIndexColumns;

	// Column
	if ((*iter).type == L"table") {
		int pos = std::stoi(rowItem.at(EXP_P2));
		auto & tblName = (*iter).name;
		ColumnInfoList columnInfoList = columnUserRepository->getListByTblName(userDbId, tblName);
		auto tblColumnName = columnInfoList.at(pos).name;
		auto foundIter = std::find_if(whereColumns.begin(), whereColumns.end(), [&tblColumnName](const auto & colName) {
			return tblColumnName == colName;
		});
		if (foundIter == whereColumns.end()) {
			whereColumns.push_back(tblColumnName);
		}
	} else if ((*iter).type == L"index") {
		PragmaIndexColumns idxColumns = indexUserRepository->getPragmaIndexColumns(userDbId, (*iter).name);		
		int pos = std::stoi(rowItem.at(EXP_P2)); // P2 - index 
		
		auto & idxColumn = idxColumns.at(pos);

		// parent table item in the results
		UserIndex userIndex = indexUserRepository->getByIndexName(userDbId, (*iter).name);
		auto pTblIter = std::find_if(results.begin(), results.end(), [&userIndex](auto & resItem) {
			return resItem.name == userIndex.tblName;
		});
		if (pTblIter == results.end()) {
			return ;
		}
		auto & parentTblResult = *pTblIter;
		auto & tblWhereColumns = parentTblResult.whereColumns;
		auto & tblWhereIndexColumns = parentTblResult.whereIndexColumns;

		// Add index columns to index result
		auto foundIter = std::find_if(whereColumns.begin(), whereColumns.end(), [&idxColumn](const auto & colName) {
			return idxColumn.name == colName;
		});
		if (foundIter == whereColumns.end()) {
			whereColumns.push_back(idxColumn.name);
		}

		auto foundIter2 = std::find_if(whereIndexColumns.begin(), whereIndexColumns.end(), [&no, &idxColumn](const auto & idx) {
			return no == idx.first && idxColumn.name == idx.second;
		});
		if (foundIter2 == whereIndexColumns.end()) {
			whereIndexColumns.push_back({ no, idxColumn.name });
		}

		// Add index columns to parent table result
		auto foundIter3 = std::find_if(tblWhereColumns.begin(), tblWhereColumns.end(), [&idxColumn](const auto & colName) {
			return idxColumn.name == colName;
		});
		if (foundIter3 == tblWhereColumns.end()) {
			tblWhereColumns.push_back(idxColumn.name);
		}

		auto foundIter4 = std::find_if(tblWhereIndexColumns.begin(), tblWhereIndexColumns.end(), [&no, &idxColumn](const auto & idx) {
			return no == idx.first && idxColumn.name == idx.second;
		});
		if (foundIter4 == tblWhereIndexColumns.end()) {
			tblWhereIndexColumns.push_back({ no, idxColumn.name });
		}
	}
}

/**
 * Compare opcode.
 * 
 * @param userDbId
 * @param rowItem
 * @param results
 * @param iter
 * @param byteCodeList
 */
void SelectSqlAnalysisService::parseWhereOrIndexColumnFromOpCompare(uint64_t userDbId, const RowItem& rowItem, 
	ByteCodeResults & results, DataList::const_iterator iter, const DataList & byteCodeList)
{
	std::wstring r1 = rowItem.at(EXP_P1); // Compare opcode: P1 - register
	std::wstring r3 = rowItem.at(EXP_P3); // Compare opcode: P3 - register

	for (auto prevIter = std::prev(iter); prevIter != byteCodeList.begin(); prevIter = std::prev(prevIter)) {
		if ((*prevIter).at(EXP_OPCODE) == L"Column" && 
			((*prevIter).at(EXP_P3) == r1 || (*prevIter).at(EXP_P3) == r3)) { // Column row : p3 - register
			parseWhereOrIndexColumnFromOpColumn(userDbId, (*prevIter), results);
		}
	}
}

/**
 * SeekRowid row.
 * 
 * @param userDbId
 * @param rowItem
 * @param results
 * @param iter
 * @param byteCodeList
 */
void SelectSqlAnalysisService::parseWhereOrIndexColumnFromSeekRowid(uint64_t userDbId, const RowItem& rowItem, ByteCodeResults &results, 
	DataList::const_iterator iter, const DataList & byteCodeList)
{
	int no = std::stoi(rowItem.at(EXP_P1)); // SeekRowid opcode: P1 - table no

	// 1.Convert P1(val = table.rowId) to column name of primary key 
	auto resIter = std::find_if(results.begin(), results.end(), [&no](const auto &item) {
		return no == item.no;
	});
	if (resIter == results.end()) {
		return;
	}
	std::wstring & tblName = (*resIter).name;
	if ((*resIter).type != L"table") {
		UserIndex userIndex = indexUserRepository->getByIndexName(userDbId, (*resIter).name);
		tblName = userIndex.tblName;
	}
	
	Columns columns = getUserColumnStrings(userDbId, tblName);
	std::wstring primaryKey = getPrimaryKeyColumn(userDbId, tblName, columns);
	auto & whereColumns = (*resIter).whereColumns;
	auto & whereIndexColumns = (*resIter).whereIndexColumns;
	auto & useIndexes = (*resIter).useIndexes;
	if (!primaryKey.empty()) {
		if ((*resIter).type == L"table") {
			auto & tblColumnName = primaryKey;
			auto foundIter = std::find_if(whereColumns.begin(), whereColumns.end(), [&tblColumnName](const auto & colName) {
				return tblColumnName == colName;
			});
			if (foundIter == whereColumns.end()) {
				whereColumns.push_back(tblColumnName);
			}
			auto foundIter2 = std::find_if(whereIndexColumns.begin(), whereIndexColumns.end(), [&no, &tblColumnName](const auto & idx) {
				return no == idx.first && tblColumnName == idx.second;
			});
			if (foundIter2 == whereIndexColumns.end()) {
				whereIndexColumns.push_back({ no, tblColumnName });
			}

			// Found primary key name, then add to Results
			std::wstring pkIndexName;
			IndexInfoList indexInfoList = indexUserRepository->getInfoListByTblName(userDbId, tblName);
			for(auto indexInfo : indexInfoList){
				auto pragmaIndexColumns = indexUserRepository->getPragmaIndexColumns(userDbId, indexInfo.name);
				if (pragmaIndexColumns.size() == 1 && pragmaIndexColumns.at(0).name == primaryKey) {
					pkIndexName = indexInfo.name;
				}
			}
			if (!pkIndexName.empty()) {
				auto foundIter3 = std::find_if(useIndexes.begin(), useIndexes.end(), [&no, &pkIndexName](const auto & useIndex) {
					return no == useIndex.first && pkIndexName == useIndex.second;
				});
				if (foundIter3 == useIndexes.end()) {
					useIndexes.push_back({ no, pkIndexName });
				}
			}
			
		} else if ((*resIter).type == L"index") {
			auto & idxColumnName = primaryKey;

			// parent table item in the results
			UserIndex userIndex = indexUserRepository->getByIndexName(userDbId, (*resIter).name);
			auto pTblIter = std::find_if(results.begin(), results.end(), [&userIndex](auto & resItem) {
				return resItem.name == userIndex.tblName;
			});
			if (pTblIter == results.end()) {
				return ;
			}
			auto & parentTblResult = *pTblIter;
			auto & tblWhereColumns = parentTblResult.whereColumns;
			auto & tblWhereIndexColumns = parentTblResult.whereIndexColumns;

			// Add index columns to index result
			auto foundIter = std::find_if(whereColumns.begin(), whereColumns.end(), [&idxColumnName](const auto & colName) {
				return idxColumnName == colName;
			});
			if (foundIter == whereColumns.end()) {
				whereColumns.push_back(idxColumnName);
			}

			auto foundIter2 = std::find_if(whereIndexColumns.begin(), whereIndexColumns.end(), [&no, &idxColumnName](const auto & idx) {
				return no == idx.first && idxColumnName == idx.second;
			});
			if (foundIter2 == whereIndexColumns.end()) {
				whereIndexColumns.push_back({ no, idxColumnName });
			}

			// Add index columns to parent table result
			auto foundIter3 = std::find_if(tblWhereColumns.begin(), tblWhereColumns.end(), [&idxColumnName](const auto & colName) {
				return idxColumnName == colName;
			});
			if (foundIter3 == tblWhereColumns.end()) {
				tblWhereColumns.push_back(idxColumnName);
			}

			auto foundIter4 = std::find_if(tblWhereIndexColumns.begin(), tblWhereIndexColumns.end(), [&no, &idxColumnName](const auto & idx) {
				return no == idx.first && idxColumnName == idx.second;
			});
			if (foundIter4 == tblWhereIndexColumns.end()) {
				tblWhereIndexColumns.push_back({ no, idxColumnName });
			}
		}
	}

	// 2.Found prev Column match P3 value
	std::wstring r3 = (*iter).at(EXP_P3); // SeekRowid opcode : p3 - register
	if (r3.empty()) {
		return;
	}
	for (auto prevIter = std::prev(iter); prevIter != byteCodeList.begin(); prevIter = std::prev(prevIter)) {
		if ((*prevIter).at(EXP_OPCODE) == L"Column" && (*prevIter).at(EXP_P3) == r3) { // Column opcode : p3 - register
			parseWhereOrIndexColumnFromOpColumn(userDbId, (*prevIter), results);
		}
	}
}

Columns SelectSqlAnalysisService::getUserColumnStrings(uint64_t userDbId, const std::wstring & tblName, const std::wstring & schema /*= std::wstring()*/)
{
	ATLASSERT(userDbId > 0 && !tblName.empty());
	ColumnInfoList columnInfoList = columnUserRepository->getListByTblName(userDbId, tblName, schema);
	Columns result;
	for (auto & item : columnInfoList) {
		result.push_back(item.name);
	}
	return result;
}

std::wstring SelectSqlAnalysisService::getPrimaryKeyColumn(uint64_t userDbId, const std::wstring & tblName, Columns & columns, const std::wstring & schema)
{
	ATLASSERT(userDbId > 0 && !tblName.empty());
	UserTable userTable = tableUserRepository->getTable(userDbId, tblName, schema);
	if (userTable.name.empty() || userTable.sql.empty()) {
		return L"";
	}

	std::wstring & createTblSql = userTable.sql;
	std::wstring primaryKey = SqlUtil::parsePrimaryKey(createTblSql);
	if (primaryKey.empty()) {
		return primaryKey;
	}

	for (auto column : columns) {
		if (column == primaryKey) {
			return primaryKey;
		}
	}
	return L"";
}

void SelectSqlAnalysisService::parseWhereOrIndexNullColumnFromSql(uint64_t userDbId, const RowItem & rowItem, ByteCodeResults & results, const std::wstring & sql)
{
	// 1.Get all table from database
	UserTableStrings allTables = getUserTableStrings(userDbId);
	if (allTables.empty()) {
		return;
	}
	

	// 2. Get using table names from sql statement 
	auto tableNames = SqlUtil::getTablesFromSelectSql(sql, allTables);
	if (tableNames.empty()) {
		return;
	}
	
	// 3.Split the sql to words
	std::wstring upsql = StringUtil::toupper(sql);
	upsql = StringUtil::replace(upsql, L",", L" ");
	upsql = StringUtil::replace(upsql, L"(", L" ");
	upsql = StringUtil::replace(upsql, L")", L" ");
	auto & upWords = StringUtil::splitByBlank(upsql);

	// 4.Get using table alias from sql statement
	// pair.left - alias, pair.second - table name
	std::vector<std::pair<std::wstring, std::wstring>> allAliases;
	size_t n = tableNames.size();
	for (size_t i = 0; i < n; i++) {
		auto & table = tableNames.at(i);
		std::wstring alias = SqlUtil::parseTableAliasFromSelectSqlUpWords(upWords, table, allTables);
		allAliases.push_back({ alias, table });
	}
	if (allAliases.empty()) {
		return;
	}
	
	// 6.Get using columns of "is null" or "not null" in the sql statement words 
	// return columnPairs, params: pair.first - table name, pair.second - column name of "is null" or "not null"
	auto columnPairs = SqlUtil::parseWhereNullColumnsFromSelectSqlUpWords(upWords, allAliases);
	if (columnPairs.empty()) {
		return;
	}

	// 7.add to results
	n = columnPairs.size();
	for (size_t i = 0; i < n; i++) {
		auto & columnPair = columnPairs.at(i);
		auto & tableName = columnPair.first;
		auto & columnName = columnPair.second;

		auto iter = std::find_if(results.begin(), results.end(), [&tableName](const auto & result) {
			return result.name == tableName;
		});
		if (iter == results.end()) {
			continue;
		}

		auto & whereColumns = (*iter).whereColumns;
		auto findIter = std::find_if(whereColumns.begin(), whereColumns.end(), [&columnName](const auto & item) {
			return StringUtil::toupper(item) == columnName;
		});
		if (findIter != whereColumns.end()) {
			continue;
		}

		Columns columns = getUserColumnStrings(userDbId, tableName);
		auto findIter2 = std::find_if(columns.begin(), columns.end(), [&columnName](const auto & item) {
			return StringUtil::toupper(item) == columnName;
		});
		if (findIter2 == columns.end()) {
			continue;
		}
		whereColumns.push_back(*findIter2);
	}
}

UserTableStrings SelectSqlAnalysisService::getUserTableStrings(uint64_t userDbId, const std::wstring & schema)
{
	ATLASSERT(userDbId > 0);
	UserTableStrings result;
	UserTableList userTableList = tableUserRepository->getListByUserDbId(userDbId, schema);
	for (auto userTable : userTableList) {
		result.push_back(userTable.name);
	}
	return result;
}

/**
 * Fetch the express clause in order by clause.
 * 
 * @param userDbId
 * @param tblName
 * @param sql
 * @return 
 */
Columns SelectSqlAnalysisService::getOrderColumns(uint64_t userDbId, const std::wstring & tblName, const std::wstring & sql)
{
	Columns result;
	auto & orderClauses = SqlUtil::getOrderExpresses(sql);
	for (auto & orderExpressClause : orderClauses) {
		if (orderExpressClause.empty()) {
			return result;
		}

		auto expresses = StringUtil::split(orderExpressClause, L",");
		std::wstring upsql = StringUtil::toupper(sql);
		// tblAliasVec is up case vector
		TableAliasVector tblAliasVec = SqlUtil::parseTableClauseFromSelectSql(upsql);
		ColumnInfoList tblColumns = columnUserRepository->getListByTblName(userDbId, tblName);
		for (auto & express : expresses) { // expresses - e.g., "uid desc, c.uid asc"
			auto & expWords = StringUtil::splitByBlank(express);
			for (auto & word : expWords) {
				auto upword = StringUtil::toupper(word);
				if (upword == L"ASC" || upword == L"DESC") {
					continue;
				}
				size_t dotPos = word.find_first_of(L'.');
				if (dotPos == std::wstring::npos) {				
					auto iter = std::find_if(tblColumns.begin(), tblColumns.end(), [&upword](auto & columnInfo) {
						return StringUtil::toupper(columnInfo.name) == upword;
					});
					// found in the columns of table
					if (iter != tblColumns.end()) {
						result.push_back(word);
						continue;
					}

					//not found in the select columns clause with table
					std::wstring upcolumnName = parseColumnByAliasFromSql(userDbId, tblName, tblAliasVec, upword, upsql);
					if (upcolumnName.empty()) {
						continue;
					}

					auto iter2 = std::find_if(tblColumns.begin(), tblColumns.end(), [&upcolumnName](auto & columnInfo) {
						return StringUtil::toupper(columnInfo.name) == upcolumnName;
					});
					// found in the columns of table
					if (iter2 != tblColumns.end()) {
						result.push_back((*iter2).name);
						continue;
					}
					continue;
				}

				// tblPrefix.columnSuffix in order by clause, such as "analysis.uid"
				std::wstring tblPrefix = upword.substr(0, dotPos);
				std::wstring columnSuffix = word.substr(dotPos+1);
				auto findIter = std::find_if(tblAliasVec.begin(), tblAliasVec.end(), [&tblPrefix](auto & alias){
					return alias.tbl == tblPrefix || alias.alias == tblPrefix;
				});
				if (findIter == tblAliasVec.end() || (*findIter).tbl != StringUtil::toupper(tblName)) {
					continue;
				}
				result.push_back(columnSuffix);
			}
		}
	}
	
	return result;
}

std::wstring SelectSqlAnalysisService::parseColumnByAliasFromSql(uint64_t userDbId, const std::wstring & tblName, 
	const TableAliasVector & tblAliasVector, const std::wstring & upColOrAlias, const std::wstring & upsql)
{
	std::wstring result;
	std::vector<std::wstring> upSelectColumns = SqlUtil::getSelectColumnsClause(upsql);
	for (auto & upSelectColumn : upSelectColumns) {
		auto upOneColumnWords = StringUtil::split(upSelectColumn, L",");
		for (auto upOneColumnWord : upOneColumnWords) {
			if (upOneColumnWord.empty()) {
				continue;
			}
			auto upwords = StringUtil::splitByBlank(upOneColumnWord);
			auto iter = std::find_if(upwords.begin(), upwords.end(), [&upColOrAlias](auto & upword) {
				return upword == upColOrAlias;
			});
			if (iter == upwords.end()) {
				continue;
			}
			auto & upMatchWord = upwords.front();
			auto dotPos = upMatchWord.find_first_of(L".");
			if (dotPos == std::wstring::npos) {
				return upMatchWord;
			}
			auto tblPrefix = upMatchWord.substr(0, dotPos);
			auto columnSuffix = upMatchWord.substr(dotPos+1);

			auto iter2 = std::find_if(tblAliasVector.begin(), tblAliasVector.end(), [&tblPrefix, &tblName](auto & tblAlias) {
				return (tblAlias.alias == tblPrefix || tblAlias.tbl == tblPrefix) && StringUtil::toupper(tblName) == tblAlias.tbl;
			});
			if (iter2 == tblAliasVector.end()) {
				continue;
			}
			return columnSuffix;
		}
	}

	return result;
}

/**
 * Verify if equal compare order no. of using column in the "order by" clause to Index columns order no.
 * Such as(1): 
 * index : idx_inspection_uti(uid, analysis_date, inspection)
 * (1)order by clause: ORDER BY uid desc
 * (1)order by clause: ORDER BY uid desc, analysis_date desc
 * (1)order by clause: ORDER BY uid desc, analysis_date desc, inspection desc
 * (1)return true
 * (2)order by clause: ORDER BY analysis_date desc, uid desc, inspection desc
 * (2)order by clause: ORDER BY uid desc, inspection desc
 * (2)order by clause: ORDER BY analysis_date desc, inspection desc
 * (2)return false
 * 
 * @param idxColumns
 * @param orderExpColumn
 * @param sortNo
 * @return 
 */
bool SelectSqlAnalysisService::isMatchIndexColumnSortNo(PragmaIndexColumns & idxColumns, const std::wstring orderExpColumn, int sortNo)
{
	int n = static_cast<int>(idxColumns.size());
	if (n < sortNo) {
		return false;
	}

	for (int i = 0; i < n; i++) {
		auto & idxColumn = idxColumns.at(i);
		if (idxColumn.name == orderExpColumn && i == sortNo) {
			return true;
		}
	}
	return false;
}

/**
 * Merge subResults to results.
 * 
 * @param results[in, out]
 * @param subResult[in]
 */
void SelectSqlAnalysisService::mergeByteCodeResults(ByteCodeResults &results, ByteCodeResults & subResults)
{
	if (subResults.empty()) {
		return;
	}

	for (auto subResult : subResults) {
		auto & subUseIndexes = subResult.useIndexes;
		auto & subWhereColumns = subResult.whereColumns;
		auto & subOrderColumns = subResult.orderColumns;
		auto & subWhereIndexColumns = subResult.whereIndexColumns;
		auto & subOrderIndexColumns = subResult.orderIndexColumns;

		for (auto & result : results) {
			if (result.name != subResult.name || result.rootPage != subResult.rootPage) {
				continue;
			}

			auto & useIndexes = result.useIndexes;
			auto & whereColumns = result.whereColumns;
			auto & orderColumns = result.orderColumns;
			auto & whereIndexColumns = result.whereIndexColumns;
			auto & orderIndexColumns = result.orderIndexColumns;

			for (auto & subUseIndex : subUseIndexes) {
				auto iter = std::find_if(useIndexes.begin(), useIndexes.end(), [&subUseIndex](auto & item) {
					return subUseIndex.second == item.second;
				});
				if (iter == useIndexes.end()) {
					useIndexes.push_back(subUseIndex);
				}
			}

			for (auto & subWhereColumn : subWhereColumns) {
				auto iter = std::find_if(whereColumns.begin(), whereColumns.end(), [&subWhereColumn](auto & item) {
					return subWhereColumn == item;
				});
				if (iter == whereColumns.end()) {
					whereColumns.push_back(subWhereColumn);
				}
			}

			for (auto & subOrderColumn : subOrderColumns) {
				auto iter = std::find_if(orderColumns.begin(), orderColumns.end(), [&subOrderColumn](auto & item) {
					return subOrderColumn == item;
				});
				if (iter == orderColumns.end()) {
					orderColumns.push_back(subOrderColumn);
				}
			}

			for (auto & subWhereIndexColumn : subWhereIndexColumns) {
				auto iter = std::find_if(whereIndexColumns.begin(), whereIndexColumns.end(), [&subWhereIndexColumn](auto & item) {
					return subWhereIndexColumn.second == item.second;
				});
				if (iter == whereIndexColumns.end()) {
					//convert index no. of subUseIndexes in subResults --------to-------> index no. of result.useIndexes
					// 1) find index name by  index no. in subResults.useIndexes.
					auto iter2 = std::find_if(subUseIndexes.begin(), subUseIndexes.end(), [&subWhereIndexColumn](auto & item) {
						return subWhereIndexColumn.first == item.first; // first - index no.
					});
					if (iter2 == subUseIndexes.end()) {
						continue;
					}
					// found it.
					auto indexName = (*iter2).second;

					// 2) find index no. by  index name in results.useIndexes.
					auto iter3 = std::find_if(useIndexes.begin(), useIndexes.end(), [&indexName](auto & item) {
						return indexName == item.second;
					});

					if (iter3 == useIndexes.end()) {
						continue;
					}
					auto & idxNo = (*iter3).first;
					whereIndexColumns.push_back({idxNo,subWhereIndexColumn.second });
				}
			}
			
			for (auto & subOrderIndexColumn : subOrderIndexColumns) {
				auto iter = std::find_if(orderIndexColumns.begin(), orderIndexColumns.end(), [&subOrderIndexColumn](auto & item) {
					return subOrderIndexColumn.second == item.second;
				});
				if (iter == orderIndexColumns.end()) {
					//convert index no. of subUseIndexes in subResults --------to-------> index no. of useIndexes in results
					// 1) find index name by  index no. in subResults.useIndexes.
					auto iter2 = std::find_if(subUseIndexes.begin(), subUseIndexes.end(), [&subOrderIndexColumn](auto & item) {
						return subOrderIndexColumn.first == item.first; // first - index no.
					});
					if (iter2 == subUseIndexes.end()) {
						continue;
					}
					// found it.
					auto indexName = (*iter2).second;

					// 2) find index no. by  index name in results.useIndexes.
					auto iter3 = std::find_if(useIndexes.begin(), useIndexes.end(), [&indexName](auto & item) {
						return indexName == item.second;
					});

					if (iter3 == useIndexes.end()) {
						continue;
					}
					auto & idxNo = (*iter3).first;
					orderIndexColumns.push_back({ idxNo, subOrderIndexColumn.second });
				}
			}
		}
	}
}

