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

 * @file   AnalysisPanel.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-12-01
 *********************************************************************/
#include "stdafx.h"
#include "AnalysisPanel.h"

BOOL AnalysisPanel::PreTranslateMessage(MSG* pMsg)
{
	if (leftView.IsWindow() && leftView.PreTranslateMessage(pMsg)) {
		return TRUE;
	}

	if (rightView.IsWindow() && rightView.PreTranslateMessage(pMsg)) {
		return TRUE;
	}
	return FALSE;
}

void AnalysisPanel::createOrShowUI()
{
	CRect clientRect;
	GetClientRect(clientRect);

	createOrShowSplitter(splitter, clientRect);
	createOrShowLeftView(leftView, clientRect);
	createOrShowRightView(rightView, clientRect);

	if (isNeededReload) {
		isNeededReload = false;
		splitter.SetSplitterPane(0, leftView, false);
		splitter.SetSplitterPane(1, rightView, false);
	}
}

void AnalysisPanel::createOrShowSplitter(CSplitterWindow & win, CRect & clientRect)
{
	CRect & rect = clientRect;
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		win.Create(m_hWnd, rect, L"", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
		win.m_cxySplitBar = 2;
		win.m_cxyMin = 250;
		//win.SetSplitterExtendedStyle(SPLIT_NONINTERACTIVE);
		win.m_bFullDrag = false;
		return;
	} else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		// show first then move window
		win.ShowWindow(SW_SHOW);
		win.MoveWindow(&rect);
		if (win.GetSplitterPos() == 4)
			win.SetSplitterPos(250);
	}
}

void AnalysisPanel::createOrShowLeftView(LeftNavigationView & win, CRect & clientRect)
{
	CRect rect(0, 0, 1, 1);
	if (::IsWindow(splitter.m_hWnd) && !win.IsWindow()) {
		win.Create(splitter.m_hWnd, rect, L"", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
		return;
	}
}

void AnalysisPanel::createOrShowRightView(RightAnalysisView & win, CRect & clientRect)
{
	CRect rect(0, 0, 1, 1);
	if (::IsWindow(splitter.m_hWnd) && !win.IsWindow()) {
		win.Create(splitter.m_hWnd, rect, L"", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
		return;
	}
}

int AnalysisPanel::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	bkgBrush.CreateSolidBrush(bkgColor);


	return 0;
}

void AnalysisPanel::OnDestroy()
{
	if (!bkgBrush.IsNull()) bkgBrush.DeleteObject();

	if (splitter.IsWindow()) splitter.DestroyWindow();
	if (leftView.IsWindow()) leftView.DestroyWindow();
}

void AnalysisPanel::OnSize(UINT nType, CSize size)
{
	createOrShowUI();
}

void AnalysisPanel::OnShowWindow(BOOL bShow, UINT nStatus)
{

}

void AnalysisPanel::OnPaint(CDCHandle dc)
{
	CPaintDC pdc(m_hWnd);
	CMemoryDC mdc(pdc, pdc.m_ps.rcPaint);
	mdc.FillRect(&pdc.m_ps.rcPaint, bkgBrush.m_hBrush);
}

BOOL AnalysisPanel::OnEraseBkgnd(CDCHandle dc)
{
	return true;
}

