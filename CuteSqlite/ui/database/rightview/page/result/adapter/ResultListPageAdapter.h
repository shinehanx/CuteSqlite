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
#include "core/common/repository/QSqlStatement.h"

typedef std::vector<std::wstring> RowItem, Columns;
// data items list
typedef std::list<RowItem> DataList;


class ResultListPageAdapter : public QAdapter<ResultListPageAdapter, CListViewCtrl>
{
public:
	ResultListPageAdapter(HWND parentHwnd, CListViewCtrl * listView);
	~ResultListPageAdapter();

	int loadListView(uint64_t userDbId, std::wstring & sql);
	
	// virtual list data load
	LRESULT fillListViewItemData(NMLVDISPINFO * pLvdi);

	void changeAllCheckedRowItems();
private:
	SqlService * sqlService = SqlService::getInstance();

	DataList datas;
	Columns columns;
	void loadHeader(QSqlStatement & query);
	int loadData(QSqlStatement & query);

	bool getIsChecked(int iItem);
};
