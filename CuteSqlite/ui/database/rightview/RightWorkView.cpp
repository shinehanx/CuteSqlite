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

 * @file   RightWorkView.cpp
 * @brief  Right work space for splitter,include data query, sql execute and so on.
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
 *                         |                          |-> TablePropertiesPage
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
#include "stdafx.h"
#include "RightWorkView.h"
#include "common/AppContext.h"
#include "core/common/Lang.h"
#include "ui/common/QWinCreater.h"
#include "ui/common/message/QPopAnimate.h"
#include <ui/common/message/QMessageBox.h>
#include "ui/database/dialog/ExportAsSqlDialog.h"
#include "ui/database/dialog/ImportFromSqlDialog.h"


#define RIGHTVIEW_TOPBAR_HEIGHT 30
#define RIGHTVIEW_BUTTON_WIDTH 16

BOOL RightWorkView::PreTranslateMessage(MSG* pMsg)
{
	BOOL result = FALSE;
	
	for (auto pagePtr : queryPagePtrs) {
		if (pagePtr && pagePtr->IsWindow() && pagePtr->PreTranslateMessage(pMsg)) {
			return TRUE;
		}
	}

	for (auto pagePtr : tablePagePtrs) {
		if (pagePtr && pagePtr->IsWindow() && pagePtr->PreTranslateMessage(pMsg)) {
			return TRUE;
		}
	}

	if (historyPage.IsWindow() && historyPage.PreTranslateMessage(pMsg)) {
		return TRUE;
	}

	if (WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST) {
		if (m_hAccel && ::TranslateAccelerator(m_hWnd, m_hAccel, pMsg)) {
			return TRUE;
		}
	}
	return FALSE;
}

RightWorkView::~RightWorkView()
{
	m_hWnd = nullptr;
}

void RightWorkView::createImageList()
{
	if (!imageList.IsNull()) {
		return;
	}
	std::wstring imgDir = ResourceUtil::getProductImagesDir();
	HINSTANCE ins = ModuleHelper::GetModuleInstance();
	queryIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\tab\\query.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	historyIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\tab\\history.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);	
	tableIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\tab\\table.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);	
	viewIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\tab\\view.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);	
	triggerIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\tab\\trigger.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);	
	tableDataIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\tab\\table-data.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);	
	tableDataDirtyIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\tab\\table-data-dirty.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	tableStructureDirtyIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\tab\\table-structure-dirty.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	createTableIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\tab\\create-table.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	createTableDirtyIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\tab\\create-table-dirty.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	queryDirtyIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\tab\\query-dirty.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);

	imageList.Create(16, 16, ILC_COLOR32, 8, 8);
	imageList.AddIcon(queryIcon); // 0 - query
	imageList.AddIcon(historyIcon); // 1 - history
	imageList.AddIcon(tableIcon); // 2 - table
	imageList.AddIcon(viewIcon); // 3 - view 
	imageList.AddIcon(triggerIcon); // 4 - trigger
	imageList.AddIcon(tableDataIcon); // 5 - table data
	imageList.AddIcon(tableDataDirtyIcon); // 6 - table data dirty
	imageList.AddIcon(tableStructureDirtyIcon); // 7 - table structure dirty
	imageList.AddIcon(createTableIcon); // 8 - create table
	imageList.AddIcon(createTableDirtyIcon); // 9 - create table dirty
	imageList.AddIcon(queryDirtyIcon); // 10 - query dirty
	
}

CRect RightWorkView::getTopRect(CRect & clientRect)
{
	return { 0, 0, clientRect.right, RIGHTVIEW_TOPBAR_HEIGHT };
}

CRect RightWorkView::getTabRect(CRect & clientRect)
{
	return { 0, RIGHTVIEW_TOPBAR_HEIGHT, clientRect.right, clientRect.bottom };
}

CRect RightWorkView::getTabRect()
{
	CRect clientRect;
	GetClientRect(clientRect);
	return getTabRect(clientRect);
}

void RightWorkView::createOrShowUI()
{
	CRect clientRect;
	GetClientRect(clientRect);

	CRect tabRect = getTabRect(clientRect);	

	createOrShowToolButtons(clientRect);
	createOrShowTabView(tabView, clientRect);
	
	adapter->createFirstQueryPage(tabRect, isInitedPages);
	createOrShowHistoryPage(historyPage, clientRect, !isInitedPages);
	
	
	isInitedPages = true;
}


void RightWorkView::createOrShowToolButtons(CRect & clientRect)
{
	createOrShowExecButtons(clientRect);
	createOrShowSaveButtons(clientRect);
	createOrShowDbButtons(clientRect);
	createOrShowTableButtons(clientRect);
}

void RightWorkView::createOrShowExecButtons(CRect & clientRect)
{
	int x = 10, y = 7, w = RIGHTVIEW_BUTTON_WIDTH, h = RIGHTVIEW_BUTTON_WIDTH;

	// Exec sql buttons
	CRect rect(x, y, x + w, y + h);
	std::wstring imgDir = ResourceUtil::getProductImagesDir();
	std::wstring normalImagePath, pressedImagePath;
	if (!execSqlButton.IsWindow()) {
		normalImagePath = imgDir + L"database\\button\\exec-sql-button-normal.png";
		pressedImagePath = imgDir + L"database\\button\\exec-sql-button-pressed.png";
		execSqlButton.SetIconPath(normalImagePath, pressedImagePath);
		execSqlButton.SetBkgColors(topbarColor, topbarHoverColor, topbarColor);		
	}
	QWinCreater::createOrShowButton(m_hWnd, execSqlButton, Config::DATABASE_EXEC_SQL_BUTTON_ID, L"", rect, clientRect);
	execSqlButton.SetToolTip(SNT(L"exec-select-sql"));

	rect.OffsetRect(w + 10, 0);
	if (!execAllButton.IsWindow()) {
		normalImagePath = imgDir + L"database\\button\\exec-all-button-normal.png";
		pressedImagePath = imgDir + L"database\\button\\exec-all-button-pressed.png";
		execAllButton.SetIconPath(normalImagePath, pressedImagePath);
		execAllButton.SetBkgColors(topbarColor, topbarHoverColor, topbarColor);
	}
	QWinCreater::createOrShowButton(m_hWnd, execAllButton, Config::DATABASE_EXEC_ALL_BUTTON_ID, L"", rect, clientRect);
	execAllButton.SetToolTip(SNT(L"exec-all-sql"));

	rect.OffsetRect(w + 10, 0);
	if (!explainSqlButton.IsWindow()) {
		normalImagePath = imgDir + L"database\\button\\explain-button-normal.png";
		pressedImagePath = imgDir + L"database\\button\\explain-button-pressed.png";
		explainSqlButton.SetIconPath(normalImagePath, pressedImagePath);
		explainSqlButton.SetBkgColors(topbarColor ,topbarHoverColor, topbarColor);		
	}
	QWinCreater::createOrShowButton(m_hWnd, explainSqlButton, Config::DATABASE_EXPLAIN_SQL_BUTTON_ID, L"", rect, clientRect);
	explainSqlButton.SetToolTip(SNT(L"explain-select-sql"));

	rect.OffsetRect(w + 10, 0);
	if (!explainQueryPlanButton.IsWindow()) {
		normalImagePath = imgDir + L"database\\button\\explain-query-plan-button-normal.png";
		pressedImagePath = imgDir + L"database\\button\\explain-query-plan-button-pressed.png";
		explainQueryPlanButton.SetIconPath(normalImagePath, pressedImagePath);
		explainQueryPlanButton.SetBkgColors(topbarColor ,topbarHoverColor, topbarColor);		
	}
	QWinCreater::createOrShowButton(m_hWnd, explainQueryPlanButton, Config::DATABASE_EXPLAIN_QUREY_PLAN_BUTTON_ID, L"", rect, clientRect);
	explainQueryPlanButton.SetToolTip(SNT(L"explain-query-plan-sql"));
	
	rect.OffsetRect(w + 40 , 0);
	if (!queryButton.IsWindow()) {
		normalImagePath = imgDir + L"database\\button\\query-button-normal.png";
		pressedImagePath = imgDir + L"database\\button\\query-button-pressed.png";
		queryButton.SetIconPath(normalImagePath, pressedImagePath);
		queryButton.SetBkgColors(topbarColor ,topbarHoverColor, topbarColor);
	}
	QWinCreater::createOrShowButton(m_hWnd, queryButton, Config::DATABASE_QUERY_BUTTON_ID, L"", rect, clientRect);
	queryButton.SetToolTip(S(L"new-query"));
	
	rect.OffsetRect(w + 10, 0);
	if (!historyButton.IsWindow()) {
		normalImagePath = imgDir + L"database\\button\\history-button-normal.png";
		pressedImagePath = imgDir + L"database\\button\\history-button-pressed.png";
		historyButton.SetIconPath(normalImagePath, pressedImagePath);
		historyButton.SetBkgColors(topbarColor ,topbarHoverColor, topbarColor);
	}
	QWinCreater::createOrShowButton(m_hWnd, historyButton, Config::DATABASE_HISTORY_BUTTON_ID, L"", rect, clientRect);
	historyButton.SetToolTip(S(L"history-tip"));
}


void RightWorkView::createOrShowSaveButtons(CRect & clientRect)
{
	CRect rect = GdiPlusUtil::GetWindowRelativeRect(historyButton.m_hWnd);
	int w = rect.Width();
	std::wstring imgDir = ResourceUtil::getProductImagesDir();
	std::wstring normalImagePath,pressedImagePath;

	rect.OffsetRect(w + 40, 0); 
	if (!saveButton.IsWindow()) {
		normalImagePath = imgDir + L"database\\button\\save-button-normal.png";
		pressedImagePath = imgDir + L"database\\button\\save-button-pressed.png";
		saveButton.SetIconPath(normalImagePath, pressedImagePath);
		saveButton.SetBkgColors(topbarColor ,topbarHoverColor, topbarColor);
	}
	QWinCreater::createOrShowButton(m_hWnd, saveButton, Config::DATABASE_SAVE_BUTTON_ID, L"", rect, clientRect);
	saveButton.SetToolTip(S(L"save-current"));

	rect.OffsetRect(w + 10, 0); 
	if (!saveAllButton.IsWindow()) {
		normalImagePath = imgDir + L"database\\button\\save-all-button-normal.png";
		pressedImagePath = imgDir + L"database\\button\\save-all-button-pressed.png";
		saveAllButton.SetIconPath(normalImagePath, pressedImagePath);
		saveAllButton.SetBkgColors(topbarColor ,topbarHoverColor, topbarColor);
	}
	QWinCreater::createOrShowButton(m_hWnd, saveAllButton, Config::DATABASE_SAVE_ALL_BUTTON_ID, L"", rect, clientRect);
	saveAllButton.SetToolTip(S(L"save-all"));
}

void RightWorkView::createOrShowDbButtons(CRect & clientRect)
{
	CRect rect = GdiPlusUtil::GetWindowRelativeRect(saveAllButton.m_hWnd);
	int w = rect.Width();
	std::wstring imgDir = ResourceUtil::getProductImagesDir();
	std::wstring normalImagePath,pressedImagePath;

	CRect labelRect = rect;
	labelRect.OffsetRect(rect.Width() + 20, 0);
	labelRect.right = labelRect.left + 70; 
	QWinCreater::createOrShowLabel(m_hWnd, databaseLabel, S(L"database").append(L":"), labelRect, clientRect, SS_RIGHT);

	// Database buttons
	rect.OffsetRect(labelRect.right - rect.left + 5, 0);
	if (!exportDatabaseButton.IsWindow()) {
		normalImagePath = imgDir + L"database\\button\\export-database-button-normal.png";
		pressedImagePath = imgDir + L"database\\button\\export-database-button-pressed.png";
		exportDatabaseButton.SetIconPath(normalImagePath, pressedImagePath);
		exportDatabaseButton.SetBkgColors(topbarColor ,topbarHoverColor, topbarColor);		
	}
	QWinCreater::createOrShowButton(m_hWnd, exportDatabaseButton, Config::DATABASE_EXPORT_BUTTON_ID, L"", rect, clientRect);
	exportDatabaseButton.SetToolTip(SNT(L"database-export-as-sql"));
	
	rect.OffsetRect(w + 10, 0);
	if (!importDatabaseButton.IsWindow()) {
		normalImagePath = imgDir + L"database\\button\\import-database-button-normal.png";
		pressedImagePath = imgDir + L"database\\button\\import-database-button-pressed.png";
		importDatabaseButton.SetIconPath(normalImagePath, pressedImagePath);
		importDatabaseButton.SetBkgColors(topbarColor ,topbarHoverColor, topbarColor);
	}
	QWinCreater::createOrShowButton(m_hWnd, importDatabaseButton, Config::DATABASE_IMPORT_BUTTON_ID, L"", rect, clientRect);
	importDatabaseButton.SetToolTip(SNT(L"database-import-from-sql"));	
}

void RightWorkView::createOrShowTableButtons(CRect & clientRect)
{
	CRect rect = GdiPlusUtil::GetWindowRelativeRect(importDatabaseButton.m_hWnd);
	int w = rect.Width();
	std::wstring imgDir = ResourceUtil::getProductImagesDir();
	std::wstring normalImagePath,pressedImagePath;

	CRect labelRect = rect;
	labelRect.OffsetRect(rect.Width() + 20, 0);
	labelRect.right = labelRect.left + 50;
	QWinCreater::createOrShowLabel(m_hWnd, tableLabel, S(L"table").append(L":"), labelRect, clientRect, SS_RIGHT);

	// table buttons
	rect.OffsetRect(labelRect.right - rect.left + 5, 0);
	if (!createTableButton.IsWindow()) {
		normalImagePath = imgDir + L"database\\button\\create-table-button-normal.png";
		pressedImagePath = imgDir + L"database\\button\\create-table-button-pressed.png";
		createTableButton.SetIconPath(normalImagePath, pressedImagePath);
		createTableButton.SetBkgColors(topbarColor ,topbarHoverColor, topbarColor);
	}
	QWinCreater::createOrShowButton(m_hWnd, createTableButton, Config::DATABASE_CREATE_TABLE_BUTTON_ID, L"", rect, clientRect);
	createTableButton.SetToolTip(SNT(L"table-create"));
	
	rect.OffsetRect(w + 10, 0);
	openTableButton.SetBkgColors(topbarColor ,topbarHoverColor, topbarColor);
	QWinCreater::createOrShowButton(m_hWnd, openTableButton, Config::DATABASE_OPEN_TABLE_BUTTON_ID, L"", rect, clientRect);
	openTableButton.SetToolTip(SNT(L"table-open"));
	enableButton(openTableButton, std::wstring(L"open-table"));

	rect.OffsetRect(w + 10, 0);
	alterTableButton.SetBkgColors(topbarColor ,topbarHoverColor, topbarColor);
	QWinCreater::createOrShowButton(m_hWnd, alterTableButton, Config::DATABASE_ALTER_TABLE_BUTTON_ID, L"", rect, clientRect);
	alterTableButton.SetToolTip(SNT(L"table-alter"));
	enableButton(alterTableButton, std::wstring(L"alter-table"));

	rect.OffsetRect(w + 10, 0);
	renameTableButton.SetBkgColors(topbarColor ,topbarHoverColor, topbarColor);
	QWinCreater::createOrShowButton(m_hWnd, renameTableButton, Config::DATABASE_RENAME_TABLE_BUTTON_ID, L"", rect, clientRect);
	renameTableButton.SetToolTip(SNT(L"table-rename"));
	enableButton(renameTableButton, std::wstring(L"rename-table"));

	rect.OffsetRect(w + 10, 0);
	truncateTableButton.SetBkgColors(topbarColor ,topbarHoverColor, topbarColor);
	QWinCreater::createOrShowButton(m_hWnd, truncateTableButton, Config::DATABASE_TRUNCATE_TABLE_BUTTON_ID, L"", rect, clientRect);
	truncateTableButton.SetToolTip(SNT(L"table-truncate"));
	enableButton(truncateTableButton, std::wstring(L"truncate-table"));

	rect.OffsetRect(w + 10, 0);
	dropTableButton.SetBkgColors(topbarColor ,topbarHoverColor, topbarColor);
	QWinCreater::createOrShowButton(m_hWnd, dropTableButton, Config::DATABASE_DROP_TABLE_BUTTON_ID, L"", rect, clientRect);
	dropTableButton.SetToolTip(SNT(L"table-drop"));
	enableButton(dropTableButton, std::wstring(L"drop-table"));

	rect.OffsetRect(w + 10, 0);
	copyTableButton.SetBkgColors(topbarColor ,topbarHoverColor, topbarColor);
	QWinCreater::createOrShowButton(m_hWnd, copyTableButton, Config::DATABASE_COPY_TABLE_BUTTON_ID, L"", rect, clientRect);
	copyTableButton.SetToolTip(SNT(L"table-copy-as"));
	enableButton(copyTableButton, std::wstring(L"copy-table"));

	rect.OffsetRect(w + 10, 0);
	shardingTableButton.SetBkgColors(topbarColor ,topbarHoverColor, topbarColor);
	QWinCreater::createOrShowButton(m_hWnd, shardingTableButton, Config::DATABASE_SHARDING_TABLE_BUTTON_ID, L"", rect, clientRect);
	shardingTableButton.SetToolTip(SNT(L"table-sharding-as")); 
	enableButton(shardingTableButton, std::wstring(L"sharding-table"));

	rect.OffsetRect(w + 10, 0);
	exportTableButton.SetBkgColors(topbarColor ,topbarHoverColor, topbarColor);
	QWinCreater::createOrShowButton(m_hWnd, exportTableButton, Config::DATABASE_EXPORT_TABLE_BUTTON_ID, L"", rect, clientRect);
	exportTableButton.SetToolTip(SNT(L"table-export-as")); 
	enableButton(exportTableButton, std::wstring(L"export-table"));

	rect.OffsetRect(w + 10, 0);
	importTableSqlButton.SetBkgColors(topbarColor ,topbarHoverColor, topbarColor);
	QWinCreater::createOrShowButton(m_hWnd, importTableSqlButton, Config::DATABASE_IMPORT_TABLE_FROM_SQL_BUTTON_ID, L"", rect, clientRect);
	importTableSqlButton.SetToolTip(SNT(L"table-import-sql"));
	enableButton(importTableSqlButton, std::wstring(L"import-table-sql"));

	rect.OffsetRect(w + 10, 0);
	importTableCsvButton.SetBkgColors(topbarColor ,topbarHoverColor, topbarColor);
	QWinCreater::createOrShowButton(m_hWnd, importTableCsvButton, Config::DATABASE_IMPORT_TABLE_FROM_CSV_BUTTON_ID, L"", rect, clientRect);
	importTableCsvButton.SetToolTip(SNT(L"table-import-csv"));
	enableButton(importTableCsvButton, std::wstring(L"import-table-csv"));
}

void RightWorkView::createOrShowTabView(QTabView &win, CRect & clientRect)
{
	CRect rect = getTabRect(clientRect);
	if (IsWindow() && !win.IsWindow()) {
		win.Create(m_hWnd, rect, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, Config::DATABASE_WORK_TAB_VIEW_ID);
		win.SetTitleBarWindow(AppContext::getInstance()->getMainFrmHwnd());
		win.SetImageList(imageList);
	} else if (IsWindow() && tabView.IsWindow()) {
		win.MoveWindow(rect);
		win.ShowWindow(true);
	}
}


void RightWorkView::createOrShowHistoryPage(HistoryPage &win, CRect & clientRect, bool isAllowCreate)
{
	CRect tabRect = getTabRect(clientRect);
	int x = 1, y = tabView.m_cyTabHeight + 1, w = tabRect.Width() - 2, h = tabRect.Height() - tabView.m_cyTabHeight - 2;
	CRect rect(x, y, x + w, y + h);
	if (isAllowCreate && IsWindow() && !win.IsWindow()) {
		win.Create(tabView.m_hWnd, rect, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN , 0);		
	} else if (IsWindow() && tabView.IsWindow() && win.IsWindow()) {
		win.MoveWindow(rect);
		win.ShowWindow(true);
	}
}

void RightWorkView::loadWindow()
{
	if (!isNeedReload) {
		return ;
	}

	isNeedReload = false;
	loadTabViewPages();
}

void RightWorkView::loadTabViewPages()
{
	ATLASSERT(!queryPagePtrs.empty());
	QueryPage * firstQueryPage = queryPagePtrs.at(0); 
	tabView.AddPage(firstQueryPage->m_hWnd, StringUtil::blkToTail(S(L"query-editor")).c_str(), 0, firstQueryPage);
	tabView.AddPage(historyPage.m_hWnd, StringUtil::blkToTail(S(L"history")).c_str(), 1, &historyPage);
	tabView.SetActivePage(0);
	databaseSupplier->activeTabPageHwnd = firstQueryPage->m_hWnd;
}


void RightWorkView::enableButton(QImageButton &btn, const std::wstring & iconPrefix)
{
	bool enabled = !databaseSupplier->selectedTable.empty();
	bool origEnabled = btn.IsWindowEnabled();
	if (origEnabled == enabled) {
		return ;
	}
	std::wstring normalImagePath, pressedImagePath;
	std::wstring imgDir = ResourceUtil::getProductImagesDir();
	if (enabled) {
		normalImagePath = imgDir + L"database\\button\\" + iconPrefix + L"-button-normal.png";
		pressedImagePath = imgDir + L"database\\button\\" + iconPrefix + L"-button-pressed.png";
	} else {
		normalImagePath = imgDir + L"database\\button\\" + iconPrefix + L"-button-disabled.png";
		pressedImagePath = imgDir + L"database\\button\\" + iconPrefix + L"-button-disabled.png";
	}
	btn.SetIconPath(normalImagePath, pressedImagePath);
	btn.EnableWindow(enabled);
}

int RightWorkView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_NEW_TABLE_ID);
	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_NEW_VIEW_ID);
	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_OPEN_VIEW_ID);
	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_DROP_VIEW_ID);
	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_NEW_TRIGGER_ID);
	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_OPEN_TRIGGER_ID);
	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_DROP_TRIGGER_ID);
	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_SHOW_TABLE_DATA_ID);
	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_ALTER_TABLE_ID);
	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_RENAME_TABLE_ID);
	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_REFRESH_SAME_TABLE_DATA_ID);
	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_DROP_TABLE_ID);
	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_COPY_TABLE_ID);
	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_EXPORT_TABLE_ID);
	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_TABLE_IMPORT_SQL_ID);
	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_TABLE_IMPORT_CSV_ID);
	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_TABLE_MANAGE_INDEX_ID);
	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_TABLE_PROPERTIES_ID);
	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_TREEVIEW_CLICK_ID);
	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_EXEC_SQL_RESULT_MESSAGE_ID);
	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_DELETE_DATABASE_ID);

	HINSTANCE ins = ModuleHelper::GetModuleInstance();
	m_hAccel = ::LoadAccelerators(ins, MAKEINTRESOURCE(RIGHT_WORKVIEW_ACCEL));

	adapter = new RightWorkViewAdapter(m_hWnd, tabView, queryPagePtrs, tablePagePtrs);
	importDatabaseAdapter = new ImportDatabaseAdapter(m_hWnd, nullptr);
	exportDatabaseAdapter = new ExportDatabaseAdapter(m_hWnd, nullptr);

	createImageList();

	topbarBrush.CreateSolidBrush(topbarColor);
	bkgBrush.CreateSolidBrush(bkgColor);
	textFont = FT(L"form-text-size");

	// tab menus
	adapter->createTabMenu();
	return 0;
}

int RightWorkView::OnDestroy()
{
	AppContext::getInstance()->unsubscribe(m_hWnd, Config::MSG_NEW_TABLE_ID);
	AppContext::getInstance()->unsubscribe(m_hWnd, Config::MSG_NEW_VIEW_ID);
	AppContext::getInstance()->unsubscribe(m_hWnd, Config::MSG_OPEN_VIEW_ID);
	AppContext::getInstance()->unsubscribe(m_hWnd, Config::MSG_DROP_VIEW_ID);
	AppContext::getInstance()->unsubscribe(m_hWnd, Config::MSG_NEW_TRIGGER_ID);
	AppContext::getInstance()->unsubscribe(m_hWnd, Config::MSG_OPEN_TRIGGER_ID);
	AppContext::getInstance()->unsubscribe(m_hWnd, Config::MSG_DROP_TRIGGER_ID);
	AppContext::getInstance()->unsubscribe(m_hWnd, Config::MSG_SHOW_TABLE_DATA_ID);
	AppContext::getInstance()->unsubscribe(m_hWnd, Config::MSG_ALTER_TABLE_ID);
	AppContext::getInstance()->unsubscribe(m_hWnd, Config::MSG_RENAME_TABLE_ID);
	AppContext::getInstance()->unsubscribe(m_hWnd, Config::MSG_REFRESH_SAME_TABLE_DATA_ID);
	AppContext::getInstance()->unsubscribe(m_hWnd, Config::MSG_DROP_TABLE_ID);
	AppContext::getInstance()->unsubscribe(m_hWnd, Config::MSG_COPY_TABLE_ID);
	AppContext::getInstance()->unsubscribe(m_hWnd, Config::MSG_EXPORT_TABLE_ID);
	AppContext::getInstance()->unsubscribe(m_hWnd, Config::MSG_TABLE_IMPORT_SQL_ID);
	AppContext::getInstance()->unsubscribe(m_hWnd, Config::MSG_TABLE_IMPORT_CSV_ID);
	AppContext::getInstance()->unsubscribe(m_hWnd, Config::MSG_TABLE_MANAGE_INDEX_ID);
	AppContext::getInstance()->unsubscribe(m_hWnd, Config::MSG_TABLE_PROPERTIES_ID);
	AppContext::getInstance()->unsubscribe(m_hWnd, Config::MSG_TREEVIEW_CLICK_ID);
	AppContext::getInstance()->unsubscribe(m_hWnd, Config::MSG_EXEC_SQL_RESULT_MESSAGE_ID);
	AppContext::getInstance()->unsubscribe(m_hWnd, Config::MSG_DELETE_DATABASE_ID);
	
	if (!bkgBrush.IsNull()) bkgBrush.DeleteObject();
	if (!topbarBrush.IsNull()) topbarBrush.DeleteObject();
	if (textFont) ::DeleteObject(textFont);
	
	if (execSqlButton.IsWindow()) execSqlButton.DestroyWindow();
	if (execAllButton.IsWindow()) execAllButton.DestroyWindow();
	if (explainSqlButton.IsWindow()) explainSqlButton.DestroyWindow();
	if (queryButton.IsWindow()) queryButton.DestroyWindow();
	if (historyButton.IsWindow()) historyButton.DestroyWindow();
	if (saveButton.IsWindow()) saveButton.DestroyWindow();
	if (saveAllButton.IsWindow()) saveAllButton.DestroyWindow();

	if (databaseLabel.IsWindow()) databaseLabel.DestroyWindow();
	if (exportDatabaseButton.IsWindow()) exportDatabaseButton.DestroyWindow();
	if (importDatabaseButton.IsWindow()) importDatabaseButton.DestroyWindow();

	if (tableLabel.IsWindow()) tableLabel.DestroyWindow();
	if (createTableButton.IsWindow()) createTableButton.DestroyWindow();
	if (openTableButton.IsWindow()) openTableButton.DestroyWindow();
	if (alterTableButton.IsWindow()) alterTableButton.DestroyWindow();
	if (renameTableButton.IsWindow()) renameTableButton.DestroyWindow();
	if (truncateTableButton.IsWindow()) truncateTableButton.DestroyWindow();
	if (dropTableButton.IsWindow()) dropTableButton.DestroyWindow();
	if (copyTableButton.IsWindow()) copyTableButton.DestroyWindow();
	if (shardingTableButton.IsWindow()) shardingTableButton.DestroyWindow();
	if (exportTableButton.IsWindow()) exportTableButton.DestroyWindow();
	if (importTableSqlButton.IsWindow()) importTableSqlButton.DestroyWindow();
	if (importTableCsvButton.IsWindow()) importTableCsvButton.DestroyWindow();

	if (tabView.IsWindow()) tabView.DestroyWindow();
	if (historyPage.IsWindow()) historyPage.DestroyWindow();
	
	if (queryIcon) ::DeleteObject(queryIcon);
	if (historyIcon) ::DeleteObject(historyIcon);
	if (tableIcon) ::DeleteObject(tableIcon);
	if (viewIcon) ::DeleteObject(viewIcon);
	if (triggerIcon) ::DeleteObject(triggerIcon);
	if (tableDataIcon) ::DeleteObject(tableDataIcon);
	if (tableDataDirtyIcon) ::DeleteObject(tableDataDirtyIcon);
	if (tableStructureDirtyIcon) ::DeleteObject(tableStructureDirtyIcon);
	if (createTableIcon) ::DeleteObject(createTableIcon);
	if (createTableDirtyIcon) ::DeleteObject(createTableDirtyIcon);
	if (queryDirtyIcon) ::DeleteObject(queryDirtyIcon);
	if (!imageList.IsNull()) imageList.Destroy();

	// destroy the pagePtr and release the memory from pagePtrs vector
	for (TableStructurePage * pagePtr : tablePagePtrs) {
		if (pagePtr && pagePtr->IsWindow()) {
			pagePtr->DestroyWindow();
			delete pagePtr;
			pagePtr = nullptr;
		} else if (pagePtr) {
			delete pagePtr;
			pagePtr = nullptr;
		}
	}

	// destroy the pagePtr and release the memory from pagePtrs vector
	for (QueryPage * pagePtr : queryPagePtrs) {
		if (pagePtr && pagePtr->IsWindow()) {
			pagePtr->DestroyWindow();
			delete pagePtr;
			pagePtr = nullptr;
		} else if (pagePtr) {
			delete pagePtr;
			pagePtr = nullptr;
		}
	}
	queryPagePtrs.clear();
	tablePagePtrs.clear();

	if (adapter) {
		delete adapter;
		adapter = nullptr;
	}
	sqlLogService->clearOldSqlLog();
	return 0;
}

void RightWorkView::OnSize(UINT nType, CSize size)
{
	createOrShowUI();	
}

void RightWorkView::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if (!bShow) {
		return ;
	}
	loadWindow();
}

void RightWorkView::OnPaint(CDCHandle dc)
{
	CPaintDC pdc(m_hWnd);
	CMemoryDC mdc(pdc, pdc.m_ps.rcPaint);
	CRect clientRect;
	GetClientRect(clientRect);


	CRect topRect = getTopRect(clientRect);
	mdc.FillRect(topRect, topbarBrush.m_hBrush);

	CRect workRect = getTabRect(clientRect);
	mdc.FillRect(workRect, bkgBrush.m_hBrush);
}

BOOL RightWorkView::OnEraseBkgnd(CDCHandle dc)
{
	return TRUE;
}

LRESULT RightWorkView::OnClickExecSqlButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	adapter->execSelectedSql();
	return 0;
}

LRESULT RightWorkView::OnClickExecAllButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	adapter->execAllSql();
	return 0;
}

LRESULT RightWorkView::OnClickExplainSqlButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	adapter->explainSelectedSql();
	return 0;
}

LRESULT RightWorkView::OnClickExplainQueryPlanButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	adapter->explainQueryPlanSql();
	return 0;
}

LRESULT RightWorkView::OnClickQueryButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	adapter->createNewQueryPage(getTabRect());
	return 0;
}

LRESULT RightWorkView::OnClickHistoryButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	CRect clientRect;
	GetClientRect(clientRect);
	if (!historyPage.IsWindow()) {
		createOrShowHistoryPage(historyPage, clientRect);
		tabView.AddPage(historyPage.m_hWnd, StringUtil::blkToTail(S(L"history")).c_str(), 1, &historyPage);
		return 0;
	}
	int n = tabView.GetPageCount();
	for (int i = 0; i < n; i++) {
		HWND pageHwnd = tabView.GetPageHWND(i);
		if (historyPage.m_hWnd == pageHwnd) {
			tabView.SetActivePage(i);
		}
	}
	return 0;
}

LRESULT RightWorkView::OnClickSaveButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	int activePage = tabView.GetActivePage();
	HWND activePageHwnd = tabView.GetPageHWND(activePage);
	for (auto ptr : queryPagePtrs) {
		if (ptr && ptr->IsWindow() && ptr->m_hWnd == activePageHwnd) {
			ptr->save();
			return 0;
		}
	}

	for (auto ptr : tablePagePtrs) {
		if (ptr && ptr->IsWindow() && ptr->m_hWnd == activePageHwnd) {
			ptr->save();
		}
	}
	return 0;
}

LRESULT RightWorkView::OnClickSaveAllButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	for (auto & ptr : queryPagePtrs) {
		if (ptr && ptr->IsWindow()) {
			ptr->save();
		}
	}

	for (auto & ptr : tablePagePtrs) {
		if (ptr && ptr->IsWindow()) {
			ptr->save();
		}
	}
	return 0;
}

LRESULT RightWorkView::OnClickExportDatabaseButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	ExportAsSqlDialog exportAsSqlDialog(m_hWnd, exportDatabaseAdapter);
	exportAsSqlDialog.DoModal(m_hWnd);

	return 0;
}

LRESULT RightWorkView::OnClickImportDatabaseButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	ImportFromSqlDialog dialog(m_hWnd, importDatabaseAdapter);
	dialog.DoModal(m_hWnd);
	return 0;
}

LRESULT RightWorkView::OnClickCreateTableButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	adapter->addNewTable(getTabRect());
	return 0;
}

LRESULT RightWorkView::OnClickOpenTableButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	adapter->showTableData(getTabRect(), 0);	
	return 0;
}

LRESULT RightWorkView::OnClickAlterTableButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	adapter->alterTable(getTabRect(), TABLE_COLUMNS_PAGE);
	return 0;
}

LRESULT RightWorkView::OnClickRenameTableButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	AppContext::getInstance()->dispatch(Config::MSG_LEFTVIEW_RENAME_TABLE_ID);
	return 0;
}

LRESULT RightWorkView::OnClickTruncateTableButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	AppContext::getInstance()->dispatch(Config::MSG_LEFTVIEW_TRUNCATE_TABLE_ID);
	return 0;
}

LRESULT RightWorkView::OnClickDropTableButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	AppContext::getInstance()->dispatch(Config::MSG_LEFTVIEW_DROP_TABLE_ID);
	return 0;
}

LRESULT RightWorkView::OnClickCopyTableButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	AppContext::getInstance()->dispatch(Config::MSG_LEFTVIEW_COPY_TABLE_ID);
	return 0;
}

LRESULT RightWorkView::OnClickShardingTableButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	AppContext::getInstance()->dispatch(Config::MSG_LEFTVIEW_SHARDING_TABLE_ID);
	return 0;
}

LRESULT RightWorkView::OnClickExportTableButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	AppContext::getInstance()->dispatch(Config::MSG_LEFTVIEW_EXPORT_TABLE_ID);
	return 0;
}

LRESULT RightWorkView::OnClickImportTableSqlButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	AppContext::getInstance()->dispatch(Config::MSG_LEFTVIEW_IMPORT_TABLE_SQL_ID);
	return 0;
}

LRESULT RightWorkView::OnClickImportTableCsvButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	AppContext::getInstance()->dispatch(Config::MSG_LEFTVIEW_IMPORT_TABLE_CSV_ID);
	return 0;
}

/**
 * Click "New table" menu or toolbar button will send this msg, wParam=NULL, lParam=NULL.
 * 
 * @param uMsg
 * @param wParam
 * @param lParam
 * @param bHandled
 * @return 
 */
LRESULT RightWorkView::OnClickNewTableElem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	adapter->addNewTable(getTabRect());
	return 0;
}



/**
 * Click "New view" menu or toolbar button will send this msg, wParam=NULL, lParam=NULL.
 * 
 * @param uMsg
 * @param wParam
 * @param lParam
 * @param bHandled
 * @return 
 */
LRESULT RightWorkView::OnClickNewViewElem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	adapter->addNewView(getTabRect());
	return 0;
}

/**
 * Click "New trigger" menu or toolbar button will send this msg, wParam=NULL, lParam=NULL.
 * 
 * @param uMsg
 * @param wParam
 * @param lParam
 * @param bHandled
 * @return 
 */
LRESULT RightWorkView::OnClickNewTriggerElem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	adapter->addNewTrigger(getTabRect());
	return 0;
}

LRESULT RightWorkView::OnClickOpenViewElem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	adapter->openView(getTabRect());
	return 0;
}

LRESULT RightWorkView::OnClickDropViewElem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	adapter->dropView();
	bHandled = 1;
	return 1;
}

LRESULT RightWorkView::OnClickOpenTriggerElem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{	
	adapter->openTrigger(getTabRect());
	return 0;
}

LRESULT RightWorkView::OnClickDropTriggerElem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	adapter->dropTrigger();
	bHandled = 1;
	return 1;
}

/**
 * Send this msg when changing the page title of this tab view , wParam=(page hwnd:HWND), lParam=(title:wchar_t *).
 * 
 * @param uMsg
 * @param wParam - page hwnd
 * @param lParam - title(wchar_t *)
 * @param bHandled
 * @return 
 */
LRESULT RightWorkView::OnChangePageTitleAndIcon(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int nPage = tabView.GetActivePage();
	if (nPage < 0) {
		return 0;
	}
	HWND hwnd = (HWND)wParam;
	wchar_t * cch = (wchar_t *)lParam;

	if (tabView.GetPageHWND(nPage) != hwnd) {
		return 0;
	}

	std::wstring title;
	for (auto pagePtr : tablePagePtrs) {
		if (pagePtr->m_hWnd == hwnd) {
			title = pagePtr->getSupplier()->getRuntimeTblName();
			tabView.SetPageTitle(nPage, title.c_str());
			tabView.SetPageImage(nPage, 2); // 2 - mod table 
		}
	}
	
	return 0;
}

/**
 * Send Config::MSG_SHOW_TABLE_DATA_ID when clicking the table open menu , wParam = NULL, lParam=NULL.
 * 
 * @param uMsg - Config::MSG_SHOW_TABLE_DATA_ID
 * @param wParam
 * @param lParam
 * @param bHandled
 * @return 
 */
LRESULT RightWorkView::OnShowTableData(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	adapter->showTableData(getTabRect(), static_cast<bool>(wParam));
	
	return 0;
}

LRESULT RightWorkView::OnClickAlterTableElem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	adapter->alterTable(getTabRect(), static_cast<TableStructurePageType>(wParam));
	return 0;
}

LRESULT RightWorkView::OnClickRenameTable(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	adapter->renameTable();
	bHandled = 1;
	return 1;
}

LRESULT RightWorkView::OnClickRrefreshSameTableData(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	uint64_t userDbId = (uint64_t)wParam;
	std::wstring tblName = *(std::wstring *)lParam;
	return adapter->refreshTableDataForSameDbTablePage(userDbId, tblName);
}

LRESULT RightWorkView::OnClickDropTable(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	adapter->dropTable();
	bHandled = 1;
	return 1;
}

LRESULT RightWorkView::OnHandleDataDirty(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bool isDirty = static_cast<bool>(lParam);
	HWND queryPageHwnd = (HWND)wParam;
	int nPage = tabView.GetActivePage();
	int n = tabView.GetPageCount();
	if (queryPageHwnd && ::IsWindow(queryPageHwnd)) {		
		for (int i = 0; i < n; i++) {
			HWND pageHwnd = tabView.GetPageHWND(i);
			if (!pageHwnd || !::IsWindow(pageHwnd)) {
				continue;
			}
			if (pageHwnd == queryPageHwnd) {
				nPage = i;
			}
		}
	}
	if (nPage < 0 || nPage >= n) {
		return 0;
	}
	if (isDirty) {
		if (tabView.GetPageImage(nPage) == 5) { // 6 - table data dirty
			tabView.SetPageImage(nPage, 6);
		} else if (tabView.GetPageImage(nPage) == 0) {
			tabView.SetPageImage(nPage, 10);
		}
	} else {
		if (tabView.GetPageImage(nPage) == 6) {
			tabView.SetPageImage(nPage, 5); // 5 - table data
		} else if (tabView.GetPageImage(nPage) == 10) {
			tabView.SetPageImage(nPage, 0);
		}
	}
	
	return 0;
}

LRESULT RightWorkView::OnHandleTableStructureDirty(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bool isDirty = static_cast<bool>(lParam);
	HWND tableStructurePageHwnd = (HWND)wParam;
	int nPage = tabView.GetActivePage();
	int n = tabView.GetPageCount();
	if (tableStructurePageHwnd && ::IsWindow(tableStructurePageHwnd)) {		
		for (int i = 0; i < n; i++) {
			HWND pageHwnd = tabView.GetPageHWND(i);
			if (!pageHwnd || !::IsWindow(pageHwnd)) {
				continue;
			}
			if (pageHwnd == tableStructurePageHwnd) {
				nPage = i;
			}
		}
	}
	if (nPage < 0 || nPage >= n) {
		return 0;
	}
	
	HWND pageHwnd = tabView.GetPageHWND(nPage);
	auto iter = std::find_if(tablePagePtrs.begin(), tablePagePtrs.end(), [&pageHwnd](const TableStructurePage * ptr) {
		return ptr->m_hWnd == pageHwnd;
	});
	if (iter == tablePagePtrs.end()) {
		return 0;
	}
	TableStructurePage * ptr = *iter;
	if (!ptr->getSupplier()->compareDatas()) {
		if (ptr->getSupplier()->getOperateType() == NEW_TABLE 
			&& tabView.GetPageImage(nPage) != 9) { // 9 - new table structure dirty
			tabView.SetPageImage(nPage, 9);
		} else if (ptr->getSupplier()->getOperateType() == MOD_TABLE 
			&& tabView.GetPageImage(nPage) != 7) { // 7 - alter table structure dirty
			tabView.SetPageImage(nPage, 7);
		}
		return 0;
	}

	if (ptr->getSupplier()->getOperateType() == NEW_TABLE 
		&& tabView.GetPageImage(nPage) != 8) { // 8 - new table structure
		tabView.SetPageImage(nPage, 8);
	} else if (ptr->getSupplier()->getOperateType() == MOD_TABLE 
		&& tabView.GetPageImage(nPage) != 2) { // 2 - alter table structure
		tabView.SetPageImage(nPage, 2);
	}
	
	return 0;
}

LRESULT RightWorkView::OnChangeTreeviewItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//if (createTableButton.IsWindow()) enableButton(createTableButton, std::wstring(L"create-table"));
	if (openTableButton.IsWindow()) enableButton(openTableButton, std::wstring(L"open-table"));
	if (alterTableButton.IsWindow()) enableButton(alterTableButton, std::wstring(L"alter-table"));
	if (renameTableButton.IsWindow()) enableButton(renameTableButton, std::wstring(L"rename-table"));
	if (truncateTableButton.IsWindow()) enableButton(truncateTableButton, std::wstring(L"truncate-table"));
	if (dropTableButton.IsWindow()) enableButton(dropTableButton, std::wstring(L"drop-table"));
	if (copyTableButton.IsWindow()) enableButton(copyTableButton, std::wstring(L"copy-table"));
	if (shardingTableButton.IsWindow()) enableButton(shardingTableButton, std::wstring(L"sharding-table"));
	if (exportTableButton.IsWindow()) enableButton(exportTableButton, std::wstring(L"export-table")); 
	if (importTableSqlButton.IsWindow()) enableButton(importTableSqlButton, std::wstring(L"import-table-sql"));
	if (importTableCsvButton.IsWindow()) enableButton(importTableCsvButton, std::wstring(L"import-table-csv"));

	return 0;
}

LRESULT RightWorkView::OnExecSqlResultMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	auto runtimeResultInfo = (ResultInfo *)lParam;
	if (runtimeResultInfo == nullptr) {
		return 0;
	}
	runtimeResultInfo->createdAt = runtimeResultInfo->createdAt.empty()  ? 
		DateUtil::getCurrentDateTime() : runtimeResultInfo->createdAt;
	// save sql log to db
	sqlLogService->createSqlLog(*runtimeResultInfo);
	return 0;
}

LRESULT RightWorkView::OnDeleteDatabase(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	uint64_t userDbId = static_cast<uint64_t>(wParam);
	if (userDbId != databaseSupplier->getSelectedUserDbId()) {
		return 1;
	}

	int n = tabView.GetPageCount();
	std::vector<int> erasePages;
	for (int i = 0; i < n; i++) {
		HWND pageHwnd = tabView.GetPageHWND(i);
		std::wstring title = tabView.GetPageTitle(i);
		// Close the QueryPage if query page belong userDbId WHEN delete database 
		bool found = false;
		for (auto iter = queryPagePtrs.begin(); iter != queryPagePtrs.end(); iter++) {
			auto & ptr = *iter;
			if (ptr->m_hWnd != pageHwnd) {
				continue;
			}
			if (ptr->getSupplier()->getOperateType() != TABLE_DATA) {
				continue;
			}
			if (ptr->getSupplier()->getRuntimeUserDbId() != userDbId) {
				continue;
			}

			if (ptr && ptr->IsWindow()) {
				ptr->DestroyWindow();
				ptr->m_hWnd = nullptr;
			}
			if (ptr) {
				delete ptr;
				ptr = nullptr;
			}
			queryPagePtrs.erase(iter);
			erasePages.push_back(i);
			found = true;
			break;
		}
		if (found) {
			continue;
		}

		// Close the QueryPage if query page belong userDbId WHEN delete database 
		for (auto iter = tablePagePtrs.begin(); iter != tablePagePtrs.end(); iter++) {
			auto & ptr = *iter;
			if (ptr->m_hWnd != pageHwnd) {
				continue;
			}
			if (ptr->getSupplier()->getOperateType() != NEW_TABLE && ptr->getSupplier()->getOperateType() != MOD_TABLE) {
				continue;
			}
			if (ptr->getSupplier()->getRuntimeUserDbId() != userDbId) {
				continue;
			}
		
			if (ptr && ptr->IsWindow()) {
				ptr->DestroyWindow();
				ptr->m_hWnd = nullptr;
			}
			if (ptr) {
				delete ptr;
				ptr = nullptr;
			}
			tablePagePtrs.erase(iter);
			erasePages.push_back(i);
			found = true;
			break;
		}
	}

	n = static_cast<int>(erasePages.size());
	for (int i = n - 1; i >= 0; i--) {
		auto nPage = erasePages.at(i);
		tabView.RemovePage(nPage);
	}
	
	return 1;
}

LRESULT RightWorkView::OnTabViewPageActivated(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
	int nPage = tabView.GetActivePage();
	if (nPage < 0) {
		return 0;
	}
	HWND hwndPage = tabView.GetPageHWND(nPage);
	databaseSupplier->activeTabPageHwnd = hwndPage;

	return 0;
}

LRESULT RightWorkView::OnTabViewCloseBtn(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
	int nPage = static_cast<int>(pnmh->idFrom);
	return closeTabViewPage(nPage);
}

LRESULT RightWorkView::closeTabViewPage(int nPage)
{
	HWND pageHwnd = tabView.GetPageHWND(nPage);
	if (pageHwnd == historyPage.m_hWnd) {
		if (historyPage.IsWindow()) {
			historyPage.DestroyWindow();
		}
		return 0;
	}

	// check the close page if it is a QueryPage
	for (auto & iter = queryPagePtrs.begin(); iter != queryPagePtrs.end(); iter++) {
		QueryPage * ptr = *iter;
		if (pageHwnd != ptr->m_hWnd) {
			continue;
		}
		if (!ptr->getSupplier()->getIsDirty()) {
			if (ptr && ptr->IsWindow()) {
				queryPagePtrs.erase(iter);
				ptr->DestroyWindow();
				ptr->m_hWnd = nullptr;
				delete ptr;
				ptr = nullptr;
			} else if (ptr) {
				ptr->m_hWnd = nullptr;
				queryPagePtrs.erase(iter);
				delete ptr;
				ptr = nullptr;
			}
			break;
		}
		// data has changed
		if (QMessageBox::confirm(m_hWnd, S(L"data-has-changed"), S(L"not-save-and-close"), S(L"cancel"))
			== Config::CUSTOMER_FORM_YES_BUTTON_ID) {
			if (ptr && ptr->IsWindow()) {
				queryPagePtrs.erase(iter);
				ptr->DestroyWindow();
				delete ptr;
				ptr = nullptr;
			} else if (ptr) {
				queryPagePtrs.erase(iter);
				delete ptr;
				ptr = nullptr;
			}
			return 0; // 0 - force close
		}
		else {
			return 1; // 1 - cancel close
		}
	}

	// check the close page if it is a TableStructurePage
	for (auto & iter = tablePagePtrs.begin(); iter != tablePagePtrs.end(); iter++) {
		TableStructurePage * ptr = *iter;
		if (pageHwnd != ptr->m_hWnd) {
			continue;
		}
		if (!ptr->getSupplier()->getIsDirty()) {
			if (ptr && ptr->IsWindow()) {
				tablePagePtrs.erase(iter);
				ptr->DestroyWindow();
				ptr->m_hWnd = nullptr;
				delete ptr;
				ptr = nullptr;
			} else if (ptr) {
				tablePagePtrs.erase(iter);
				ptr->m_hWnd = nullptr;
				delete ptr;
				ptr = nullptr;
			}
			break;
		}

		// data has changed
		if (QMessageBox::confirm(m_hWnd, S(L"data-has-changed"), S(L"not-save-and-close"), S(L"cancel"))
			== Config::CUSTOMER_FORM_YES_BUTTON_ID) {
			if (ptr && ptr->IsWindow()) {
				tablePagePtrs.erase(iter);
				ptr->DestroyWindow();
				delete ptr;
				ptr = nullptr;
			} else if (ptr) {
				tablePagePtrs.erase(iter);
				delete ptr;
				ptr = nullptr;
			}
			return 0; // 0 - force close
		}
		else {
			return 1; // 1 - cancel close
		}
	}

	int n = tabView.GetPageCount();
	if (!n) {
		UpdateWindow();
	}
	return 0;
}

LRESULT RightWorkView::OnTabViewContextMenu(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
	LPTBVCONTEXTMENUINFO menuInfo = (LPTBVCONTEXTMENUINFO)pnmh;
	databaseSupplier->nSelTabPage =  static_cast<int>(menuInfo->hdr.idFrom);
	CPoint pt = menuInfo->pt;
	adapter->popupTabMenu(pt); 
	return 0;
}

HBRUSH RightWorkView::OnCtlColorStatic(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, topbarColor);
	::SetTextColor(hdc, textColor);
	::SelectObject(hdc, textFont);
	return topbarBrush.m_hBrush;
}

void RightWorkView::OnClickCloseThisMenu(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	ATLASSERT(databaseSupplier->nSelTabPage >= 0);
	int & nPage = databaseSupplier->nSelTabPage;
	closeTabViewPage(nPage);
	tabView.RemovePage(nPage);
}

void RightWorkView::OnClickCloseOthersMenu(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	ATLASSERT(databaseSupplier->nSelTabPage >= 0);
	int & nPage = databaseSupplier->nSelTabPage;
	OnClickCloseRightMenu(uNotifyCode, nID, wndCtl);
	OnClickCloseLeftMenu(uNotifyCode, nID, wndCtl);
}

void RightWorkView::OnClickCloseLeftMenu(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	ATLASSERT(databaseSupplier->nSelTabPage >= 0);
	int n = tabView.GetPageCount();
	int nPage = databaseSupplier->nSelTabPage;
	for (int i = nPage - 1; i >= 0; i--) {
		closeTabViewPage(i);
		tabView.RemovePage(i);
	}
}

void RightWorkView::OnClickCloseRightMenu(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	ATLASSERT(databaseSupplier->nSelTabPage >= 0); 
	int n = tabView.GetPageCount();
	int nPage = databaseSupplier->nSelTabPage;
	for (int i = n - 1; i > nPage; i--) {
		closeTabViewPage(i);
		tabView.RemovePage(i);
	}
}
