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
#include "ui/database/rightview/page/result/adapter/ResultListPageAdapter.h"
#include "ui/database/dialog/adapter/ExportDatabaseAdapter.h"
#include "ui/database/dialog/adapter/ImportDatabaseAdapter.h"
#include "ui/common/treeview/QTreeViewCtrl.h"

class LeftTreeView : public CWindowImpl<LeftTreeView> 
{
public:
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

		// table menus
		COMMAND_ID_HANDLER_EX(Config::TABLE_OPEN_MENU_ID, OnClickOpenTableMenu)
		COMMAND_ID_HANDLER_EX(Config::TABLE_CREATE_MENU_ID, OnClickNewTableMenu)
		COMMAND_ID_HANDLER_EX(Config::TABLE_ALTER_MENU_ID, OnClickAlterTableMenu)

		MESSAGE_HANDLER_EX(Config::MSG_LEFTVIEW_REFRESH_DATABASE_ID, OnRefreshDatabase)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
private:
	bool isNeedReload = true;
	COLORREF bkgColor = RGB(255, 255, 255);
	HBRUSH bkgBrush = nullptr;
	COLORREF topbarColor = RGB(238, 238, 238);
	HBRUSH topbarBrush = nullptr;
	HFONT comboFont = nullptr;

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
	ResultListPageAdapter * resultListPageAdapter = nullptr;

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
	
	void OnClickNewViewMenu(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickNewTriggerMenu(UINT uNotifyCode, int nID, HWND hwnd);
	
	// table menus
	void OnClickOpenTableMenu(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickNewTableMenu(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickAlterTableMenu(UINT uNotifyCode, int nID, HWND hwnd);


	// MSG
	LRESULT OnRefreshDatabase(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void doRefreshDatabase();
	void doDeleteDatabase();
	void doCreateDatabase();
	void doOpenDatabase();
	void doCopyDatabase();
	void doExportAsSql();
	void doImportFromSql();
	void doNewView();
	void doNewTrigger();
	void doNewTable();
	void doAlterTable();
};
