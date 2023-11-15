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

 * @file   ResultTabView.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-05-22
 *********************************************************************/
#include "stdafx.h"
#include "ResultTabView.h"
#include "common/AppContext.h"
#include "core/common/Lang.h"
#include "utils/ResourceUtil.h"
#include "core/common/exception/QSqlExecuteException.h"
#include "ui/common/message/QPopAnimate.h"
#include "utils/PerformUtil.h"

#define RESULT_TOPBAR_HEIGHT 30
#define RESULT_BUTTON_WIDTH 16

void ResultTabView::setup(QueryPageSupplier * supplier)
{
	this->supplier = supplier;
}

BOOL ResultTabView::PreTranslateMessage(MSG* pMsg)
{
	if (resultTableDataPage.IsWindow() && resultTableDataPage.PreTranslateMessage(pMsg)) {
		return TRUE;
	}

	for (auto & ptr : resultListPagePtrs) {
		if (ptr->IsWindow() && ptr->PreTranslateMessage(pMsg)) {
			return true;
		}
	}
	return false;
}

HWND ResultTabView::getActiveResultListPageHwnd()
{
	if (resultListPagePtrs.empty()) {
		return nullptr;
	}
	HWND activeHwnd = tabView.GetPageHWND(tabView.GetActivePage());
	if (!activeHwnd) {
		return nullptr;
	}
	for (auto resultListPagePtr : resultListPagePtrs) {
		if (activeHwnd == resultListPagePtr->m_hWnd) {
			return resultListPagePtr->m_hWnd;
		}
	}
	return nullptr;
}

bool ResultTabView::isActiveTableDataPage()
{
	int nPage = tabView.GetActivePage();
	if (nPage < 0) {
		return false;
	}
	HWND activeHwnd = tabView.GetPageHWND(nPage);
	if (!activeHwnd) {
		return false;
	}
	if (activeHwnd == resultTableDataPage.m_hWnd) {
		return true;
	}
	return false;
}

void ResultTabView::activeTableDataPage()
{
	if (!resultTableDataPage.IsWindow()) {
		CRect clientRect;
		GetClientRect(clientRect);
		createOrShowResultTableDataPage(resultTableDataPage, clientRect);
		int nPage = tabView.AddPage(resultTableDataPage.m_hWnd, StringUtil::blkToTail(S(L"result-table-data")).c_str(), 2, &resultTableDataPage);
		tabView.SetActivePage(nPage);
		return ;
	} 
	
	int n = tabView.GetPageCount();
	for (int i = 0; i < n; i++) {
		if (tabView.GetPageHWND(i) != resultTableDataPage.m_hWnd) {
			continue;
		}
		tabView.SetActivePage(i);
	}
}

void ResultTabView::loadTableDatas(std::wstring & table)
{
	if (resultInfoPage.IsWindow()) {
		resultInfoPage.clear();
	}
	supplier->setRuntimeTblName(table);
	resultTableDataPage.setup(table);
	resultTableDataPage.loadTableDatas();
}

void ResultTabView::clearResultListPage()
{
	// destroy window and delete ptr from resultListPage vector
	for (auto & resultListPagePtr : resultListPagePtrs) {
		if (resultListPagePtr && resultListPagePtr->IsWindow()) {
			int idx = getPageIndex(resultListPagePtr->m_hWnd);
			if (idx != -1) {
				tabView.RemovePage(idx);
			}
			if (resultListPagePtr->IsWindow()) {
				resultListPagePtr->DestroyWindow();
			}
			delete resultListPagePtr;
			resultListPagePtr = nullptr;
		} else if (resultListPagePtr) {
			delete resultListPagePtr;
			resultListPagePtr = nullptr;
		}
	}
	resultListPagePtrs.clear();
}

void ResultTabView::clearMessage()
{
	resultInfoPage.clear();
}

ResultListPage * ResultTabView::addResultToListPage(std::wstring & sql, int tabNo)
{
	CRect clientRect;
	GetClientRect(clientRect);
	CRect pageRect = getPageRect(clientRect);
	int x = 1, y = pageRect.top, w = pageRect.Width() - 2, h = pageRect.Height();
	CRect rect(x, y, x + w, y + h);

	ResultListPage * resultListPagePtr = nullptr;
	int n = static_cast<int>(resultListPagePtrs.size());
	if (tabNo-1 < n) {
		resultListPagePtr = resultListPagePtrs.at(tabNo -1);
		resultListPagePtr->setup(supplier, sql);
		resultListPagePtr->loadListView();
	} else {
		resultListPagePtr = new ResultListPage();
		resultListPagePtr->setup(supplier, sql);
		resultListPagePtr->Create(tabView.m_hWnd, rect, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0);
		std::wstring pageTitle = S(L"result-list").append(L" ").append(std::to_wstring(tabNo));
		int nInsert = static_cast<int>(resultListPagePtrs.size());
		tabView.InsertPage(nInsert, resultListPagePtr->m_hWnd, pageTitle.c_str(), 0, resultListPagePtr);
		resultListPagePtrs.push_back(resultListPagePtr);
	}
	
	resultListPagePtr->MoveWindow(rect);
	resultListPagePtr->ShowWindow(SW_SHOW);
	return resultListPagePtr;
}


void ResultTabView::removeResultListPageFrom(int nSelectSqlCount)
{
	int n = static_cast<int>(resultListPagePtrs.size());
	if (n <= nSelectSqlCount) {
		return;
	}
	for (int i = n - 1; i >= nSelectSqlCount ; i--) {
		auto ptr = resultListPagePtrs.at(i);
		if (ptr && ptr->IsWindow()) {
			for (int j = tabView.GetPageCount() -1; j >=0; j--) {
				if (ptr->m_hWnd == tabView.GetPageHWND(j)) {
					tabView.RemovePage(j); //  will destroy page window
					break;
				}
			}
			// Notice : tabView.RemovePage(j) has DestroyWindow
			if (ptr->IsWindow()) {
				ptr->DestroyWindow();
			}
			delete ptr;
			ptr = nullptr;
		} else if (ptr) {
			delete ptr;
			ptr = nullptr;
		}

		resultListPagePtrs.erase(resultListPagePtrs.begin() + i);
	}
}

/**
 * Exec sql and show message to MessagePage.
 * 
 * @param sql
 */
bool ResultTabView::execSqlToInfoPage(const std::wstring & sql)
{
	ATLASSERT(!sql.empty());
	resetRuntimeResultInfo();

	runtimeResultInfo.userDbId = databaseSupplier->getSelectedUserDbId();
	auto bt = PerformUtil::begin();
	try {		
		runtimeResultInfo.effectRows = sqlService->executeSql(databaseSupplier->getSelectedUserDbId(), sql);
		runtimeResultInfo.sql = sql;
		runtimeResultInfo.execTime = PerformUtil::end(bt);
		runtimeResultInfo.transferTime = PerformUtil::end(bt);
		runtimeResultInfo.totalTime = PerformUtil::end(bt);
		runtimeResultInfo.msg = S(L"execute-sql-success");
		AppContext::getInstance()->dispatch(Config::MSG_EXEC_SQL_RESULT_MESSAGE_ID, NULL, (LPARAM)&runtimeResultInfo);
		return true;
	} catch (QSqlExecuteException & ex) {
		Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
		ex.setRollBack(true);
		QPopAnimate::report(ex);

		runtimeResultInfo.code = std::stoi(ex.getCode());
		runtimeResultInfo.sql = sql;
		runtimeResultInfo.effectRows = 0;
		runtimeResultInfo.execTime = PerformUtil::end(bt);
		runtimeResultInfo.transferTime = PerformUtil::end(bt);
		runtimeResultInfo.totalTime = PerformUtil::end(bt);
		runtimeResultInfo.msg = ex.getMsg();
		AppContext::getInstance()->dispatch(Config::MSG_EXEC_SQL_RESULT_MESSAGE_ID, NULL, (LPARAM)&runtimeResultInfo);
	}
	return false;
}

int ResultTabView::getPageIndex(HWND hwnd)
{
	int n = tabView.GetPageCount();
	for (int i = 0; i < n; i++) {
		if (tabView.GetPageHWND(i) == hwnd) {
			return i;
		}
	}
	return -1;
}

void ResultTabView::setActivePage(int pageIndex)
{
	tabView.SetActivePage(pageIndex);
}

void ResultTabView::activeResultInfoPage()
{
	int n = tabView.GetPageCount();
	for (int i = 0; i < n; i++) {
		if (tabView.GetPageHWND(i) != resultInfoPage.m_hWnd) {
			continue;
		}
		tabView.SetActivePage(i);
	}
}

void ResultTabView::createImageList()
{
	if (!imageList.IsNull()) {
		return;
	}
	std::wstring imgDir = ResourceUtil::getProductImagesDir();
	HINSTANCE ins = ModuleHelper::GetModuleInstance();
	
	resultIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\tab\\result.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	infoIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\tab\\info.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	tableDataIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\tab\\table-data.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	tablePropertiesIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\tab\\table-properties.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);

	imageList.Create(16, 16, ILC_COLOR32, 0, 4); 
	imageList.AddIcon(resultIcon); // 0 - result
	imageList.AddIcon(infoIcon); // 1 - info
	imageList.AddIcon(tableDataIcon); // 2 - tableData
	imageList.AddIcon(tablePropertiesIcon);// 3 - properties 
}

CRect ResultTabView::getTabRect(CRect & clientRect)
{
	return { 0, 0, clientRect.right, tabView.m_cyTabHeight };
}

CRect ResultTabView::getPageRect(CRect & clientRect)
{
	return { 0, tabView.m_cyTabHeight, clientRect.right, clientRect.bottom };
}

void ResultTabView::resetRuntimeResultInfo()
{
	runtimeResultInfo.sql.clear();
	runtimeResultInfo.effectRows = 0; 
	runtimeResultInfo.execTime.clear();
	runtimeResultInfo.totalTime.clear();
	runtimeResultInfo.code = 0;
	runtimeResultInfo.msg.clear();
}

void ResultTabView::createOrShowUI()
{
	CRect clientRect;
	GetClientRect(clientRect);
	createOrShowTabView(tabView, clientRect);
	createOrShowResultInfoPage(resultInfoPage, clientRect);
	createOrShowResultTableDataPage(resultTableDataPage, clientRect);
	if (supplier && supplier->getOperateType() == TABLE_DATA) {
		createOrShowTablePropertiesPage(tablePropertiesPage, clientRect);
	}
}


void ResultTabView::createOrShowTabView(QTabView &win, CRect & clientRect)
{
	CRect rect = clientRect;
	if (IsWindow() && !win.IsWindow()) {
		win.Create(m_hWnd, rect, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, Config::DATABASE_WORK_TAB_VIEW_ID);
		win.SetImageList(imageList);
		win.enableCloseBtn(false);
	}
	else if (IsWindow() && tabView.IsWindow()) {
		win.MoveWindow(rect);
		win.ShowWindow(true);
	}
}

void ResultTabView::createOrShowResultInfoPage(ResultInfoPage & win, CRect &clientRect)
{
	CRect pageRect = getPageRect(clientRect);
	int x = 1, y = pageRect.top + 1, w = pageRect.Width() - 2, h = pageRect.Height()  - 2;
	CRect rect(x, y, x + w, y + h);
	if (IsWindow() && !win.IsWindow()) {
		win.setup(supplier);
		win.Create(tabView.m_hWnd, rect, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0);
	} else if (IsWindow() && tabView.IsWindow()) {		
		win.MoveWindow(rect);
		win.ShowWindow(true);
	}
}


void ResultTabView::createOrShowResultTableDataPage(ResultTableDataPage & win, CRect &clientRect)
{
	CRect pageRect = getPageRect(clientRect);
	int x = 1, y = pageRect.top + 1, w = pageRect.Width() - 2, h = pageRect.Height() - 2;
	CRect rect(x, y, x + w, y + h);
	if (IsWindow() && !win.IsWindow()) {
		win.setup(supplier, std::wstring());
		win.Create(tabView.m_hWnd, rect, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0);
	} else if (IsWindow() && tabView.IsWindow()) {
		win.MoveWindow(rect);
		win.ShowWindow(true);
	}
}

void ResultTabView::createOrShowTablePropertiesPage(TablePropertiesPage & win, CRect &clientRect)
{
	CRect pageRect = getPageRect(clientRect);
	int x = 1, y = pageRect.top + 1, w = pageRect.Width() - 2, h = pageRect.Height()  - 2;
	CRect rect(x, y, x + w, y + h);
	if (IsWindow() && !win.IsWindow()) {
		win.setup(supplier);
		win.Create(tabView.m_hWnd, rect, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0);
	} else if (IsWindow() && tabView.IsWindow()) {		
		win.MoveWindow(rect);
		win.ShowWindow(true);
	}
}

void ResultTabView::loadWindow()
{
	if (!isNeedReload) {
		return;
	}

	isNeedReload = false;
	loadTabViewPages();
}


void ResultTabView::loadTabViewPages()
{
	tabView.AddPage(resultInfoPage.m_hWnd, StringUtil::blkToTail(S(L"result-info")).c_str(), 1, &resultInfoPage);
	tabView.AddPage(resultTableDataPage.m_hWnd, StringUtil::blkToTail(S(L"result-table-data")).c_str(), 2, &resultTableDataPage);
	if (supplier->getOperateType() == TABLE_DATA) {
		tabView.AddPage(tablePropertiesPage.m_hWnd, StringUtil::blkToTail(S(L"table-properties")).c_str(), 3, &tablePropertiesPage);
	}
	tabView.SetActivePage(0);
}

int ResultTabView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	bkgBrush = ::CreateSolidBrush(bkgColor);

	createImageList();
	return 0;
}

int ResultTabView::OnDestroy()
{
	if (bkgBrush) ::DeleteObject(bkgBrush);

	
	if (resultInfoPage.IsWindow()) resultInfoPage.DestroyWindow();
	if (resultTableDataPage.IsWindow()) resultTableDataPage.DestroyWindow();
	if (tablePropertiesPage.IsWindow()) tablePropertiesPage.DestroyWindow();

	if (resultIcon) ::DeleteObject(resultIcon);
	if (infoIcon) ::DeleteObject(infoIcon);
	if (tableDataIcon) ::DeleteObject(tableDataIcon);
	if (objectIcon) ::DeleteObject(objectIcon);
	if (tablePropertiesIcon) ::DeleteObject(tablePropertiesIcon);

	clearResultListPage();

	if (tabView.IsWindow()) tabView.DestroyWindow();
	return 0;
}

void ResultTabView::OnSize(UINT nType, CSize size)
{
	createOrShowUI();
}

void ResultTabView::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if (!bShow) {
		return;
	}
	loadWindow();
}

void ResultTabView::OnPaint(CDCHandle dc)
{
	CPaintDC pdc(m_hWnd);
	pdc.FillRect(&(pdc.m_ps.rcPaint), bkgBrush);
}

BOOL ResultTabView::OnEraseBkgnd(CDCHandle dc)
{
	return TRUE;
}

void ResultTabView::activeTablePropertiesPage()
{
	if (!tablePropertiesPage.IsWindow()) {
		CRect clientRect;
		GetClientRect(clientRect);
		createOrShowTablePropertiesPage(tablePropertiesPage, clientRect);
		int nPage = tabView.AddPage(tablePropertiesPage.m_hWnd, StringUtil::blkToTail(S(L"table-properties")).c_str(), 3, &tablePropertiesPage);
		tabView.SetActivePage(nPage);
		return ;
	} 
	
	int n = tabView.GetPageCount();
	for (int i = 0; i < n; i++) {
		if (tabView.GetPageHWND(i) != tablePropertiesPage.m_hWnd) {
			continue;
		}
		tabView.SetActivePage(i);
	}
}


