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

 * @file   RightWorkViewToolBarAdapter.h
 * @brief  The adapter class is a assistance class for RightWorkView
 * 
 * @author Xuehan Qin
 * @date   2023-11-22
 *********************************************************************/
#pragma once
#include "ui/common/adapter/QAdapter.h"
#include "core/service/db/DatabaseService.h"
#include "ui/database/rightview/page/QueryPage.h"
#include "ui/database/rightview/page/TableStructurePage.h"
#include "ui/common/tabview/QTabView.h"
#include "ui/database/supplier/DatabaseSupplier.h"

class RightWorkViewAdapter : public QAdapter<RightWorkViewAdapter> {
public:
	RightWorkViewAdapter(HWND parentHwnd, QTabView & tabView, 
		std::vector<QueryPage *> & queryPagePtrs, std::vector<TableStructurePage *> & tablePagePtrs);

	void execSelectedSql();
	void execAllSql();
	void explainSelectedSql();
	

	void createFirstQueryPage(CRect & tabRect);
	void createOrShowQueryPage(QueryPage &win, CRect & tabRect);
	void createOrShowTableStructurePage(TableStructurePage &win, CRect & tabRect);
	
	void addNewTable(CRect & tabRect);
	void addNewView(CRect & tabRect);
	void addNewTrigger(CRect & tabRect);

	void openView(CRect & tabRect);
	void dropView();

	void openTrigger(CRect & tabRect);
	void dropTrigger();

	void showTableData(CRect & tabRect, bool isPropertyPage);
	void alterTable(CRect & tabRect, TableStructurePageType tblStructPageType);
	void renameTable();
	void dropTable();

	LRESULT refreshTableDataForSameDbTablePage(uint64_t userDbId, const std::wstring & theTblName);
private:
	QTabView & tabView;
	std::vector<QueryPage *> & queryPagePtrs;
	std::vector<TableStructurePage *> & tablePagePtrs;

	DatabaseSupplier * databaseSupplier = DatabaseSupplier::getInstance();
	DatabaseService * databaseService = DatabaseService::getInstance();
};
