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

 * @file   ObjectsTreeViewAdapter.cpp
 * @brief  This adapter class provides the display of objects tree controls, as well as operations
 * 
 * @author Xuehan Qin
 * @date   2023-10-05
 *********************************************************************/
#include "stdafx.h"
#include "ObjectsTreeViewAdapter.h"
#include "utils/ResourceUtil.h"
#include "core/common/Lang.h"
#include "core/common/exception/QRuntimeException.h"
#include "ui/common/message/QPopAnimate.h"
#include "ui/common/message/QMessageBox.h"

ObjectsTreeViewAdapter::ObjectsTreeViewAdapter(HWND parentHwnd, CTreeViewCtrlEx * view)
{
	this->parentHwnd = parentHwnd;
	this->dataView = view;

	createImageList();
}

ObjectsTreeViewAdapter::~ObjectsTreeViewAdapter()
{
	if (folderBitmap) ::DeleteObject(folderBitmap);
	if (tableBitmap) ::DeleteObject(tableBitmap);
	if (viewBitmap) ::DeleteObject(viewBitmap);
	if (triggerBitmap) ::DeleteObject(triggerBitmap);
}


void ObjectsTreeViewAdapter::loadTreeView()
{
	dataView->DeleteAllItems();
	this->dataView->SetImageList(imageList);
	
	HTREEITEM hTablesFolderItem = dataView->InsertItem(S(L"tables").c_str(), 0, 0, NULL, TVI_LAST);
	HTREEITEM hViewsFolderItem = dataView->InsertItem(S(L"views").c_str(), 0, 0, NULL, TVI_LAST);
	HTREEITEM hTriggersFolderItem = dataView->InsertItem(S(L"triggers").c_str(), 0, 0, NULL, TVI_LAST);

	std::wstring exportSelectedDbId = settingService->getSysInit(L"export_selected_db_id");
	if (exportSelectedDbId.empty()) {
		return;
	}
	uint64_t userDbId = std::stoul(exportSelectedDbId);
	loadTablesForTreeView(hTablesFolderItem, userDbId);
	loadViewsForTreeView(hViewsFolderItem, userDbId);
	loadTriggersForTreeView(hTriggersFolderItem, userDbId);
}


void ObjectsTreeViewAdapter::checkToTree(HTREEITEM hItem, BOOL bCheck)
{
	if(hItem != NULL) {
        setChildCheck(hItem, bCheck);
        setParentCheck(hItem, bCheck);
    }
}


void ObjectsTreeViewAdapter::setChildCheck(HTREEITEM hItem, BOOL bCheck)
{
	HTREEITEM hChildItem = dataView->GetChildItem(hItem);
    while(hChildItem) {
        dataView->SetCheckState(hChildItem, bCheck);
        setChildCheck(hChildItem, bCheck);
        hChildItem =  dataView->GetNextSiblingItem(hChildItem);
    }
}


void ObjectsTreeViewAdapter::setParentCheck(HTREEITEM hItem, BOOL bCheck)
{
	HTREEITEM hParent = dataView->GetParentItem(hItem);
    if(hParent == NULL)
        return ;

    if(bCheck) {
        HTREEITEM hSlibing = dataView->GetNextSiblingItem(hItem);
        BOOL bFlag = TRUE;
        // ��ǰItem��ǰ���ֵܽڵ����Ƿ�ȫ��ѡ�У�
        while(hSlibing) {
            if(!dataView->GetCheckState(hSlibing)) {
                bFlag = FALSE; // ����ֵܽڵ�����һ��û��ѡ��
                break;
            }
            hSlibing = dataView->GetNextSiblingItem(hSlibing); 
        }

        if(bFlag) {
            hSlibing = dataView->GetPrevSiblingItem(hItem);
            while(hSlibing) {
                if(!dataView->GetCheckState(hSlibing)) {
                    bFlag = FALSE; // ǰ���ֵܽڵ�����һ��û��ѡ��
                    break;
                }
                hSlibing = dataView->GetPrevSiblingItem(hSlibing);
            }
        }

        // bFlagΪTRUE����ʾ��ǰ�ڵ������ǰ���ֵܽڵ㶼��ѡ�У���������丸�ڵ�ҲΪѡ��
		if (bFlag) {
			dataView->SetCheckState(hParent, TRUE);
		}            
    } else { // ��ǰ�ڵ���Ϊδѡ�У���Ȼ�丸�ڵ�ҲҪ����Ϊδѡ��
        dataView->SetCheckState(hParent, FALSE);
    }

    // �ݹ����
    setParentCheck(hParent, dataView->GetCheckState(hParent));
}

void ObjectsTreeViewAdapter::createImageList()
{
	if (!imageList.IsNull()) {
		return;
	}
	std::wstring imgDir = ResourceUtil::getProductImagesDir();
	HINSTANCE ins = ModuleHelper::GetModuleInstance();
	folderBitmap = (HBITMAP)::LoadImageW(ins, (imgDir + L"database\\tree\\folder.bmp").c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	tableBitmap = (HBITMAP)::LoadImageW(ins, (imgDir + L"database\\tree\\table.bmp").c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	viewBitmap = (HBITMAP)::LoadImageW(ins, (imgDir + L"database\\tree\\view.bmp").c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	triggerBitmap = (HBITMAP)::LoadImageW(ins, (imgDir + L"database\\tree\\trigger.bmp").c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	imageList.Create(16, 16, ILC_COLOR32, 0, 4);
	imageList.Add(folderBitmap); // 0 - Objects : folder
	imageList.Add(tableBitmap); // 1 - Objects : table	
	imageList.Add(viewBitmap);// 2 - Objects : view
	imageList.Add(triggerBitmap);// 3 - Objects : trigger
}

/**
 * Load tables for TreeView.
 * 
 * @param hTablesFolderItem The tables folder item
 * @param userDb UserDb refrence
 */
void ObjectsTreeViewAdapter::loadTablesForTreeView(HTREEITEM hTablesFolderItem, uint64_t userDbId)
{
	try {
		UserTableList tableList = databaseService->getUserTables(userDbId);
		for (UserTable item : tableList) {
			HTREEITEM hTblItem = dataView->InsertItem(item.name.c_str(), 1, 1, hTablesFolderItem, TVI_LAST);
		}		
	} catch (QRuntimeException &ex) {
		Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::error(parentHwnd, S(L"error-text").append(ex.getMsg()).append(L",[code:").append(ex.getCode()).append(L"]"));
	}
}

/**
* Load views for TreeView.
*
* @param hViewsFolderItem The views folder item
* @param userDb UserDb refrence
*/
void ObjectsTreeViewAdapter::loadViewsForTreeView(HTREEITEM hViewsFolderItem, uint64_t userDbId)
{
	try {
		UserTableList list = databaseService->getUserViews(userDbId);
		for (UserTable item : list) {
			dataView->InsertItem(item.name.c_str(), 2, 2, hViewsFolderItem, TVI_LAST);
		}
	}
	catch (QRuntimeException &ex) {
		Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::error(parentHwnd, S(L"error-text").append(ex.getMsg()).append(L",[code:").append(ex.getCode()).append(L"]"));
	}
}

/**
* Load triggers for TreeView.
*
* @param hViewsFolderItem - The triggers folder item
* @param userDb - UserDb refrence
*/
void ObjectsTreeViewAdapter::loadTriggersForTreeView(HTREEITEM hTriggersFolderItem, uint64_t userDbId)
{
	try {
		UserTableList list = databaseService->getUserTriggers(userDbId);
		for (UserTable item : list) {
			dataView->InsertItem(item.name.c_str(), 3, 3, hTriggersFolderItem, TVI_LAST);
		}
	}
	catch (QRuntimeException &ex) {
		Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::error(parentHwnd, S(L"error-text").append(ex.getMsg()).append(L",[code:").append(ex.getCode()).append(L"]"));
	}
}