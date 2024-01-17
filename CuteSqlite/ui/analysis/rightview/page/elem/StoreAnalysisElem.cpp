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

 * @file   WhereAnalysisTableIdxElem.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-12-08
 *********************************************************************/
#include "stdafx.h"
#include "StoreAnalysisElem.h"
#include "core/common/Lang.h"
#include "ui/common/QWinCreater.h"
#include "utils/StringUtil.h"
#include "ui/common/message/QPopAnimate.h"
#include "common/AppContext.h"

StoreAnalysisElem::StoreAnalysisElem(const std::wstring & _title, const StoreAnalysisItems & _storeAnalysisItems)
	: title(_title), storeAnalysisItems(_storeAnalysisItems)
{

}

StoreAnalysisElem::~StoreAnalysisElem()
{
	m_hWnd = nullptr;
}


void StoreAnalysisElem::refreshHorizBars()
{
	for (auto & ptr : valPtrs) {
		ptr->Invalidate(true);
	}
}

int StoreAnalysisElem::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	bkgBrush.CreateSolidBrush(bkgColor);
	textFont = FT(L"form-text-size");
	sectionFont = FTB(L"properites-section-font", true);
	return 0;
}

void StoreAnalysisElem::OnDestroy()
{
	if (!bkgBrush.IsNull()) bkgBrush.DeleteObject();
	if (textFont) ::DeleteObject(textFont);
	if (sectionFont) ::DeleteObject(sectionFont);

	if (titleLabel.IsWindow()) titleLabel.DestroyWindow();

	clearLabelPtrs();
	clearValPtrs();
}

void StoreAnalysisElem::OnSize(UINT nType, CSize size)
{
	if (size.cx == 0 || size.cy == 0) {
		return;
	}
	createOrShowUI();
}

void StoreAnalysisElem::OnShowWindow(BOOL bShow, UINT nStatus)
{

}

void StoreAnalysisElem::OnPaint(CDCHandle dc)
{
	CPaintDC pdc(m_hWnd);
	CMemoryDC mdc(pdc, pdc.m_ps.rcPaint);
	mdc.FillRect(&pdc.m_ps.rcPaint, bkgBrush.m_hBrush);
}

BOOL StoreAnalysisElem::OnEraseBkgnd(CDCHandle dc)
{
	return true;
}

HBRUSH StoreAnalysisElem::OnCtlStaticColor(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, bkgColor);
	if (titleLabel.m_hWnd == hwnd) {
		::SetTextColor(hdc, sectionColor); 
		::SelectObject(hdc, sectionFont);
	} else {
		::SetTextColor(hdc, textColor); 
		::SelectObject(hdc, textFont);
	}
	
	return bkgBrush.m_hBrush;
}

HBRUSH StoreAnalysisElem::OnCtlBtnColor(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, btnBkgColor);
	::SetTextColor(hdc, textColor); 
	::SelectObject(hdc, textFont);
	return bkgBrush.m_hBrush;
}


void StoreAnalysisElem::createOrShowUI()
{
	CRect clientRect;
	GetClientRect(clientRect);

	createOrShowLabels(clientRect);
	createOrShowElems(clientRect);
}

void StoreAnalysisElem::createOrShowLabels(CRect & clientRect)
{
	int x = 0, y = 0, w = 500, h = 20;
	CRect rect(x, y, x + w, y + h);	
	QWinCreater::createOrShowLabel(m_hWnd, titleLabel, getTitle(), rect, clientRect, SS_LEFT | SS_CENTERIMAGE);

	
}

void StoreAnalysisElem::createOrShowElems(CRect & clientRect)
{
	CRect rcLast = GdiPlusUtil::GetWindowRelativeRect(titleLabel);
	CRect rcLabel = rcLast;
	rcLabel.right = rcLabel.left + 300;
	CRect rcBar = rcLast;
	rcBar.OffsetRect(rcLabel.Width() + 5, 0);
	rcBar.right = rcBar.left + 500;

	size_t n = storeAnalysisItems.size();
	size_t labelLen = labelPtrs.size();
	for (size_t i = 0; i < n; ++i) {
		rcLabel.OffsetRect(0, rcLabel.Height() + 5);
		rcBar.OffsetRect(0, rcBar.Height() + 5);
		auto & item = storeAnalysisItems.at(i);
		CStatic * labelPtr = nullptr;
		QHorizontalBar * valPtr = nullptr;
		if (i + 1 < labelLen) {
			labelPtr = labelPtrs.at(i);
			valPtr = valPtrs.at(i);
		} else {
			labelPtr = new CStatic();
			valPtr = new QHorizontalBar(item.val, item.percent, item.color);
			labelPtrs.push_back(labelPtr);
			valPtrs.push_back(valPtr);
		}

		QWinCreater::createOrShowLabel(m_hWnd, *labelPtr, item.name, rcLabel, clientRect, SS_RIGHT | SS_CENTERIMAGE);
		createOrShowHorizontalBar(*valPtr, rcBar, clientRect);
	}
}

void StoreAnalysisElem::createOrShowHorizontalBar(QHorizontalBar &win, CRect & rect, CRect clientRect)
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

void StoreAnalysisElem::clearLabelPtrs()
{
	for (auto ptr : labelPtrs) {
		if (ptr && ptr->IsWindow()) {
			ptr->DestroyWindow();
			ptr->m_hWnd = nullptr;
		}
		if (ptr) {
			delete ptr;
			ptr = nullptr;
		}
	}
	labelPtrs.clear();
}

void StoreAnalysisElem::clearValPtrs()
{
	for (auto ptr : valPtrs) {
		if (ptr && ptr->IsWindow()) {
			ptr->DestroyWindow();
			ptr->m_hWnd = nullptr;
		}
		if (ptr) {
			delete ptr;
			ptr = nullptr;
		}
	}
	valPtrs.clear();
}
