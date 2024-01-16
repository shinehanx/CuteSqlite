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
#include "SelectColumnsAnalysisElem.h"
#include "core/common/Lang.h"
#include "ui/common/QWinCreater.h"
#include "utils/StringUtil.h"
#include "ui/common/message/QPopAnimate.h"
#include "common/AppContext.h"

SelectColumnsAnalysisElem::SelectColumnsAnalysisElem(const SelectColumns & _selectColumns)
	: selectColumns(_selectColumns)
{
	
}


SelectColumnsAnalysisElem::~SelectColumnsAnalysisElem()
{
	m_hWnd = nullptr;
}

const Columns SelectColumnsAnalysisElem::getSelectedColumns()
{
	Columns selectedColumns;
	for (auto & ptr : selectColumnCheckBoxPtrs) {
		if (!ptr->GetCheck()) {
			continue;
		}
		CString str;
		ptr->GetWindowText(str);
		if (str.IsEmpty()) {
			continue;
		}

		selectedColumns.push_back(str.GetString());
	}

	return selectedColumns;
}

int SelectColumnsAnalysisElem::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	bkgBrush.CreateSolidBrush(bkgColor);
	textFont = FT(L"form-text-size");
	return 0;
}

void SelectColumnsAnalysisElem::OnDestroy()
{
	if (!bkgBrush.IsNull()) bkgBrush.DeleteObject();
	if (textFont) ::DeleteObject(textFont);

	if (useColsLabel.IsWindow()) useColsLabel.DestroyWindow();
	if (useColsCountLabel.IsWindow()) useColsCountLabel.DestroyWindow();
	if (adviceLabel.IsWindow()) adviceLabel.DestroyWindow();

	clearSelectColumnCheckBoxPtrs();
}

void SelectColumnsAnalysisElem::OnSize(UINT nType, CSize size)
{
	if (size.cx == 0 || size.cy == 0) {
		return;
	}
	createOrShowUI();
}

void SelectColumnsAnalysisElem::OnShowWindow(BOOL bShow, UINT nStatus)
{

}

void SelectColumnsAnalysisElem::OnPaint(CDCHandle dc)
{
	CPaintDC pdc(m_hWnd);
	pdc.FillRect(&pdc.m_ps.rcPaint, bkgBrush.m_hBrush);
}

BOOL SelectColumnsAnalysisElem::OnEraseBkgnd(CDCHandle dc)
{
	return true;
}

HBRUSH SelectColumnsAnalysisElem::OnCtlStaticColor(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, bkgColor);	
	::SelectObject(hdc, textFont);
	if (adviceLabel,IsWindow() && hwnd == adviceLabel.m_hWnd) {
		::SetTextColor(hdc, hintColor);
	} else {
		::SetTextColor(hdc,textColor);
	}	
	return bkgBrush.m_hBrush;
}


void SelectColumnsAnalysisElem::createOrShowUI()
{
	CRect clientRect;
	GetClientRect(clientRect);

	createOrShowLabels(clientRect);
	createOrShowCheckBoxes(clientRect);
	createOrShowColumnCountElems(clientRect);
}

void SelectColumnsAnalysisElem::createOrShowLabels(CRect & clientRect)
{
	std::wstring text1 = S(L"select-use-column").append(L":");
	int x = 0, y = 0, w = clientRect.Width() - 10, h = 20;
	CRect rect(x, y, x + w, y + h);
	
	QWinCreater::createOrShowLabel(m_hWnd, useColsLabel, text1, rect, clientRect, SS_LEFT | SS_CENTERIMAGE);
}

void SelectColumnsAnalysisElem::createOrShowCheckBoxes(CRect & clientRect)
{
	CRect rcLast = GdiPlusUtil::GetWindowRelativeRect(useColsLabel.m_hWnd);
	int x = 10, y = rcLast.bottom + 5, w = 250, h = 20;
	CRect rect(x, y, x + w, y + h);
	int i = 0;
	for (auto & column : selectColumns) {
		if (i > 0) {
			if (rect.right + w > clientRect.right) {
				rect.left = x;
				rect.top = rect.top + h + 10;
				rect.right = rect.left + w;
				rect.bottom = rect.top + h;
			} else {
				rect.OffsetRect(w + 10, 0);
			}			
		}
		auto iter = std::find_if(selectColumnCheckBoxPtrs.begin(), selectColumnCheckBoxPtrs.end(), [&column](const auto & ptr) {
			if (!ptr || !ptr->IsWindow()) {
				return false;
			}
			CString str;
			ptr->GetWindowText(str);
			return column.fullName == str.GetString();
		});

		if (iter != selectColumnCheckBoxPtrs.end()) {
			QWinCreater::createOrShowCheckBox(m_hWnd, **iter, Config::ANALYSIS_SELECT_COLUMN_CHECKBOX_ID_START + (i++), column.fullName, rect, clientRect);	
			continue;
		}
		CButton * columnCheckBoxPtr = new CButton();
		QWinCreater::createOrShowCheckBox(m_hWnd, *columnCheckBoxPtr,  Config::ANALYSIS_SELECT_COLUMN_CHECKBOX_ID_START + (i++), column.fullName, rect, clientRect);
		columnCheckBoxPtr->SetCheck(1);
		selectColumnCheckBoxPtrs.push_back(columnCheckBoxPtr);
	}
	
}


void SelectColumnsAnalysisElem::createOrShowColumnCountElems(CRect & clientRect)
{
	std::wstring text2 = S(L"select-use-column-count").append(L": ");
	std::wstring text3 = S(L"select-use-column-advice");
	std::wstring text4 = S(L"select-use-column-tips");

	CRect rcLast = GdiPlusUtil::GetWindowRelativeRect(selectColumnCheckBoxPtrs.back()->m_hWnd);
	
	int x = 0, y = rcLast.bottom + 5, w = clientRect.Width() - 10, h = 20;
	CRect rect(x, y, x + w, y + h);
	size_t n = selectColumns.size();
	text2.append(std::to_wstring(n));
	QWinCreater::createOrShowLabel(m_hWnd, useColsCountLabel, text2, rect, clientRect, SS_LEFT | SS_CENTERIMAGE);

	std::wstring val = settingService->getSysInit(L"max-select-columns");
	size_t maxSelectColumns = val.empty() ? MAX_ADVICE_COLUMNS_COUNT : std::stoul(val);
	std::wstring adviceText;
	if (maxSelectColumns > n) {
		adviceText = text4;
		hintColor = RGB(8, 92, 14);
	} else {
		adviceText = StringUtil::replace(text3, L"{maxColumns}", std::to_wstring(maxSelectColumns));
		hintColor = RGB(71, 109, 208);
	}

	rect.OffsetRect(0, h + 5);		
	QWinCreater::createOrShowLabel(m_hWnd, adviceLabel, adviceText, rect, clientRect, SS_LEFT | SS_CENTERIMAGE);
}

void SelectColumnsAnalysisElem::clearSelectColumnCheckBoxPtrs()
{
	for (auto ptr : selectColumnCheckBoxPtrs) {
		if (ptr && ptr->IsWindow()) {
			ptr->DestroyWindow();
			ptr->m_hWnd = nullptr;
		}
		if (ptr) {
			delete ptr;
			ptr = nullptr;
		}
	}
	selectColumnCheckBoxPtrs.clear();
}


