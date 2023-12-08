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
#include "WhereAnalysisTableIdxElem.h"
#include "core/common/Lang.h"
#include "ui/common/QWinCreater.h"

WhereAnalysisTableIdxElem::WhereAnalysisTableIdxElem(const TableIndexAnalysis & _tableIndexAnalysis)
	: tableIndexAnalysis(_tableIndexAnalysis)
{
	
}

int WhereAnalysisTableIdxElem::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	bkgBrush.CreateSolidBrush(bkgColor);
	textFont = FT(L"form-text-size");
	return 0;
}

void WhereAnalysisTableIdxElem::OnDestroy()
{
	if (!bkgBrush.IsNull()) bkgBrush.DeleteObject();
	if (textFont) ::DeleteObject(textFont);

	if (tableColumnLabel.IsWindow()) tableColumnLabel.DestroyWindow();
	if (createIdxForPerfLabel.IsWindow()) createIdxForPerfLabel.DestroyWindow();
	if (createIdxTogetherButton.IsWindow()) createIdxTogetherButton.DestroyWindow();
	if (createIdxIndividButton.IsWindow()) createIdxIndividButton.DestroyWindow();

	clearTableColumnCheckBoxPtrs();
}

void WhereAnalysisTableIdxElem::OnSize(UINT nType, CSize size)
{
	if (size.cx == 0 || size.cy == 0) {
		return;
	}
	createOrShowUI();
}

void WhereAnalysisTableIdxElem::OnShowWindow(BOOL bShow, UINT nStatus)
{

}

void WhereAnalysisTableIdxElem::OnPaint(CDCHandle dc)
{
	CPaintDC pdc(m_hWnd);
	pdc.FillRect(&pdc.m_ps.rcPaint, bkgBrush.m_hBrush);
}

BOOL WhereAnalysisTableIdxElem::OnEraseBkgnd(CDCHandle dc)
{
	return true;
}

HBRUSH WhereAnalysisTableIdxElem::OnCtlStaticColor(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, bkgColor);	
	::SetTextColor(hdc, textColor); 
	::SelectObject(hdc, textFont);
	
	return bkgBrush.m_hBrush;
}

HBRUSH WhereAnalysisTableIdxElem::OnCtlBtnColor(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, btnBkgColor);
	::SetTextColor(hdc, textColor); 
	::SelectObject(hdc, textFont);
	return bkgBrush.m_hBrush;
}

void WhereAnalysisTableIdxElem::createOrShowUI()
{
	CRect clientRect;
	GetClientRect(clientRect);

	createOrShowLabels(clientRect);
	createOrShowCheckBoxes(clientRect);
	createOrShowButtons(clientRect);
}

void WhereAnalysisTableIdxElem::createOrShowLabels(CRect & clientRect)
{
	std::wstring text1 = S(L"where-use-table-column");
	std::wstring text2 = S(L"create-index-for-performance");

	int x = 5, y = 5, w = clientRect.Width() - 10, h = 20;
	CRect rect(x, y, x + w, y + h);
	QWinCreater::createOrShowLabel(m_hWnd, tableColumnLabel, text1, rect, clientRect, SS_LEFT | SS_CENTERIMAGE);

	rect.OffsetRect(0, h + 5);
	QWinCreater::createOrShowLabel(m_hWnd, createIdxForPerfLabel, text2, rect, clientRect, SS_LEFT | SS_CENTERIMAGE);
}

void WhereAnalysisTableIdxElem::createOrShowCheckBoxes(CRect & clientRect)
{
	CRect rcLast = GdiPlusUtil::GetWindowRelativeRect(createIdxForPerfLabel.m_hWnd);
	int x = 10, y = rcLast.bottom + 5, w = 150, h = 20;
	CRect rect(x, y, x + w, y + h);
	CButton * columnCheckBoxPtr = new CButton();
	std::wstring text = L"Column1";
	QWinCreater::createOrShowCheckBox(m_hWnd, *columnCheckBoxPtr, 0, text, rect, clientRect);
	tableColumnCheckBoxPtrs.push_back(columnCheckBoxPtr);
}


void WhereAnalysisTableIdxElem::createOrShowButtons(CRect & clientRect)
{
	CRect rcLast = GdiPlusUtil::GetWindowRelativeRect(tableColumnCheckBoxPtrs.back()->m_hWnd);
	int x = clientRect.Width() - 20 - 180, y = rcLast.bottom + 5, w = 180, h = 30;
	CRect rect(x, y, x + w, y + h);
	createOrShowButton(m_hWnd, createIdxIndividButton, Config::ANALYSIS_CREATE_INDEX_INDIVID_BUTTON_ID, S(L"create-index-individ"), rect, clientRect);
	
	rect.OffsetRect(- w - 10, 0);
	createOrShowButton(m_hWnd, createIdxTogetherButton, Config::ANALYSIS_CREATE_INDEX_TOGETHER_BUTTON_ID, S(L"create-index-togeger"), rect, clientRect);
}

void WhereAnalysisTableIdxElem::createOrShowButton(HWND hwnd, CButton & win, UINT id, std::wstring text, CRect rect, CRect &clientRect, DWORD exStyle)
{
	if (::IsWindow(hwnd) && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN  | WS_CLIPSIBLINGS ; 
		if (exStyle) dwStyle = dwStyle | exStyle;
		win.Create(hwnd, rect, text.c_str(), dwStyle , 0, id);
		return ;
	} else if (::IsWindow(hwnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
	}
}

void WhereAnalysisTableIdxElem::clearTableColumnCheckBoxPtrs()
{
	for (auto ptr : tableColumnCheckBoxPtrs) {
		if (ptr && ptr->IsWindow()) {
			ptr->DestroyWindow();
		}
		if (ptr) {
			//delete ptr;
			//ptr = nullptr;
		}
	}
	tableColumnCheckBoxPtrs.clear();
}
