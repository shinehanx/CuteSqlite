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
#include "ui/common/adapter/QAdapter.h"
#include "core/service/db/SqlService.h"
#include "core/service/db/DatabaseService.h"
#include "core/common/repository/QSqlStatement.h"

class ResultListPageAdapter : public QAdapter<ResultListPageAdapter, CListViewCtrl>
{
public:
	ResultListPageAdapter(HWND parentHwnd, CListViewCtrl * listView);
	~ResultListPageAdapter();

	int loadListView(uint64_t userDbId, std::wstring & sql);
	
	// virtual list data load
	LRESULT fillListViewItemData(NMLVDISPINFO * pLvdi);

	void changeSelectAllItems();

	UserTableStrings getRuntimeTables();
	Columns getRuntimeColumns();
	DataList getRuntimeDatas();

	UserColumnList getRuntimeUserColumns(std::wstring & tblName);
	UserTable getRuntimeUserTable(std::wstring & tblName);
private:
	SqlService * sqlService = SqlService::getInstance();
	DatabaseService * databaseService = DatabaseService::getInstance();
	
	// the runtime varibles
	uint64_t runtimeUserDbId = 0;
	UserTableStrings runtimeTables;
	Columns runtimeColumns;
	DataList runtimeDatas;

	void loadRuntimeTables(uint64_t userDbId, std::wstring & sql);
	void loadRuntimeHeader(QSqlStatement & query);
	int loadRuntimeData(QSqlStatement & query);

	bool getIsChecked(int iItem);
};
