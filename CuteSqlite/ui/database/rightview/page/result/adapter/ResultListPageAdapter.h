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

 * @file   ResultListPageAdapter.h
 * @brief  data list view adapter class of ResultListPage, get the data and showListView;
 * 
 * @author Xuehan Qin
 * @date   2023-05-28
 *********************************************************************/
#pragma once
#include <atlctrls.h>
#include <cstdint>
#include <string>
#include <list>
#include <tuple>
#include "ui/common/adapter/QAdapter.h"
#include "core/service/db/SqlService.h"
#include "core/service/db/DatabaseService.h"
#include "core/service/db/TableService.h"
#include "core/common/repository/QSqlStatement.h"
#include "ui/common/listview/QListViewCtrl.h"

/**
 * Define FilterTuple and DataFilters
 * that is the query condition vector of the filters that used in ResultFilterDialog to restore the filter condition.
 * the tuple param means 0-and/or, 1-column, 2-operator, 3-value
 */
typedef std::tuple<std::wstring, std::wstring, std::wstring, std::wstring> FilterTuple;
typedef std::vector<FilterTuple> DataFilters;

// query result type 
typedef enum {
	QUERY_SQL_RESULT,
	QUERY_TABLE_DATA
} ResultType;

#define TABLE_DATA_SETTING_PREFIX L"table-data-"
class ResultListPageAdapter : public QAdapter<ResultListPageAdapter, QListViewCtrl>
{
public:
	ResultListPageAdapter(HWND parentHwnd, QListViewCtrl * listView, ResultType resultType = QUERY_SQL_RESULT);
	~ResultListPageAdapter();

	int loadListView(uint64_t userDbId, std::wstring & sql);
	
	// Add filters for result list
	int loadFilterListView();

	// virtual list data load
	LRESULT fillDataInListViewSubItem(NMLVDISPINFO * pLvdi);

	RowItem getFirstSelectdRowItem();
	int getFirstSelectdIndex();
	DataList getSelectedDatas();
	int getSelectedItemCount();
	
	uint64_t getRuntimeUserDbId() { return runtimeUserDbId; }
	void setRuntimeUserDbId(uint64_t userDbId);

	UserTableStrings getRuntimeTables();
	void setRuntimeTables(const UserTableStrings & val);
	Columns getRuntimeColumns();
	void setRuntimeColumns(const Columns & columns);
	DataList getRuntimeDatas();
	void setRuntimeDatas(const DataList & dataList);

	void addListViewChangeVal(SubItemValue &subItemVal);

	ColumnInfoList getRuntimeColumnInfos(std::wstring & tblName);
	UserTable getRuntimeUserTable(std::wstring & tblName);
	Columns getRuntimeValidFilterColumns();

	DataFilters getRuntimeFilters();
	void setRuntimeFilters(DataFilters & filters);
	void clearRuntimeFilters();
	bool isRuntimeFiltersEmpty();

	// copy data
	void copyAllRowsToClipboard();
	void copySelRowsToClipboard();
	void copyAllRowsAsSql();
	void copySelRowsAsSql();

	// system sys_init table key prefix
	void setSettingPrefix(std::wstring & prefix);

	void changeRuntimeDatasItem(int iItem, int iSubItem, std::wstring & origText, std::wstring & newText);
	void invalidateSubItem(int iItem, int iSubItem);

	// create/copy a new row operation
	void createNewRow();
	void copyNewRow();
	// save/delete/cancel
	bool save();
	bool remove(bool confirm=true);
	bool cancel();

	int removeRowFromDb(int nSelItem, RowItem & rowItem);

	bool isDirty();

	// query result info
	ResultInfo & getRuntimeResultInfo();
	void sendExecSqlMessage(ResultInfo & resultInfo, bool isWait = false);	
	std::wstring & getRuntimeSql() { return runtimeSql; }
private:
	SqlService * sqlService = SqlService::getInstance();
	DatabaseService * databaseService = DatabaseService::getInstance();
	TableService * tableService = TableService::getInstance();
	
	// the runtime variables
	uint64_t runtimeUserDbId = 0;
	std::wstring originSql;
	std::wstring runtimeSql;

	UserTableStrings runtimeTables;
	Columns runtimeColumns;
	DataList runtimeDatas;   // runtime data(s) for showing list view
	std::vector<int> runtimeNewRows; // runtimeDatas index for create or copy a new row
	ResultInfo runtimeResultInfo;

	DataFilters runtimeFilters;

	ResultType resultType;
	std::wstring settingPrefix;

	void loadRuntimeTables(uint64_t userDbId, std::wstring & sql);
	void loadRuntimeHeader(QSqlStatement & query);
	int loadRuntimeData(QSqlStatement & query);
	void loadLimitParams(LimitParams & limitParams);

	bool getIsChecked(int iItem);

	std::wstring buildRungtimeSqlWithFilters();

	//save datas
	bool saveChangeVals();
	bool saveNewRows();
	bool restoreChangeVals();
	void resetRuntimeResultInfo();
};
