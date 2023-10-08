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

DatabaseMenuAdapter::DatabaseMenuAdapter(HWND parentHwnd, CTreeViewCtrlEx * view)
{
	this->parentHwnd = parentHwnd;
	this->dataView = view;

	createMenu();
}

DatabaseMenuAdapter::~DatabaseMenuAdapter()
{

}

void DatabaseMenuAdapter::createMenu()
{
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING, Config::DATABASE_CREATE_MENU_ID, S(L"database-create").c_str());
	menu.AppendMenu(MF_STRING, Config::DATABASE_OPEN_MENU_ID, S(L"database-open").c_str());
	menu.AppendMenu(MF_STRING, Config::DATABASE_REFRESH_MENU_ID, S(L"database-refresh").c_str());
	menu.AppendMenu(MF_STRING, Config::DATABASE_DELETE_MENU_ID, S(L"database-delete").c_str());
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
}

void DatabaseMenuAdapter::popupMenu(CPoint & pt)
{
	menu.TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, parentHwnd);
}