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
 *                    |-> QueryPage
 *                    |      |-> CHorSplitterWindow
 *                    |            |-> QHelpEdit -> QSqlEdit(Scintilla)
 *                    |            |-> ResultTabView
 *                    |                    |-> QTabView
 *                    |                          |-> ResultListPage
 *                    |                          |-> ResultInfoPage
 *                    |                          |-> ResultTableDataPage
 *                    |-> NewTablePage
 * @author Xuehan Qin
 * @date   2023-05-21
 *********************************************************************/
#include "stdafx.h"
#include "RightWorkView.h"
#include "common/AppContext.h"
#include "core/common/Lang.h"
#include "ui/common/QWinCreater.h"


#define RIGHTVIEW_TOPBAR_HEIGHT 30
#define RIGHTVIEW_BUTTON_WIDTH 16

BOOL RightWorkView::PreTranslateMessage(MSG* pMsg)
{
	BOOL result = FALSE;
	if (queryPage.IsWindow() && queryPage.PreTranslateMessage(pMsg)) {
		return TRUE;
	}
	for (auto pagePtr : newTablePagePtrs) {
		if (pagePtr->IsWindow() && pagePtr->PreTranslateMessage(pMsg)) {
			return TRUE;
		}
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
	queryBitmap = (HBITMAP)::LoadImageW(ins, (imgDir + L"database\\tab\\query.bmp").c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	historyBitmap = (HBITMAP)::LoadImageW(ins, (imgDir + L"database\\tab\\history.bmp").c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);	
	tableBitmap = (HBITMAP)::LoadImageW(ins, (imgDir + L"database\\tab\\table.bmp").c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);	

	imageList.Create(16, 16, ILC_COLOR32, 0, 4);
	imageList.Add(queryBitmap); // 0 - query
	imageList.Add(historyBitmap); // 1 - history
	imageList.Add(tableBitmap); // 2 - table
	
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
	createOrShowQueryPage(queryPage, clientRect);
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

void RightWorkView::createOrShowHistoryPage(CEdit &win, CRect & clientRect)
{
	CRect tabRect = getTabRect(clientRect);
	int x = 1, y = tabView.m_cyTabHeight + 1, w = tabRect.Width() - 2, h = tabRect.Height() - tabView.m_cyTabHeight - 2;
	CRect rect(x, y, x + w, y + h);
	if (IsWindow() && !win.IsWindow()) {
		win.Create(tabView.m_hWnd, rect, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | ES_READONLY, 0);		
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

void RightWorkView::createOrShowNewTablePage(NewTablePage &win, CRect & clientRect)
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
	tabView.AddPage(queryPage.m_hWnd, S(L"query-editor").c_str(), 0, &queryPage);
	tabView.AddPage(historyPage.m_hWnd, S(L"history-log").c_str(), 1, &historyPage);

	supplier->mainTabPages.push_back({ DatabaseSupplier::QUERY_PAGE, queryPage.m_hWnd });
	supplier->mainTabPages.push_back({ DatabaseSupplier::HISTORY_PAGE, historyPage.m_hWnd });
	tabView.SetActivePage(0);
}

int RightWorkView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_NEW_TABLE_ID);
	createImageList();

	topbarBrush = ::CreateSolidBrush(topbarColor);
	bkgBrush = ::CreateSolidBrush(bkgColor);
	return 0;
}

int RightWorkView::OnDestroy()
{
	AppContext::getInstance()->unsuscribe(m_hWnd, Config::MSG_NEW_TABLE_ID);

	if (topbarBrush) ::DeleteObject(topbarBrush);
	if (bkgBrush) ::DeleteObject(bkgBrush);

	if (execSqlButton.IsWindow()) execSqlButton.DestroyWindow();
	if (execAllButton.IsWindow()) execAllButton.DestroyWindow();

	if (tabView.IsWindow()) tabView.DestroyWindow();
	if (historyPage.IsWindow()) historyPage.DestroyWindow();
	if (queryPage.IsWindow()) queryPage.DestroyWindow();
	
	if (queryBitmap) ::DeleteObject(queryBitmap);
	if (historyBitmap) ::DeleteObject(historyBitmap);
	if (tableBitmap) ::DeleteObject(tableBitmap);
	if (!imageList.IsNull()) imageList.Destroy();

	// destroy the pagePtr and release the memory from pagePtrs vector
	for (QPage * pagePtr : newTablePagePtrs) {
		if (pagePtr && pagePtr->IsWindow()) {
			pagePtr->DestroyWindow();
			delete pagePtr;
			pagePtr = nullptr;
		}
	}
	newTablePagePtrs.clear();
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
	mdc.FillRect(topRect, topbarBrush);

	CRect workRect = getTabRect(clientRect);
	mdc.FillRect(workRect, bkgBrush);
}

BOOL RightWorkView::OnEraseBkgnd(CDCHandle dc)
{
	return TRUE;
}

LRESULT RightWorkView::OnClickExecSqlButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	if (tabView.GetPageHWND(tabView.GetActivePage()) == queryPage.m_hWnd) {
		queryPage.execAndShow();
	}
	// execute sql statements from supplier->sqlVector
	

	return 0;
}

LRESULT RightWorkView::OnClickExecAllButton(UINT uNotifyCode, int nID, HWND hwnd)
{
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
	NewTablePage * newTablePage = new NewTablePage();
	createOrShowNewTablePage(*newTablePage, clientRect);
	newTablePagePtrs.push_back(newTablePage);

	// nImage = 2 : table 
	tabView.AddPage(newTablePage->m_hWnd, S(L"new-table").c_str(), 2, newTablePage);

	supplier->mainTabPages.push_back({ DatabaseSupplier::NEW_TABLE_PAGE, newTablePage->m_hWnd });
}
