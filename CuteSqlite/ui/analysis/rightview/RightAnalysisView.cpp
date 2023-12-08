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

 * @file   RightAnalysisView.cpp
 * @Class Tree  RightAnalysisView
 *                 |->QTabView(tabView)
 *                         |-> SqlLogPage
 *                               |-> SqlLogListBox
 *                                        |-> SqlLogListItem
 * 
 * 
 * @author Xuehan Qin
 * @date   2023-12-01
 *********************************************************************/
#include "stdafx.h"
#include "RightAnalysisView.h"
#include "utils/ResourceUtil.h"
#include "common/AppContext.h"
#include "core/common/Lang.h"
#include "utils/SqlUtil.h"
#include <ui/common/message/QPopAnimate.h>
#include <ui/common/QWinCreater.h>

#define RIGHT_ANALYSIS_VIEW_TOPBAR_HEIGHT 30
#define RIGHT_ANALYSIS_VIEW_BUTTON_WIDTH 16


BOOL RightAnalysisView::PreTranslateMessage(MSG* pMsg)
{
	return FALSE;
}

void RightAnalysisView::createImageList()
{
	if (!imageList.IsNull()) {
		return;
	}
	std::wstring imgDir = ResourceUtil::getProductImagesDir();
	HINSTANCE ins = ModuleHelper::GetModuleInstance();

	firstIcon = (HICON)::LoadImageW(ins, (imgDir + L"analysis\\tab\\first.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	sqlLogIcon = (HICON)::LoadImageW(ins, (imgDir + L"analysis\\tree\\sql-log.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	perfReportIcon = (HICON)::LoadImageW(ins, (imgDir + L"analysis\\tree\\analysis-report.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);

	imageList.Create(16, 16, ILC_COLOR32, 8, 8);
	imageList.AddIcon(firstIcon); // 0 - first
	imageList.AddIcon(sqlLogIcon); // 1 - sql log
	imageList.AddIcon(perfReportIcon); // 2 - perf report
}

CRect RightAnalysisView::getTopRect(CRect & clientRect)
{
	return { 0, 0, clientRect.right, RIGHT_ANALYSIS_VIEW_TOPBAR_HEIGHT };
}

CRect RightAnalysisView::getTabRect(CRect & clientRect)
{
	return { 0, RIGHT_ANALYSIS_VIEW_TOPBAR_HEIGHT, clientRect.right, clientRect.bottom };
}

CRect RightAnalysisView::getTabRect()
{
	CRect clientRect;
	GetClientRect(clientRect);
	return getTabRect(clientRect);
}


void RightAnalysisView::createOrShowUI()
{
	CRect clientRect;
	GetClientRect(clientRect);
	CRect tabRect = getTabRect(clientRect);

	createOrShowTabView(tabView, clientRect);	
	createOrShowFirstPage(firstPage, clientRect, !isInitedPages);
	
	isInitedPages = true;
}


void RightAnalysisView::createOrShowTabView(QTabView &win, CRect & clientRect)
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


void RightAnalysisView::createOrShowFirstPage(FirstPage &win, CRect & clientRect, bool isAllowCreate /*= true*/)
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

void RightAnalysisView::createOrShowSqlLogPage(SqlLogPage &win, CRect & clientRect, bool isAllowCreate /*= true*/)
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


void RightAnalysisView::createOrShowPerfAnalysisPage(PerfAnalysisPage &win, CRect & clientRect, bool isAllowCreate /*= true*/)
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

void RightAnalysisView::loadWindow()
{
	if (!isNeedReload) {
		return ;
	}

	isNeedReload = false;
	loadTabViewPages();
}

void RightAnalysisView::loadTabViewPages()
{
 	tabView.AddPage(firstPage.m_hWnd, StringUtil::blkToTail(S(L"home")).c_str(), 0, firstPage.m_hWnd);
	tabView.SetActivePage(0);
// 	databaseSupplier->activeTabPageHwnd = firstQueryPage->m_hWnd;
}

int RightAnalysisView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_SHOW_SQL_LOG_PAGE_ID);

	bkgBrush.CreateSolidBrush(bkgColor);
	topbarBrush.CreateSolidBrush(topbarColor);
	createImageList();
	return 0;
}

void RightAnalysisView::OnDestroy()
{
	AppContext::getInstance()->unsuscribe(m_hWnd, Config::MSG_SHOW_SQL_LOG_PAGE_ID);

	if (!bkgBrush.IsNull()) bkgBrush.DeleteObject();
	if (!topbarBrush.IsNull()) topbarBrush.DeleteObject();
	if (!firstIcon) ::DeleteObject(firstIcon);
	if (!sqlLogIcon) ::DeleteObject(sqlLogIcon);
	if (!perfReportIcon) ::DeleteObject(perfReportIcon);
	if (!imageList.IsNull()) imageList.Destroy();

	if (firstPage.IsWindow()) firstPage.DestroyWindow();
	if (sqlLogPage.IsWindow()) sqlLogPage.DestroyWindow();
	clearPerfAnalysisPagePtrs();

	if (tabView.IsWindow()) tabView.DestroyWindow();	
}

void RightAnalysisView::OnSize(UINT nType, CSize size)
{
	createOrShowUI();
}

void RightAnalysisView::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if (!bShow) {
		return ;
	}
	loadWindow();
}

void RightAnalysisView::OnPaint(CDCHandle dc)
{
	CPaintDC pdc(m_hWnd);
	CMemoryDC mdc(pdc, pdc.m_ps.rcPaint);
	CRect clientRect;
	GetClientRect(clientRect);

	CRect topRect = getTopRect(clientRect);
	mdc.FillRect(topRect, topbarBrush.m_hBrush);

	CRect tabRect = getTabRect(clientRect);
	mdc.FillRect(tabRect, bkgBrush.m_hBrush);
}

LRESULT RightAnalysisView::OnTabViewCloseBtn(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
	int nPage = static_cast<int>(pnmh->idFrom);
	return closeTabViewPage(nPage); 
}

LRESULT RightAnalysisView::closeTabViewPage(int nPage)
{
	HWND pageHwnd = tabView.GetPageHWND(nPage);
	if (pageHwnd == firstPage.m_hWnd) {
		return 1;  // 1 - cancel close
	}

	if (pageHwnd == sqlLogPage.m_hWnd) {
		if (sqlLogPage.IsWindow()) {
			sqlLogPage.DestroyWindow();
		}
		return 0;
	}

	return 0;  // 0 - force close
}


void RightAnalysisView::clearPerfAnalysisPagePtrs()
{
	for (auto ptr : perfAnalysisPagePtrs) {
		if (ptr && ptr->IsWindow()) {
			ptr->DestroyWindow();
		}
		if (ptr) {
			//delete ptr;
			//ptr = nullptr;
		}
	}
	perfAnalysisPagePtrs.clear();
}

BOOL RightAnalysisView::OnEraseBkgnd(CDCHandle dc)
{
	return TRUE;
}

HBRUSH RightAnalysisView::OnCtlColorStatic(HDC hdc, HWND hwnd)
{
	return bkgBrush.m_hBrush;
}

LRESULT RightAnalysisView::OnHandleAnalysisSql(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	uint64_t userDbId = static_cast<uint64_t>(wParam);
	uint64_t sqlLogId = static_cast<uint64_t>(lParam);
	if (!sqlLogId) {
		Q_ERROR(L"analysis failed, sqlLogId is empty");
		QPopAnimate::error(S(L"sql-log-id-empty"));
	}
	SqlLog sqlLog = sqlLogService->getSqlLog(sqlLogId);
	if (sqlLog.sql.empty()) {
		Q_ERROR(L"analysis failed, sql log not found in database, sqlLogId:{}", sqlLogId);
		QPopAnimate::error(S(L"sql-log-not-found"));
		return 0;
	}

	if (!SqlUtil::isSelectSql(sqlLog.sql)) {
		QPopAnimate::error(S(L"not-select-sql"));
		return 0;
	}
	CRect clientRect;
	GetClientRect(clientRect);
	CRect tabRect = getTabRect(clientRect);
	
	for (auto ptr : perfAnalysisPagePtrs) {
		if (ptr->getSqlLogId() == sqlLogId) {
			int n = tabView.GetPageCount();
			for (int i = 0; i < n; i++) {
				if (tabView.GetPageHWND(i) != ptr->m_hWnd) {
					continue;
				}
				tabView.SetActivePage(i);
				return 0;
			}
		}
	}
	
	PerfAnalysisPage * newPage = new PerfAnalysisPage(sqlLogId);
	createOrShowPerfAnalysisPage(*newPage, clientRect, true);
	perfAnalysisPagePtrs.push_back(newPage);
	std::wstring title = L"perf-report-";
	title.append(std::to_wstring(sqlLogId));
	tabView.AddPage(newPage->m_hWnd, title.c_str(), 2, newPage->m_hWnd);
	
	return 0;
}

LRESULT RightAnalysisView::OnHandleShowSqlLogPage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CRect clientRect;
	GetClientRect(clientRect);
	CRect tabRect = getTabRect(clientRect);

	createOrShowSqlLogPage(sqlLogPage, clientRect, true);
	int n = tabView.GetPageCount();
	bool found = false;
	for (int i = 0; i < n; i++) {
		HWND pageHwnd = tabView.GetPageHWND(i);
		if (pageHwnd == sqlLogPage.m_hWnd) {
			tabView.SetActivePage(i);
			found = true;
			break;
		}
	}
	if (!found) {
		tabView.AddPage(sqlLogPage.m_hWnd, S(L"sql-log").c_str(), 1, sqlLogPage.m_hWnd);
	}
	return 0;
}


