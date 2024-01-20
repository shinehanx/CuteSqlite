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

 * @file   DbPragmaParamsPage.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2024-01-19
 *********************************************************************/
#include "stdafx.h"
#include "DbPragmaParamsPage.h"
#include "core/common/Lang.h"
#include "utils/ResourceUtil.h"
#include "ui/common/QWinCreater.h"
#include "utils/StringUtil.h"
#include "common/AppContext.h"
#include "utils/ColumnsUtil.h"
#include "utils/SqlUtil.h"
#include "core/common/exception/QSqlExecuteException.h"
#include "ui/common/message/QPopAnimate.h"


BOOL DbPragmaParamsPage::PreTranslateMessage(MSG* pMsg)
{
	return FALSE;
}

DbPragmaParamsPage::DbPragmaParamsPage(uint64_t userDbId)
{
	supplier.setRuntimeUserDbId(userDbId);
	supplier.setUserDb(databaseService->getUserDb(userDbId));
}


uint64_t DbPragmaParamsPage::getUserDbId()
{
	return supplier.getRuntimeUserDbId();
}


void DbPragmaParamsPage::createOrShowUI()
{
	CRect clientRect;
	GetClientRect(clientRect);
	nHeightSum = 0;

	createOrShowTitleElems(clientRect);
	createOrShowPragmaParamElems(clientRect);

	// onSize will trigger init the v-scrollbar
	CSize size(clientRect.Width(), clientRect.Height());
	initScrollBar(size);
}

void DbPragmaParamsPage::createOrShowTitleElems(CRect & clientRect)
{
	int x = 20, y = 15, w = 32, h = 32;
	CRect rect(x, y, x + w, y + h);
	createOrShowImage(titleImage, rect, clientRect);

	rect.OffsetRect(w + 10, 5);
	rect.right = rect.left + 500;
	
	std::wstring title = S(L"db-pragma-params-text");
	createOrShowEdit(titleEdit, 0, title, rect, clientRect, 0);

	rect.OffsetRect(500 + 10, 0);
	rect.right = rect.left + 60;
	QWinCreater::createOrShowLabel(m_hWnd, databaseLabel, S(L"database").append(L":"), rect, clientRect, SS_RIGHT);

	rect.OffsetRect(60 + 5, 0);
	rect.right = rect.left + 80;
	title = supplier.getUserDb().name;
	createOrShowEdit(databaseEdit, 0, title, rect, clientRect, 0);

	rect.OffsetRect(80 + 5, 0);
	rect.right = rect.left + 100;
	QWinCreater::createOrShowLabel(m_hWnd, pragmaLabel, S(L"select-pragma"), rect, clientRect, SS_RIGHT);

	rect.OffsetRect(100 + 5, -2);
	rect.right = rect.left + 150;
	QWinCreater::createOrShowComboBox(m_hWnd, pragmaComboBox, Config::ANALYSIS_DB_STORE_TBL_COMBOBOX_ID, rect, clientRect, SS_RIGHT);

	rect.left = clientRect.Width() - 20 - 60;
	rect.right = rect.left + 60;
	QWinCreater::createOrShowButton(m_hWnd, refreshButton, Config::TABLE_PROPERTIES_REFRESH_BUTTON_ID, S(L"refresh-page"), rect, clientRect);
	nHeightSum = rect.bottom;
}


void DbPragmaParamsPage::createOrShowPragmaParamElems(CRect & clientRect)
{
	CRect rcLast = GdiPlusUtil::GetWindowRelativeRect(titleEdit);
	int x = 20, y = rcLast.bottom + 50, w = clientRect.Width() - 40 < 800 ? 800 : clientRect.Width() - 40, h = 25;
	CRect rect(x, y, x + w, y + h);
	auto dbPragmaParams = adapter->getDbPragmaParams(supplier.getRuntimeUserDbId());
	for (auto & param : dbPragmaParams) {
		auto iter = std::find_if(paramElemPtrs.begin(), paramElemPtrs.end(), [&param](auto & ptr) {
			return ptr->getData().labelText == param.labelText;
		});
		QParamElem * ptr = nullptr;
		if (iter != paramElemPtrs.end()) {
			ptr = *iter;
		} else {
			ptr = new QParamElem(param);
			ptr->setBkgColor(bkgColor);
			paramElemPtrs.push_back(ptr);
		}
		createOrShowPragmaParamElem(*ptr, rect, clientRect);
		rect.OffsetRect(0, h + 5);
	}

	nHeightSum = rect.bottom + 300;
}


void DbPragmaParamsPage::createOrShowImage(QStaticImage &win, CRect & rect, CRect & clientRect)
{
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_VISIBLE |WS_CLIPCHILDREN  | WS_CLIPSIBLINGS ;
		std::wstring imgDir = ResourceUtil::getProductImagesDir();
		std::wstring imgPath = imgDir + L"analysis\\page\\db-pragma-params.png";
		win.load(imgPath.c_str(), BI_PNG, true);
		win.Create(m_hWnd, rect, L"", dwStyle , 0);
		return;
	} else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
	}
}

void DbPragmaParamsPage::createOrShowEdit(WTL::CEdit & win, UINT id, std::wstring text, CRect rect, CRect &clientRect, DWORD exStyle /*= 0*/)
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


void DbPragmaParamsPage::createOrShowPragmaParamElem(QParamElem &win, CRect & rect, CRect clientRect)
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

void DbPragmaParamsPage::loadWindow()
{
	if (!isNeedReload) {
		return;
	}
	isNeedReload = false;
	loadPragmaComboBox();
}


void DbPragmaParamsPage::loadPragmaComboBox()
{
	pragmaComboBox.ResetContent();
	pragmaComboBox.AddString(L"All");

	uint64_t userDbId = supplier.getRuntimeUserDbId();
	
	std::vector<std::wstring> strs;

	for (auto tbl : strs) {
		pragmaComboBox.AddString(tbl.c_str());
	}
	pragmaComboBox.SetCurSel(0);
}

int DbPragmaParamsPage::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	bool ret = QPage::OnCreate(lpCreateStruct);
	textFont = FT(L"form-text-size");
	titleFont = FTB(L"properites-title-font", true);
	sectionFont = FTB(L"properites-section-font", true);
	comboFont = FT(L"combobox-size");

	linePen.CreatePen(PS_SOLID, 1, lineColor);
	headerBrush.CreateSolidBrush(headerBkgColor);

	adapter = new DbPragmaParamPageAdapter(m_hWnd, this, &supplier);

	
	return ret;
}

int DbPragmaParamsPage::OnDestroy()
{
	bool ret = QPage::OnDestroy();
	if (!titleFont) ::DeleteObject(titleFont);
	if (!textFont) ::DeleteObject(textFont);
	if (!sectionFont) ::DeleteObject(sectionFont); 
	if (!comboFont) ::DeleteObject(comboFont); 

	if (!linePen.IsNull()) linePen.DeleteObject();
	if (!headerBrush.IsNull()) headerBrush.DeleteObject();
	

	if (titleImage.IsWindow()) titleImage.DestroyWindow();
	if (titleEdit.IsWindow()) titleEdit.DestroyWindow();
	if (databaseLabel.IsWindow()) databaseLabel.DestroyWindow();
	if (databaseEdit.IsWindow()) databaseEdit.DestroyWindow();
	if (pragmaLabel.IsWindow()) pragmaLabel.DestroyWindow();
	if (pragmaComboBox.IsWindow()) pragmaComboBox.DestroyWindow();
	if (refreshButton.IsWindow()) refreshButton.DestroyWindow();
	
	void clearParamElemPtrs();

	if (adapter) delete adapter;
	return ret;
}


void DbPragmaParamsPage::paintItem(CDC & dc, CRect & paintRect)
{
	dc.FillRect(paintRect, bkgColor);
	int nPos = GetScrollPos(SB_VERT);
	int x = 20, y = 15 + 32 + 10 - nPos * cyChar, w = paintRect.Width() - 2 * 20, h = 1;

	HPEN oldPen = dc.SelectPen(linePen);
	dc.MoveTo(x, y);
	dc.LineTo(x + w, y);
	dc.SelectPen(oldPen);	
}

void DbPragmaParamsPage::initScrollBar(CSize & clientSize)
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

LRESULT DbPragmaParamsPage::OnVScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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


LRESULT DbPragmaParamsPage::OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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



LRESULT DbPragmaParamsPage::OnClickRefreshButton(UINT uNotifyCode, int nID, HWND hwnd)
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


LRESULT DbPragmaParamsPage::OnChangePragmaComboBox(UINT uNotifyCode, int nID, HWND hwnd)
{
	clearParamElemPtrs();
	createOrShowUI();
	return 0;
}

void DbPragmaParamsPage::clearParamElemPtrs()
{
	for (auto ptr : paramElemPtrs) {
		if (ptr && ptr->IsWindow()) {
			ptr->DestroyWindow();
			ptr->m_hWnd = nullptr;
		}
		if (ptr) {
			delete ptr;
			ptr = nullptr;
		}
	}
	paramElemPtrs.clear();
}

HBRUSH DbPragmaParamsPage::OnCtlColorStatic(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, bkgColor);
	
	::SetTextColor(hdc, textColor); 
	::SelectObject(hdc, textFont);

	return bkgBrush.m_hBrush;
}

HBRUSH DbPragmaParamsPage::OnCtlColorBtn(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, btnBkgColor);
	::SetTextColor(hdc, textColor); 
	::SelectObject(hdc, textFont);
	return bkgBrush.m_hBrush;
}

HBRUSH DbPragmaParamsPage::OnCtlColorListBox(HDC hdc, HWND hwnd)
{
	::SetTextColor(hdc, textColor); // Text area foreground color
	::SetBkColor(hdc, bkgColor); // Text area background color
	::SelectObject(hdc, comboFont);
	return AtlGetStockBrush(WHITE_BRUSH);
}

