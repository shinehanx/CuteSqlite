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
#include "core/service/db/TableService.h"
#include "core/common/repository/QSqlStatement.h"
#include "ui/common/listview/QListViewCtrl.h"
#include "ui/database/rightview/page/supplier/TableStructureSupplier.h"
#include "ui/database/supplier/DatabaseSupplier.h"

#define NEW_TBL_EMPTY_INDEX_SIZE 6

class TableIndexesPageAdapter : public QAdapter<TableIndexesPageAdapter, QListViewCtrl>
{
public:
	TableIndexesPageAdapter(HWND parentHwnd, QListViewCtrl * listView, TableStructureSupplier * supplier);
	~TableIndexesPageAdapter();

	int loadTblIndexesListView(uint64_t userDbId, const std::wstring & tblName, const std::wstring & schema = L"");
	LRESULT fillDataInListViewSubItem(NMLVDISPINFO * pLvdi);

	void changeRuntimeDatasItem(int iItem, int iSubItem, std::wstring & origText, const std::wstring & newText);
	void invalidateSubItem(int iItem, int iSubItem);

	// create a new index row operation
	void createNewIndex();
	// delete a index row operation
	bool deleteSelIndexes(bool confirm=true);
	std::wstring getSubItemString(int iItem, int iSubItem);
	void changeColumnText(int iItem, int iSubItem, const std::wstring & text);
	void clickListViewSubItem(NMITEMACTIVATE * clickItem);
	std::wstring generateConstraintsClause(bool hasAutoIncrement = false);
	std::vector<std::wstring> generateCreateIndexesDDL(const std::wstring & schema, const std::wstring & tblName);
	void changePrimaryKey(ColumnInfoList & pkColumns);
	void deleteTableColumnName(const std::wstring & columnName);

	TableStructureSupplier * getSupplier() { return supplier; }

	bool verifyIfDuplicatedPrimaryKey(int iItem);
	void selectListViewItemForManage();

	bool isDirty();
private:
	TableStructureSupplier * supplier = nullptr;
	DatabaseSupplier * databaseSupplier = DatabaseSupplier::getInstance();
	DatabaseService * databaseService = DatabaseService::getInstance();
	TableService * tableService = TableService::getInstance();

	void loadHeadersForListView();
	int loadEmptyRowsForListView();
	int loadIndexRowsForListView(uint64_t userDbId, const std::wstring & tblName, const std::wstring & schema);

	bool getIsChecked(int iItem);
	int getSelIndexType(const std::wstring & dataType);
	void removeSelectedItem(int nSelItem);
	void generateOneConstraintClause(const IndexInfo &item, bool hasAutoIncrement, std::wstring &ss);
	void generateOneCreateIndexDDL(const IndexInfo &item, const std::wstring & schema, const std::wstring &tblName, std::wstring &ss);
	
	bool changeListViewCheckBox(int iItem, int iSubItem);
	void refreshPreviewSql();
};
