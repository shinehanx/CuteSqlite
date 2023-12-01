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
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-12-01
 *********************************************************************/
#include "stdafx.h"
#include "RightAnalysisView.h"
#include "utils/ResourceUtil.h"
#include "common/AppContext.h"
#include "core/common/Lang.h"

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

	sqlLogIcon = (HICON)::LoadImageW(ins, (imgDir + L"analysis\\tree\\sql-log.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);

	imageList.Create(16, 16, ILC_COLOR32, 8, 8);
	imageList.AddIcon(sqlLogIcon); // 0 - query
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
	createOrShowSqlLogPage(sqlLogPage, clientRect, true);
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
 	tabView.AddPage(sqlLogPage.m_hWnd, StringUtil::blkToTail(S(L"sql-log")).c_str(), 0, &sqlLogPage);
	tabView.SetActivePage(0);
// 	databaseSupplier->activeTabPageHwnd = firstQueryPage->m_hWnd;
}

int RightAnalysisView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	bkgBrush.CreateSolidBrush(bkgColor);
	topbarBrush.CreateSolidBrush(topbarColor);

	createImageList();
	return 0;
}

void RightAnalysisView::OnDestroy()
{
	if (!bkgBrush.IsNull()) bkgBrush.DeleteObject();
	if (!topbarBrush.IsNull()) topbarBrush.DeleteObject();
	if (!textFont) ::DeleteObject(textFont);
	if (!sqlLogIcon) ::DeleteObject(sqlLogIcon);

	if (!imageList.IsNull()) imageList.Destroy();
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

	CRect workRect = getTabRect(clientRect);
	mdc.FillRect(workRect, bkgBrush.m_hBrush);
}

BOOL RightAnalysisView::OnEraseBkgnd(CDCHandle dc)
{
	return TRUE;
}

HBRUSH RightAnalysisView::OnCtlColorStatic(HDC hdc, HWND hwnd)
{
	return bkgBrush.m_hBrush;
}
