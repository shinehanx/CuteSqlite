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

DatabaseMenuAdapter::DatabaseMenuAdapter(HWND parentHwnd, CTreeViewCtrlEx * view)
{
	this->parentHwnd = parentHwnd;
	this->dataView = view;

	createImageList();
	createMenu();
}

DatabaseMenuAdapter::~DatabaseMenuAdapter()
{
	menu.DestroyMenu();
	newSubMenu.DestroyMenu();
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

	menuBrush.CreateSolidBrush(RGB(255,255,255));//RGB(255,128,128));
	MENUINFO mi;
	mi.cbSize = sizeof(MENUINFO);
	mi.fMask = MIM_BACKGROUND | MIM_STYLE; 
	mi.hbrBack = (HBRUSH)menuBrush;
	mi.dwStyle = MNS_CHECKORBMP;
	::SetMenuInfo((HMENU)(menu.m_hMenu),&mi);

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

void DatabaseMenuAdapter::popupMenu(CPoint & pt)
{
	menu.TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, parentHwnd);
}