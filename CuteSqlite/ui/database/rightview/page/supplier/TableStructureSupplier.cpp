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

 * @file   TableStructureSupplier.cpp
 * @brief  This class supply runtime data for TableStructurePage/TableTabView/TableColumnsPage/TableIndexesPage objects
 * 
 * @author Xuehan Qin
 * @date   2023-10-21
 *********************************************************************/
#include "stdafx.h"
#include "TableStructureSupplier.h"
#include "core/common/Lang.h"
#include "utils/StringUtil.h"

const Columns TableStructureSupplier::colsHeadColumns = { S(L"column-name"), S(L"data-type"), L"Not Null", L"PK", L"Auto Insc", L"Unique", S(L"default"), S(L"check")};
const std::vector<int> TableStructureSupplier::colsHeadSizes = { 150, 100, 70, 70, 70, 70, 150, 100 };
const std::vector<int> TableStructureSupplier::colsHeadFormats = { LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_LEFT, LVCFMT_LEFT };
const std::vector<std::wstring> TableStructureSupplier::colsDataTypeList = { L"INTEGER", L"TEXT", L"BLOB", L"REAL", L"NUMERIC"};

const Columns TableStructureSupplier::idxHeadColumns = { S(L"index-name"), S(L"columns"), S(L"index-type"), S(L"index-unique"), S(L"partial-clause")}; 
const std::vector<int> TableStructureSupplier::idxHeadSizes = { 150, 200, 100, 80, 200};
const std::vector<int> TableStructureSupplier::idxHeadFormats = { LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_CENTER, LVCFMT_LEFT};
const std::vector<std::wstring> TableStructureSupplier::idxTypeList = {L"Unique", L"Primary Key",  L"Check", L"Index"};

const Columns TableStructureSupplier::frkHeadColumns = { S(L"foreignkey-name"), S(L"referencing-columns"), S(L"referenced-table"), S(L"referenced-columns"), S(L"on-update"), S(L"on-delete"), S(L"partial-clause")};
const std::vector<int> TableStructureSupplier::frkHeadSizes = { 150, 150, 150, 150, 150, 150, 150};
const std::vector<int> TableStructureSupplier::frkHeadFormats = { LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_LEFT};
const std::vector<std::wstring> TableStructureSupplier::frkOnUpdateTypeList = {L"SET NULL", L"SET DEFAULT",  L"CASCADE", L"RESTRICT"};
const std::vector<std::wstring> TableStructureSupplier::frkOnDeleteTypeList = {L"SET NULL", L"SET DEFAULT",  L"CASCADE", L"RESTRICT"}; 

ColumnInfo & TableStructureSupplier::getColsRuntimeData(int nSelItem)
{
	ATLASSERT(nSelItem < static_cast<int>(colsRuntimeDatas.size()));
	return colsRuntimeDatas.at(nSelItem);
}

void TableStructureSupplier::eraseColsRuntimeData(int nSelItem)
{
	if (nSelItem >= static_cast<int>(colsRuntimeDatas.size())) {
		return;
	}
	auto iter = colsRuntimeDatas.begin();
	for (int j = 0; j < nSelItem; j++) {
		iter++;
	}
	colsRuntimeDatas.erase(iter);
}


void TableStructureSupplier::eraseColsOrigData(int nSelItem)
{
	if (nSelItem >= static_cast<int>(colsOrigDatas.size())) {
		return;
	}
	auto iter = colsOrigDatas.begin();
	for (int j = 0; j < nSelItem; j++) {
		iter++;
	}
	colsOrigDatas.erase(iter);
}


IndexInfo & TableStructureSupplier::getIdxRuntimeData(int nSelItem)
{
	ATLASSERT(nSelItem < static_cast<int>(idxRuntimeDatas.size()));
	return idxRuntimeDatas.at(nSelItem);
}

void TableStructureSupplier::eraseIdxRuntimeData(int nSelItem)
{
	if (nSelItem >= static_cast<int>(idxRuntimeDatas.size())) {
		return;
	}
	auto iter = idxRuntimeDatas.begin();
	for (int j = 0; j < nSelItem; j++) {
		iter++;
	}
	idxRuntimeDatas.erase(iter);
}

ForeignKey & TableStructureSupplier::getFrkRuntimeData(int nSelItem)
{
	ATLASSERT(nSelItem < static_cast<int>(frkRuntimeDatas.size()));
	return frkRuntimeDatas.at(nSelItem);
}

void TableStructureSupplier::eraseFrkRuntimeData(int nSelItem)
{
	if (nSelItem >= static_cast<int>(frkRuntimeDatas.size())) {
		return;
	}
	auto iter = frkRuntimeDatas.begin();
	for (int j = 0; j < nSelItem; j++) {
		iter++;
	}
	frkRuntimeDatas.erase(iter);
}

/**
 * if delete index, update related column runtime data through should be deleted index.
 * 
 * @param indexInfo
 */
void TableStructureSupplier::updateRelatedColumnsIfDeleteIndex(const IndexInfo &delIndexInfo)
{
	if (delIndexInfo.type.empty() || delIndexInfo.columns.empty()) {
		return;
	}
	auto columns = StringUtil::split(delIndexInfo.columns, L",");

	int n = static_cast<int>(colsRuntimeDatas.size());
	for (int i = 0; i < n; i++) {
		ColumnInfo &columnInfo = colsRuntimeDatas.at(i);
		
		auto iter = std::find_if(columns.begin(), columns.end(), [&columnInfo](std::wstring &column) {
			return columnInfo.name == column;
		});

		if (iter == columns.end()) {
			continue;
		}

		// change primary key column
		if (delIndexInfo.type == idxTypeList.at(1)) { // 1- Primary Key

			columnInfo.pk = 0;
			columnInfo.ai = 0;
		}		
	}
}

/**
 * if delete index, update related column runtime data through should be deleted index.
 * 
 * @param indexInfo
 */
void TableStructureSupplier::updateRelatedColumnsIfChangeIndex(const IndexInfo & changeIndexInfo)
{
	if (changeIndexInfo.type.empty() || changeIndexInfo.columns.empty()) {
		return;
	}
	auto columns = StringUtil::split(changeIndexInfo.columns, L",");

	size_t n = colsRuntimeDatas.size();
	for (size_t i = 0; i < n; i++) {
		ColumnInfo &columnInfo = colsRuntimeDatas[i];
		if (colsOrigDatas.size() < i + 1) {
			colsOrigDatas.push_back(columnInfo);
		}
		ColumnInfo &origInfo = colsOrigDatas[i];		
		
		auto iter = std::find_if(columns.begin(), columns.end(), [&columnInfo](std::wstring &column) {
			return columnInfo.name == column;
		});

		if (iter == columns.end()) {
			continue;
		}

		// change primary key column
		if (changeIndexInfo.type == idxTypeList.at(1)) { // 1- Primary Key
			origInfo.pk = columnInfo.pk;
			origInfo.ai = columnInfo.ai;

			columnInfo.pk = 1;
			columnInfo.ai = changeIndexInfo.ai;
		} 		
	}
}

/**
 * if update column name, update related index column and foreign key column
 * 
 * @param origName
 * @param newName
 */
void TableStructureSupplier::updateRelatedColumnsIfChangeColumnName(std::wstring& origName, const std::wstring& newName)
{
	// update column name of indexes
	updateColumnNameInIdxRuntimeDatas(origName, newName);
	// update column name of foreign keys
	updateColumnNameInFrkRuntimeDatas(origName, newName);
}

/**
 * change columns value in listView of TableIndexesPage when TableColumnsPage changing column name.
 * 
 * @param oldColumnName
 * @param newColumnName
 */
void TableStructureSupplier::updateColumnNameInIdxRuntimeDatas(const std::wstring & oldColumnName, const std::wstring & newColumnName)
{
	if (oldColumnName.empty() || newColumnName.empty() || oldColumnName == newColumnName) {
		return;
	}
	IndexInfoList & indexes = getIdxRuntimeDatas();
	int n = static_cast<int>(indexes.size());
	for (int i = 0; i < n; i++) {
		auto & item = indexes.at(i);
		auto columns = StringUtil::split(item.columns, L",");
		auto iter = std::find(columns.begin(), columns.end(), oldColumnName);
		if (iter == columns.end()) {
			continue;
		}
		(*iter) = newColumnName;
		item.columns = StringUtil::implode(columns, L",");
	}
}

/**
 * change columns value in listView of TableForeinkeysPage when TableColumnsPage changing column name.
 * 
 * @param oldColumnName
 * @param newColumnName
 */
void TableStructureSupplier::updateColumnNameInFrkRuntimeDatas(const std::wstring & oldColumnName, const std::wstring & newColumnName)
{
	if (oldColumnName.empty() || newColumnName.empty() || oldColumnName == newColumnName) {
		return;
	}
	ForeignKeyList & foreignKeys = getFrkRuntimeDatas();
	int n = static_cast<int>(foreignKeys.size());
	for (int i = 0; i < n; i++) {
		auto & item = foreignKeys.at(i);
		auto columns = StringUtil::split(item.columns, L",");
		auto iter = std::find(columns.begin(), columns.end(), oldColumnName);
		if (iter == columns.end()) {
			continue;
		}
		(*iter) = newColumnName;
		item.columns = StringUtil::implode(columns, L",");
	}
}
