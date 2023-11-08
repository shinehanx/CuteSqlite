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

 * @file   TableMenuAdapter.cpp
 * @brief  Select database or folder then pop up the menu,
 *		   This class will impletement all the features of database popmenu   
 * 
 * @author Xuehan Qin
 * @date   2023-08-17
 *********************************************************************/

#include "stdafx.h"
#include "DatabaseMenuAdapter.h"
#include "core/common/Lang.h"
#include "utils/MenuUtil.h"
#include "ui/common/message/QMessageBox.h"
#include "ui/common/message/QPopAnimate.h"
#include "common/AppContext.h"

DatabaseMenuAdapter::DatabaseMenuAdapter(HWND parentHwnd, CTreeViewCtrlEx * view)
{
	this->parentHwnd = parentHwnd;
	this->dataView = view;
	menuBrush.CreateSolidBrush(RGB(255,255,255));//RGB(255,128,128));

	createImageList();
	createMenu();
	createViewsMenu();
	createTriggersMenu();
}

DatabaseMenuAdapter::~DatabaseMenuAdapter()
{
	menu.DestroyMenu();
	newSubMenu.DestroyMenu();
	viewsMenu.DestroyMenu();
	triggersMenu.DestroyMenu();

	if (!menuBrush.IsNull()) menuBrush.DeleteObject();
	if (createDatabaseIcon) ::DeleteObject(createDatabaseIcon);
	if (openDatabaseIcon) ::DeleteObject(openDatabaseIcon);
	if (refreshDatabaseIcon) ::DeleteObject(refreshDatabaseIcon);
	if (deleteDatabaseIcon) ::DeleteObject(deleteDatabaseIcon);
	if (copyDatabaseIcon) ::DeleteObject(copyDatabaseIcon);
	if (exportAsSqlIcon) ::DeleteObject(exportAsSqlIcon);
	if (importFromSqlIcon) ::DeleteObject(importFromSqlIcon);
	if (newMenuIcon) ::DeleteObject(newMenuIcon);
	if (newTableIcon) ::DeleteObject(newTableIcon);
	if (newViewIcon) ::DeleteObject(newViewIcon);
	if (newTriggerIcon) ::DeleteObject(newTriggerIcon);

	if (openViewIcon) ::DeleteObject(openViewIcon);
	if (dropViewIcon) ::DeleteObject(dropViewIcon);
	if (openTriggerIcon) ::DeleteObject(openTriggerIcon);
	if (dropTriggerIcon) ::DeleteObject(dropTriggerIcon);
}


void DatabaseMenuAdapter::createImageList()
{
	std::wstring imgDir = ResourceUtil::getProductImagesDir();
	HINSTANCE ins = ModuleHelper::GetModuleInstance();
	createDatabaseIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\menu\\create-database.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	openDatabaseIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\menu\\open-database.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	refreshDatabaseIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\menu\\refresh-database.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	deleteDatabaseIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\menu\\delete-database.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	copyDatabaseIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\menu\\copy.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	exportAsSqlIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\menu\\export-as-sql.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	importFromSqlIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\menu\\import-from-sql.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	
	newMenuIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\menu\\new-menu.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	newTableIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\menu\\create-table.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	newViewIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\menu\\create-view.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	newTriggerIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\menu\\create-trigger.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);

	openViewIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\menu\\open-view.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	dropViewIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\menu\\drop-view.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);

	openTriggerIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\menu\\open-trigger.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	dropTriggerIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\menu\\drop-trigger.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
}


void DatabaseMenuAdapter::initMenuInfo(HMENU hMenu)
{
	MENUINFO mi;
	mi.cbSize = sizeof(MENUINFO);
	mi.fMask = MIM_BACKGROUND | MIM_STYLE; 
	mi.hbrBack = (HBRUSH)menuBrush;
	mi.dwStyle = MNS_CHECKORBMP;
	::SetMenuInfo(hMenu,&mi);
}

void DatabaseMenuAdapter::createMenu()
{
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING, Config::DATABASE_CREATE_MENU_ID, S(L"database-create").c_str());
	menu.AppendMenu(MF_STRING, Config::DATABASE_OPEN_MENU_ID, S(L"database-open").c_str());
	menu.AppendMenu(MF_STRING, Config::DATABASE_REFRESH_MENU_ID, S(L"database-refresh").c_str());
	menu.AppendMenu(MF_STRING, Config::DATABASE_DELETE_MENU_ID, S(L"database-delete").c_str());
	menu.AppendMenu(MF_STRING, Config::DATABASE_COPY_MENU_ID, S(L"database-copy").c_str());
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, Config::DATABASE_EXPORT_AS_SQL_MENU_ID, S(L"database-export-as-sql").c_str());
	menu.AppendMenu(MF_STRING, Config::DATABASE_IMPORT_FROM_SQL_MENU_ID, S(L"database-import-from-sql").c_str());
	menu.AppendMenu(MF_SEPARATOR);
	// New - the sub popup menu
	newSubMenu.CreatePopupMenu();
	newSubMenu.AppendMenu(MF_STRING, Config::DATABASE_NEW_TABLE_MENU_ID, S(L"database-new-table").c_str());
	newSubMenu.AppendMenu(MF_STRING, Config::DATABASE_NEW_VIEW_MENU_ID, S(L"database-new-view").c_str());
	newSubMenu.AppendMenu(MF_STRING, Config::DATABASE_NEW_TRIGGER_MENU_ID, S(L"database-new-trigger").c_str());

	menu.AppendMenu(MF_STRING, newSubMenu.m_hMenu, S(L"database-new").c_str());

	initMenuInfo(menu.m_hMenu);

	MenuUtil::addIconToMenuItem(menu.m_hMenu, Config::DATABASE_CREATE_MENU_ID, MF_BYCOMMAND, createDatabaseIcon);
	MenuUtil::addIconToMenuItem(menu.m_hMenu, Config::DATABASE_OPEN_MENU_ID, MF_BYCOMMAND, openDatabaseIcon);
	MenuUtil::addIconToMenuItem(menu.m_hMenu, Config::DATABASE_REFRESH_MENU_ID, MF_BYCOMMAND, refreshDatabaseIcon);
	MenuUtil::addIconToMenuItem(menu.m_hMenu, Config::DATABASE_DELETE_MENU_ID, MF_BYCOMMAND, deleteDatabaseIcon);
	MenuUtil::addIconToMenuItem(menu.m_hMenu, Config::DATABASE_COPY_MENU_ID, MF_BYCOMMAND, copyDatabaseIcon);
	MenuUtil::addIconToMenuItem(menu.m_hMenu, Config::DATABASE_EXPORT_AS_SQL_MENU_ID, MF_BYCOMMAND, exportAsSqlIcon);
	MenuUtil::addIconToMenuItem(menu.m_hMenu, Config::DATABASE_IMPORT_FROM_SQL_MENU_ID, MF_BYCOMMAND, importFromSqlIcon);
	MenuUtil::addIconToMenuItem(menu.m_hMenu, Config::DATABASE_NEW_MENU_ID, MF_BYCOMMAND, newMenuIcon);
	MenuUtil::addIconToMenuItem(menu.m_hMenu, Config::DATABASE_NEW_TABLE_MENU_ID, MF_BYCOMMAND, newTableIcon);
	MenuUtil::addIconToMenuItem(menu.m_hMenu, Config::DATABASE_NEW_VIEW_MENU_ID, MF_BYCOMMAND, newViewIcon);
	MenuUtil::addIconToMenuItem(menu.m_hMenu, Config::DATABASE_NEW_TRIGGER_MENU_ID, MF_BYCOMMAND, newTriggerIcon);
}


void DatabaseMenuAdapter::createViewsMenu()
{
	viewsMenu.CreatePopupMenu();
	viewsMenu.AppendMenu(MF_STRING, Config::DATABASE_NEW_VIEW_MENU_ID, S(L"create-view").c_str());
	viewsMenu.AppendMenu(MF_STRING, Config::DATABASE_OPEN_VIEW_MENU_ID, S(L"open-view").c_str());
	viewsMenu.AppendMenu(MF_STRING, Config::DATABASE_DROP_VIEW_MENU_ID, S(L"drop-view").c_str());

	initMenuInfo(viewsMenu.m_hMenu);
	
	MenuUtil::addIconToMenuItem(viewsMenu.m_hMenu, Config::DATABASE_NEW_VIEW_MENU_ID, MF_BYCOMMAND, newViewIcon);
	MenuUtil::addIconToMenuItem(viewsMenu.m_hMenu, Config::DATABASE_OPEN_VIEW_MENU_ID, MF_BYCOMMAND, openViewIcon);
	MenuUtil::addIconToMenuItem(viewsMenu.m_hMenu, Config::DATABASE_DROP_VIEW_MENU_ID, MF_BYCOMMAND, dropViewIcon);
}


void DatabaseMenuAdapter::createTriggersMenu()
{
	triggersMenu.CreatePopupMenu();
	triggersMenu.AppendMenu(MF_STRING, Config::DATABASE_NEW_TRIGGER_MENU_ID, S(L"create-trigger").c_str());
	triggersMenu.AppendMenu(MF_STRING, Config::DATABASE_OPEN_TRIGGER_MENU_ID, S(L"open-trigger").c_str());
	triggersMenu.AppendMenu(MF_STRING, Config::DATABASE_DROP_TRIGGER_MENU_ID, S(L"drop-trigger").c_str());

	initMenuInfo(triggersMenu.m_hMenu);
	
	MenuUtil::addIconToMenuItem(triggersMenu.m_hMenu, Config::DATABASE_NEW_TRIGGER_MENU_ID, MF_BYCOMMAND, newTriggerIcon);
	MenuUtil::addIconToMenuItem(triggersMenu.m_hMenu, Config::DATABASE_OPEN_TRIGGER_MENU_ID, MF_BYCOMMAND, openTriggerIcon);
	MenuUtil::addIconToMenuItem(triggersMenu.m_hMenu, Config::DATABASE_DROP_TRIGGER_MENU_ID, MF_BYCOMMAND, dropTriggerIcon);
}


void DatabaseMenuAdapter::popupMenu(CPoint & pt)
{
	menu.TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, parentHwnd);
}

void DatabaseMenuAdapter::popupViewsMenu(CPoint & pt, bool isViewItem)
{
	UINT uEnable = isViewItem ? MF_BYCOMMAND | MF_ENABLED : MF_BYCOMMAND | MF_DISABLED;
	viewsMenu.EnableMenuItem(Config::DATABASE_OPEN_VIEW_MENU_ID, uEnable);
	viewsMenu.EnableMenuItem(Config::DATABASE_DROP_VIEW_MENU_ID, uEnable);
	viewsMenu.TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, parentHwnd);
}

void DatabaseMenuAdapter::popupTriggersMenu(CPoint & pt, bool isTriggerItem)
{
	UINT uEnable = isTriggerItem ? MF_BYCOMMAND | MF_ENABLED : MF_BYCOMMAND | MF_DISABLED;
	triggersMenu.EnableMenuItem(Config::DATABASE_OPEN_TRIGGER_MENU_ID, uEnable);
	triggersMenu.EnableMenuItem(Config::DATABASE_DROP_TRIGGER_MENU_ID, uEnable);

	triggersMenu.TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, parentHwnd);
}

bool DatabaseMenuAdapter::dropView()
{
	if (QMessageBox::confirm(parentHwnd, S(L"drop-view-confirm-text")) != Config::CUSTOMER_FORM_YES_BUTTON_ID) {
		return false;
	}
	try {
		databaseService->dropView(supplier->getSelectedUserDbId(), supplier->selectedViewName);
		QPopAnimate::success(S(L"drop-view-success-text")); 
		if (AppContext::getInstance()->dispatchForResponse(Config::MSG_DROP_VIEW_ID, NULL, NULL)) {
			CTreeItem selItem = dataView->GetSelectedItem();
			dataView->DeleteItem(selItem.m_hTreeItem);
		}		
		return true;
	} catch (QSqlExecuteException &ex) {
		QPopAnimate::report(ex);
		return false;
	} catch (QRuntimeException &ex) {
		QPopAnimate::report(ex); 
	}
	return false;
}

bool DatabaseMenuAdapter::dropTrigger()
{
	if (QMessageBox::confirm(parentHwnd, S(L"drop-trigger-confirm-text")) != Config::CUSTOMER_FORM_YES_BUTTON_ID) {
		return false;
	}
	try {
		databaseService->dropTrigger(supplier->getSelectedUserDbId(), supplier->selectedTriggerName);
		QPopAnimate::success(S(L"drop-trigger-success-text")); 
		if (AppContext::getInstance()->dispatchForResponse(Config::MSG_DROP_TRIGGER_ID, NULL, NULL)) {
			CTreeItem selItem = dataView->GetSelectedItem();
			dataView->DeleteItem(selItem.m_hTreeItem);
		}
		return true;
	} catch (QSqlExecuteException &ex) {
		QPopAnimate::report(ex);
		return false;
	} catch (QRuntimeException &ex) {
		QPopAnimate::report(ex); 
	}
	return false;
}
