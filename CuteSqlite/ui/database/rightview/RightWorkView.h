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
 * @ClassChain  RightWorkView
 *                |->QTabView(tabView)
 *                         |-> QueryPage
 *                         |      |-> CHorSplitterWindow
 *                         |            |-> QHelpEdit ** QSqlEdit(Scintilla)
 *                         |            |-> ResultTabView
 *                         |                    |-> QTabView
 *                         |                          |-> ResultListPage
 *                         |                          |-> ResultInfoPage
 *                         |                          |-> ResultTableDataPage
 *                         |-> TableStructurePage
 *					       |       |-> TableTabView
 *                         |              |->QTabView(tabView)  
 *                         |                    |->TableColumnsPage
 *                         |                    |->TableIndexesPage
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
#include "ui/database/rightview/page/QueryPage.h"
#include "ui/database/rightview/page/HistoryPage.h"
#include "ui/database/supplier/DatabaseSupplier.h"
#include "ui/database/rightview/page/TableStructurePage.h"

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
		COMMAND_HANDLER_EX(Config::DATABASE_EXEC_SQL_BUTTON_ID, BN_CLICKED, OnClickExecSqlButton)
		COMMAND_HANDLER_EX(Config::DATABASE_EXEC_ALL_BUTTON_ID, BN_CLICKED, OnClickExecAllButton)
		MESSAGE_HANDLER(Config::MSG_NEW_TABLE_ID, OnClickNewTableElem)
		MESSAGE_HANDLER(Config::MSG_NEW_VIEW_ID, OnClickNewViewElem)
		MESSAGE_HANDLER(Config::MSG_NEW_TRIGGER_ID, OnClickNewTriggerElem)
		MESSAGE_HANDLER(Config::MSG_QTABVIEW_CHANGE_PAGE_TITLE, OnChangePageTitle)
		MESSAGE_HANDLER(Config::MSG_SHOW_TABLE_DATA_ID, OnShowTableData)
		NOTIFY_CODE_HANDLER (TBVN_PAGEACTIVATED, OnTabViewPageActivated)
		NOTIFY_CODE_HANDLER (TBVN_TABCLOSEBTN, OnTabViewCloseBtn)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

private:
	bool isNeedReload = true;
	COLORREF bkgColor = RGB(255, 255, 255);
	HBRUSH bkgBrush = nullptr;

	COLORREF topbarColor = RGB(238, 238, 238);
	HBRUSH topbarBrush = nullptr;

	QImageButton execSqlButton;
	QImageButton execAllButton;

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

	DatabaseSupplier * supplier = DatabaseSupplier::getInstance();
	
	
	void createImageList();

	CRect getTopRect(CRect & clientRect);
	CRect getTabRect(CRect & clientRect);

	void createOrShowUI();
	void createOrShowToolButtons(CRect & clientRect);
	void createOrShowTabView(QTabView &win, CRect & clientRect);
	void createFirstQueryPage(CRect & clientRect);
	void createOrShowHistoryPage(HistoryPage &win, CRect & clientRect);
	void createOrShowQueryPage(QueryPage &win, CRect & clientRect);
	void createOrShowTableStructurePage(TableStructurePage &win, CRect & clientRect);

	void loadWindow();
	void loadTabViewPages();

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	int OnDestroy();
	void OnSize(UINT nType, CSize size);
	void OnShowWindow(BOOL bShow, UINT nStatus);
	void OnPaint(CDCHandle dc);
	BOOL OnEraseBkgnd(CDCHandle dc);

	LRESULT OnClickExecSqlButton(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnClickExecAllButton(UINT uNotifyCode, int nID, HWND hwnd);

	// Click "New table" menu or toolbar button will send this msg, wParam=NULL, lParam=NULL
	LRESULT OnClickNewTableElem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	// Click "New view" menu or toolbar button will send this msg, wParam=NULL, lParam=NULL
	LRESULT OnClickNewViewElem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	// Click "New trigger" menu or toolbar button will send this msg, wParam=NULL, lParam=NULL
	LRESULT OnClickNewTriggerElem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	
	// Send this msg when changing tab view title caption, wParam=(page index), lParam=NULL
	LRESULT OnChangePageTitle(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	// Send this msg when clicking the table open menu , wParam = NULL, lParam=NULL
	LRESULT OnShowTableData(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnTabViewPageActivated(int idCtrl, LPNMHDR pnmh, BOOL &bHandled);
	LRESULT OnTabViewCloseBtn(int idCtrl, LPNMHDR pnmh, BOOL &bHandled);

	void doAddNewTable();
	void doAddNewView();
	void doAddNewTrigger();
};