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

 * @file   SqlLogListItem.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-11-15
 *********************************************************************/
#pragma once
#include "stdafx.h"
#include "SqlLogListItem.h"
#include "common/AppContext.h"
#include "utils/EntityUtil.h"
#include "core/common/Lang.h"
#include "ui/common/QWinCreater.h"
#include "utils/FontUtil.h"
#include "utils/ClipboardUtil.h"
#include "utils/StringUtil.h"
#include "utils/GdiUtil.h"
#include "utils/PerformUtil.h"
#include "utils/Log.h"
#include "ui/common/message/QPopAnimate.h"

SqlLogListItem::SqlLogListItem(ResultInfo & info, QueryPageSupplier * supplier)
{
	this->info = EntityUtil::copy(info);
	this->supplier = supplier;
}

void SqlLogListItem::select(bool state)
{
	selectState = state;
	COLORREF brushColor = selectState ? bkgSelectColor : bkgColor;
	
	if (!bkgBrush.IsNull()) {
		bkgBrush.DeleteObject();
		bkgBrush.CreateSolidBrush(brushColor);
	}
	Invalidate(true);
}

int SqlLogListItem::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	bkgBrush.CreateSolidBrush(bkgColor);
	this->textFont = FT(L"log-list-item-text-size");
	this->sqlFont = FT(L"log-list-item-sql-size");
	return 0;
}

int SqlLogListItem::OnDestroy()
{
	if (!bkgBrush.IsNull())  bkgBrush.DeleteObject();
	if (textFont) ::DeleteObject(textFont);
	if (sqlFont) ::DeleteObject(sqlFont);

	if (createdAtLabel.IsWindow()) createdAtLabel.DestroyWindow();
	if (topImage.IsWindow()) topImage.DestroyWindow();
	if (sqlEdit.IsWindow()) sqlEdit.DestroyWindow();
	if (stateImage.IsWindow()) stateImage.DestroyWindow();
	if (resultMsgLabel.IsWindow()) resultMsgLabel.DestroyWindow();
	if (execTimeLabel.IsWindow()) execTimeLabel.DestroyWindow();
	if (useButton.IsWindow()) useButton.DestroyWindow();
	if (topButton.IsWindow()) topButton.DestroyWindow();
	if (explainButton.IsWindow()) explainButton.DestroyWindow();
	if (copyButton.IsWindow()) copyButton.DestroyWindow();
	if (deleteButton.IsWindow()) deleteButton.DestroyWindow();
	return 0;
}

void SqlLogListItem::OnSize(UINT nType, CSize size)
{
	createOrShowUI();
}

void SqlLogListItem::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if (!bShow) {
		return ;
	}
	loadWindow();
}

void SqlLogListItem::OnPaint(CDCHandle dc)
{
	CPaintDC pdc(m_hWnd);
	pdc.FillRect(&pdc.m_ps.rcPaint, bkgBrush.m_hBrush);
}

BOOL SqlLogListItem::OnEraseBkgnd(CDCHandle dc)
{
	return true;
}

HBRUSH SqlLogListItem::OnCtlStaticColor(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, bkgColor);
	if (hwnd == sqlEdit.m_hWnd) {
		::SetTextColor(hdc, sqlColor); 
		::SelectObject(hdc, sqlFont);
	} else {
		::SetTextColor(hdc, textColor); 
		::SelectObject(hdc, textFont);
	}
	
	return bkgBrush.m_hBrush;
}

HBRUSH SqlLogListItem::OnCtlBtnColor(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, btnBkgColor);
	::SetTextColor(hdc, textColor); 
	::SelectObject(hdc, textFont);
	return bkgBrush.m_hBrush;
}

void SqlLogListItem::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!bTracking) {  
        TRACKMOUSEEVENT tme;  
        tme.cbSize = sizeof(TRACKMOUSEEVENT);  
        tme.dwFlags = TME_LEAVE | TME_HOVER;//Ҫ��������Ϣ����  
        tme.hwndTrack = m_hWnd;  
        tme.dwHoverTime = 10;// �������˲���,�޷�����mouseHover  
  
		 //MOUSELEAVE|MOUSEHOVER��Ϣ�ɴ˺�������.  
        if (::_TrackMouseEvent(&tme)) {  
            bTracking = true;     
        }  
    }
}

void SqlLogListItem::OnMouseHover(WPARAM wParam, CPoint ptPos)
{
	//select(false);
}

void SqlLogListItem::OnMouseLeave()
{
	//select(false);
}

void SqlLogListItem::OnClickUseButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	supplier->setCacheUseSql(getInfo().sql.c_str());

	//class chain : SqlLogListItem(this) -> SqlLogListBox -> SqlLogDialog(here)
	HWND ppHwnd = GetParent().GetParent().m_hWnd;
	::PostMessage(ppHwnd, Config::MSG_QUERY_PAGE_USE_SQL_ID, NULL, NULL);
}

void SqlLogListItem::OnClickCopyButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	CString str;
	sqlEdit.GetWindowText(str);
	ClipboardUtil::copyToClipboard(str);
	QPopAnimate::success(S(L"copy-to-clipboard"));
}

void SqlLogListItem::OnClickExplainButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	std::wstring sql = getInfo().sql;
	std::wstring upline = StringUtil::toupper(getInfo().sql);
	if (!upline.empty() && upline.find(L"EXPLAIN") != 0) {
		sql = L"EXPLAIN " + getInfo().sql;
	}
	supplier->setCacheUseSql(sql.c_str());

	//class chain : SqlLogListItem(this) -> SqlLogListBox -> SqlLogDialog(here)
	HWND ppHwnd = GetParent().GetParent().m_hWnd;
	::PostMessage(ppHwnd, Config::MSG_QUERY_PAGE_USE_SQL_ID, NULL, NULL);
}

void SqlLogListItem::OnClickTopButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	//class chain : SqlLogListItem(this) -> SqlLogListBox -> SqlLogDialog(here)
	HWND ppHwnd = GetParent().GetParent().m_hWnd;
	::PostMessage(ppHwnd, Config::MSG_QUERY_PAGE_TOP_SQL_LOG_ID, WPARAM(getInfo().id), LPARAM(getInfo().top));
}

void SqlLogListItem::OnClickDeleteButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	//class chain : SqlLogListItem(this) -> SqlLogListBox -> SqlLogDialog(here)
	HWND ppHwnd = GetParent().GetParent().m_hWnd;
	::PostMessage(ppHwnd, Config::MSG_QUERY_PAGE_DEL_SQL_LOG_ID, WPARAM(getInfo().id), NULL);
}

void SqlLogListItem::createOrShowUI()
{
	CRect clientRect;
	GetClientRect(clientRect);

	auto _begin1 = PerformUtil::begin();
	createTopElems(clientRect);
	auto _end1 = PerformUtil::end(_begin1);

	auto _begin2 = PerformUtil::begin();
	createMiddleElems(clientRect);
	auto _end2 = PerformUtil::end(_begin2);

	auto _begin3 = PerformUtil::begin();
	createBottomElems(clientRect);
	auto _end3 = PerformUtil::end(_begin3);

	Q_DEBUG(L"_end1:{}, _end2:{}, _end3:{}", _end1, _end2, _end3);
}

void SqlLogListItem::createTopElems(CRect & clientRect)
{
	int x = 10, y = 5, w = 150, h = 12;
	CRect rect(x, y, x + w, y + h);
	QWinCreater::createOrShowLabel(m_hWnd, createdAtLabel, getInfo().createdAt, rect, clientRect, SS_LEFT, 10);

	x = clientRect.right - 20, w = 12, h = 12;
	if (getInfo().top) {
		rect = { x, y, x + w, y + h };
		topImage.load(GdiUtil::copyBitmap(supplier->topBitmap));
		topImage.setBkgColor(bkgColor); 
		QWinCreater::createOrShowImage(m_hWnd, topImage, 0, rect, clientRect);
	}
}

void SqlLogListItem::createMiddleElems(CRect & clientRect)
{
	CRect rcTop = GdiPlusUtil::GetWindowRelativeRect(createdAtLabel); 

	int x = 20, y = rcTop.bottom + 5, w = clientRect.Width() - 20 * 2, h = 60;
	CRect rect(x, y, x + w, y + h);
	DWORD dwStyle = ES_MULTILINE;
	std::wstring fontName = Lang::fontName();
	CSize  size = FontUtil::measureTextSize(getInfo().sql.c_str(), Lang::fontSize(L"log-list-item-sql-size"), false, fontName.c_str());
	if (size.cx / (w * 1.0) > 3.0) {
		dwStyle = dwStyle | WS_VSCROLL;
	}
	QWinCreater::createOrShowEdit(m_hWnd, sqlEdit, 0, getInfo().sql, rect, clientRect, textFont, dwStyle);

	rect.OffsetRect(0, h + 5);
	rect.right = rect.left + 14;
	rect.bottom = rect.top + 14;
	
	if (getInfo().code == 0 && supplier->sucessBitmap) {
		stateImage.load(GdiUtil::copyBitmap(supplier->sucessBitmap));
	}else if (supplier->errorBitmap){
		stateImage.load(GdiUtil::copyBitmap(supplier->errorBitmap));
	}
	
	stateImage.setBkgColor(bkgColor); 
	QWinCreater::createOrShowImage(m_hWnd, stateImage, 0, rect, clientRect);
	std::wstring tip = L"[code:" ;
	tip.append(std::to_wstring(info.code)).append(L"] ").append(info.msg);
	if (info.code) {
		stateImage.setToolTip(tip.c_str());
	}
	
	rect.OffsetRect(rect.Width() + 10, 3);
	rect.right = rect.left + 200;
	rect.bottom = rect.top + 20;
	std::wstring resultMsg = L"Effect Rows:";
	resultMsg.append(std::to_wstring(getInfo().effectRows)).append(L", Code:")
		.append(std::to_wstring(getInfo().code));
	QWinCreater::createOrShowLabel(m_hWnd, resultMsgLabel, resultMsg, rect, clientRect, SS_LEFT, 10);

	rect.MoveToX(clientRect.right - 200 - 20);
	std::wstring execTime = L"Total Times:";
	execTime.append(getInfo().totalTime);
	QWinCreater::createOrShowLabel(m_hWnd, execTimeLabel, execTime, rect, clientRect, SS_RIGHT, 10);
}

void SqlLogListItem::createBottomElems(CRect & clientRect)
{
	auto _begin1 = PerformUtil::begin();
	CRect rcMid = GdiPlusUtil::GetWindowRelativeRect(execTimeLabel);

	auto end1 = PerformUtil::end(_begin1);
	auto _begin2 = PerformUtil::begin();

	int x = clientRect.right - 5 * (50 + 10) - 20, y = rcMid.bottom + 5, w = 50, h = 20; 
	CRect rect(x, y, x + w, y + h);
	createOrShowButton(m_hWnd, useButton, Config::SQL_LOG_ITEM_USE_BUTTON_ID, S(L"use"), rect, clientRect);

	auto end2 = PerformUtil::end(_begin2);
	auto _begin3 = PerformUtil::begin();

	rect.OffsetRect(w + 10, 0);
	createOrShowButton(m_hWnd, explainButton, Config::SQL_LOG_ITEM_EXPLAIN_BUTTON_ID,S( L"explain"), rect, clientRect);

	auto end3 = PerformUtil::end(_begin3);
	auto _begin4 = PerformUtil::begin();

	rect.OffsetRect(w + 10, 0);
	std::wstring topText = info.top ? S(L"untop") : S(L"top");
	createOrShowButton(m_hWnd, topButton, Config::SQL_LOG_ITEM_TOP_BUTTON_ID, topText, rect, clientRect);

	auto end4 = PerformUtil::end(_begin4);
	auto _begin5 = PerformUtil::begin();

	rect.OffsetRect(w + 10, 0);
	createOrShowButton(m_hWnd, copyButton, Config::SQL_LOG_ITEM_COPY_BUTTON_ID, S(L"copy"), rect, clientRect);

	auto end5 = PerformUtil::end(_begin5);
	auto _begin6 = PerformUtil::begin();

	rect.OffsetRect(w + 10, 0);
	createOrShowButton(m_hWnd, deleteButton, Config::SQL_LOG_ITEM_DELELE_BUTTON_ID, S(L"delete"), rect, clientRect);

	auto end6 = PerformUtil::end(_begin6);
	Q_DEBUG(L"t1:{},t2:{},t3:{},t4:{},t5:{},t6:{}", end1, end2, end3, end4, end5, end6);
}

void SqlLogListItem::createOrShowButton(HWND hwnd, CButton & win, UINT id, std::wstring text, CRect rect, CRect &clientRect, DWORD exStyle)
{
	if (::IsWindow(hwnd) && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN  | WS_CLIPSIBLINGS ; 
		if (exStyle) dwStyle = dwStyle | exStyle;
		auto _begin1 = PerformUtil::begin();
		win.Create(hwnd, rect, text.c_str(), dwStyle , 0, id);
		Q_DEBUG(L"create button time:{}", PerformUtil::end(_begin1));
		auto end1 = PerformUtil::end(_begin1);

		return ;
	} else if (::IsWindow(hwnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
	}
}
void SqlLogListItem::loadWindow()
{
	
}

