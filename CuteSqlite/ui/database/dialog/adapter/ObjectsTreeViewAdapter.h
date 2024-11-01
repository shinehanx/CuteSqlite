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

 * @file   ObjectsTreeViewAdapter.h
 * @brief  This adapter class provides the display of objects tree controls, as well as operations
 * 
 * @author Xuehan Qin
 * @date   2023-10-05
 *********************************************************************/
#pragma once
#include "ui/common/adapter/QAdapter.h"
#include "core/entity/Entity.h"
#include "core/service/system/SettingService.h"
#include "core/service/db/DatabaseService.h"
#include "core/service/db/TableService.h"
#include "ui/database/supplier/DatabaseSupplier.h"

class ObjectsTreeViewAdapter : public QAdapter<ObjectsTreeViewAdapter, CTreeViewCtrlEx>
{
public:
	ObjectsTreeViewAdapter(HWND parentHwnd, CTreeViewCtrlEx * view);
	~ObjectsTreeViewAdapter();

	void loadTreeView();
	void selectItem(uint64_t userDbId);
	void checkToTree(HTREEITEM hItem , BOOL bCheck);
    void setChildCheck(HTREEITEM hItem, BOOL bCheck);
    void setParentCheck(HTREEITEM hItem, BOOL bCheck);
	void selectAll(BOOL bCheck);

	bool getSelUserTableList(uint64_t userDbId, UserTableList & tblList);
	bool getSelUserViewList(uint64_t userDbId, UserViewList & viewList);
	bool getSelUserTriggerList(uint64_t userDbId, UserTriggerList &triggerList);
private:
	CImageList imageList;

	HICON folderIcon = nullptr;
	HICON tableIcon = nullptr;
	HICON viewIcon = nullptr;
	HICON triggerIcon = nullptr;

	HTREEITEM hTablesFolderItem = nullptr;
	HTREEITEM hViewsFolderItem = nullptr ;
	HTREEITEM hTriggersFolderItem = nullptr;

	SettingService * settingService = SettingService::getInstance();
	DatabaseService * databaseService = DatabaseService::getInstance();
	TableService * tableService = TableService::getInstance();
	DatabaseSupplier * databaseSupplier = DatabaseSupplier::getInstance();

	void createImageList();
	void loadTablesForTreeView(HTREEITEM hTablesFolderItem, uint64_t userDbId);
	void loadViewsForTreeView(HTREEITEM hViewsFolderItem, uint64_t userDbId);
	void loadTriggersForTreeView(HTREEITEM hTriggersFolderItem, uint64_t userDbId);


};
