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

 * @file   TableIndexesPageAdapter.h
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-10-10
 *********************************************************************/
#pragma once
#include "ui/common/adapter/QAdapter.h"
#include "core/service/db/SqlService.h"
#include "core/service/db/DatabaseService.h"
#include "core/common/repository/QSqlStatement.h"
#include "ui/common/listview/QListViewCtrl.h"

#define NEW_TBL_EMPTY_INDEX_SIZE 6

class TableIndexesPageAdapter : public QAdapter<TableIndexesPageAdapter, QListViewCtrl>
{
public:
	TableIndexesPageAdapter(HWND parentHwnd, QListViewCtrl * listView, TblOperateType resultType = NEW_TABLE);
	~TableIndexesPageAdapter();

	int loadTblIndexesListView(uint64_t userDbId, const std::wstring & schema, const std::wstring & tblName = L"");
	LRESULT fillDataInListViewSubItem(NMLVDISPINFO * pLvdi);

	void changeRuntimeDatasItem(int iItem, int iSubItem, std::wstring & origText, const std::wstring & newText);
	void invalidateSubItem(int iItem, int iSubItem);

	// create/copy a new column row operation
	void createNewIndex();
	// delete a column row operation
	bool deleteSelIndexes(bool confirm=true);
	std::wstring getSubItemString(int iItem, int iSubItem);
	void changeColumnText(int iItem, int iSubItem, const std::wstring & text);
	void clickListViewSubItem(NMITEMACTIVATE * clickItem);
	std::wstring genderateIndexesSqlClause(bool hasAutoIncrement = false);
	void changePrimaryKey(ColumnInfoList & pkColumns);
private:
	const static Columns headerColumns;
	const static std::vector<int> columnSizes;
	const static std::vector<int> columnFormats;
	const static std::vector<std::wstring> indexTypeList;

	// store the runtime data of the column(s) settings
	IndexInfoList runtimeDatas;

	TblOperateType operateType;

	DatabaseService * databaseService = DatabaseService::getInstance();

	void loadHeadersForListView();
	int loadEmptyRowsForListView();
	int loadIndexRowsForListView(uint64_t userDbId, const std::wstring & schema, const std::wstring & tblName);

	bool getIsChecked(int iItem);
	int getSelIndexType(const std::wstring & dataType);
	void removeSelectedItem(int nSelItem);
};
