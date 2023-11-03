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

 * @file   TableMenuAdapter.h
 * @brief  Select table or folder then pop up the menu,
 *		   This class will impletement all the features of table popmenu 
 * 
 * @author Xuehan Qin
 * @date   2023-08-17
 *********************************************************************/
#pragma once

#include "ui/common/adapter/QAdapter.h"
#include "core/entity/Entity.h"
#include "core/service/db/TableService.h"
#include "ui/database/supplier/DatabaseSupplier.h"

class TableMenuAdapter : public QAdapter<TableMenuAdapter, CTreeViewCtrlEx>
{
public:
	TableMenuAdapter(HWND parentHwnd, CTreeViewCtrlEx * view);
	~TableMenuAdapter();

	void popupMenu(CPoint & pt);
	void openTable();
	bool renameTable();
	bool truncateTable();
	bool dropTable();
	bool copyTable();
	bool shardingTable();
	bool exportTable();
	void manageIndex();
	void manageForeignKey();
private:
	CBrush menuBrush;

	HICON openTableIcon = nullptr;
	HICON createTableIcon = nullptr;
	HICON alterTableIcon = nullptr;
	HICON renameTableIcon = nullptr;
	HICON trucateTableIcon = nullptr;
	HICON dropTableIcon = nullptr;	
	HICON copyTableIcon = nullptr;
	HICON shardingTableIcon = nullptr;
	HICON exportTableIcon = nullptr;
	HICON importFromSqlIcon = nullptr;
	HICON importFromCsvIcon = nullptr;
	HICON manageIndexIcon = nullptr;
	HICON manageForeignKeyIcon = nullptr;
	HICON propertiesIcon = nullptr;
	
	CMenu menu;

	DatabaseSupplier * supplier = DatabaseSupplier::getInstance();
	TableService * tableService = TableService::getInstance();

	void createImageList();
	void createMenu();
	
};
