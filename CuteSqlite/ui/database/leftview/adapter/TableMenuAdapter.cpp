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
#include "ui/database/leftview/dialog/RenameTableDialog.h"
#include "ui/common/message/QPopAnimate.h"
#include "ui/common/message/QMessageBox.h"
#include "core/common/exception/QSqlExecuteException.h"
#include "ui/database/leftview/dialog/CopyTableDialog.h"
#include "ui/database/rightview/page/result/dialog/ExportResultDialog.h"

TableMenuAdapter::TableMenuAdapter(HWND parentHwnd, CTreeViewCtrlEx * view)
{
	this->parentHwnd = parentHwnd;
	this->dataView = view;
	menuBrush.CreateSolidBrush(RGB(255,255,255));//RGB(255,128,128));
	createImageList();
	createMenu();
	createColumnsMenu();
	createIndexesMenu();
}

TableMenuAdapter::~TableMenuAdapter()
{
	if (menu.IsMenu()) menu.DestroyMenu();
	if (!menuBrush.IsNull()) menuBrush.DeleteObject();
	if (openTableIcon) ::DeleteObject(openTableIcon);
	if (createTableIcon) ::DeleteObject(createTableIcon);
	if (alterTableIcon) ::DeleteObject(alterTableIcon);
	if (renameTableIcon) ::DeleteObject(renameTableIcon);
	if (trucateTableIcon) ::DeleteObject(trucateTableIcon);
	if (dropTableIcon) ::DeleteObject(dropTableIcon);
	if (copyTableIcon) ::DeleteObject(copyTableIcon);
	if (shardingTableIcon) ::DeleteObject(shardingTableIcon);
	if (exportTableIcon) ::DeleteObject(exportTableIcon);
	if (importFromSqlIcon) ::DeleteObject(importFromSqlIcon);
	if (importFromCsvIcon) ::DeleteObject(importFromCsvIcon);
	if (manageIndexesIcon) ::DeleteObject(manageIndexesIcon);
	if (manageForeignKeyIcon) ::DeleteObject(manageForeignKeyIcon);
	if (propertiesIcon) ::DeleteObject(propertiesIcon);

	if (manageColumnsIcon) ::DeleteObject(manageColumnsIcon);
	if (dropColumnIcon) ::DeleteObject(dropColumnIcon);
	if (dropIndexIcon) ::DeleteObject(dropIndexIcon);
}

void TableMenuAdapter::initMenuInfo(HMENU hMenu)
{
	MENUINFO mi;
	mi.cbSize = sizeof(MENUINFO);
	mi.fMask = MIM_BACKGROUND | MIM_STYLE; 
	mi.hbrBack = (HBRUSH)menuBrush;
	mi.dwStyle = MNS_CHECKORBMP;
	::SetMenuInfo(hMenu,&mi);
}

void TableMenuAdapter::createImageList()
{
	std::wstring imgDir = ResourceUtil::getProductImagesDir();
	HINSTANCE ins = ModuleHelper::GetModuleInstance();
	openTableIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\menu\\open-table.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	createTableIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\menu\\create-table.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	alterTableIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\menu\\alter-table.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	renameTableIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\menu\\rename-table.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	trucateTableIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\menu\\truncate-table.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	dropTableIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\menu\\drop-table.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	copyTableIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\menu\\copy.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	shardingTableIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\menu\\sharding.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	exportTableIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\menu\\export-as-sql.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	importFromSqlIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\menu\\import-from-sql.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	importFromCsvIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\menu\\import-from-csv.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	manageIndexesIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\menu\\manage-index.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	manageForeignKeyIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\menu\\manage-foreign-key.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	propertiesIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\menu\\properties.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);

	manageColumnsIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\menu\\manager-columns.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	dropColumnIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\menu\\drop-column.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	dropIndexIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\menu\\drop-index.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
}

void TableMenuAdapter::createMenu()
{
	menu.CreatePopupMenu();
	menu.AppendMenu(MF_STRING, Config::TABLE_OPEN_MENU_ID, S(L"table-open").c_str()); 
	menu.AppendMenu(MF_STRING, Config::TABLE_CREATE_MENU_ID, S(L"table-create").c_str()); 
	menu.AppendMenu(MF_STRING, Config::TABLE_ALTER_MENU_ID, S(L"table-alter").c_str());
	menu.AppendMenu(MF_STRING, Config::TABLE_RENAME_MENU_ID, S(L"table-rename").c_str());
	menu.AppendMenu(MF_STRING, Config::TABLE_TRUNCATE_MENU_ID, S(L"table-truncate").c_str());
	menu.AppendMenu(MF_STRING, Config::TABLE_DROP_MENU_ID, S(L"table-drop").c_str());
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, Config::TABLE_COPY_MENU_ID, S(L"table-copy-as").c_str());
	menu.AppendMenu(MF_STRING, Config::TABLE_SHARDING_MENU_ID, S(L"table-sharding-as").c_str());
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, Config::TABLE_EXPORT_MENU_ID, S(L"table-export").c_str());
	menu.AppendMenu(MF_STRING, Config::TABLE_IMPORT_SQL_MENU_ID, S(L"table-import-sql").c_str());
	menu.AppendMenu(MF_STRING, Config::TABLE_IMPORT_CSV_MENU_ID, S(L"table-import-csv").c_str());
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, Config::TABLE_MANAGE_INDEX_MENU_ID, S(L"table-manage-index").c_str());
	menu.AppendMenu(MF_STRING, Config::TABLE_MANAGE_FOREIGNKEY_MENU_ID, S(L"table-manage-foreign-key").c_str());
	menu.AppendMenu(MF_SEPARATOR);
	menu.AppendMenu(MF_STRING, Config::TABLE_PROPERTIES_MENU_ID, S(L"properties").c_str());

	initMenuInfo(menu.m_hMenu);

	MenuUtil::addIconToMenuItem(menu.m_hMenu, Config::TABLE_OPEN_MENU_ID, MF_BYCOMMAND, openTableIcon);
	MenuUtil::addIconToMenuItem(menu.m_hMenu, Config::TABLE_CREATE_MENU_ID, MF_BYCOMMAND, createTableIcon);
	MenuUtil::addIconToMenuItem(menu.m_hMenu, Config::TABLE_ALTER_MENU_ID, MF_BYCOMMAND, alterTableIcon);
	MenuUtil::addIconToMenuItem(menu.m_hMenu, Config::TABLE_RENAME_MENU_ID, MF_BYCOMMAND, renameTableIcon);
	MenuUtil::addIconToMenuItem(menu.m_hMenu, Config::TABLE_TRUNCATE_MENU_ID, MF_BYCOMMAND, trucateTableIcon);
	MenuUtil::addIconToMenuItem(menu.m_hMenu, Config::TABLE_DROP_MENU_ID, MF_BYCOMMAND, dropTableIcon);
	MenuUtil::addIconToMenuItem(menu.m_hMenu, Config::TABLE_COPY_MENU_ID, MF_BYCOMMAND, copyTableIcon);
	MenuUtil::addIconToMenuItem(menu.m_hMenu, Config::TABLE_SHARDING_MENU_ID, MF_BYCOMMAND, shardingTableIcon);
	MenuUtil::addIconToMenuItem(menu.m_hMenu, Config::TABLE_EXPORT_MENU_ID, MF_BYCOMMAND, exportTableIcon);
	MenuUtil::addIconToMenuItem(menu.m_hMenu, Config::TABLE_IMPORT_SQL_MENU_ID, MF_BYCOMMAND, importFromSqlIcon);
	MenuUtil::addIconToMenuItem(menu.m_hMenu, Config::TABLE_IMPORT_CSV_MENU_ID, MF_BYCOMMAND, importFromCsvIcon);
	MenuUtil::addIconToMenuItem(menu.m_hMenu, Config::TABLE_MANAGE_INDEX_MENU_ID, MF_BYCOMMAND, manageIndexesIcon);
	MenuUtil::addIconToMenuItem(menu.m_hMenu, Config::TABLE_MANAGE_FOREIGNKEY_MENU_ID, MF_BYCOMMAND, manageForeignKeyIcon);
	MenuUtil::addIconToMenuItem(menu.m_hMenu, Config::TABLE_PROPERTIES_MENU_ID, MF_BYCOMMAND, propertiesIcon);
	
}


void TableMenuAdapter::createColumnsMenu()
{
	columnsMenu.CreatePopupMenu();
	columnsMenu.AppendMenu(MF_STRING, Config::TABLE_MANAGE_COLUMNS_MENU_ID, S(L"table-manage-columns").c_str()); 
	columnsMenu.AppendMenu(MF_STRING, Config::TABLE_DROP_COLUMN_MENU_ID, S(L"table-drop-column").c_str());

	initMenuInfo(columnsMenu.m_hMenu);

	MenuUtil::addIconToMenuItem(columnsMenu.m_hMenu, Config::TABLE_MANAGE_COLUMNS_MENU_ID, MF_BYCOMMAND, manageColumnsIcon);
	MenuUtil::addIconToMenuItem(columnsMenu.m_hMenu, Config::TABLE_DROP_COLUMN_MENU_ID, MF_BYCOMMAND, dropColumnIcon); 
}

void TableMenuAdapter::createIndexesMenu()
{
	indexesMenu.CreatePopupMenu();
	indexesMenu.AppendMenu(MF_STRING, Config::TABLE_MANAGE_INDEX_MENU_ID, S(L"table-manage-indexes").c_str()); 
	indexesMenu.AppendMenu(MF_STRING, Config::TABLE_DROP_INDEX_MENU_ID, S(L"table-drop-index").c_str());

	initMenuInfo(indexesMenu.m_hMenu);

	MenuUtil::addIconToMenuItem(indexesMenu.m_hMenu, Config::TABLE_MANAGE_INDEX_MENU_ID, MF_BYCOMMAND, manageIndexesIcon);
	MenuUtil::addIconToMenuItem(indexesMenu.m_hMenu, Config::TABLE_DROP_INDEX_MENU_ID, MF_BYCOMMAND, dropIndexIcon);
}

bool TableMenuAdapter::renameTable()
{
	RenameTableDialog dialog(parentHwnd);
	if (dialog.DoModal(parentHwnd) != Config::QDIALOG_YES_BUTTON_ID) {
		return false;
	}
	try {
		tableService->renameTable(supplier->getSelectedUserDbId(), supplier->oldTableName, supplier->newTableName, supplier->selectedSchema);
		QPopAnimate::success(S(L"rename-table-success-text"));
		if (AppContext::getInstance()->dispatchForResponse(Config::MSG_RENAME_TABLE_ID, NULL, NULL)) {
			CTreeItem selItem = dataView->GetSelectedItem();
			selItem.SetText(supplier->newTableName.c_str());
			supplier->selectedTable = supplier->newTableName;
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


bool TableMenuAdapter::truncateTable()
{
	if (QMessageBox::confirm(parentHwnd, S(L"truncate-table-confirm-text")) != Config::CUSTOMER_FORM_YES_BUTTON_ID) {
		return false;
	}
	try {
		tableService->truncateTable(supplier->getSelectedUserDbId(), supplier->selectedTable, supplier->selectedSchema);
		QPopAnimate::success(S(L"truncate-table-success-text")); 
		AppContext::getInstance()->dispatch(Config::MSG_REFRESH_SAME_TABLE_DATA_ID,
			WPARAM(supplier->getSelectedUserDbId()), LPARAM(&supplier->selectedTable));
		return true;
	} catch (QSqlExecuteException &ex) {
		QPopAnimate::report(ex);
		return false;
	} catch (QRuntimeException &ex) {
		QPopAnimate::report(ex);
	}
	return false;
}


bool TableMenuAdapter::dropTable()
{
	if (QMessageBox::confirm(parentHwnd, S(L"drop-table-confirm-text")) != Config::CUSTOMER_FORM_YES_BUTTON_ID) {
		return false;
	}
	try {
		tableService->dropTable(supplier->getSelectedUserDbId(), supplier->selectedTable, supplier->selectedSchema);
		QPopAnimate::success(S(L"drop-table-success-text")); 
		if (AppContext::getInstance()->dispatchForResponse(Config::MSG_DROP_TABLE_ID, NULL, NULL)) {
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


bool TableMenuAdapter::copyTable()
{
	CopyTableDialog dialog(parentHwnd);
	if (dialog.DoModal(parentHwnd) == Config::QDIALOG_YES_BUTTON_ID) {
		return true;
	}
	return false;
}


bool TableMenuAdapter::shardingTable()
{
	CopyTableDialog dialog(parentHwnd, true);
	if (dialog.DoModal(parentHwnd) == Config::QDIALOG_YES_BUTTON_ID) {
		return true;
	}
	return false;
}


bool TableMenuAdapter::exportTable()
{
	ResultListPageAdapter resultAdapter(parentHwnd, nullptr);

	UserTableStrings tblStrings({ supplier->selectedTable });
	Columns tblColumns = tableService->getUserColumnStrings(supplier->getSelectedUserDbId(), supplier->selectedTable, supplier->selectedSchema);
	DataList tblDatas = tableService->getTableDataList(supplier->getSelectedUserDbId(), 
		supplier->selectedTable, 1, 100000000, supplier->selectedSchema);	
	resultAdapter.setRuntimeUserDbId(supplier->getSelectedUserDbId());
	resultAdapter.setRuntimeTables(tblStrings);
	resultAdapter.setRuntimeColumns(tblColumns);
	resultAdapter.setRuntimeDatas(tblDatas);
	ExportResultDialog dialog(parentHwnd, &resultAdapter);
	if (dialog.DoModal(parentHwnd) == Config::QDIALOG_YES_BUTTON_ID) {
		return true;
	}
	return false;
}


void TableMenuAdapter::manageIndex()
{
	// wParam == 1, active the Indexes tab page in the TableStructurePage
	AppContext::getInstance()->dispatch(Config::MSG_ALTER_TABLE_ID, TABLE_INDEXS_PAGE, NULL);
}


void TableMenuAdapter::manageColumn()
{
	// wParam == 0, active the column tab page in the TableStructurePage
	AppContext::getInstance()->dispatch(Config::MSG_ALTER_TABLE_ID, TABLE_COLUMNS_PAGE, NULL);
}

void TableMenuAdapter::dropColumn()
{
	// wParam == 0, active the column tab page in the TableStructurePage
	AppContext::getInstance()->dispatch(Config::MSG_ALTER_TABLE_ID, TABLE_COLUMNS_PAGE, NULL);
}


void TableMenuAdapter::dropIndex()
{
	// wParam == 0, active the Indexes tab page in the TableStructurePage
	AppContext::getInstance()->dispatch(Config::MSG_ALTER_TABLE_ID, TABLE_INDEXS_PAGE, NULL);
}

void TableMenuAdapter::manageForeignKey()
{
	// wParam == 1, active the Foreign Key tab page in the TableStructurePage
	AppContext::getInstance()->dispatch(Config::MSG_ALTER_TABLE_ID, TABLE_FOREIGN_KEYS_PAGE, NULL);
}

void TableMenuAdapter::popupMenu(CPoint & pt)
{
	menu.TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, parentHwnd);
}


void TableMenuAdapter::popupColumnsMenu(CPoint & pt, bool isColumnItem)
{
	UINT uEnable = isColumnItem ? MF_BYCOMMAND | MF_ENABLED : MF_BYCOMMAND | MF_DISABLED;
	columnsMenu.EnableMenuItem(Config::TABLE_DROP_COLUMN_MENU_ID, uEnable);
	columnsMenu.TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, parentHwnd);
}


void TableMenuAdapter::popupIndexesMenu(CPoint & pt, bool isIndexItem)
{
	UINT uEnable = isIndexItem ? MF_BYCOMMAND | MF_ENABLED : MF_BYCOMMAND | MF_DISABLED;
	indexesMenu.EnableMenuItem(Config::TABLE_DROP_INDEX_MENU_ID, uEnable);
	indexesMenu.TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, parentHwnd);
}

/**
 * open table for show table data.
 * 
 * @param userDbId
 * @param tblName
 * @param schema
 */
void TableMenuAdapter::openTable()
{
	AppContext::getInstance()->dispatch(Config::MSG_SHOW_TABLE_DATA_ID, NULL, NULL);
}


void TableMenuAdapter::showProperties()
{
	AppContext::getInstance()->dispatch(Config::MSG_SHOW_TABLE_DATA_ID, 1, NULL);
}
