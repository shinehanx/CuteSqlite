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

 * @file   SqlLogPage.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-12-01
 *********************************************************************/
#include "stdafx.h"
#include "SqlLogPage.h"
#include "common/Config.h"
#include "common/AppContext.h"
#include "core/common/Lang.h"
#include "ui/common/message/QPopAnimate.h"
#include "ui/common/QWinCreater.h"
#include "utils/PerformUtil.h"
#include "ui/common/message/QMessageBox.h"

BOOL SqlLogPage::PreTranslateMessage(MSG* pMsg)
{
	return FALSE;
}

void SqlLogPage::createOrShowUI()
{
	CRect clientRect;
	GetClientRect(clientRect);

	createOrShowSearchEdit(searchEdit, clientRect);
	createOrShowSqlLogListBox(sqlLogListBox, clientRect);
	createOrShowPageElems(clientRect);
}

void SqlLogPage::createOrShowSearchEdit(QSearchEdit & win, CRect & clientRect)
{
	int x = 30, y = 20, w = 300, h = 28;
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


void SqlLogPage::createOrShowSqlLogListBox(SqlLogListBox & win, CRect & clientRect)
{
	CRect rcLast = GdiPlusUtil::GetWindowRelativeRect(searchEdit.m_hWnd);
	int x = 2, y = rcLast.bottom + 10, w = clientRect.Width() - 4, h = clientRect.Height() - y - 33;
	CRect rect = { x, y, x + w, y + h }; 
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		win.setup(supplier);
		win.Create(m_hWnd, rect, L"", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VSCROLL | WS_BORDER, 0, Config::DIALOG_SQL_LOG_LIST_ID);
	} else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
	}
}


void SqlLogPage::createOrShowPageElems(CRect & clientRect)
{
	CRect rcLast = GdiPlusUtil::GetWindowRelativeRect(sqlLogListBox.m_hWnd);
	int x = 10, y = rcLast.bottom + 5, w = 120, h = 20 ;
	CRect rect = { x, y, x + w, y + h }; 
	CString str;
	if (pageLabel.IsWindow()) {
		pageLabel.GetWindowText(str);
	}
	QWinCreater::createOrShowLabel(m_hWnd, pageLabel, str.GetString(), rect, clientRect, SS_LEFT | SS_CENTERIMAGE);

	rect.OffsetRect(w + 5, 0);
	w = 80;
	rect.right = rect.left + w;
	QWinCreater::createOrShowButton(m_hWnd, firstPageButton, Config::FIRST_PAGE_BUTTON_ID, S(L"first-page"), rect, clientRect);

	rect.OffsetRect(w + 5, 0);
	QWinCreater::createOrShowButton(m_hWnd, prevPageButton, Config::PREV_PAGE_BUTTON_ID, S(L"prev-page"), rect, clientRect);

	rect.OffsetRect(w + 5, 0);
	QWinCreater::createOrShowButton(m_hWnd, nextPageButton, Config::NEXT_PAGE_BUTTON_ID, S(L"next-page"), rect, clientRect);

	rect.OffsetRect(w + 5, 0);
	QWinCreater::createOrShowButton(m_hWnd, lastPageButton, Config::LAST_PAGE_BUTTON_ID, S(L"last-page"), rect, clientRect);

}

void SqlLogPage::loadWindow()
{
	if (!isNeedReload) {
		return;
	}
	isNeedReload = false;

	curPage = 1;
	loadSqlLogListBox(curPage);
}


void SqlLogPage::loadSqlLogListBox(int page)
{
	// clear all items and groups

	SqlLogList topList, list;
	uint64_t total;
	if (keyword.empty()) {
		topList = sqlLogService->getTopSqlLog();
		list = sqlLogService->getPageSqlLog(page, perPage);
		total = sqlLogService->getSqlLogCount();
	} else {
		topList = sqlLogService->getTopSqlLogByKeyword(keyword);
		list = sqlLogService->getPageSqlLogByKeyword(keyword, page, perPage);
		total = sqlLogService->getSqlLogCountByKeyword(keyword);
	}

	if (list.empty()) {
		return;
	}
	maxPage = total % perPage ? static_cast<int>(total / perPage) + 1 : static_cast<int>(total / perPage);
	if (page > maxPage) {
		curPage = maxPage;
		return;
	}
	if (page < 1) {
		curPage = 1;
		return;
	}
	sqlLogListBox.clearAllItems();
	std::wstring pageText = getPageText(page, perPage, total);
	pageLabel.SetWindowText(pageText.c_str());
	std::vector<std::wstring> dates = sqlLogService->getDatesFromList(list);
	auto _begin = PerformUtil::begin();
	
	int enableBtns = SW_ANALYSIS_BTN | SW_COPY_BTN | SW_TOP_BTN | SW_DELELE_BTN ;
	if (!topList.empty() && page == 1) {
		sqlLogListBox.addGroup(L"Top");
		for (auto & item : topList) {
			sqlLogListBox.addItem(item, enableBtns);
		}
	}
	
	for (auto & date : dates) {
		if (date.empty()) {
			continue;
		}
		std::wstring fmtDate = formatDateForDisplay(date);
		sqlLogListBox.addGroup(fmtDate, date);
		SqlLogList dateList = sqlLogService->getFilteredListByDate(list, date);
		for (auto & item : dateList) {			
			sqlLogListBox.addItem(item, enableBtns);
		}		
	}
	sqlLogListBox.reloadVScroll();
	std::wstring _tt = PerformUtil::end(_begin);
	Q_DEBUG(L"spend time:" +  _tt);
}

int SqlLogPage::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	supplier = new QueryPageSupplier();
	this->textFont = FT(L"log-list-item-sql-size");
	bkgBrush.CreateSolidBrush(bkgColor);
	return 0;
}

int SqlLogPage::OnDestroy()
{
	if (!bkgBrush.IsNull()) bkgBrush.DeleteObject();
	if (textFont) ::DeleteObject(textFont);

	if (searchEdit.IsWindow()) searchEdit.DestroyWindow();
	if (sqlLogListBox.IsWindow()) sqlLogListBox.DestroyWindow();

	if (pageLabel.IsWindow()) pageLabel.DestroyWindow();
	if (firstPageButton.IsWindow()) firstPageButton.DestroyWindow();
	if (prevPageButton.IsWindow()) prevPageButton.DestroyWindow();
	if (nextPageButton.IsWindow()) nextPageButton.DestroyWindow();
	if (lastPageButton.IsWindow()) lastPageButton.DestroyWindow();

	isNeedReload = true;
	return 0;
}

HBRUSH SqlLogPage::OnCtlEditColor(HDC hdc, HWND hwnd)
{
	return bkgBrush.m_hBrush;
}


HBRUSH SqlLogPage::OnCtlStaticColor(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, bkgColor);
	::SetTextColor(hdc, textColor);
	::SelectObject(hdc, textFont);
	return bkgBrush.m_hBrush;
}

HBRUSH SqlLogPage::OnCtlBtnColor(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, bkgColor);
	::SetTextColor(hdc, textColor); 
	::SelectObject(hdc, textFont);
	return bkgBrush.m_hBrush;
}


void SqlLogPage::paintItem(CDC & dc, CRect & paintRect)
{
	dc.FillRect(paintRect, bkgBrush.m_hBrush);
}

std::wstring SqlLogPage::formatDateForDisplay(const std::wstring & date)
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

LRESULT SqlLogPage::OnClickFirstPageButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	curPage = 1;
	loadSqlLogListBox(curPage);
	return 0;
}


LRESULT SqlLogPage::OnClickPrevPageButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	curPage = (curPage <= 1) ? 1 : curPage - 1;
	loadSqlLogListBox(curPage);
	return 0;
}


LRESULT SqlLogPage::OnClickNextPageButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	curPage = (curPage >= maxPage) ? maxPage : curPage + 1;
	loadSqlLogListBox(curPage);
	return 0;
}


LRESULT SqlLogPage::OnClickLastPageButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	curPage = maxPage;
	loadSqlLogListBox(curPage);
	return 0;
}


LRESULT SqlLogPage::OnClickAnalysisButton(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// class chain : SqlLogPage(this)->QTabView->RightAnalysisView(Here)
	// HWND ppHwnd = GetParent().GetParent().m_hWnd; // RightAnalysisView
	//::PostMessage(ppHwnd, Config::MSG_ANALYSIS_SQL_ID, wParam, lParam);
	// wParam - userDbId, lParam = sqlLogId
	AppContext::getInstance()->dispatch(Config::MSG_ANALYSIS_SQL_ID, wParam, lParam);
	
	return 0;
}

LRESULT SqlLogPage::OnClickTopButton(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	uint64_t id = static_cast<uint64_t>(wParam);
	int top = static_cast<int>(lParam);
	if (!id) {
		return 0;
	}
	try {
		sqlLogService->topSqlLog(id, !top);
		curPage = 1;
		loadSqlLogListBox(curPage);
	} catch (QRuntimeException &ex) {
		QPopAnimate::report(ex);
	}
	
	return 0;
}

LRESULT SqlLogPage::OnClickDeleteButton(UINT uMsg, WPARAM wParam, LPARAM lParam)
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
		curPage = 1;
		loadSqlLogListBox(curPage);
	} catch (QRuntimeException &ex) {
		QPopAnimate::report(ex);
	}
	
	return 0;
}


LRESULT SqlLogPage::OnClickSearchButton(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	try {
		keyword = searchEdit.getText();

		curPage = 1;
		loadSqlLogListBox(curPage);
	} catch (QRuntimeException &ex) {
		QPopAnimate::report(ex);
	}
	
	return 0;
}


LRESULT SqlLogPage::OnLoadNextPageSqlLog(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	try {
		loadSqlLogListBox(++curPage);
	} catch (QRuntimeException &ex) {
		QPopAnimate::report(ex);
	}
	return 0;
}

std::wstring SqlLogPage::getPageText(int curPage, int perPage, uint64_t total)
{
	ATLASSERT(curPage && perPage);
	int pages = total % perPage ? static_cast<int>(total / perPage) + 1 
		: static_cast<int>(total / perPage);
	return fmt::format(L"Page: {}/{}, Total:{}", 
		std::to_wstring(curPage), std::to_wstring(pages), std::to_wstring(total));
}