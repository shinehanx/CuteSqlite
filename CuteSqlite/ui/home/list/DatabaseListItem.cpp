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

 * @file   DatabaseListItem.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-11-25
 *********************************************************************/
#include "stdafx.h"
#include "DatabaseListItem.h"
#include "core/common/Lang.h"
#include "utils/EntityUtil.h"
#include "ui/common/QWinCreater.h"
#include "utils/ResourceUtil.h"

DatabaseListItem::DatabaseListItem(UserDb & _userDb)
{
	this->userDb = EntityUtil::copy(_userDb);
}


DatabaseListItem::~DatabaseListItem()
{
	m_hWnd = nullptr;
}

uint64_t DatabaseListItem::getUserDbId()
{
	return userDb.id;
}

void DatabaseListItem::createOrShowUI()
{
	CRect clientRect;
	GetClientRect(clientRect);
	createOrShowStoreImage(storeImage, clientRect);
	createOrShowPathEdit(clientRect);
}


void DatabaseListItem::createOrShowStoreImage(QStaticImage & win, CRect &clientRect)
{
	int x = 20, y = 40, w = 14, h = 14;
	CRect rect(x, y, x + w, y + h);
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_VISIBLE |WS_CLIPCHILDREN  | WS_CLIPSIBLINGS | SS_NOTIFY; // SS_NOTIFY - 表示static接受点击事件
		std::wstring imgDir = ResourceUtil::getProductImagesDir();
		std::wstring imgPath = imgDir + L"home\\list\\store.png";
		win.load(imgPath.c_str(), BI_PNG, true);
		win.Create(m_hWnd, rect, L"", dwStyle , 0);
		return;
	} else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW); 
	}
}

void DatabaseListItem::createOrShowPathEdit(CRect &clientRect)
{
	CRect rect(40, 40, clientRect.right - 20, clientRect.bottom - 10);
	createOrShowEdit(pathEdit, Config::DBLIST_ITEM_PATH_EDIT_ID, userDb.path, rect, clientRect, ES_MULTILINE | ES_AUTOVSCROLL);
}

void DatabaseListItem::createOrShowEdit(WTL::CEdit & win, UINT id, std::wstring text, CRect rect, CRect &clientRect, DWORD exStyle)
{
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_VISIBLE |WS_TABSTOP | SS_NOTIFY;
		if (exStyle) {
			dwStyle |= exStyle;
		}
		win.Create(m_hWnd, rect, text.c_str(), dwStyle , 0, id);
		win.SetReadOnly(TRUE);
		win.SetWindowText(text.c_str());
		return;
	} else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
		win.SetWindowText(text.c_str());
	}
}

int DatabaseListItem::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	bkgBrush.CreateSolidBrush(bkgColor);
	bkgHoverBrush.CreateSolidBrush(bkgHoverColor);
	nameFont = FTB(L"panel-list-header-size", true);
	textFont = FT(L"panel-list-item-size");
	return 0;
}

int DatabaseListItem::OnDestroy()
{
	if (bkgBrush.IsNull()) bkgBrush.DeleteObject();
	if (bkgHoverBrush.IsNull()) bkgHoverBrush.DeleteObject();
	if (nameFont) ::DeleteObject(nameFont);
	if (textFont) ::DeleteObject(textFont);
	if (storeImage.IsWindow()) storeImage.DestroyWindow();
	if (pathEdit .IsWindow()) pathEdit.DestroyWindow();
	return 0;
}

void DatabaseListItem::OnSize(UINT nType, CSize size)
{
	createOrShowUI();
}

void DatabaseListItem::OnShowWindow(BOOL bShow, UINT nStatus)
{

}

void DatabaseListItem::OnPaint(CDCHandle dc)
{
	CPaintDC pdc(m_hWnd);
	CMemoryDC mdc(pdc, pdc.m_ps.rcPaint);
	CRect clientRect;
	GetClientRect(clientRect);
	if (isHover) {
		mdc.FillRect(&pdc.m_ps.rcPaint, bkgHoverBrush.m_hBrush);
	}else {
		mdc.FillRect(&pdc.m_ps.rcPaint, bkgBrush.m_hBrush);
	}
	
	int x = 10, y = 10, w = DATABASE_LIST_ITEM_WIDTH - 20, h = 40;
	CRect rect(x, y, x + w, y + h);
	int oldMode = mdc.SetBkMode(TRANSPARENT);
	HFONT oldFont = mdc.SelectFont(nameFont);
	COLORREF oldColor = mdc.SetTextColor(textColor);
	std::wstring text = userDb.name;
	mdc.DrawText(text.c_str(), static_cast<int>(text.size()), rect, DT_LEFT | DT_VCENTER);
	mdc.SetTextColor(oldColor);
	mdc.SelectFont(oldFont);
	mdc.SetBkMode(oldMode);
}

BOOL DatabaseListItem::OnEraseBkgnd(CDCHandle dc)
{
	return TRUE;
}

void DatabaseListItem::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!isTracking) {  
        TRACKMOUSEEVENT tme;  
        tme.cbSize = sizeof(TRACKMOUSEEVENT);  
        tme.dwFlags = TME_LEAVE | TME_HOVER;//要触发的消息类型  
        tme.hwndTrack = m_hWnd;  
        tme.dwHoverTime = 10;// 如果不设此参数,无法触发mouseHover  
  
		 //MOUSELEAVE|MOUSEHOVER消息由此函数触发.  
        if (::_TrackMouseEvent(&tme)) {  
            isTracking = true; 
        }  
    }
	
}

void DatabaseListItem::OnMouseHover(WPARAM wParam, CPoint ptPos)
{
	isHover = true;
	Invalidate(true);
	pathEdit.Invalidate(true);
}

void DatabaseListItem::OnMouseLeave()
{
	isHover = false;
	Invalidate(true);
	pathEdit.Invalidate(true);
	
	isTracking = false;
}


void DatabaseListItem::OnLButtonDown(UINT nFlags, CPoint point)
{
	UINT nID = static_cast<UINT>(::GetWindowLongPtr(m_hWnd, GWLP_ID));
	::PostMessage(GetParent().m_hWnd, Config::MSG_DBLIST_ITEM_CLICK_ID, (WPARAM)nID, (LPARAM)userDb.id);
}

HBRUSH DatabaseListItem::OnCtlStaticColor(HDC hdc, HWND hwnd)
{
	::SetTextColor(hdc, textColor); 
	::SelectObject(hdc, textFont);
	if (isHover) {
		::SetBkColor(hdc, bkgHoverColor);
		return bkgHoverBrush.m_hBrush;
	} else {
		::SetBkColor(hdc, bkgColor);
		return bkgBrush.m_hBrush;
	}
	
}

void DatabaseListItem::OnClickPathEdit(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	UINT ctrlId = static_cast<UINT>(::GetWindowLongPtr(m_hWnd, GWLP_ID));
	::PostMessage(GetParent().m_hWnd, Config::MSG_DBLIST_ITEM_CLICK_ID, (WPARAM)ctrlId, (LPARAM)userDb.id);
}
