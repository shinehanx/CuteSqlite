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

 * @file   DatabaseMenuAdapter.h
 * @brief  Select database or folder then pop up the menu,
 *		   This class will impletement all the features of popmenu 
 * 
 * @author Xuehan Qin
 * @date   2023-08-17
 *********************************************************************/
#pragma once

#include "ui/common/adapter/QAdapter.h"
#include "core/entity/Entity.h"
#include "core/service/db/DatabaseService.h"
#include "ui/database/supplier/DatabaseSupplier.h"

class DatabaseMenuAdapter : public QAdapter<DatabaseMenuAdapter, CTreeViewCtrlEx>
{
public:
	DatabaseMenuAdapter(HWND parentHwnd, CTreeViewCtrlEx * view);
	~DatabaseMenuAdapter();

	void popupMenu(CPoint & pt);
	void popupViewsMenu(CPoint & pt, bool isViewItem);
	void popupTriggersMenu(CPoint & pt, bool isViewItem);

	bool dropView();
	bool dropTrigger();
private:	
	HICON createDatabaseIcon = nullptr;
	HICON openDatabaseIcon = nullptr;
	HICON refreshDatabaseIcon = nullptr;
	HICON deleteDatabaseIcon = nullptr;
	HICON copyDatabaseIcon = nullptr;
	HICON exportAsSqlIcon = nullptr;
	HICON importFromSqlIcon = nullptr;
	HICON newMenuIcon = nullptr;
	HICON newTableIcon = nullptr;
	HICON newViewIcon = nullptr;
	HICON newTriggerIcon = nullptr;

	HICON openViewIcon = nullptr;
	HICON dropViewIcon = nullptr;

	HICON openTriggerIcon = nullptr;
	HICON dropTriggerIcon = nullptr;

	CMenu menu;
	CMenu newSubMenu;
	CMenu viewsMenu;
	CMenu triggersMenu;

	DatabaseSupplier * supplier = DatabaseSupplier::getInstance();
	DatabaseService * databaseService = DatabaseService::getInstance();

	void createImageList();
	void createMenu();
	void createViewsMenu();
	void createTriggersMenu();
};
