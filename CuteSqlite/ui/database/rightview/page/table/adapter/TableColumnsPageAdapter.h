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

 * @file   TableColumnsPageAdapter.h
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

#define NEW_TBL_EMPTY_COLUMN_SIZE 6
// Support create/modify table
typedef enum 
{
	NEW_TABLE,
	MOD_TABLE,
} TblOperateType;

class TableColumnsPageAdapter : public QAdapter<TableColumnsPageAdapter, QListViewCtrl>
{
public:
	TableColumnsPageAdapter(HWND parentHwnd, QListViewCtrl * listView, TblOperateType resultType = NEW_TABLE);
	~TableColumnsPageAdapter();

	int loadNewTblListView(uint64_t userDbId, const std::wstring & schema);
	LRESULT fillListViewWithEmptyItemData(NMLVDISPINFO * pLvdi);
private:
	const static Columns columns;
	const static std::vector<int> columnSizes;
	const static std::vector<int> columnFormats;
	const static std::vector<std::wstring> dataTypeList;

	TblOperateType operateType;

	void loadColumnsForListView();
	int loadEmptyRowsForListView();

	bool getIsChecked(int iItem);
	
};
