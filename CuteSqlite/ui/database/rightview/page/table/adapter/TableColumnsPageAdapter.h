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
#include "core/service/db/TableService.h"
#include "core/common/repository/QSqlStatement.h"
#include "ui/common/listview/QListViewCtrl.h"
#include "ui/database/rightview/page/supplier/TableStructureSupplier.h"
#include "ui/database/supplier/DatabaseSupplier.h"

class TableColumnsPageAdapter : public QAdapter<TableColumnsPageAdapter, QListViewCtrl>
{
public:
	TableColumnsPageAdapter(HWND parentHwnd, QListViewCtrl * listView, TableStructureSupplier * supplier);
	~TableColumnsPageAdapter();

	int loadTblColumnsListView();
	LRESULT fillDataInListViewSubItem(NMLVDISPINFO * pLvdi);

	void changeRuntimeDatasItem(int iItem, int iSubItem, const std::wstring & newText);
	void invalidateSubItem(int iItem, int iSubItem);
	void invalidateRow(int iItem);

	// create/copy a new column row operation
	void createNewColumn();
	// delete a column row operation
	bool deleteSelColumns(bool confirm=true);
	bool moveUpSelColumns();
	bool moveDownSelColumns();

	// click the subitem
	void clickListViewSubItem(NMITEMACTIVATE * clickItem);

	// Get all column names from dataview
	std::vector<std::wstring> getAllColumnNames(const std::vector<std::wstring> & excludeNames = std::vector<std::wstring>()) const ;

	ColumnInfo getRuntimeData(int nItem) const;

	std::wstring genderateCreateColumnsSqlClause() const;
	std::pair<std::wstring,std::wstring> generateInsertColumnsClause();
	std::wstring genderateAlterColumnsSqlClauseForMysql();

	ColumnInfoList getPrimaryKeyColumnInfoList();
	bool verifyExistsAutoIncrement();

	bool changeListViewCheckBox(int iItem, int iSubItem);

	bool existsColumnNameInRuntimeIndexes(const std::wstring & columnName);

	void selectListViewItemForManage();
private:
	TableStructureSupplier * supplier = nullptr;
	DatabaseSupplier * databaseSupplier = DatabaseSupplier::getInstance();
	DatabaseService * databaseService = DatabaseService::getInstance();
	TableService * tableService = TableService::getInstance();

	void loadHeadersForListView();
	int loadEmptyRowsForListView();
	int loadColumnRowsForListView(uint64_t userDbId, const std::wstring & schema, const std::wstring & tblName);

	bool getIsChecked(int iItem);
	int getSelDataType(const std::wstring & dataType);

	// functions for verify the primary key and auto increment 
	int verifyExistsOtherAutoIncrement(int iItem);
	bool verifyDataTypeAllowAutoIncrement(int iItem);
	int verifyExistsPrimaryKeyInSameRow(int iItem);	
	void validPrimaryKeyInSameRow(int iItem);
	void invalidExistsPrimaryKeyInOtherRow(int iItem);
	void invalidExistsAutoIncrementInSameRow(int iItem);
	
	void refreshPreviewSql();

};
