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

#define RESULT_TOPBAR_HEIGHT 30
#define RESULT_BUTTON_WIDTH 16


BOOL ResultTabView::PreTranslateMessage(MSG* pMsg)
{
	if (resultTableDataPage.IsWindow() && resultTableDataPage.PreTranslateMessage(pMsg)) {
		return TRUE;
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
	HWND activeHwnd = tabView.GetPageHWND(tabView.GetActivePage());
	if (!activeHwnd) {
		return false;
	}
	if (activeHwnd == resultTableDataPage.m_hWnd) {
		return true;
	}
	return false;
}

void ResultTabView::loadTableDatas(std::wstring & table)
{
	resultInfoPage.clear();
	resultTableDataPage.setup(table);
	resultTableDataPage.loadTableDatas();
}

void ResultTabView::clearResultListPage()
{
	// destry window and delete ptr from resultListPage vector
	for (auto resultListPagePtr : resultListPagePtrs) {
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
		}
	}
	resultListPagePtrs.clear();
}

void ResultTabView::clearMessage()
{
	resultInfoPage.clear();
}

void ResultTabView::addResultListPage(std::wstring & sql, int tabNo)
{
	CRect clientRect;
	GetClientRect(clientRect);
	CRect pageRect = getPageRect(clientRect);
	int x = 1, y = pageRect.top, w = pageRect.Width() - 2, h = pageRect.Height();
	CRect rect(x, y, x + w, y + h);

	ResultListPage * resultListPagePtr = new ResultListPage();
	resultListPagePtr->setup(sql);
	resultListPagePtr->Create(tabView.m_hWnd, rect, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0);
	std::wstring pageTitle = S(L"result-list").append(L" ").append(std::to_wstring(tabNo));
	int nInsert = static_cast<int>(resultListPagePtrs.size());
	tabView.InsertPage(nInsert, resultListPagePtr->m_hWnd, pageTitle.c_str(), 0, resultListPagePtr);
	resultListPagePtrs.push_back(resultListPagePtr);
	resultListPagePtr->MoveWindow(rect);
	resultListPagePtr->ShowWindow(SW_SHOW);
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

void ResultTabView::createImageList()
{
	if (!imageList.IsNull()) {
		return;
	}
	std::wstring imgDir = ResourceUtil::getProductImagesDir();
	HINSTANCE ins = ModuleHelper::GetModuleInstance();
	
	resultBitmap = (HBITMAP)::LoadImageW(ins, (imgDir + L"database\\tab\\result.bmp").c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	infoBitmap = (HBITMAP)::LoadImageW(ins, (imgDir + L"database\\tab\\info.bmp").c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	tableDataBitmap = (HBITMAP)::LoadImageW(ins, (imgDir + L"database\\tab\\table-data.bmp").c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	objectBitmap = (HBITMAP)::LoadImageW(ins, (imgDir + L"database\\tab\\object.bmp").c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	imageList.Create(16, 16, ILC_COLOR32, 0, 4);
	imageList.Add(resultBitmap); // 0 - result
	imageList.Add(infoBitmap); // 1 - info
	imageList.Add(tableDataBitmap); // 2 - tableData
	imageList.Add(objectBitmap);// 3 - object
}

CRect ResultTabView::getTabRect(CRect & clientRect)
{
	return { 0, 0, clientRect.right, tabView.m_cyTabHeight };
}

CRect ResultTabView::getPageRect(CRect & clientRect)
{
	return { 0, tabView.m_cyTabHeight, clientRect.right, clientRect.bottom };
}

void ResultTabView::createOrShowUI()
{
	CRect clientRect;
	GetClientRect(clientRect);
	createOrShowTabView(tabView, clientRect);
	createOrShowResultInfoPage(resultInfoPage, clientRect);
	createOrShowResultTableDataPage(resultTableDataPage, clientRect);
}


void ResultTabView::createOrShowTabView(QTabView &win, CRect & clientRect)
{
	CRect rect = clientRect;
	if (IsWindow() && !win.IsWindow()) {
		win.Create(m_hWnd, rect, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, Config::DATABASE_WORK_TAB_VIEW_ID);
		win.SetImageList(imageList);
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
		win.Create(tabView.m_hWnd, rect, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0);
	}
	else if (IsWindow() && tabView.IsWindow()) {
		
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
		win.Create(tabView.m_hWnd, rect, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0);
	}
	else if (IsWindow() && tabView.IsWindow()) {
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
	tabView.AddPage(resultInfoPage.m_hWnd, S(L"result-info").c_str(), 1, &resultInfoPage);
	tabView.AddPage(resultTableDataPage.m_hWnd, S(L"result-table-data").c_str(), 2, &resultTableDataPage);
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

	if (tabView.IsWindow()) tabView.DestroyWindow();
	if (resultInfoPage.IsWindow()) resultInfoPage.DestroyWindow();
	if (resultTableDataPage.IsWindow()) resultTableDataPage.DestroyWindow();

	if (resultBitmap) ::DeleteObject(resultBitmap);
	if (infoBitmap) ::DeleteObject(infoBitmap);
	if (tableDataBitmap) ::DeleteObject(tableDataBitmap);
	if (objectBitmap) ::DeleteObject(objectBitmap);

	clearResultListPage();
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
	return true;
}
