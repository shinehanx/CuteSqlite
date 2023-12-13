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

 * @file   PerfAnalysisPage.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-12-03
 *********************************************************************/
#include "stdafx.h"
#include "PerfAnalysisPage.h"
#include "core/common/Lang.h"
#include "utils/ResourceUtil.h"
#include "ui/common/QWinCreater.h"
#include "utils/StringUtil.h"
#include "common/AppContext.h"


BOOL PerfAnalysisPage::PreTranslateMessage(MSG* pMsg)
{
	return FALSE;
}

PerfAnalysisPage::PerfAnalysisPage(uint64_t sqlLogId)
{
	supplier.setSqlLogId(sqlLogId);
	supplier.setSqlLog(sqlLogService->getSqlLog(sqlLogId));

}


uint64_t PerfAnalysisPage::getSqlLogId()
{
	return supplier.getSqlLogId();
}

void PerfAnalysisPage::createOrShowUI()
{
	CRect clientRect;
	GetClientRect(clientRect);
	nHeightSum = 0;

	createOrShowTitleElems(clientRect);
	createOrShowOrigSqlEditor(clientRect);
	createOrShowExpQueryPlanElems(clientRect);
	createOrShowWhereAnalysisElems(clientRect);

	// onSize will trigger init the v-scrollbar
	CSize size(clientRect.Width(), clientRect.Height());
	initScrollBar(size);
}

void PerfAnalysisPage::createOrShowTitleElems(CRect & clientRect)
{
	int x = 20, y = 15, w = 32, h = 32;
	CRect rect(x, y, x + w, y + h);
	createOrShowImage(titleImage, rect, clientRect);

	rect.OffsetRect(32 + 10, 5);
	rect.right = clientRect.Width() - 100;
	
	std::wstring title = S(L"analysis-text1");
	createOrShowEdit(titleEdit, 0, title, rect, clientRect, 0);

	rect.left = clientRect.Width() - 20 - 60;
	rect.right = rect.left + 60;
	QWinCreater::createOrShowButton(m_hWnd, refreshButton, Config::TABLE_PROPERTIES_REFRESH_BUTTON_ID, S(L"refresh-page"), rect, clientRect);
}

void PerfAnalysisPage::createOrShowOrigSqlEditor(CRect &clientRect)
{
	CRect rcTop = GdiPlusUtil::GetWindowRelativeRect(titleEdit.m_hWnd);
	int x = 20, y = rcTop.bottom + 30, w = 200, h = 24;
	CRect rect(x, y, x + w, y + h);
	QWinCreater::createOrShowLabel(m_hWnd, origSqlLabel, S(L"original-sql").append(L":"), rect, clientRect, SS_LEFT | SS_CENTERIMAGE);

	rect.OffsetRect(0, h + 5);
	rect.right = rect.left + clientRect.Width() - 40;
	rect.bottom = rect.top + 100;
	crateOrShowEditor(origSqlEditor, rect, clientRect);
}

void PerfAnalysisPage::crateOrShowEditor(QSqlEdit &win, CRect &rect, CRect &clientRect)
{
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		win.Create(m_hWnd, rect, 0); // cancel WS_EX_CLIENTEDGE ( 3D )
		win.initEdit(10, _T("Courier New"));
	}else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
		Q_INFO(L"QHelpEdit(editor),rect.w{}:{},rect.h:{}", rect.Width(), rect.Height());
	}
}


void PerfAnalysisPage::createOrShowExpQueryPlanElems(CRect &clientRect)
{
	ExplainQueryPlans queryPlans = supplier.getExplainQueryPlans();
	if (queryPlans.empty()) {
		return;
	}

	CRect rectLast = GdiPlusUtil::GetWindowRelativeRect(origSqlEditor.m_hWnd);
	int x = 20, y = rectLast.bottom + 20, w = w = clientRect.Width() - 40, h = 24;
	CRect rect(x, y, x + w, y + h);
	QWinCreater::createOrShowLabel(m_hWnd, explainQueryPlanLabel, S(L"explain-query-plan").append(L":"), rect, clientRect, SS_LEFT | SS_CENTERIMAGE);

	
	size_t i = 0;
	for (auto & item : queryPlans) {
		rect.OffsetRect(0, h + 10);
		if (expQueryPlanPtrs.size() > i) {
			auto ptr = expQueryPlanPtrs.at(i);
			if (ptr && ptr->IsWindow()) {
				ptr->MoveWindow(rect);
				i++;
				continue;
			}			
		}
		CStatic * ptr = new CStatic();
		QWinCreater::createOrShowLabel(m_hWnd, *ptr, item.detail, rect, clientRect, SS_LEFT | SS_CENTERIMAGE);
		expQueryPlanPtrs.push_back(ptr);
		i++;
	}
}

void PerfAnalysisPage::createOrShowWhereAnalysisElems(CRect &clientRect)
{
	const auto & byteCodeResults = supplier.getByteCodeResults();
	size_t n = byteCodeResults.size();
	if (!n) {
		return;
	}

	bool found = false;
	for (auto & item : byteCodeResults) {
		if (item.type != L"table") {
			continue;
		}
		if (!item.whereColumns.empty()) {
			found = true;
		}
	}
	if (!found) {
		return;
	}

	CRect rectLast;
	if (expQueryPlanPtrs.size()) {
		rectLast = GdiPlusUtil::GetWindowRelativeRect(expQueryPlanPtrs.back()->m_hWnd);
	} else {
		rectLast = GdiPlusUtil::GetWindowRelativeRect(origSqlEditor.m_hWnd);
	}
	int x = 20, y = rectLast.bottom + 20, w = clientRect.Width() - 40, h = 24;
	CRect rect(x, y, x + w, y + h);
	QWinCreater::createOrShowLabel(m_hWnd, whereAnalysisLabel, S(L"where-clause-analysis").append(L":"), rect, clientRect, SS_LEFT | SS_CENTERIMAGE);

	createOrShowWhereAnalysisItemsForTable(clientRect);
}


void PerfAnalysisPage::createOrShowWhereAnalysisItemsForTable(CRect &clientRect)
{
	CRect rcLast = GdiPlusUtil::GetWindowRelativeRect(whereAnalysisLabel.m_hWnd);
	int x = 20, y = rcLast.bottom + 10, w = clientRect.Width() - 40, h = 180;
	CRect rect(x, y, x + w, y + h);

	const ByteCodeResults & byteCodeResults = supplier.getByteCodeResults();
	for (auto & item : byteCodeResults) {
		if (item.type != L"table") {
			continue;
		}

		if (item.whereColumns.empty()) {
			continue;
		}

		auto iter = std::find_if(whereAnalysisElemPtrs.begin(), whereAnalysisElemPtrs.end(), [&item](auto ptr) {
			return ptr->getByteCodeResult().no == item.no;
		});
		
		if (iter != whereAnalysisElemPtrs.end()) {
			createOrShowClauseAnalysisElem(*(*iter), rect, clientRect);
			rect.OffsetRect(0, h + 10);
			continue;
		}
		WhereOrderClauseAnalysisElem * ptr = new WhereOrderClauseAnalysisElem(WHERE_CLAUSE, item);
		createOrShowClauseAnalysisElem(*ptr, rect, clientRect);
		whereAnalysisElemPtrs.push_back(ptr);
		rect.OffsetRect(0, h + 10);		
	}
	
}


void PerfAnalysisPage::createOrShowOrderAnalysisElems(CRect &clientRect)
{
	const auto & byteCodeResults = supplier.getByteCodeResults();
	size_t n = byteCodeResults.size();
	if (!n) {
		return;
	}

	bool found = false;
	for (auto & item : byteCodeResults) {
		if (item.type != L"table") {
			continue;
		}
		if (!item.orderColumns.empty()) {
			found = true;
		}
	}
	if (!found) {
		return;
	}

	CRect rectLast;
	if (expQueryPlanPtrs.size()) {
		rectLast = GdiPlusUtil::GetWindowRelativeRect(expQueryPlanPtrs.back()->m_hWnd);
	} else {
		rectLast = GdiPlusUtil::GetWindowRelativeRect(origSqlEditor.m_hWnd);
	}
	int x = 20, y = rectLast.bottom + 20, w = clientRect.Width() - 40, h = 24;
	CRect rect(x, y, x + w, y + h);
	QWinCreater::createOrShowLabel(m_hWnd, orderAnalysisLabel, S(L"order-clause-analysis").append(L":"), rect, clientRect, SS_LEFT | SS_CENTERIMAGE);

	createOrShowOrderAnalysisItemsForTable(clientRect);
}


void PerfAnalysisPage::createOrShowOrderAnalysisItemsForTable(CRect &clientRect)
{
	CRect rcLast = GdiPlusUtil::GetWindowRelativeRect(orderAnalysisLabel.m_hWnd);
	int x = 20, y = rcLast.bottom + 10, w = clientRect.Width() - 40, h = 180;
	CRect rect(x, y, x + w, y + h);

	const ByteCodeResults & byteCodeResults = supplier.getByteCodeResults();
	for (auto & item : byteCodeResults) {
		if (item.type != L"table") {
			continue;
		}

		if (item.orderColumns.empty()) {
			continue;
		}

		auto iter = std::find_if(orderAnalysisElemPtrs.begin(), orderAnalysisElemPtrs.end(), [&item](auto ptr) {
			return ptr->getByteCodeResult().no == item.no;
		});
		
		if (iter != orderAnalysisElemPtrs.end()) {
			createOrShowClauseAnalysisElem(*(*iter), rect, clientRect);
			rect.OffsetRect(0, h + 10);
			continue;
		}
		WhereOrderClauseAnalysisElem * ptr = new WhereOrderClauseAnalysisElem(ORDER_CLAUSE, item);
		createOrShowClauseAnalysisElem(*ptr, rect, clientRect);
		orderAnalysisElemPtrs.push_back(ptr);
		rect.OffsetRect(0, h + 10);		
	}
}

void PerfAnalysisPage::createOrShowClauseAnalysisElem(WhereOrderClauseAnalysisElem & win, CRect & rect, CRect & clientRect)
{
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN  | WS_CLIPSIBLINGS ;
		win.Create(m_hWnd, rect);
		return;
	} else if (::IsWindow(m_hWnd) && win.IsWindow() && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
	}
}


void PerfAnalysisPage::createOrShowImage(QStaticImage &win, CRect & rect, CRect & clientRect)
{
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_VISIBLE |WS_CLIPCHILDREN  | WS_CLIPSIBLINGS ;
		std::wstring imgDir = ResourceUtil::getProductImagesDir();
		std::wstring imgPath = imgDir + L"analysis\\page\\perf-analysis.png";
		win.load(imgPath.c_str(), BI_PNG, true);
		win.Create(m_hWnd, rect, L"", dwStyle , 0);
		return;
	} else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
	}
}

void PerfAnalysisPage::createOrShowEdit(WTL::CEdit & win, UINT id, std::wstring text, CRect rect, CRect &clientRect, DWORD exStyle /*= 0*/)
{
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_VISIBLE |WS_TABSTOP;
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


void PerfAnalysisPage::clearExpQueryPlanPtrs()
{
	for (auto ptr : expQueryPlanPtrs) {
		if (ptr && ptr->IsWindow()) {
			ptr->DestroyWindow();
		}
		if (ptr) {
			delete ptr;
			ptr = nullptr;
		}
	}
	expQueryPlanPtrs.clear();
}

void PerfAnalysisPage::clearWhereAnalysisElemPtrs()
{
	for (auto ptr : whereAnalysisElemPtrs) {
		if (ptr && ptr->IsWindow()) {
			ptr->DestroyWindow();
		}
		if (ptr) {
			delete ptr;
			ptr = nullptr;
		}
	}
	whereAnalysisElemPtrs.clear();
}

void PerfAnalysisPage::clearOrderAnalysisElemPtrs()
{
	for (auto ptr : orderAnalysisElemPtrs) {
		if (ptr && ptr->IsWindow()) {
			ptr->DestroyWindow();
		}
		if (ptr) {
			delete ptr;
			ptr = nullptr;
		}
	}
	orderAnalysisElemPtrs.clear();
}

void PerfAnalysisPage::loadWindow()
{
	if (!isNeedReload) {
		return;
	}
	isNeedReload = false;
	laodOrigSqlEditor();
}

void PerfAnalysisPage::laodOrigSqlEditor()
{
	origSqlEditor.addText(supplier.getSqlLog().sql);
}

int PerfAnalysisPage::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	bool ret = QPage::OnCreate(lpCreateStruct);
	textFont = FT(L"form-text-size");
	titleFont = FTB(L"properites-title-font", true);
	sectionFont = FTB(L"properites-section-font", true);

	linePen.CreatePen(PS_SOLID, 1, lineColor);
	headerBrush.CreateSolidBrush(headerBkgColor);

	adapter = new PerfAnalysisPageAdapter(m_hWnd, this, &supplier);

	
	return ret;
}

int PerfAnalysisPage::OnDestroy()
{
	bool ret = QPage::OnDestroy();
	if (!titleFont) ::DeleteObject(titleFont);
	if (!textFont) ::DeleteObject(textFont);
	if (!sectionFont) ::DeleteObject(sectionFont); 

	if (!linePen.IsNull()) linePen.DeleteObject();
	if (!headerBrush.IsNull()) headerBrush.DeleteObject();
	

	if (titleImage.IsWindow()) titleImage.DestroyWindow();
	if (titleEdit.IsWindow()) titleEdit.DestroyWindow();
	if (refreshButton.IsWindow()) refreshButton.DestroyWindow();
	if (origSqlLabel.IsWindow()) origSqlLabel.DestroyWindow();
	if (origSqlEditor.IsWindow()) origSqlEditor.DestroyWindow();
	if (newSqlEditor.IsWindow()) newSqlEditor.DestroyWindow();

	if (explainQueryPlanLabel.IsWindow()) explainQueryPlanLabel.DestroyWindow();
	clearExpQueryPlanPtrs();
	if (whereAnalysisLabel.IsWindow()) whereAnalysisLabel.DestroyWindow();
	clearWhereAnalysisElemPtrs();
	if (orderAnalysisLabel.IsWindow()) orderAnalysisLabel.DestroyWindow();
	clearWhereAnalysisElemPtrs();

	if (adapter) delete adapter;	
	return ret;
}


void PerfAnalysisPage::paintItem(CDC & dc, CRect & paintRect)
{
	dc.FillRect(paintRect, bkgColor);
	int nPos = GetScrollPos(SB_VERT);
	int x = 20, y = 15 + 32 + 10 - nPos * cyChar, w = paintRect.Width() - 2 * 20, h = 1;

	HPEN oldPen = dc.SelectPen(linePen);
	dc.MoveTo(x, y);
	dc.LineTo(x + w, y);
	dc.SelectPen(oldPen);

	
}

void PerfAnalysisPage::initScrollBar(CSize & clientSize)
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

LRESULT PerfAnalysisPage::OnVScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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
		::ScrollWindow(m_hWnd, 0, (nHeightSum / 100) * (iVscrollPos - si.nPos), nullptr, nullptr);
		Invalidate(true);
	}
	 return 0;
}


LRESULT PerfAnalysisPage::OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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


LRESULT PerfAnalysisPage::OnClickRefreshButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	nHeightSum = 0;

	CRect clientRect;
	GetClientRect(clientRect);
	
	si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS; 
	si.nMin = 0; // must set si.fMask = SIF_RANGE
	si.nMax = 100; // must set si.fMask = SIF_RANGE
	si.nPos = 0; // must set si.fMask = SIF_POS

	// must set si.fMask = SIF_PAGE
	si.nPage = 1;
	vScrollPages = si.nPage;
	::SetScrollInfo(m_hWnd, SB_VERT, &si, TRUE);

	createOrShowUI();
	Invalidate(true);
	return 0;
}

HBRUSH PerfAnalysisPage::OnCtlStaticColor(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, bkgColor);
	if (hwnd == origSqlLabel.m_hWnd) {
		::SetTextColor(hdc, sectionColor); 
		::SelectObject(hdc, sectionFont);
	}else if ((explainQueryPlanLabel.IsWindow() && hwnd == explainQueryPlanLabel.m_hWnd)
		|| (whereAnalysisLabel.IsWindow() && hwnd == whereAnalysisLabel.m_hWnd)) {
		::SetTextColor(hdc, sectionColor); 
		::SelectObject(hdc, sectionFont);
	}  else {
		::SetTextColor(hdc, textColor); 
		::SelectObject(hdc, textFont);
	}
	
	return bkgBrush.m_hBrush;
}

HBRUSH PerfAnalysisPage::OnCtlBtnColor(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, btnBkgColor);
	::SetTextColor(hdc, textColor); 
	::SelectObject(hdc, textFont);
	return bkgBrush.m_hBrush;
}
