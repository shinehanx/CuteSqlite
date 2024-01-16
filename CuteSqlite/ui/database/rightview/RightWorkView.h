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
 
 * @file   RightWorkView.h
 * @brief  Right work space for splitter,include data query, sql execute and so on.
 *		   Follow the view-handler design pattern,
 *			handler: RightWorkView.
 *			view: QPage and it's subclass in the page folder.
 *			supplier: DatabaseSupplier
 * @Class Tree  RightWorkView
 *                 |->QTabView(tabView)
 *                         |-> QueryPage
 *                         |      |-> CHorSplitterWindow
 *                         |            |-> QueryPageEditor <- QHelpEdit <- QHelpPage
 *                         |            |      |                 |-> QSqlEdit (Scintilla)
 *                         |            |      |-> SqlLogDialog 
 *                         |            |               |-> SqlLogList
 *                         |            |                      |->SqlLogListItem         
 *                         |            |-> ResultTabView
 *                         |                    |-> QTabView
 *                         |                          |-> ResultListPage
 *                         |                          |-> ResultInfoPage
 *                         |                          |-> ResultTableDataPage
 *                         |-> TableStructurePage
 *                         |       |-> TableTabView
 *                         |              |->QTabView(tabView)  
 *                         |                    |->TableColumnsPage
 *                         |                    |->TableIndexesPage
 *                         |                    |->TableForeignPage
 *                         |-> HistoryPage
 * @author Xuehan Qin
 * @date   2023-05-21
 *********************************************************************/
#pragma once
#include <atlwin.h>
#include <atlcrack.h>
#include <atltypes.h>
#include "core/entity/Entity.h"
#include "core/service/db/DatabaseService.h"
#include "ui/common/tabview/QTabView.h"
#include "ui/common/button/QImageButton.h"
#include "ui/common/image/QStaticImage.h"
#include "ui/database/rightview/page/QueryPage.h"
#include "ui/database/rightview/page/HistoryPage.h"
#include "ui/database/supplier/DatabaseSupplier.h"
#include "ui/database/rightview/page/TableStructurePage.h"
#include "ui/database/rightview/adapter/RightWorkViewAdapter.h"
#include "ui/database/dialog/adapter/ExportDatabaseAdapter.h"
#include "ui/database/dialog/adapter/ImportDatabaseAdapter.h"

class RightWorkView : public CWindowImpl<RightWorkView>
{
public:
	BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP_EX(LeftTreeView)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_SIZE(OnSize)
		MSG_WM_SHOWWINDOW(OnShowWindow)
		MSG_WM_PAINT(OnPaint)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MSG_WM_CTLCOLORSTATIC(OnCtlColorStatic)

		// Exec sql
		COMMAND_ID_HANDLER_EX(Config::DATABASE_EXEC_SQL_BUTTON_ID, OnClickExecSqlButton)
		COMMAND_ID_HANDLER_EX(Config::DATABASE_EXEC_ALL_BUTTON_ID, OnClickExecAllButton)
		COMMAND_ID_HANDLER_EX(Config::DATABASE_EXPLAIN_SQL_BUTTON_ID, OnClickExplainSqlButton)
		COMMAND_ID_HANDLER_EX(Config::DATABASE_QUERY_BUTTON_ID, OnClickQueryButton)
		COMMAND_ID_HANDLER_EX(Config::DATABASE_HISTORY_BUTTON_ID, OnClickHistoryButton)
		// save
		COMMAND_ID_HANDLER_EX(Config::DATABASE_SAVE_BUTTON_ID, OnClickSaveButton)
		COMMAND_ID_HANDLER_EX(Config::DATABASE_SAVE_ALL_BUTTON_ID, OnClickSaveAllButton)
		// database import/export
		COMMAND_HANDLER_EX(Config::DATABASE_EXPORT_BUTTON_ID, BN_CLICKED, OnClickExportDatabaseButton)
		COMMAND_HANDLER_EX(Config::DATABASE_IMPORT_BUTTON_ID, BN_CLICKED, OnClickImportDatabaseButton)
		// table button
		COMMAND_HANDLER_EX(Config::DATABASE_CREATE_TABLE_BUTTON_ID, BN_CLICKED, OnClickCreateTableButton)
		COMMAND_HANDLER_EX(Config::DATABASE_OPEN_TABLE_BUTTON_ID, BN_CLICKED, OnClickOpenTableButton)
		COMMAND_HANDLER_EX(Config::DATABASE_ALTER_TABLE_BUTTON_ID, BN_CLICKED, OnClickAlterTableButton)
		COMMAND_HANDLER_EX(Config::DATABASE_RENAME_TABLE_BUTTON_ID, BN_CLICKED, OnClickRenameTableButton)
		COMMAND_HANDLER_EX(Config::DATABASE_TRUNCATE_TABLE_BUTTON_ID, BN_CLICKED, OnClickTruncateTableButton)
		COMMAND_HANDLER_EX(Config::DATABASE_DROP_TABLE_BUTTON_ID, BN_CLICKED, OnClickDropTableButton)
		COMMAND_HANDLER_EX(Config::DATABASE_COPY_TABLE_BUTTON_ID, BN_CLICKED, OnClickCopyTableButton)
		COMMAND_HANDLER_EX(Config::DATABASE_SHARDING_TABLE_BUTTON_ID, BN_CLICKED, OnClickShardingTableButton)
		COMMAND_HANDLER_EX(Config::DATABASE_EXPORT_TABLE_BUTTON_ID, BN_CLICKED, OnClickExportTableButton)
		COMMAND_HANDLER_EX(Config::DATABASE_IMPORT_TABLE_FROM_SQL_BUTTON_ID, BN_CLICKED, OnClickImportTableSqlButton)
		COMMAND_HANDLER_EX(Config::DATABASE_IMPORT_TABLE_FROM_CSV_BUTTON_ID, BN_CLICKED, OnClickImportTableCsvButton)
		

		MESSAGE_HANDLER(Config::MSG_NEW_TABLE_ID, OnClickNewTableElem)
		MESSAGE_HANDLER(Config::MSG_NEW_VIEW_ID, OnClickNewViewElem)
		MESSAGE_HANDLER(Config::MSG_OPEN_VIEW_ID, OnClickOpenViewElem)
		MESSAGE_HANDLER(Config::MSG_DROP_VIEW_ID, OnClickDropViewElem)
		MESSAGE_HANDLER(Config::MSG_NEW_TRIGGER_ID, OnClickNewTriggerElem)
		MESSAGE_HANDLER(Config::MSG_OPEN_TRIGGER_ID, OnClickOpenTriggerElem)
		MESSAGE_HANDLER(Config::MSG_DROP_TRIGGER_ID, OnClickDropTriggerElem)
		MESSAGE_HANDLER(Config::MSG_QTABVIEW_CHANGE_PAGE_TITLE, OnChangePageTitleAndIcon)
		MESSAGE_HANDLER(Config::MSG_SHOW_TABLE_DATA_ID, OnShowTableData)
		MESSAGE_HANDLER(Config::MSG_ALTER_TABLE_ID, OnClickAlterTableElem)
		MESSAGE_HANDLER(Config::MSG_RENAME_TABLE_ID, OnClickRenameTable)
		MESSAGE_HANDLER(Config::MSG_REFRESH_SAME_TABLE_DATA_ID, OnClickRrefreshSameTableData)
		MESSAGE_HANDLER(Config::MSG_DROP_TABLE_ID, OnClickDropTable)
		MESSAGE_HANDLER(Config::MSG_DATA_DIRTY_ID, OnHandleDataDirty)
		MESSAGE_HANDLER(Config::MSG_TABLE_STRUCTURE_DIRTY_ID, OnHandleTableStructureDirty)
		MESSAGE_HANDLER(Config::MSG_TREEVIEW_CLICK_ID, OnChangeTreeviewItem)
		MESSAGE_HANDLER(Config::MSG_EXEC_SQL_RESULT_MESSAGE_ID, OnExecSqlResultMessage)

		NOTIFY_CODE_HANDLER (TBVN_PAGEACTIVATED, OnTabViewPageActivated)
		NOTIFY_CODE_HANDLER (TBVN_TABCLOSEBTN, OnTabViewCloseBtn)
		NOTIFY_CODE_HANDLER (TBVN_CONTEXTMENU, OnTabViewContextMenu)
		
		// tab menu
		COMMAND_ID_HANDLER_EX(Config::TABVIEW_CLOSE_THIS_MENU_ID, OnClickCloseThisMenu)
		COMMAND_ID_HANDLER_EX(Config::TABVIEW_CLOSE_OTHERS_MENU_ID, OnClickCloseOthersMenu)
		COMMAND_ID_HANDLER_EX(Config::TABVIEW_CLOSE_LEFT_MENU_ID, OnClickCloseLeftMenu)
		COMMAND_ID_HANDLER_EX(Config::TABVIEW_CLOSE_RIGHT_MENU_ID, OnClickCloseRightMenu)

		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	~RightWorkView();
private:
	bool isNeedReload = true;
	bool isInitedPages = false;
	COLORREF bkgColor = RGB(255, 255, 255);	
	COLORREF textColor = RGB(255, 255, 255);
	CBrush bkgBrush;

	COLORREF topbarColor = RGB(81, 81, 81);
	COLORREF topbarHoverColor = RGB(102, 102, 104);
	CBrush topbarBrush ;
	HFONT textFont;

	HACCEL m_hAccel = nullptr;

	// exec buttons
	QImageButton execSqlButton;
	QImageButton execAllButton;
	QImageButton explainSqlButton;
	QImageButton queryButton;
	QImageButton historyButton;
	QImageButton saveButton;
	QImageButton saveAllButton;

	// database buttons
	CStatic databaseLabel;
	QImageButton exportDatabaseButton;
	QImageButton importDatabaseButton;
	
	// table buttons
	CStatic tableLabel;
	QImageButton createTableButton;
	QImageButton openTableButton;
	QImageButton alterTableButton;	
	QImageButton renameTableButton;
	QImageButton truncateTableButton;
	QImageButton dropTableButton;
	QImageButton copyTableButton;
	QImageButton shardingTableButton;
	QImageButton exportTableButton;
	QImageButton importTableSqlButton;
	QImageButton importTableCsvButton;

	QTabView tabView;
	HistoryPage historyPage;
	std::vector<QueryPage *> queryPagePtrs; // The pointers of dynamically created page
	std::vector<TableStructurePage *> tablePagePtrs; // The pointers of dynamically created page
	CImageList imageList;

	HICON queryIcon = nullptr;
	HICON historyIcon = nullptr;
	HICON tableIcon = nullptr;
	HICON viewIcon = nullptr;
	HICON triggerIcon = nullptr;
	HICON tableDataIcon = nullptr;
	HICON tableDataDirtyIcon = nullptr;
	HICON tableStructureDirtyIcon = nullptr;
	HICON createTableIcon = nullptr;
	HICON createTableDirtyIcon = nullptr;
	HICON queryDirtyIcon = nullptr;

	RightWorkViewAdapter * adapter = nullptr;
	DatabaseSupplier * databaseSupplier = DatabaseSupplier::getInstance();
	DatabaseService * databaseService = DatabaseService::getInstance();
	SqlLogService * sqlLogService = SqlLogService::getInstance();
	ExportDatabaseAdapter * exportDatabaseAdapter = nullptr;
	ImportDatabaseAdapter * importDatabaseAdapter = nullptr;
	
	void createImageList();

	CRect getTopRect(CRect & clientRect);
	CRect getTabRect(CRect & clientRect);
	CRect getTabRect();

	void createOrShowUI();
	void createOrShowToolButtons(CRect & clientRect);
	void createOrShowExecButtons(CRect & clientRect);
	void createOrShowSaveButtons(CRect & clientRect);
	void createOrShowDbButtons(CRect & clientRect);
	void createOrShowTableButtons(CRect & clientRect);
	
	void createOrShowTabView(QTabView &win, CRect & clientRect);
	void createOrShowHistoryPage(HistoryPage &win, CRect & clientRect, bool isAllowCreate = true);

	void loadWindow();
	void loadTabViewPages();

	void enableButton(QImageButton &btn, const std::wstring & iconPrefix);

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	int OnDestroy();
	void OnSize(UINT nType, CSize size);
	void OnShowWindow(BOOL bShow, UINT nStatus);
	void OnPaint(CDCHandle dc);
	BOOL OnEraseBkgnd(CDCHandle dc);

	LRESULT OnClickExecSqlButton(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnClickExecAllButton(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnClickExplainSqlButton(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnClickQueryButton(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnClickHistoryButton(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnClickSaveButton(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnClickSaveAllButton(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnClickExportDatabaseButton(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnClickImportDatabaseButton(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnClickCreateTableButton(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnClickOpenTableButton(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnClickAlterTableButton(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnClickRenameTableButton(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnClickTruncateTableButton(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnClickDropTableButton(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnClickCopyTableButton(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnClickShardingTableButton(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnClickExportTableButton(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnClickImportTableSqlButton(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnClickImportTableCsvButton(UINT uNotifyCode, int nID, HWND hwnd);

	// Click "New table" menu or toolbar button will send this msg, wParam=NULL, lParam=NULL
	LRESULT OnClickNewTableElem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	// Click "New view" menu or toolbar button will send this msg, wParam=NULL, lParam=NULL
	LRESULT OnClickNewViewElem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	// Click "New trigger" menu or toolbar button will send this msg, wParam=NULL, lParam=NULL
	LRESULT OnClickNewTriggerElem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	// Click "Open View" menu or toolbar button will send this msg, wParam=NULL, lParam=NULL
	LRESULT OnClickOpenViewElem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	// Click "Drop View" menu or toolbar button will send this msg, wParam=NULL, lParam=NULL
	LRESULT OnClickDropViewElem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	// Click "Open Trigger" menu or toolbar button will send this msg, wParam=NULL, lParam=NULL
	LRESULT OnClickOpenTriggerElem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	// Click "Drop Trigger" menu or toolbar button will send this msg, wParam=NULL, lParam=NULL
	LRESULT OnClickDropTriggerElem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	
	// Send this msg when changing tab view title caption, wParam=(page index), lParam=NULL
	LRESULT OnChangePageTitleAndIcon(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	// Send this msg when clicking the table open menu , wParam = NULL, lParam=NULL
	LRESULT OnShowTableData(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClickAlterTableElem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClickRenameTable(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClickRrefreshSameTableData(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClickDropTable(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnHandleDataDirty(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnHandleTableStructureDirty(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnChangeTreeviewItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	// After executed the SQL statement, system record the sql log£¬wParam- NULL£¬lParam - point of adapter.runtimeResultInfo
	LRESULT OnExecSqlResultMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnTabViewPageActivated(int idCtrl, LPNMHDR pnmh, BOOL &bHandled);
	LRESULT OnTabViewCloseBtn(int idCtrl, LPNMHDR pnmh, BOOL &bHandled);
	LRESULT OnTabViewContextMenu(int idCtrl, LPNMHDR pnmh, BOOL &bHandled);

	HBRUSH OnCtlColorStatic(HDC hdc, HWND hwnd);
	void OnClickCloseThisMenu(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnClickCloseOthersMenu(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnClickCloseLeftMenu(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnClickCloseRightMenu(UINT uNotifyCode, int nID, CWindow wndCtl);
	LRESULT closeTabViewPage(int nPage);
};