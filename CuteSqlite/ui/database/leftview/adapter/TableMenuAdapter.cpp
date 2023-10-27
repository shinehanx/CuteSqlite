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
 *		   This class will impletement all the features of table popmenu   
 * 
 * @author Xuehan Qin
 * @date   2023-08-17
 *********************************************************************/

#include "stdafx.h"
#include "TableMenuAdapter.h"
#include "core/common/Lang.h"
#include "utils/MenuUtil.h"
#include "common/AppContext.h"

TableMenuAdapter::TableMenuAdapter(HWND parentHwnd, CTreeViewCtrlEx * view)
{
	this->parentHwnd = parentHwnd;
	this->dataView = view;
	createImageList();
	createMenu();	
}

TableMenuAdapter::~TableMenuAdapter()
{
	menu.DestroyMenu();
	if (!menuBrush.IsNull()) menuBrush.DeleteObject();
	if (openTableIcon) ::DeleteObject(openTableIcon);
	if (createTableIcon) ::DeleteObject(createTableIcon);
	if (alterTableIcon) ::DeleteObject(alterTableIcon);
	if (trucateTableIcon) ::DeleteObject(trucateTableIcon);
	if (dropTableIcon) ::DeleteObject(dropTableIcon);
	if (copyTableIcon) ::DeleteObject(copyTableIcon);
	if (exportTableIcon) ::DeleteObject(exportTableIcon);
	if (importFromSqlIcon) ::DeleteObject(importFromSqlIcon);
	if (importFromCsvIcon) ::DeleteObject(importFromCsvIcon);
	if (manageIndexIcon) ::DeleteObject(manageIndexIcon);
	if (propertiesIcon) ::DeleteObject(propertiesIcon);
}


void TableMenuAdapter::createImageList()
{
	std::wstring imgDir = ResourceUtil::getProductImagesDir();
	HINSTANCE ins = ModuleHelper::GetModuleInstance();
	openTableIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\menu\\open-table.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	createTableIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\menu\\create-table.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	alterTableIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\menu\\alter-table.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	trucateTableIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\menu\\truncate-table.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	dropTableIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\menu\\drop-table.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	copyTableIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\menu\\copy.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	exportTableIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\menu\\export-as-sql.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	importFromSqlIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\menu\\import-from-sql.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	importFromCsvIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\menu\\import-from-csv.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	manageIndexIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\menu\\manage-index.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	propertiesIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\menu\\properties.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
}

void TableMenuAdapter::createMenu()
{
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING, Config::TABLE_OPEN_MENU_ID, S(L"table-open").c_str());
	menu.AppendMenu(MF_STRING, Config::TABLE_CREATE_MENU_ID, S(L"table-create").c_str()); 
	menu.AppendMenu(MF_STRING, Config::TABLE_ALTER_MENU_ID, S(L"table-alter").c_str());
	menu.AppendMenu(MF_STRING, Config::TABLE_TRUCATE_MENU_ID, S(L"table-truncate").c_str());
	menu.AppendMenu(MF_STRING, Config::TABLE_DROP_MENU_ID, S(L"table-drop").c_str());
	menu.AppendMenu(MF_STRING, Config::TABLE_COPY_MENU_ID, S(L"table-copy-as").c_str());
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, Config::TABLE_EXPORT_MENU_ID, S(L"table-export").c_str());
	menu.AppendMenu(MF_STRING, Config::TABLE_IMPORT_SQL_MENU_ID, S(L"table-import-sql").c_str());
	menu.AppendMenu(MF_STRING, Config::TABLE_IMPORT_CSV_MENU_ID, S(L"table-import-csv").c_str());
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, Config::TABLE_MANAGE_INDEX_MENU_ID, S(L"table-manage-index").c_str());
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, Config::TABLE_PROPERTIES_MENU_ID, S(L"properties").c_str());

	menuBrush.CreateSolidBrush(RGB(255,255,255));//RGB(255,128,128));
	MENUINFO mi;
	mi.cbSize = sizeof(MENUINFO);
	mi.fMask = MIM_BACKGROUND | MIM_STYLE;
	mi.hbrBack = (HBRUSH)menuBrush;
	mi.dwStyle = MNS_CHECKORBMP;
	::SetMenuInfo((HMENU)(menu.m_hMenu),&mi);

	MenuUtil::addIconToMenuItem(menu.m_hMenu, Config::TABLE_OPEN_MENU_ID, MF_BYCOMMAND, openTableIcon);
	MenuUtil::addIconToMenuItem(menu.m_hMenu, Config::TABLE_CREATE_MENU_ID, MF_BYCOMMAND, createTableIcon);
	MenuUtil::addIconToMenuItem(menu.m_hMenu, Config::TABLE_ALTER_MENU_ID, MF_BYCOMMAND, alterTableIcon);
	MenuUtil::addIconToMenuItem(menu.m_hMenu, Config::TABLE_TRUCATE_MENU_ID, MF_BYCOMMAND, trucateTableIcon);
	MenuUtil::addIconToMenuItem(menu.m_hMenu, Config::TABLE_DROP_MENU_ID, MF_BYCOMMAND, dropTableIcon);
	MenuUtil::addIconToMenuItem(menu.m_hMenu, Config::TABLE_COPY_MENU_ID, MF_BYCOMMAND, copyTableIcon);
	MenuUtil::addIconToMenuItem(menu.m_hMenu, Config::TABLE_EXPORT_MENU_ID, MF_BYCOMMAND, exportTableIcon);
	MenuUtil::addIconToMenuItem(menu.m_hMenu, Config::TABLE_IMPORT_SQL_MENU_ID, MF_BYCOMMAND, importFromSqlIcon);
	MenuUtil::addIconToMenuItem(menu.m_hMenu, Config::TABLE_IMPORT_CSV_MENU_ID, MF_BYCOMMAND, importFromCsvIcon);
	MenuUtil::addIconToMenuItem(menu.m_hMenu, Config::TABLE_MANAGE_INDEX_MENU_ID, MF_BYCOMMAND, manageIndexIcon);
	MenuUtil::addIconToMenuItem(menu.m_hMenu, Config::TABLE_PROPERTIES_MENU_ID, MF_BYCOMMAND, propertiesIcon);
	
}

void TableMenuAdapter::popupMenu(CPoint & pt)
{
	menu.TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, parentHwnd);
}

/**
 * open table for show table data.
 * 
 * @param userDbId
 * @param tblName
 * @param schema
 */
void TableMenuAdapter::openTable(uint64_t userDbId, const std::wstring & tblName, const std::wstring & schema)
{
	AppContext::getInstance()->dispatch(Config::MSG_SHOW_TABLE_DATA_ID, NULL, NULL);
}
