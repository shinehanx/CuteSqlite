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

 * @file   SqlLogDialog.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-11-14
 *********************************************************************/
#include "stdafx.h"
#include "SqlLogDialog.h"
#include <atltypes.h>
#include "ui/common/QWinCreater.h"
#include "core/common/Lang.h"
#include "ui/common/message/QPopAnimate.h"
#include "ui/common/message/QMessageBox.h"

void SqlLogDialog::setup(HWND parentHwnd, QueryPageEditorAdapter * adapter, CRect parentWinRect) 
{
	int x = parentWinRect.right - SQL_LOG_DIALOG_WIDTH,
		y = parentWinRect.bottom + 5,
		w = SQL_LOG_DIALOG_WIDTH,
		h = SQL_LOG_DIALOG_HEIGHT;
	winRect = { x, y, x + w , y + h };

	this->parentHwnd = parentHwnd;
	this->adapter = adapter;
}


void SqlLogDialog::createOrShowUI()
{
	CRect clientRect;
	GetClientRect(clientRect);

	createOrShowCloseButton(clientRect);
	createOrShowSearchEdit(searchEdit, clientRect);
	createOrShowSqlLogListBox(sqlLogListBox, clientRect);
}


void SqlLogDialog::createOrShowCloseButton(CRect & clientRect)
{
	int x = clientRect.right - 21, y = 5, w = 16 , h = 16;
	CRect rect = { x, y, x + w, y + h };
	
	std::wstring normalImagePath = ResourceUtil::getProductImagesDir() + L"common\\message\\close-button-normal.png";
	std::wstring pressedImagePath = ResourceUtil::getProductImagesDir() + L"common\\message\\close-button-pressed.png";
	closeButton.SetIconPath(normalImagePath, pressedImagePath);
	closeButton.SetBkgColors(RGB(238, 238, 238), RGB(238, 238, 238), RGB(238, 238, 238));
	QWinCreater::createOrShowButton(m_hWnd, closeButton, Config::QPOP_ANIMATE_CLOSE_BUTTON_ID, L"", rect, clientRect);
}


void SqlLogDialog::createOrShowSearchEdit(QSearchEdit & win, CRect & clientRect)
{
	int x = 30, y = 20, w = clientRect.Width() - 60 , h = 28;
	CRect rect = { x, y, x + w, y + h }; 

	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		win.Create(m_hWnd, rect, L"", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS , 0, Config::SEARCH_EDIT_ID);
		HFONT font = FT(L"search-edit-size");
		win.setFont(font);
		win.setCueBanner(std::wstring(L"SELECT,UPDATE,INSERT,CREATE ..."));
		win.setSel(0, -1);
		win.setFocus();
		DeleteObject(font);
		return;
	} else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
	}
}


void SqlLogDialog::createOrShowSqlLogListBox(SqlLogListBox & win, CRect & clientRect)
{
	CRect rcLast = GdiPlusUtil::GetWindowRelativeRect(searchEdit.m_hWnd);
	int x = 2, y = rcLast.bottom + 10, w = clientRect.Width() - 4, h = clientRect.Height() - y - 13;
	CRect rect = { x, y, x + w, y + h }; 
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		win.setup(adapter->getSupplier());
		win.Create(m_hWnd, rect, L"", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VSCROLL, 0, Config::DIALOG_SQL_LOG_LIST_ID);
	} else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
	}
}


void SqlLogDialog::loadWindow()
{
	if (!isNeedReload) {
		return;
	}
	isNeedReload = false;

	loadSqlLogListBox();
}


void SqlLogDialog::loadSqlLogListBox()
{
	sqlLogListBox.clearAllItems();
	SqlLogList topList = sqlLogService->getTopSqlLog();
	SqlLogList list = sqlLogService->getAllSqlLog();
	std::vector<std::wstring> dates = sqlLogService->getDatesFromList(list);

	if (!topList.empty()) {
		sqlLogListBox.addGroup(L"Top");
		for (auto & item : topList) {
			sqlLogListBox.addItem(item);
		}
	}

	for (auto & date : dates) {
		if (date.empty()) {
			continue;
		}
		std::wstring fmtDate = formatDateForDisplay(date);
		sqlLogListBox.addGroup(fmtDate);
		SqlLogList dateList = sqlLogService->getFilteredListByDate(list, date);
		for (auto & item : dateList) {
			sqlLogListBox.addItem(item);
		}		
	}
	sqlLogListBox.reloadVScroll();
}

LRESULT SqlLogDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	MoveWindow(winRect); //  注意，这个winRect必须赋值，不然不显示

	CRect clientRect;
	GetClientRect(clientRect);
	// 圆角窗口
	HRGN hRgn = ::CreateRoundRectRgn(0, 0, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, 20, 20);
	::SetWindowRgn(m_hWnd, hRgn, TRUE);

	// 置顶悬浮窗口
	//SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); 

	bkgBrush = ::CreateSolidBrush(bkgColor);
	return 0;
}

LRESULT SqlLogDialog::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (bkgBrush) ::DeleteObject(bkgBrush);
	if (textFont) ::DeleteObject(textFont);

	if (closeButton.IsWindow()) closeButton.DestroyWindow();
	if (searchEdit.IsWindow()) searchEdit.DestroyWindow();
	if (sqlLogListBox.IsWindow()) sqlLogListBox.DestroyWindow();
	return 0;
}

LRESULT SqlLogDialog::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	createOrShowUI();
	return 0;
}


LRESULT SqlLogDialog::OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!wParam) { 
		return 0;
	}
	loadWindow();
	return 0;
}

LRESULT SqlLogDialog::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CPaintDC dc(m_hWnd);
	CMemoryDC mdc(dc, dc.m_ps.rcPaint);
	mdc.FillRect(&(dc.m_ps.rcPaint), bkgBrush);
	return 0;
}

HBRUSH SqlLogDialog::OnCtlEditColor(HDC hdc, HWND hwnd)
{
	return bkgBrush;
}

LRESULT SqlLogDialog::OnClickCloseButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	EndDialog(0);
	return 0;
}

LRESULT SqlLogDialog::OnClickUseButton(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	EndDialog(Config::QDIALOG_YES_BUTTON_ID);
	return 0;
}


LRESULT SqlLogDialog::OnClickTopButton(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	uint64_t id = static_cast<uint64_t>(wParam);
	if (!id) {
		return 0;
	}
	try {
		sqlLogService->topSqlLog(id);
		loadSqlLogListBox();
	} catch (QRuntimeException &ex) {
		QPopAnimate::report(ex);
	}
	
	return 0;
}

LRESULT SqlLogDialog::OnClickDeleteButton(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	uint64_t id = static_cast<uint64_t>(wParam);
	if (!id) {
		return 0;
	}
	if (QMessageBox::confirm(m_hWnd, S(L"delete-sql-log-confirm-text")) != Config::CUSTOMER_FORM_YES_BUTTON_ID) {
		return 0;
	}

	try {
		sqlLogService->removeSqlLog(id);
		loadSqlLogListBox();
	} catch (QRuntimeException &ex) {
		QPopAnimate::report(ex);
	}
	
	return 0;
}


LRESULT SqlLogDialog::OnClickSearchButton(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	try {
		loadSqlLogListBox();
	} catch (QRuntimeException &ex) {
		QPopAnimate::report(ex);
	}
	
	return 0;
}

std::wstring SqlLogDialog::formatDateForDisplay(const std::wstring & date)
{
	if (date.empty()) {
		return date;
	}
	std::wstring result;
	std::wstring curdate = DateUtil::getCurrentDate();
	if (curdate == date) {
		std::wstring week = DateUtil::getWeekDay(CTime::GetCurrentTime());
		result = S(L"today");
		result.append(L" - ").append(date).append(L" ").append(week);
	} else if (date == DateUtil::getYestoday()) {
		std::wstring week = DateUtil::getWeekDay(DateUtil::getYestodayTime());
		result = S(L"yestoday");
		result.append(L" - ").append(date).append(L" ").append(week);
	}  else {
		std::wstring week = DateUtil::getWeekDay(DateUtil::getTimeFromString(date + L" 00:00:00"));
		result.append(date).append(L" ").append(week);
	}
	return result;
}
