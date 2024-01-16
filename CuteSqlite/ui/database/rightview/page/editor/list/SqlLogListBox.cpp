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
#include "utils/PerformUtil.h"
#include "utils/Log.h"
#include "core/common/Lang.h"
#include <utils/DateUtil.h>

#define SQL_LOG_LISTBOX_ITEM_HEIGHT 145
#define SQL_LOG_LISTBOX_GRROUP_HEIGHT 20

int SqlLogListBox::cxChar = 5; // Need initialize in OnCreate
int SqlLogListBox::cyChar = 5; // Need initialize in OnCreate
int SqlLogListBox::iVscrollPos = 5;
int SqlLogListBox::vScrollPages = 5;


SqlLogListBox::~SqlLogListBox()
{
	m_hWnd = nullptr;
}

void SqlLogListBox::setup(QueryPageSupplier * supplier)
{
	this->supplier = supplier;
}

void SqlLogListBox::addGroup(const std::wstring & group, const std::wstring & origDate)
{
	if (group.empty()) {
		return;
	}
	if (!origDate.empty() && !items.empty()) {
		auto & lastItem = items.back();
		if (!lastItem->getInfo().top && DateUtil::getDateFromDateTime(lastItem->getInfo().createdAt) == origDate) {
			return;
		}
	}
	
	CRect clientRect;
	GetClientRect(clientRect);

	CStatic * groupLabel = new CStatic();
	int x = 0, y = 0, w = clientRect.Width(), h = 20;
	CRect rect;
	int offset = 0;
	if (!winHwnds.empty()) {
		CRect rcLast = GdiPlusUtil::GetWindowRelativeRect(winHwnds.back());
		y = rcLast.bottom + 5;
		offset = 5;
	}
	rect = { x, y, x + w, y + h };
	std::wstring fmtgrp = L"  " + group;
	createOrShowGroupLabel(*groupLabel, fmtgrp, rect, clientRect);
	groups.push_back(groupLabel);
	winHwnds.push_back(groupLabel->m_hWnd);
	nHeightSum += rect.Height() + offset;
}

void SqlLogListBox::addItem(ResultInfo & info, int enableBtns)
{
	CRect clientRect;
	GetClientRect(clientRect);

	SqlLogListItem * item = new SqlLogListItem(info, supplier, enableBtns);
	int x = 0, y = 0, w = clientRect.Width(), h = SQL_LOG_LISTBOX_ITEM_HEIGHT;
	CRect rect;
	int offset = 0;
	if (!winHwnds.empty()) {
		CRect rcLast = GdiPlusUtil::GetWindowRelativeRect(winHwnds.back());
		y = rcLast.bottom + 5;
		offset = 5;
	} 
	rect = { x, y, x + w, y + h };
	auto _begin2 = PerformUtil::begin();
	createOrShowItem(*item, rect, clientRect);
	Q_DEBUG(L"Add item Spend time:" + PerformUtil::end(_begin2));
	items.push_back(item);
	winHwnds.push_back(item->m_hWnd);
	nHeightSum += rect.Height() + offset;
}

void SqlLogListBox::clearAllItems()
{
	clearItems();
	clearGroups();
	winHwnds.clear();
	nHeightSum = 0;
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

	resizeGroupsAndItems(clientRect);
	// calc the scrollbar sum height
	if (!winHwnds.empty()) {
		nHeightSum = GdiPlusUtil::GetWindowRelativeRect(winHwnds.back()).bottom;
	} else {
		nHeightSum = 0;
	}
// 	for (HWND hwnd : winHwnds) {
// 		CRect rect = GdiPlusUtil::GetWindowRelativeRect(hwnd);
// 		nHeightSum = rect.Height() + 5;
// 	}
}

void SqlLogListBox::createOrShowGroupLabel(CStatic & win, std::wstring text, CRect & rect, CRect & clientRect)
{
	QWinCreater::createOrShowLabel(m_hWnd, win, text, rect, clientRect, SS_LEFT, 14);
}


void SqlLogListBox::resizeGroupsAndItems(CRect & clientRect)
{
	CRect rect;
	for (auto item : items) {
		rect = GdiPlusUtil::GetWindowRelativeRect(item->m_hWnd);
		rect.right = rect.left + clientRect.Width();
		item->MoveWindow(rect);
	}

	for (auto group : groups) {
		rect = GdiPlusUtil::GetWindowRelativeRect(group->m_hWnd);
		rect.right = rect.left + clientRect.Width();
		group->MoveWindow(rect);
	}
}

void SqlLogListBox::createOrShowItem(SqlLogListItem & win, CRect & rect, CRect & clientRect)
{
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN  | WS_CLIPSIBLINGS | SS_NOTIFY; // SS_NOTIFY - 表示static接受点击事件
		win.Create(m_hWnd, rect);	
		return;
	} else if (::IsWindow(m_hWnd) && win.IsWindow() && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
	}
}

int SqlLogListBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	bkgBrush.CreateSolidBrush(bkgColor);
	groupFont = FTB(L"form-text-size", true);

	// Init the scrollbar params 
	HDC hdc  = ::GetDC(m_hWnd);
	::GetTextMetrics(hdc, &tm);
	cxChar = tm.tmAveCharWidth;
	cyChar = tm.tmHeight + tm.tmExternalLeading;
	::ReleaseDC(m_hWnd, hdc);

	return 0;
}

int SqlLogListBox::OnDestroy()
{
	if (!bkgBrush.IsNull()) bkgBrush.DeleteObject();
	if (groupFont) ::DeleteObject(groupFont);

	clearItems();
	clearGroups();
	winHwnds.clear();
	return 0;
}


void SqlLogListBox::clearGroups()
{
	for (auto & group : groups) {
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
}

void SqlLogListBox::clearItems()
{
	for (auto & item : items) {
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

	mdc.FillRect(&pdc.m_ps.rcPaint, bkgBrush.m_hBrush);
}

BOOL SqlLogListBox::OnEraseBkgnd(CDCHandle dc)
{
	return true;
}

void SqlLogListBox::reloadVScroll()
{
	CRect clientRect;
	GetClientRect(clientRect);

	//scroll bar
	CSize size(clientRect.Width(), clientRect.Height());
	initScrollBar(size);
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
	si.nMax = 100; // must set si.fMask = SIF_RANGE ，滚动条的最大pos数, 即从开始位置滚动到末尾位置要移动多少次鼠标滚轮	
	si.nPos = 0 ; // must set si.fMask = SIF_POS， 滚动条的起始pos位置

	// must set si.fMask = SIF_PAGE 每页的pos数 = 最大的总页数 / 页数， 即可视窗口(clientRect.height)能包含几个pos
	si.nPage = si.nMax % pageNums ?  
		si.nMax / pageNums + 1 : si.nMax / pageNums;

	vScrollPages = si.nPage;
	::SetScrollInfo(m_hWnd, SB_VERT, &si, TRUE);

	
}

LRESULT SqlLogListBox::OnVScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UINT nSBCode = LOWORD(wParam);
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	::GetScrollInfo(m_hWnd, SB_VERT, &si);
	iVscrollPos = si.nPos;

	bool isDown = false, isUp = false;
	switch (LOWORD(wParam))
	{
	case SB_LINEDOWN:
		si.nPos  += 1;
		isDown = true;
		break;
	case SB_LINEUP:
		si.nPos -= 1;
		break;
	case SB_PAGEDOWN:
		si.nPos += vScrollPages;
		isDown = true;
		break;
	case SB_PAGEUP:
		si.nPos -= vScrollPages;
		break;
	case SB_THUMBTRACK:
		si.nPos = si.nTrackPos ;
		break;
	}
	// si.nPos = min(100 - vScrollPages, max(0, si.nPos));
	si.nPos = min(100, max(0, si.nPos));
	if (si.nPos > iVscrollPos) {
		isDown = true;
	}else if (si.nPos < iVscrollPos) {
		isUp = true;
	}
	si.fMask = SIF_POS;
	::SetScrollInfo(m_hWnd, SB_VERT, &si, TRUE);
	::GetScrollInfo(m_hWnd, SB_VERT, &si);

	CRect clientRect;
	GetClientRect(clientRect);
	if (iVscrollPos != si.nPos) {
		int scrollHeight = nHeightSum * (iVscrollPos - si.nPos) / 100;
		::ScrollWindow(m_hWnd, 0, scrollHeight, nullptr, nullptr);
		Invalidate(true);
	}

	if (si.nPos == 100 && !winHwnds.empty()) {
		CRect lastRect = GdiPlusUtil::GetWindowRelativeRect(winHwnds.back());
		CRect clientRect;
		GetClientRect(clientRect);
		if (lastRect.bottom != clientRect.bottom) {
			::ScrollWindow(m_hWnd, 0, clientRect.bottom - lastRect.bottom, nullptr, nullptr);
			Invalidate(true);
		}
	} else if (si.nPos == 0 && !winHwnds.empty()) { 
		CRect firstRect = GdiPlusUtil::GetWindowRelativeRect(winHwnds.front());
		CRect clientRect;
		GetClientRect(clientRect);
		if (firstRect.top != clientRect.top) {
			::ScrollWindow(m_hWnd, 0, clientRect.top - firstRect.top, nullptr, nullptr);
			Invalidate(true);
		}
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

HBRUSH SqlLogListBox::OnCtlStaticColor(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, bkgColor);
	
	::SetTextColor(hdc, groupColor); 
	::SelectObject(hdc, groupFont);
	
	return bkgBrush.m_hBrush;
}
