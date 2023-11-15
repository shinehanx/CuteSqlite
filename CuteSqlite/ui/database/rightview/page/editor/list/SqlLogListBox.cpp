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

 * @file   SqlLogList.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-11-14
 *********************************************************************/
#include "stdafx.h"
#include "SqlLogListBox.h"
#include "ui/common/QWinCreater.h"

#define SQL_LOG_LISTBOX_ITEM_HEIGHT 150
#define SQL_LOG_LISTBOX_GRROUP_HEIGHT 20

int SqlLogListBox::cxChar = 5;
int SqlLogListBox::cyChar = 5;
int SqlLogListBox::iVscrollPos = 5;
int SqlLogListBox::vScrollPages = 5;

SqlLogListBox::SqlLogListBox()
{

}

void SqlLogListBox::addGroup(const std::wstring & group)
{
	if (group.empty()) {
		return;
	}
	CRect clientRect;
	GetClientRect(clientRect);

	CStatic * groupLabel = new CStatic();
	int x = 0, y = 0, w = clientRect.Width(), h = 20;
	CRect rect;
	if (!items.empty()) {
		auto lastItem = items.back();
		CRect rcLast = GdiPlusUtil::GetWindowRelativeRect(lastItem->m_hWnd);
		y = rcLast.bottom + 5;
	} else {
		y = 5;
	}
	rect = { x, y, x + w, y + h };
	createOrShowGroupLabel(*groupLabel, group, rect, clientRect);
	groups.push_back(groupLabel);
}

void SqlLogListBox::addItem(ResultInfo & info)
{
	CRect clientRect;
	GetClientRect(clientRect);

	SqlLogListItem * item = new SqlLogListItem(info);
	int x = 0, y = 0, w = clientRect.Width(), h = SQL_LOG_LISTBOX_ITEM_HEIGHT;
	CRect rect;
	if (!items.empty()) {
		auto lastItem = items.back();
		CRect rcLast = GdiPlusUtil::GetWindowRelativeRect(lastItem->m_hWnd);
		y = rcLast.bottom + 5;
	} else {
		y = 0;
	}
	rect = { x, y, x + w, y + h };
	createOrShowItem(*item, rect, clientRect);
	items.push_back(item);

	//scroll bar
	nHeightSum = rect.bottom + 100;	
	CSize size(clientRect.Width(), clientRect.Height());
	initScrollBar(size);
}

void SqlLogListBox::selectItem(int nItem)
{

}

void SqlLogListBox::removeItem(int nItem)
{

}

void SqlLogListBox::createOrShowUI()
{
	// onSize will trigger init the v-scrollbar
	CRect clientRect;
	GetClientRect(clientRect);

	// calc the scrollbar sum height
	if (!items.empty()) {
		auto lastItem = items.back();
		nHeightSum = GdiPlusUtil::GetWindowRelativeRect(lastItem->m_hWnd).bottom + 100;
	} else {
		nHeightSum = clientRect.bottom;
	}

	CSize size(clientRect.Width(), clientRect.Height());
	initScrollBar(size);
}

void SqlLogListBox::createOrShowGroupLabel(CStatic & win, std::wstring text, CRect & rect, CRect & clientRect)
{
	QWinCreater::createOrShowLabel(m_hWnd, win, text, rect, clientRect, SS_LEFT, 14);
}

void SqlLogListBox::createOrShowItem(SqlLogListItem & win, CRect & rect, CRect & clientRect)
{
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_VISIBLE  | WS_CLIPCHILDREN  | WS_CLIPSIBLINGS | SS_NOTIFY; // SS_NOTIFY - 表示static接受点击事件
		win.Create(m_hWnd, rect);	
		return;
	} else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
	}
}

int SqlLogListBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	bkgBrush = ::CreateSolidBrush(bkgColor);
	return 0;
}

int SqlLogListBox::OnDestroy()
{
	if (bkgBrush) ::DeleteObject(bkgBrush);

	for (auto item : items) {
		if (item && item->IsWindow()) {
			item->DestroyWindow();
			delete item;
			item = nullptr;
		} else if (item) {
			delete item;
			item = nullptr;
		}
	}
	items.clear();

	for (auto group : groups) {
		if (group && group->IsWindow()) {
			group->DestroyWindow();
			delete group;
			group = nullptr;
		} else if (group) {
			delete group;
			group = nullptr;
		}
	}
	groups.clear();
	return 0;
}

void SqlLogListBox::OnSize(UINT nType, CSize size)
{
	createOrShowUI();
}

void SqlLogListBox::OnShowWindow(BOOL bShow, UINT nStatus)
{

}

void SqlLogListBox::OnPaint(CDCHandle dc)
{
	CPaintDC pdc(m_hWnd);
	CMemoryDC mdc(pdc, pdc.m_ps.rcPaint);

	mdc.FillRect(&pdc.m_ps.rcPaint, bkgBrush);
}

BOOL SqlLogListBox::OnEraseBkgnd(CDCHandle dc)
{
	return true;
}

void SqlLogListBox::calcHeightSum()
{

}

void SqlLogListBox::initScrollBar(CSize & clientSize)
{
	if (clientSize.cx == 0 || clientSize.cy == 0 || nHeightSum == 0) {
		return ;
	}
	int pageNums = nHeightSum % clientSize.cy ? 
		nHeightSum / clientSize.cy + 1 : nHeightSum / clientSize.cy;
	si.cbSize = sizeof(SCROLLINFO);   // setting the scrollbar

	// change 3 values(SIF_RANGE: si.nMin, si.nMax, si.nPage; SIF_PAGE:si.nPage; SIF_POS: si.nPos)
	si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS; 
	si.nMin = 0; // must set si.fMask = SIF_RANGE
	si.nMax = 100; // must set si.fMask = SIF_RANGE
	si.nPos = 0; // must set si.fMask = SIF_POS

	// must set si.fMask = SIF_PAGE
	si.nPage = si.nMax % pageNums ? 
		si.nMax / pageNums + 1 : si.nMax / pageNums;

	vScrollPages = si.nPage;
	::SetScrollInfo(m_hWnd, SB_VERT, &si, TRUE);
}

LRESULT SqlLogListBox::OnVScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UINT nSBCode = LOWORD(wParam);
	int nPos = GetScrollPos(SB_VERT);
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	::GetScrollInfo(m_hWnd, SB_VERT, &si);
	iVscrollPos = si.nPos;


	switch (LOWORD(wParam))
	{
	case SB_LINEDOWN:
		si.nPos  += 1;
		break;
	case SB_LINEUP:
		si.nPos -= 1;
		break;
	case SB_PAGEDOWN:
		si.nPos += vScrollPages;
		break;
	case SB_PAGEUP:
		si.nPos -= vScrollPages;
		break;
	case SB_THUMBTRACK:
		si.nPos = si.nTrackPos ;
		break;
	}
	si.nPos = min(100 - vScrollPages, max(0, si.nPos));
	si.fMask = SIF_POS;
	::SetScrollInfo(m_hWnd, SB_VERT, &si, TRUE);
	::GetScrollInfo(m_hWnd, SB_VERT, &si);

	if (iVscrollPos != si.nPos) {
		::ScrollWindow(m_hWnd, 0, cyChar * (iVscrollPos - si.nPos), nullptr, nullptr);
		Invalidate(true);
	}
	 return 0;
}

LRESULT SqlLogListBox::OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int fwKeys = GET_KEYSTATE_WPARAM(wParam);
	short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
	
	if (zDelta == 120) { // 正向
		WPARAM newParam = MAKEWPARAM(SB_LINEUP, 0) ;
		OnVScroll(0, newParam, NULL, bHandled);
	}else if (zDelta == -120) { // 反向
		WPARAM newParam = MAKEWPARAM(SB_LINEDOWN, 0) ;
		OnVScroll(0, newParam, NULL, bHandled);
	}
	return 0;
}
