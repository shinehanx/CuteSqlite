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

 * @file   LeftTreeViewAdapter.h
 * @brief  This adapter class provides the display of tree controls, as well as operations
 * 
 * @author Xuehan Qin
 * @date   2023-05-20
 *********************************************************************/
#pragma once
#include "ui/common/adapter/QAdapter.h"
#include "core/entity/Entity.h"
#include "core/service/db/DatabaseService.h"
#include "core/service/db/TableService.h"
#include "ui/database/supplier/DatabaseSupplier.h"

class LeftTreeViewAdapter : public QAdapter<LeftTreeViewAdapter, CTreeViewCtrlEx>
{
public:
	LeftTreeViewAdapter(HWND parentHwnd, CTreeViewCtrlEx * view);
	~LeftTreeViewAdapter();

	void createUserDatabase(std::wstring & dbPath);
	void openUserDatabase(std::wstring & dbPath);
	void activeUserDatabase(uint64_t userDbId);
	void copyUserDatabase(const std::wstring & toDbPath);

	void loadTreeView();
	CTreeItem getSeletedItem();
	uint64_t getSeletedItemData();
	void removeSeletedDbTreeItem();
	void selectDbTreeItem(uint64_t userDbId);
	void selectTableTreeItem(uint64_t userDbId, const std::wstring tableName);

	uint64_t getSeletedUserDbId();

	UserDbList getDbs();
	void loadDbs();
	void initDatabaseSupplier();

	HTREEITEM getChildFolderItem(HTREEITEM hTreeItem, const std::wstring & folderName);

	void loadTablesForTreeView(HTREEITEM hTablesFolderItem, UserDb & userDb, bool isLoadColumnsAndIndex = false);
	void loadDbIndexesForTreeView(HTREEITEM hIndexesFolderItem, UserDb & userDb);
	void loadViewsForTreeView(HTREEITEM hViewsFolderItem, UserDb & userDb);
	void loadTriggersForTreeView(HTREEITEM hTriggersFolderItem, UserDb & userDb);

	void loadTblColumsForTreeView(HTREEITEM hFieldsFolderItem, uint64_t userDbId, UserTable & userTable);
	void loadTblIndexesForTreeView(HTREEITEM hIndexesFolderItem, uint64_t userDbId, UserTable & userTable);
	void expandTreeItem(LPNMTREEVIEW ptr);
private:
	CImageList imageList;
	UserDbList dbs;

	HICON databaseIcon = nullptr;
	HICON folderIcon = nullptr;
	HICON tableIcon = nullptr;
	HICON fieldIcon = nullptr;
	HICON indexIcon = nullptr;
	HICON viewIcon = nullptr;
	HICON triggerIcon = nullptr;

	DatabaseService * databaseService = DatabaseService::getInstance();
	TableService * tableService = TableService::getInstance();
	DatabaseSupplier * databaseSupplier = DatabaseSupplier::getInstance();

	void createImageList();
	
	void doTrackParentTreeItemForSupplier(CTreeItem &treeItem);
};


