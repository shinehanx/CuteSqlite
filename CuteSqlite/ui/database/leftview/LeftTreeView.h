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

 * @file   LeftTreeView.h
 * @brief  Left tree view for splitter
 * 
 * @author Xuehan Qin
 * @date   2023-05-19
 *********************************************************************/
#pragma once
#include <atlwin.h>
#include <atlcrack.h>
#include <atltypes.h>
#include "core/entity/Entity.h"
#include "core/service/db/DatabaseService.h"
#include "ui/common/button/QImageButton.h"
#include "ui/database/leftview/adapter/LeftTreeViewAdapter.h"
#include "ui/database/leftview/adapter/DatabaseMenuAdapter.h"
#include "ui/database/leftview/adapter/TableMenuAdapter.h"
#include "ui/database/dialog/adapter/ExportDatabaseAdapter.h"
#include "ui/database/dialog/adapter/ImportDatabaseAdapter.h"
#include "ui/common/treeview/QTreeViewCtrl.h"

class LeftTreeView : public CWindowImpl<LeftTreeView> 
{
public:
	BOOL PreTranslateMessage(MSG* pMsg);
	~LeftTreeView();

	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP_EX(LeftTreeView)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_SIZE(OnSize)
		MSG_WM_SHOWWINDOW(OnShowWindow)
		MSG_WM_PAINT(OnPaint)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MSG_WM_CTLCOLORLISTBOX(OnCtlColorListBox)
		MSG_WM_CTLCOLOREDIT(OnCtlColorEdit)

		NOTIFY_HANDLER(Config::DATABASE_TREEVIEW_ID, TVN_SELCHANGED, OnChangedTreeViewItem)
		NOTIFY_HANDLER(Config::DATABASE_TREEVIEW_ID, TVN_ITEMEXPANDING, OnExpendingTreeViewItem)
		NOTIFY_HANDLER(Config::DATABASE_TREEVIEW_ID, TVN_GETINFOTIP, OnShowTreeViewItemToolTip)
		NOTIFY_HANDLER(Config::DATABASE_TREEVIEW_ID, NM_DBLCLK, OnDbClickTreeViewItem)
		NOTIFY_HANDLER(Config::DATABASE_TREEVIEW_ID, NM_RCLICK, OnRightClickTreeViewItem)
		COMMAND_HANDLER_EX(Config::TREEVIEW_SELECTED_DB_COMBOBOX_ID, CBN_SELENDOK, OnChangeSelectDbComboBox)

		// buttons
		COMMAND_HANDLER_EX(Config::DATABASE_CREATE_BUTTON_ID, BN_CLICKED, OnClickCreateDatabaseButton)
		COMMAND_HANDLER_EX(Config::DATABASE_OPEN_BUTTON_ID, BN_CLICKED, OnClickOpenDatabaseButton)
		COMMAND_HANDLER_EX(Config::DATABASE_REFRESH_BUTTON_ID, BN_CLICKED, OnClickRefreshDatabaseButton)
		COMMAND_HANDLER_EX(Config::DATABASE_DELETE_BUTTON_ID, BN_CLICKED, OnClickDeleteDatabaseButton)

		// database menus
		COMMAND_ID_HANDLER_EX(Config::DATABASE_CREATE_MENU_ID, OnClickCreateDatabaseMenu)
		COMMAND_ID_HANDLER_EX(Config::DATABASE_OPEN_MENU_ID, OnClickOpenDatabaseMenu)
		COMMAND_ID_HANDLER_EX(Config::DATABASE_REFRESH_MENU_ID, OnClickRefreshDatabaseMenu)
		COMMAND_ID_HANDLER_EX(Config::DATABASE_DELETE_MENU_ID, OnClickDeleteDatabaseMenu)
		COMMAND_ID_HANDLER_EX(Config::DATABASE_COPY_MENU_ID, OnClickCopyDatabaseMenu)
		COMMAND_ID_HANDLER_EX(Config::DATABASE_EXPORT_AS_SQL_MENU_ID, OnClickExportAsSqlMenu)
		COMMAND_ID_HANDLER_EX(Config::DATABASE_IMPORT_FROM_SQL_MENU_ID, OnClickImportFromSqlMenu)
		COMMAND_ID_HANDLER_EX(Config::DATABASE_NEW_TABLE_MENU_ID, OnClickNewTableMenu)
		COMMAND_ID_HANDLER_EX(Config::DATABASE_NEW_VIEW_MENU_ID, OnClickNewViewMenu)
		COMMAND_ID_HANDLER_EX(Config::DATABASE_NEW_TRIGGER_MENU_ID, OnClickNewTriggerMenu)

		// view menus
		COMMAND_ID_HANDLER_EX(Config::DATABASE_OPEN_VIEW_MENU_ID, OnClickOpenViewMenu)
		COMMAND_ID_HANDLER_EX(Config::DATABASE_DROP_VIEW_MENU_ID, OnClickDropViewMenu)

		// trigger menus
		COMMAND_ID_HANDLER_EX(Config::DATABASE_OPEN_TRIGGER_MENU_ID, OnClickOpenTriggerMenu)
		COMMAND_ID_HANDLER_EX(Config::DATABASE_DROP_TRIGGER_MENU_ID, OnClickDropTriggerMenu)

		// table menus
		COMMAND_ID_HANDLER_EX(Config::TABLE_OPEN_MENU_ID, OnClickOpenTableMenu)
		COMMAND_ID_HANDLER_EX(Config::TABLE_CREATE_MENU_ID, OnClickNewTableMenu)
		COMMAND_ID_HANDLER_EX(Config::TABLE_ALTER_MENU_ID, OnClickAlterTableMenu)
		COMMAND_ID_HANDLER_EX(Config::TABLE_RENAME_MENU_ID, OnClickRenameTableMenu)
		COMMAND_ID_HANDLER_EX(Config::TABLE_TRUNCATE_MENU_ID, OnClickTruncateTableMenu)
		COMMAND_ID_HANDLER_EX(Config::TABLE_DROP_MENU_ID, OnClickDropTableMenu)
		COMMAND_ID_HANDLER_EX(Config::TABLE_COPY_MENU_ID, OnClickCopyTableMenu)
		COMMAND_ID_HANDLER_EX(Config::TABLE_SHARDING_MENU_ID, OnClickShardingTableMenu)
		COMMAND_ID_HANDLER_EX(Config::TABLE_EXPORT_MENU_ID, OnClickExportTableMenu)
		COMMAND_ID_HANDLER_EX(Config::TABLE_IMPORT_SQL_MENU_ID, OnClickImportFromSqlMenu)
		COMMAND_ID_HANDLER_EX(Config::TABLE_IMPORT_CSV_MENU_ID, OnClickImportFromCsvMenu)
		COMMAND_ID_HANDLER_EX(Config::TABLE_MANAGE_INDEX_MENU_ID, OnClickManageIndexMenu)
		COMMAND_ID_HANDLER_EX(Config::TABLE_MANAGE_FOREIGNKEY_MENU_ID, OnClickManageForeignKeyMenu)
		COMMAND_ID_HANDLER_EX(Config::TABLE_PROPERTIES_MENU_ID, OnClickPropertiesMenu)

		// column menu
		COMMAND_ID_HANDLER_EX(Config::TABLE_MANAGE_COLUMNS_MENU_ID, OnClickManageColumnsMenu)
		COMMAND_ID_HANDLER_EX(Config::TABLE_DROP_COLUMN_MENU_ID, OnClickDropColumnMenu)
		COMMAND_ID_HANDLER_EX(Config::TABLE_DROP_INDEX_MENU_ID, OnClickDropIndexMenu)

		// MSG
		MESSAGE_HANDLER_EX(Config::MSG_LEFTVIEW_REFRESH_DATABASE_ID, OnRefreshDatabase)
		MESSAGE_HANDLER_EX(Config::MSG_LEFTVIEW_CREATE_DATABASE_ID, OnCreateDatabase)
		MESSAGE_HANDLER_EX(Config::MSG_LEFTVIEW_RENAME_TABLE_ID, OnRenameTable)
		MESSAGE_HANDLER_EX(Config::MSG_LEFTVIEW_TRUNCATE_TABLE_ID, OnTruncateTable)
		MESSAGE_HANDLER_EX(Config::MSG_LEFTVIEW_DROP_TABLE_ID, OnDropTable)
		MESSAGE_HANDLER_EX(Config::MSG_LEFTVIEW_COPY_TABLE_ID, OnCopyTable)
		MESSAGE_HANDLER_EX(Config::MSG_LEFTVIEW_SHARDING_TABLE_ID, OnShardingTable)
		MESSAGE_HANDLER_EX(Config::MSG_LEFTVIEW_EXPORT_TABLE_ID, OnExportTable)
		MESSAGE_HANDLER_EX(Config::MSG_LEFTVIEW_IMPORT_TABLE_SQL_ID, OnImportTableFromSql)
		MESSAGE_HANDLER_EX(Config::MSG_LEFTVIEW_IMPORT_TABLE_CSV_ID, OnImportTableFromCsv)
		MESSAGE_HANDLER_EX(Config::MSG_DROP_FILES_ID, OnDropFiles)
		MESSAGE_HANDLER_EX(Config::MSG_LEFTVIEW_SELECT_TABLE_ID, OnSelectTable)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
private:
	bool isNeedReload = true;
	COLORREF bkgColor = RGB(255, 255, 255);
	CBrush bkgBrush;
	COLORREF topbarColor = RGB(81, 81, 81);
	CBrush topbarBrush;
	HFONT comboFont = nullptr;

	HACCEL m_hAccel = nullptr;

	QImageButton createDbButton;
	QImageButton openDbButton;
	QImageButton refreshDbButton;
	QImageButton deleteDbButton;
	CComboBox selectedDbComboBox;

	QTreeViewCtrl treeView;

	// refreshLock 
	bool refreshLock = false;
	
	// singleton pointer
	LeftTreeViewAdapter * treeViewAdapter = nullptr;
	DatabaseMenuAdapter * databaseMenuAdapter = nullptr;
	ExportDatabaseAdapter * exportDatabaseAdapter = nullptr;
	ImportDatabaseAdapter * importDatabaseAdapter = nullptr;
	TableMenuAdapter * tableMenuAdapter = nullptr;
	DatabaseService * databaseService = DatabaseService::getInstance();
	DatabaseSupplier * databaseSupplier = DatabaseSupplier::getInstance();

	CRect getTopRect(CRect & clientRect);
	CRect getTreeRect(CRect & clientRect);

	void createOrShowUI();
	void createOrShowCreateDbButton(QImageButton & win, CRect &clientRect);
	void createOrShowOpenDbButton(QImageButton & win, CRect &clientRect);
	void createOrShowRefreshDbButton(QImageButton & win, CRect &clientRect);
	void createOrShowDeleteDbButton(QImageButton & win, CRect &clientRect);
	void createOrShowTreeView(QTreeViewCtrl & win, CRect & clientRect);
	void createOrShowSelectedDbComboBox(CComboBox & win, CRect & clientRect);

	void loadWindow();
	void loadComboBox();
	void selectComboBox(uint64_t userDbId);

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	int OnDestroy();
	void OnSize(UINT nType, CSize size);
	void OnShowWindow(BOOL bShow, UINT nStatus);
	void OnPaint(CDCHandle dc);
	BOOL OnEraseBkgnd(CDCHandle dc);
	HBRUSH OnCtlColorListBox(HDC hdc, HWND hwnd);
	HBRUSH OnCtlColorEdit(HDC hdc, HWND hwnd);

	
	// change selected treeview item .
	LRESULT OnChangedTreeViewItem(int wParam, LPNMHDR lParam, BOOL& bHandled);
	LRESULT OnExpendingTreeViewItem(int wParam, LPNMHDR lParam, BOOL& bHandled);

	LRESULT OnShowTreeViewItemToolTip(int wParam, LPNMHDR lParam, BOOL& bHandled);
	// double click treeview item .
	LRESULT OnDbClickTreeViewItem(int wParam, LPNMHDR lParam, BOOL& bHandled);
	// right click treeview item
	LRESULT OnRightClickTreeViewItem(int wParam, LPNMHDR lParam, BOOL &bHandled);

	//buttons
	LRESULT OnClickCreateDatabaseButton(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnClickOpenDatabaseButton(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnClickRefreshDatabaseButton(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnClickDeleteDatabaseButton(UINT uNotifyCode, int nID, HWND hwnd);

	LRESULT OnChangeSelectDbComboBox(UINT uNotifyCode, int nID, HWND hwnd);

	// database menus
	void OnClickCreateDatabaseMenu(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickOpenDatabaseMenu(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickRefreshDatabaseMenu(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickDeleteDatabaseMenu(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickCopyDatabaseMenu(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickExportAsSqlMenu(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickImportFromSqlMenu(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickManageIndexMenu(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickManageForeignKeyMenu(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickPropertiesMenu(UINT uNotifyCode, int nID, HWND hwnd);

	void OnClickManageColumnsMenu(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickDropColumnMenu(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickDropIndexMenu(UINT uNotifyCode, int nID, HWND hwnd);

	void OnClickNewViewMenu(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickNewTriggerMenu(UINT uNotifyCode, int nID, HWND hwnd);

	// view menus
	void OnClickOpenViewMenu(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickDropViewMenu(UINT uNotifyCode, int nID, HWND hwnd);

	// trigger menus
	void OnClickOpenTriggerMenu(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickDropTriggerMenu(UINT uNotifyCode, int nID, HWND hwnd);
	
	// table menus
	void OnClickOpenTableMenu(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickNewTableMenu(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickAlterTableMenu(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickRenameTableMenu(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickTruncateTableMenu(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickDropTableMenu(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickCopyTableMenu(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickShardingTableMenu(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickExportTableMenu(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickImportFromCsvMenu(UINT uNotifyCode, int nID, HWND hwnd);


	// MSG
	LRESULT OnRefreshDatabase(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCreateDatabase(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnRenameTable(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnTruncateTable(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDropTable(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCopyTable(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnShardingTable(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnExportTable(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnImportTableFromSql(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnImportTableFromCsv(UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT OnDropFiles(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnSelectTable(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void doRefreshDatabase();
	void doDeleteDatabase();
	void doCreateDatabase();
	void doOpenDatabase();
	void doCopyDatabase();
	void doExportAsSql();
	void doImportFromSql();
	void doImportFromCsv();
	void doNewView();
	void doNewTrigger();
	void doNewTable();
	void doAlterTable();
	void doRenameTable();
};
