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
 *                         |                          |-> ResultTableInfoPage
 *                         |-> TableStructurePage
 *                         |       |-> TableTabView
 *                         |              |->QTabView(tabView)  
 *                         |                    |->TableColumnsPage
 *                         |                    |->TableIndexesPage
 *                         |                    |->TableForeignKeysPage
 *                         |-> HistoryPage
 * @author Xuehan Qin
 * @date   2023-05-21
 *********************************************************************/
#include "stdafx.h"
#include "RightWorkView.h"
#include "common/AppContext.h"
#include "core/common/Lang.h"
#include "ui/common/QWinCreater.h"
#include "ui/database/rightview//page/dialog/ViewDialog.h"
#include "ui/database/rightview//page/dialog/TriggerDialog.h"
#include "ui/common/message/QPopAnimate.h"


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
	return FALSE;
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

	imageList.Create(16, 16, ILC_COLOR32, 7, 7);
	imageList.AddIcon(queryIcon); // 0 - query
	imageList.AddIcon(historyIcon); // 1 - history
	imageList.AddIcon(tableIcon); // 2 - new table
	imageList.AddIcon(viewIcon); // 3 - view
	imageList.AddIcon(triggerIcon); // 4 - trigger
	imageList.AddIcon(tableDataIcon); // 5 - table data
	imageList.AddIcon(tableDataDirtyIcon); // 6 - table data dirty
	imageList.AddIcon(tableStructureDirtyIcon); // 7 - table structure dirty
	
}

CRect RightWorkView::getTopRect(CRect & clientRect)
{
	return { 0, 0, clientRect.right, RIGHTVIEW_TOPBAR_HEIGHT };
}

CRect RightWorkView::getTabRect(CRect & clientRect)
{
	return { 0, RIGHTVIEW_TOPBAR_HEIGHT, clientRect.right, clientRect.bottom };
}

void RightWorkView::createOrShowUI()
{
	CRect clientRect;
	GetClientRect(clientRect);
	createOrShowToolButtons(clientRect);
	createOrShowTabView(tabView, clientRect);
	createFirstQueryPage(clientRect);
	createOrShowHistoryPage(historyPage, clientRect);	
}


void RightWorkView::createOrShowToolButtons(CRect & clientRect)
{
	int x = 10 , y = 7, w = RIGHTVIEW_BUTTON_WIDTH, h = RIGHTVIEW_BUTTON_WIDTH;
	CRect rect(x, y, x + w, y + h);
	std::wstring imgDir = ResourceUtil::getProductImagesDir();
	std::wstring normalImagePath = imgDir + L"database\\button\\exec-sql-button-normal.png";
	std::wstring pressedImagePath = imgDir + L"database\\button\\exec-sql-button-pressed.png";
	execSqlButton.SetIconPath(normalImagePath, pressedImagePath);
	execSqlButton.SetBkgColors(RGB(238, 238, 238), RGB(238, 238, 238), RGB(238, 238, 238));
	QWinCreater::createOrShowButton(m_hWnd, execSqlButton, Config::DATABASE_EXEC_SQL_BUTTON_ID, L"", rect, clientRect);
	execSqlButton.SetToolTip(S(L"exec-select-sql"));

	rect.OffsetRect(w + 10, 0);
	normalImagePath = imgDir + L"database\\button\\exec-all-button-normal.png";
	pressedImagePath = imgDir + L"database\\button\\exec-all-button-pressed.png";
	execAllButton.SetIconPath(normalImagePath, pressedImagePath);
	execAllButton.SetBkgColors(RGB(238, 238, 238), RGB(238, 238, 238), RGB(238, 238, 238));
	QWinCreater::createOrShowButton(m_hWnd, execAllButton, Config::DATABASE_EXEC_ALL_BUTTON_ID, L"", rect, clientRect);
	execAllButton.SetToolTip(S(L"exec-all-sql"));
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

void RightWorkView::createFirstQueryPage(CRect & clientRect)
{
	//queryPagePtrs.clear();
	QueryPage * firstQueryPage = nullptr;
	if (queryPagePtrs.empty()) {
		firstQueryPage = new QueryPage();
		firstQueryPage->setup(QUERY_DATA);
		createOrShowQueryPage(*firstQueryPage, clientRect); 
		queryPagePtrs.push_back(firstQueryPage);
	} else {
		firstQueryPage = queryPagePtrs.at(0);
		createOrShowQueryPage(*firstQueryPage, clientRect);		
	}
}

void RightWorkView::createOrShowHistoryPage(HistoryPage &win, CRect & clientRect)
{
	CRect tabRect = getTabRect(clientRect);
	int x = 1, y = tabView.m_cyTabHeight + 1, w = tabRect.Width() - 2, h = tabRect.Height() - tabView.m_cyTabHeight - 2;
	CRect rect(x, y, x + w, y + h);
	if (IsWindow() && !win.IsWindow()) {
		win.Create(tabView.m_hWnd, rect, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN , 0);		
	} else if (IsWindow() && tabView.IsWindow()) {
		win.MoveWindow(rect);
		win.ShowWindow(true);
	}
}

void RightWorkView::createOrShowQueryPage(QueryPage &win, CRect & clientRect)
{
	CRect tabRect = getTabRect(clientRect);
	int x = 1, y = tabView.m_cyTabHeight + 1, w = tabRect.Width() - 2, h = tabRect.Height() - tabView.m_cyTabHeight - 2;
	CRect rect(x, y, x + w, y + h);
	if (IsWindow() && !win.IsWindow()) {
		win.Create(tabView.m_hWnd, rect, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN , 0);
	}
	else if (IsWindow() && tabView.IsWindow()) {
		win.MoveWindow(rect);
		win.ShowWindow(true);
	}
}

void RightWorkView::createOrShowTableStructurePage(TableStructurePage &win, CRect & clientRect)
{
	CRect tabRect = getTabRect(clientRect);
	int x = 1, y = tabView.m_cyTabHeight + 1, w = tabRect.Width() - 2, h = tabRect.Height() - tabView.m_cyTabHeight - 2;
	CRect rect(x, y, x + w, y + h);
	if (IsWindow() && !win.IsWindow()) {
		win.Create(tabView.m_hWnd, rect, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN , 0);
	}
	else if (IsWindow() && tabView.IsWindow()) {
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
	createImageList();

	topbarBrush.CreateSolidBrush(topbarColor);
	bkgBrush.CreateSolidBrush(bkgColor);
	return 0;
}

int RightWorkView::OnDestroy()
{
	AppContext::getInstance()->unsuscribe(m_hWnd, Config::MSG_NEW_TABLE_ID);
	AppContext::getInstance()->unsuscribe(m_hWnd, Config::MSG_NEW_VIEW_ID);
	AppContext::getInstance()->unsuscribe(m_hWnd, Config::MSG_OPEN_VIEW_ID);
	AppContext::getInstance()->unsuscribe(m_hWnd, Config::MSG_DROP_VIEW_ID);
	AppContext::getInstance()->unsuscribe(m_hWnd, Config::MSG_NEW_TRIGGER_ID);
	AppContext::getInstance()->unsuscribe(m_hWnd, Config::MSG_OPEN_TRIGGER_ID);
	AppContext::getInstance()->unsuscribe(m_hWnd, Config::MSG_DROP_TRIGGER_ID);
	AppContext::getInstance()->unsuscribe(m_hWnd, Config::MSG_SHOW_TABLE_DATA_ID);
	AppContext::getInstance()->unsuscribe(m_hWnd, Config::MSG_ALTER_TABLE_ID);
	AppContext::getInstance()->unsuscribe(m_hWnd, Config::MSG_RENAME_TABLE_ID);
	AppContext::getInstance()->unsuscribe(m_hWnd, Config::MSG_REFRESH_SAME_TABLE_DATA_ID);
	AppContext::getInstance()->unsuscribe(m_hWnd, Config::MSG_DROP_TABLE_ID);
	AppContext::getInstance()->unsuscribe(m_hWnd, Config::MSG_COPY_TABLE_ID);
	AppContext::getInstance()->unsuscribe(m_hWnd, Config::MSG_EXPORT_TABLE_ID);
	AppContext::getInstance()->unsuscribe(m_hWnd, Config::MSG_TABLE_IMPORT_SQL_ID);
	AppContext::getInstance()->unsuscribe(m_hWnd, Config::MSG_TABLE_IMPORT_CSV_ID);
	AppContext::getInstance()->unsuscribe(m_hWnd, Config::MSG_TABLE_MANAGE_INDEX_ID);
	AppContext::getInstance()->unsuscribe(m_hWnd, Config::MSG_TABLE_PROPERTIES_ID);

	if (!bkgBrush.IsNull()) bkgBrush.DeleteObject();
	if (!topbarBrush.IsNull()) topbarBrush.DeleteObject();
	
	if (execSqlButton.IsWindow()) execSqlButton.DestroyWindow();
	if (execAllButton.IsWindow()) execAllButton.DestroyWindow();

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
	int nPage = tabView.GetActivePage();
	if (nPage < 0) {
		return 0;
	}
	HWND activeHwnd = tabView.GetPageHWND(nPage);
	for (auto pagePtr : queryPagePtrs) {
		if (pagePtr && pagePtr->IsWindow () && activeHwnd == pagePtr->m_hWnd) {
			pagePtr->execAndShow(true);
		}		
	}
	// execute sql statements from supplier->sqlVector
	return 0;
}

LRESULT RightWorkView::OnClickExecAllButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	int nPage = tabView.GetActivePage();
	if (nPage < 0) {
		return 0;
	}
	HWND activeHwnd = tabView.GetPageHWND(nPage);
	for (auto pagePtr : queryPagePtrs) {
		if (pagePtr && pagePtr->IsWindow () && activeHwnd == pagePtr->m_hWnd) {
			pagePtr->execAndShow(false);
		}		
	}
	
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
	doAddNewTable();
	return 0;
}

void RightWorkView::doAddNewTable()
{
	CRect clientRect;
	GetClientRect(clientRect);
	TableStructurePage * newTablePage = new TableStructurePage();
	newTablePage->setup(PageOperateType::NEW_TABLE);
	createOrShowTableStructurePage(*newTablePage, clientRect);
	tablePagePtrs.push_back(newTablePage);

	// nImage = 3 : VIEW 
	tabView.AddPage(newTablePage->m_hWnd, StringUtil::blkToTail(S(L"new-table")).c_str(), 2, newTablePage);

	databaseSupplier->activeTabPageHwnd = newTablePage->m_hWnd;
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
	doAddNewView();
	return 0;
}

void RightWorkView::doAddNewView()
{
	ViewDialog viewDialog(m_hWnd);
	if (viewDialog.DoModal(m_hWnd) != Config::QDIALOG_YES_BUTTON_ID) {
		return ;
	}
	
	CRect clientRect;
	GetClientRect(clientRect);
	std::wstring binDir = ResourceUtil::getProductBinDir();
	std::wstring tplPath = binDir + L"res\\tpl\\create-view-sql.tpl";
	std::wstring content = FileUtil::readFile(tplPath);
	std::wstring viewName = L"\"" + databaseSupplier->newViewName + L"\"";
	content = StringUtil::replace(content, L"{<!--view_name-->}", viewName);

	QueryPage * newViewPage = new QueryPage();
	newViewPage->setup(CREATE_VIEW, content);
	createOrShowQueryPage(*newViewPage, clientRect);
	queryPagePtrs.push_back(newViewPage);

	// nImage = 3 : view 
	tabView.AddPage(newViewPage->m_hWnd, StringUtil::blkToTail(S(L"new-view")).c_str(), 3, newViewPage);
	databaseSupplier->activeTabPageHwnd = newViewPage->m_hWnd;
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
	doAddNewTrigger();
	return 0;
}

LRESULT RightWorkView::OnClickOpenViewElem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UserView userView = databaseService->getUserView(databaseSupplier->getSelectedUserDbId(), databaseSupplier->selectedViewName);
	if (userView.name.empty()) {
		QPopAnimate::error(E(L"200004"));
		return 0;
	}
	std::wstring & content = userView.sql;
	CRect clientRect;
	GetClientRect(clientRect);

	QueryPage * newViewPage = new QueryPage();
	newViewPage->setup(MODIFY_VIEW, content);
	createOrShowQueryPage(*newViewPage, clientRect);
	queryPagePtrs.push_back(newViewPage);

	// nImage = 3 : view 
	std::wstring name = L"[View]";
	name.append(userView.name);
	tabView.AddPage(newViewPage->m_hWnd, StringUtil::blkToTail(name).c_str(), 3, newViewPage);
	databaseSupplier->activeTabPageHwnd = newViewPage->m_hWnd;

	return 0;
}

LRESULT RightWorkView::OnClickDropViewElem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int n = tabView.GetPageCount();
	uint64_t userDbId = databaseSupplier->getSelectedUserDbId();
	std::wstring viewName =  L"[View]" + databaseSupplier->selectedViewName;
	std::stack<int> delPageStack;
	for (int i = 0; i < n; i++) {
		HWND pageHwnd = tabView.GetPageHWND(i);
		std::wstring pageTitle = tabView.GetPageTitle(i);
		StringUtil::trim(pageTitle);
		// same name
		if (pageTitle != viewName) {
			continue;
		}
		// query page has properties of TABLE_DATA and  same user db id
		auto iter = std::find_if(queryPagePtrs.begin(), queryPagePtrs.end(), [&pageHwnd, &userDbId](QueryPage * page) {
			if (page && page->IsWindow() 
				&& page->m_hWnd == pageHwnd 
				&& page->getSupplier()->getOperateType() == MODIFY_VIEW 
				&& page->getSupplier()->getRuntimeUserDbId() == userDbId) {
				page->DestroyWindow();
				delete page;
				page = nullptr;
				return true;
			}
			return false;
		});

		// reload table data
		if (iter != queryPagePtrs.end()) {
			queryPagePtrs.erase(iter);
			delPageStack.push(i);
			continue;
		}
	}

	while (!delPageStack.empty()) {
		int nPage = delPageStack.top();
		delPageStack.pop();
		tabView.RemovePage(nPage);
	}
	bHandled = 1;
	return 1;
}

LRESULT RightWorkView::OnClickOpenTriggerElem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UserTrigger userTrigger = databaseService->getUserTrigger(databaseSupplier->getSelectedUserDbId(), databaseSupplier->selectedTriggerName);
	if (userTrigger.name.empty()) {
		QPopAnimate::error(E(L"200004"));
		return 0;
	}
	std::wstring & content = userTrigger.sql;
	CRect clientRect;
	GetClientRect(clientRect);

	QueryPage * newViewPage = new QueryPage();
	newViewPage->setup(MODIFY_TRIGGER, content);
	createOrShowQueryPage(*newViewPage, clientRect);
	queryPagePtrs.push_back(newViewPage);

	// nImage = 4 : trigger 
	std::wstring name = L"[Trigger]";
	name.append(userTrigger.name);
	tabView.AddPage(newViewPage->m_hWnd, StringUtil::blkToTail(name).c_str(), 4, newViewPage);
	databaseSupplier->activeTabPageHwnd = newViewPage->m_hWnd;

	return 0;
}

LRESULT RightWorkView::OnClickDropTriggerElem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int n = tabView.GetPageCount();
	uint64_t userDbId = databaseSupplier->getSelectedUserDbId();
	std::wstring & triggerName = databaseSupplier->selectedTriggerName;
	std::wstring vname = L"[Trigger]" + triggerName;
	std::stack<int> delPageStack;
	for (int i = 0; i < n; i++) {
		HWND pageHwnd = tabView.GetPageHWND(i);
		std::wstring pageTitle = tabView.GetPageTitle(i);
		StringUtil::trim(pageTitle);		
		// same name
		if (pageTitle != vname) {
			continue;
		}
		// query page has properties of MODIFY_TRIGGER and  same user db id
		auto iter = std::find_if(queryPagePtrs.begin(), queryPagePtrs.end(), [&pageHwnd, &userDbId](QueryPage * page) {
			if (page && page->IsWindow() 
				&& page->m_hWnd == pageHwnd 
				&& page->getSupplier()->getOperateType() == MODIFY_TRIGGER
				&& page->getSupplier()->getRuntimeUserDbId() == userDbId) {
				page->DestroyWindow();
				delete page;
				page = nullptr;
				return true;
			}
			return false;
		});

		// erase from queryPagePtrs
		if (iter != queryPagePtrs.end()) {
			queryPagePtrs.erase(iter);
			delPageStack.push(i);
			continue;
		}
	}

	// erase from tabView pages
	while (!delPageStack.empty()) {
		int nPage = delPageStack.top();
		delPageStack.pop();
		tabView.RemovePage(nPage);
	}
	bHandled = 1;
	return 1;
}

void RightWorkView::doAddNewTrigger()
{
	TriggerDialog triggerDialog(m_hWnd);
	if (triggerDialog.DoModal(m_hWnd) != Config::QDIALOG_YES_BUTTON_ID) {
		return ;
	}

	std::wstring binDir = ResourceUtil::getProductBinDir();
	std::wstring tplPath = binDir + L"res\\tpl\\create-trigger-sql.tpl";
	std::wstring content = FileUtil::readFile(tplPath);
	std::wstring viewName = L"\"" + databaseSupplier->newViewName + L"\"";
	content = StringUtil::replace(content, L"{<!--trigger_name-->}", viewName);

	CRect clientRect;
	GetClientRect(clientRect);
	QueryPage * newTriggerPage = new QueryPage();
	newTriggerPage->setup(CREATE_TRIGGER, content);
	createOrShowQueryPage(*newTriggerPage, clientRect); 
	queryPagePtrs.push_back(newTriggerPage); 

	// nImage = 2 : table 
	tabView.AddPage(newTriggerPage->m_hWnd, StringUtil::blkToTail(S(L"new-trigger")).c_str(), 4, newTriggerPage);

	databaseSupplier->activeTabPageHwnd = newTriggerPage->m_hWnd;
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
LRESULT RightWorkView::OnChangePageTitle(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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
	if (databaseSupplier->selectedTable.empty()) {
		return 0;
	}
	// 1.find the queryPage title is equal select table name
	int n = tabView.GetPageCount();
	int foundQueryPage = -1;
	for (int i = 0; i < n; i++) {
		HWND pageHwnd = tabView.GetPageHWND(i);
		std::wstring title = tabView.GetPageTitle(i);
		StringUtil::trim(title); // trim the title
		if (title != databaseSupplier->selectedTable) {
			continue;
		}

		auto iter = std::find_if(queryPagePtrs.begin(), queryPagePtrs.end(), [&pageHwnd](QueryPage * ptr) {
			return ptr->m_hWnd == pageHwnd ;
		});
		if (iter == queryPagePtrs.end()) {
			continue;
		}
		foundQueryPage = i; // found, and active this page and load table data
		tabView.SetActivePage(foundQueryPage);
		QueryPage * tableDataPage = (*iter);
		if (wParam == 1) { // 1- properties page
			//active table info page
			tableDataPage->getResultTabView().activeTablePropertiesPage(); // active table properties page first
		} else {
			tableDataPage->getResultTabView().activeTableDataPage(); // active table data page first
		}
		
		tableDataPage->getResultTabView().loadTableDatas(databaseSupplier->selectedTable);
		return 0;
	}

	// 2.check if exists a queryPage of same table name in tabView, then active this queryPage, 
	//   otherwise create a new queryPage for show table data
	if (foundQueryPage == -1) { // not found, create
		CRect clientRect;
		GetClientRect(clientRect);
		QueryPage * tableDataPage = new QueryPage();
		tableDataPage->setup(TABLE_DATA);
		createOrShowQueryPage(*tableDataPage, clientRect);
		queryPagePtrs.push_back(tableDataPage);

		// nImage = 3 : view 
		std::wstring tblName = databaseSupplier->selectedTable;
		tabView.AddPage(tableDataPage->m_hWnd, StringUtil::blkToTail(tblName).c_str(), 5, tableDataPage);
		foundQueryPage = tabView.GetPageCount() - 1;
		databaseSupplier->activeTabPageHwnd = tableDataPage->m_hWnd;
		tabView.SetActivePage(foundQueryPage);	
		if (wParam == 1) { // 1- properties page
			//active table info page
			tableDataPage->getResultTabView().activeTablePropertiesPage(); // active table properties page first
		} else {
			tableDataPage->getResultTabView().activeTableDataPage(); // active table data page first
		}
		tableDataPage->getResultTabView().loadTableDatas(databaseSupplier->selectedTable); 
	}

	
	return 0;
}

LRESULT RightWorkView::OnClickAlterTableElem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ATLASSERT(!databaseSupplier->selectedTable.empty()); 

	// 1.find the queryPage title is equal select table name
	int n = tabView.GetPageCount();
	int foundPage = -1;
	for (int i = 0; i < n; i++) {
		HWND pageHwnd = tabView.GetPageHWND(i);
		std::wstring title = tabView.GetPageTitle(i);
		StringUtil::trim(title); // trim the title
		if (title != databaseSupplier->selectedTable) {
			continue;
		}
		uint64_t userDbId = databaseSupplier->getSelectedUserDbId();
		auto iter = std::find_if(tablePagePtrs.begin(), tablePagePtrs.end(), [&pageHwnd, &userDbId](TableStructurePage * ptr) {
			return ptr->m_hWnd == pageHwnd && userDbId == ptr->getSupplier()->getRuntimeUserDbId();
		});
		if (iter == tablePagePtrs.end()) {
			continue;
		}
		foundPage = i; // found, and active this page and load table data
		
		TableStructurePage * tableStructPage = (*iter);
		tableStructPage->activePage(static_cast<TableStructurePageType>(wParam));
		databaseSupplier->activeTabPageHwnd = tableStructPage->m_hWnd;
		tabView.SetActivePage(foundPage);
		return 0;
	}

	// 2.check if exists a tableStructPage of same table name in tabView, then active this tableStructPage, 
	//   otherwise create a new tableStructPage for show table data
	if (foundPage == -1) { // not found, create
		CRect clientRect;
		GetClientRect(clientRect);
		TableStructurePage * tableStructPage = new TableStructurePage();
		tableStructPage->setup(PageOperateType::MOD_TABLE, databaseSupplier->selectedTable, (TableStructurePageType)wParam);
		createOrShowTableStructurePage(*tableStructPage, clientRect);
		tablePagePtrs.push_back(tableStructPage);

		std::wstring tblName = databaseSupplier->selectedTable;
		tabView.AddPage(tableStructPage->m_hWnd, StringUtil::blkToTail(tblName).c_str(), 2, tableStructPage);
		foundPage = tabView.GetPageCount() - 1;
		databaseSupplier->activeTabPageHwnd = tableStructPage->m_hWnd;
		tabView.SetActivePage(foundPage);
	}
	return 0;
}

LRESULT RightWorkView::OnClickRenameTable(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int n = tabView.GetPageCount();
	uint64_t userDbId = databaseSupplier->getSelectedUserDbId();
	std::wstring & oldTableName = databaseSupplier->oldTableName;
	std::wstring & newTableName = databaseSupplier->newTableName;
	for (int i = 0; i < n; i++) {
		HWND pageHwnd = tabView.GetPageHWND(i);
		std::wstring pageTitle = tabView.GetPageTitle(i);
		StringUtil::trim(pageTitle);
		// same name
		if (pageTitle != oldTableName) {
			continue;
		}
		// query page has properties of TABLE_DATA and  same user db id
		auto iter = std::find_if(queryPagePtrs.begin(), queryPagePtrs.end(), [&pageHwnd, &userDbId](QueryPage * page) {
			if (page && page->IsWindow() 
				&& page->m_hWnd == pageHwnd 
				&& page->getSupplier()->getOperateType() == TABLE_DATA 
				&& page->getSupplier()->getRuntimeUserDbId() == userDbId) {
				return true;
			}
			return false;
		});
		if (iter != queryPagePtrs.end()) {
			tabView.SetPageTitle(i, newTableName.c_str());
			(*iter)->getResultTabView().loadTableDatas(newTableName);
			continue;
		}

		// database page has properties of TABLE_DATA and  same user db id
		auto iter2 = std::find_if(tablePagePtrs.begin(), tablePagePtrs.end(), [&pageHwnd, &userDbId](TableStructurePage * page) {
			if (page && page->IsWindow() 
				&& page->m_hWnd == pageHwnd 
				&& page->getSupplier()->getOperateType() == MOD_TABLE 
				&& page->getSupplier()->getRuntimeUserDbId() == userDbId) {
				return true;
			}
			return false;
		});
		if (iter2 != tablePagePtrs.end()) {
			tabView.SetPageTitle(i, newTableName.c_str());
			(*iter2)->renameTable(oldTableName, newTableName);
		}
	}
	bHandled = 1;
	return 1;
}

LRESULT RightWorkView::OnClickRrefreshSameTableData(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	uint64_t userDbId = (uint64_t)wParam;
	std::wstring tblName = *(std::wstring *)lParam;
	return doRefreshTableDataForSameDbTablePage(userDbId, tblName);
	return 0;
}

LRESULT RightWorkView::OnClickDropTable(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int n = tabView.GetPageCount();
	uint64_t userDbId = databaseSupplier->getSelectedUserDbId();
	std::wstring & tblName = databaseSupplier->selectedTable;
	std::stack<int> delPageStack;
	for (int i = 0; i < n; i++) {
		HWND pageHwnd = tabView.GetPageHWND(i);
		std::wstring pageTitle = tabView.GetPageTitle(i);
		StringUtil::trim(pageTitle);
		// same name
		if (pageTitle != tblName) {
			continue;
		}
		// query page has properties of TABLE_DATA and  same user db id
		auto iter = std::find_if(queryPagePtrs.begin(), queryPagePtrs.end(), [&pageHwnd, &userDbId](QueryPage * page) {
			if (page && page->IsWindow() 
				&& page->m_hWnd == pageHwnd 
				&& page->getSupplier()->getOperateType() == TABLE_DATA 
				&& page->getSupplier()->getRuntimeUserDbId() == userDbId) {
				page->DestroyWindow();
				delete page;
				page = nullptr;
				return true;
			}
			return false;
		});

		// reload table data
		if (iter != queryPagePtrs.end()) {
			queryPagePtrs.erase(iter);
			delPageStack.push(i);
			continue;
		}

		// database page has properties of TABLE_DATA and  same user db id
		auto iter2 = std::find_if(tablePagePtrs.begin(), tablePagePtrs.end(), [&pageHwnd, &userDbId](TableStructurePage * page) {
			if (page && page->IsWindow() 
				&& page->m_hWnd == pageHwnd 
				&& page->getSupplier()->getOperateType() == MOD_TABLE 
				&& page->getSupplier()->getRuntimeUserDbId() == userDbId) {
				page->DestroyWindow();
				delete page;
				page = nullptr;
				return true;
			}
			return false;
		});
		if (iter2 != tablePagePtrs.end()) {
			tablePagePtrs.erase(iter2);
			delPageStack.push(i);	
		}
	}

	while (!delPageStack.empty()) {
		int nPage = delPageStack.top();
		delPageStack.pop();
		tabView.RemovePage(nPage);
	}
	bHandled = 1;
	return 1;
}

LRESULT RightWorkView::OnHandleDataDirty(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bool isDirty = static_cast<bool>(lParam);
	int activePage = tabView.GetActivePage();
	if (isDirty) {
		if (tabView.GetPageImage(activePage) != 6) { // 6 - table data dirty
			tabView.SetPageImage(activePage, 6);
		}		
	} else {
		if (tabView.GetPageImage(activePage) != 5) {
			tabView.SetPageImage(activePage, 5); // 5 - table data
		}
	}
	
	return 0;
}

LRESULT RightWorkView::OnHandleTableStructureDirty(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bool isDirty = static_cast<bool>(lParam);
	int activePage = tabView.GetActivePage();

	if (isDirty) {
		if (tabView.GetPageImage(activePage) != 7) { // 7 - table structure dirty
			tabView.SetPageImage(activePage, 7);
		}		
	} else {
		HWND activePageHwnd = tabView.GetPageHWND(activePage);
		auto iter = std::find_if(tablePagePtrs.begin(), tablePagePtrs.end(), [&activePageHwnd](const TableStructurePage * ptr) {
			return ptr->m_hWnd == activePageHwnd;
		});
		if (iter == tablePagePtrs.end()) {
			return 0;
		}
		TableStructurePage * ptr = *iter;
		if (!ptr->getSupplier()->compareDatas()) {
			if (tabView.GetPageImage(activePage) != 7) { // 7 - table structure dirty
				tabView.SetPageImage(activePage, 7);
			}
			return 0;
		}

		if (tabView.GetPageImage(activePage) != 2) { // 2- table structure
			tabView.SetPageImage(activePage, 2);
		}
	}
	return 0;
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
	int n = tabView.GetPageCount();
	if (!n) {
		UpdateWindow();
	}
	return 0;
}

LRESULT RightWorkView::doRefreshTableDataForSameDbTablePage(uint64_t userDbId, const std::wstring & theTblName)
{
	int n = tabView.GetPageCount();
	userDbId = userDbId ? userDbId : databaseSupplier->getSelectedUserDbId();
	std::wstring tblName = !theTblName.empty() ? theTblName : databaseSupplier->selectedTable;
	for (int i = 0; i < n; i++) {
		HWND pageHwnd = tabView.GetPageHWND(i);
		std::wstring pageTitle = tabView.GetPageTitle(i);
		StringUtil::trim(pageTitle);
		// same name
		if (pageTitle != tblName) {
			continue;
		}
		// query page has properties of TABLE_DATA and  same user db id
		auto iter = std::find_if(queryPagePtrs.begin(), queryPagePtrs.end(), [&pageHwnd, &userDbId](QueryPage * page) {
			if (page && page->IsWindow()
				&& page->m_hWnd == pageHwnd
				&& page->getSupplier()->getOperateType() == TABLE_DATA
				&& page->getSupplier()->getRuntimeUserDbId() == userDbId) {
				return true;
			}
			return false;
		});

		// reload table data
		if (iter != queryPagePtrs.end()) {
			(*iter)->getResultTabView().loadTableDatas(tblName);
		}
	}
	return true;
}