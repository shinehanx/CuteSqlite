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

 * @file   TableForeignkeysPageAdapter.h
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-10-10
 *********************************************************************/
#pragma once
#include "ui/common/adapter/QAdapter.h"
#include "core/service/db/SqlService.h"
#include "core/service/db/DatabaseService.h"
#include "core/service/db/TableService.h"
#include "core/common/repository/QSqlStatement.h"
#include "ui/common/listview/QListViewCtrl.h"
#include "ui/database/rightview/page/supplier/TableStructureSupplier.h"

#define NEW_TBL_EMPTY_INDEX_SIZE 6

class TableForeignkeysPageAdapter : public QAdapter<TableForeignkeysPageAdapter, QListViewCtrl>
{
public:
	TableForeignkeysPageAdapter(HWND parentHwnd, QListViewCtrl * listView, TableStructureSupplier * supplier);
	~TableForeignkeysPageAdapter();

	int loadTblForeignkeysListView(uint64_t userDbId, const std::wstring & tblName, const std::wstring & schema = L"");
	LRESULT fillDataInListViewSubItem(NMLVDISPINFO * pLvdi);

	void changeRuntimeDatasItem(int iItem, int iSubItem, std::wstring & origText, const std::wstring & newText);
	void invalidateSubItem(int iItem, int iSubItem);

	// create/copy a new column row operation
	void createNewIndex();
	// delete a column row operation
	bool deleteSelForeignKeys(bool confirm=true);
	std::wstring getSubItemString(int iItem, int iSubItem);
	void changeColumnText(int iItem, int iSubItem, const std::wstring & text);
	void clickListViewSubItem(NMITEMACTIVATE * clickItem);
	std::wstring genderateCreateForeignKeyClause();
	void changePrimaryKey(ColumnInfoList & pkColumns);
	void deleteTableColumnName(const std::wstring & columnName);

	TableStructureSupplier * getSupplier() { return supplier; }

	bool verifyIfDuplicatedPrimaryKey(int iItem);
private:
	TableStructureSupplier * supplier = nullptr;
	DatabaseService * databaseService = DatabaseService::getInstance();
	TableService * tableService = TableService::getInstance();

	void loadHeadersForListView();
	int loadEmptyRowsForListView();
	int loadForeignkeyRowsForListView(uint64_t userDbId, const std::wstring & tblName, const std::wstring & schema);

	bool getIsChecked(int iItem);
	int getSelIndexType(const std::wstring & dataType);
	void removeSelectedItem(int nSelItem);
	void generateOneForeignKeyClause(ForeignKey &item, std::wstring &ss);
};
