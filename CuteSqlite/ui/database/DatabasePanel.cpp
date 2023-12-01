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

 * @file   DatabasePanel.cpp
 * @brief  Database Panel for HomeView
 * 
 * @author Xuehan Qin
 * @date   2023-05-19
 *********************************************************************/
#include "stdafx.h"
#include "DatabasePanel.h"
#include "utils/GdiUtil.h"

BOOL DatabasePanel::PreTranslateMessage(MSG* pMsg)
{
	if (leftTreeView.IsWindow() && leftTreeView.PreTranslateMessage(pMsg)) {
		return TRUE;
	}

	if (rightWorkView.IsWindow() && rightWorkView.PreTranslateMessage(pMsg)) {
		return TRUE;
	}
	return FALSE;
}

void DatabasePanel::createOrShowUI()
{
	CRect clientRect;
	GetClientRect(clientRect);

	createOrShowSplitter(splitter, clientRect);
	createOrShowLeftTreeView(leftTreeView, clientRect);
	createOrShowRightWorkView(rightWorkView, clientRect);

	if (isNeededReload) {
		isNeededReload = false;
		splitter.SetSplitterPane(0, leftTreeView, false);
		splitter.SetSplitterPane(1, rightWorkView, false);
	}
}


void DatabasePanel::createOrShowSplitter(CSplitterWindow & win, CRect & clientRect)
{
	CRect & rect = clientRect;
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		win.Create(m_hWnd, rect, L"", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
		win.m_cxySplitBar = 2;
		win.m_cxyMin = 250;
		//win.SetSplitterExtendedStyle(SPLIT_NONINTERACTIVE);
		win.m_bFullDrag = false;
		return;
	}
	else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		// show first then move window
		win.ShowWindow(SW_SHOW);
		win.MoveWindow(&rect);
		if (win.GetSplitterPos() == 4)
			win.SetSplitterPos(250);
	}
}


void DatabasePanel::createOrShowLeftTreeView(LeftTreeView & win, CRect & clientRect)
{
	CRect rect(0, 0, 1, 1);
	if (::IsWindow(splitter.m_hWnd) && !win.IsWindow()) {
		win.Create(splitter.m_hWnd, rect, L"", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
		return;
	}
}

void DatabasePanel::createOrShowRightWorkView(RightWorkView & win, CRect & clientRect)
{
	CRect rect(0, 0, 1, 1);
	if (::IsWindow(splitter.m_hWnd) && !win.IsWindow()) {
		win.Create(splitter.m_hWnd, rect, L"", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
		return;
	}
}

LRESULT DatabasePanel::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	
	return 0;
}

LRESULT DatabasePanel::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (splitter.IsWindow()) splitter.DestroyWindow();
	if (leftTreeView.IsWindow()) leftTreeView.DestroyWindow();
	if (rightWorkView.IsWindow()) rightWorkView.DestroyWindow();
	return 0;
}

LRESULT DatabasePanel::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	createOrShowUI();
	return 0;
}

LRESULT DatabasePanel::OnShowWindow(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (!wParam) {
		return 0;
	}
	return 0;
}

LRESULT DatabasePanel::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC pdc(m_hWnd);
	CMemoryDC mdc(pdc, pdc.m_ps.rcPaint);
	
	//HBRUSH brush = AtlGetStockBrush(GRAY_BRUSH);
	HBRUSH brush = ::CreateSolidBrush(RGB(0x2c, 0x2c, 0x2c));
	mdc.FillRect(&pdc.m_ps.rcPaint, brush);
	::DeleteObject(brush);

	return 0;
}

BOOL DatabasePanel::OnEraseBkgnd(CDCHandle dc)
{
	return true;
}
