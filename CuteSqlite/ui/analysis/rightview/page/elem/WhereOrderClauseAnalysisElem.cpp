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
#include "WhereOrderClauseAnalysisElem.h"
#include "core/common/Lang.h"
#include "ui/common/QWinCreater.h"
#include "utils/StringUtil.h"

WhereOrderClauseAnalysisElem::WhereOrderClauseAnalysisElem(SqlClauseType _clauseType, const ByteCodeResult & _byteCodeResult)
	: clauseType(_clauseType), byteCodeResult(_byteCodeResult)
{
	
}

int WhereOrderClauseAnalysisElem::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	bkgBrush.CreateSolidBrush(bkgColor);
	textFont = FT(L"form-text-size");
	return 0;
}

void WhereOrderClauseAnalysisElem::OnDestroy()
{
	if (!bkgBrush.IsNull()) bkgBrush.DeleteObject();
	if (textFont) ::DeleteObject(textFont);

	if (tableLabel.IsWindow()) tableLabel.DestroyWindow();
	if (useColsLabel.IsWindow()) useColsLabel.DestroyWindow();
	if (useIdxLabel.IsWindow()) useIdxLabel.DestroyWindow();
	if (createIdxForPerfLabel.IsWindow()) createIdxForPerfLabel.DestroyWindow();
	if (createIdxTogetherButton.IsWindow()) createIdxTogetherButton.DestroyWindow();
	if (createIdxIndividButton.IsWindow()) createIdxIndividButton.DestroyWindow();

	clearTableColumnCheckBoxPtrs();
}

void WhereOrderClauseAnalysisElem::OnSize(UINT nType, CSize size)
{
	if (size.cx == 0 || size.cy == 0) {
		return;
	}
	createOrShowUI();
}

void WhereOrderClauseAnalysisElem::OnShowWindow(BOOL bShow, UINT nStatus)
{

}

void WhereOrderClauseAnalysisElem::OnPaint(CDCHandle dc)
{
	CPaintDC pdc(m_hWnd);
	pdc.FillRect(&pdc.m_ps.rcPaint, bkgBrush.m_hBrush);
}

BOOL WhereOrderClauseAnalysisElem::OnEraseBkgnd(CDCHandle dc)
{
	return true;
}

HBRUSH WhereOrderClauseAnalysisElem::OnCtlStaticColor(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, bkgColor);	
	::SetTextColor(hdc, textColor); 
	::SelectObject(hdc, textFont);
	
	return bkgBrush.m_hBrush;
}

HBRUSH WhereOrderClauseAnalysisElem::OnCtlBtnColor(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, btnBkgColor);
	::SetTextColor(hdc, textColor); 
	::SelectObject(hdc, textFont);
	return bkgBrush.m_hBrush;
}


bool WhereOrderClauseAnalysisElem::isEqualColumns(std::vector<std::wstring> cluaseColumns, std::vector<std::pair<int, std::wstring>> indexColumns)
{
	if (cluaseColumns.size() != indexColumns.size()) {
		return false;
	}
	size_t n = cluaseColumns.size();
	for (size_t i = 0; i < n; i++) {
		auto & columnName = cluaseColumns.at(i);
		auto iter = std::find_if(indexColumns.begin(), indexColumns.end(), [&columnName](const auto & idx) {
			return columnName == idx.second;
		});
		if (iter == indexColumns.end()) {
			return false;
		}
	}
	return true;
}

void WhereOrderClauseAnalysisElem::createOrShowUI()
{
	CRect clientRect;
	GetClientRect(clientRect);

	createOrShowLabels(clientRect);
	createOrShowCheckBoxes(clientRect);
	createOrShowButtons(clientRect);
}

void WhereOrderClauseAnalysisElem::createOrShowLabels(CRect & clientRect)
{
	std::wstring text1 = S(L"in-table");
	std::wstring text2 = clauseType == WHERE_CLAUSE ? S(L"where-use-column") : S(L"order-use-column");
	std::wstring text3 = S(L"use-index");
	std::wstring text4 = S(L"create-index-for-performance");

	int x = 5, y = 0, w = clientRect.Width() - 10, h = 20;
	CRect rect(x, y, x + w, y + h);
	std::wstring tblName = L"\"" + byteCodeResult.name + L"\"";
	text1.append(L": ").append(tblName);
	QWinCreater::createOrShowLabel(m_hWnd, tableLabel, text1, rect, clientRect, SS_LEFT | SS_CENTERIMAGE);

	rect.OffsetRect(0, h + 5);
	std::wstring columns = L"NONE";
	auto & clauseColumns = clauseType == WHERE_CLAUSE ? byteCodeResult.whereColumns : byteCodeResult.orderColumns;
	if (!clauseColumns.empty()) {
		columns = StringUtil::implode(clauseColumns, L",");
		columns = StringUtil::addSymbolToWords(columns, L",", L"\"");
	}
	text2.append(L": ").append(columns);
	QWinCreater::createOrShowLabel(m_hWnd, useColsLabel, text2, rect, clientRect, SS_LEFT | SS_CENTERIMAGE);

	text3.append(L":");
	if (!byteCodeResult.useIndexes.empty()) {		
		std::wstring idxText;
		int i = 0,j = 0;
		for (auto & item : byteCodeResult.useIndexes) {
			if (i++)  idxText.append(L","); 
			idxText.append(L"\"").append(item.second).append(L"\" (using: ");
			j = 0;
			for (auto &col : byteCodeResult.indexColumns) {
				if (col.first != item.first)  continue;
				if (j++)  idxText.append(L",");			
				idxText.append(L"\"").append(col.second).append(L"\"");				
			}
			idxText.append(L")");
		}
		text3.append(idxText);
	} else {
		text3.append(L"NONE");
	}
	rect.OffsetRect(0, h + 5);
	QWinCreater::createOrShowLabel(m_hWnd, useIdxLabel, text3, rect, clientRect, SS_LEFT | SS_CENTERIMAGE);

	if (!isEqualColumns(clauseColumns, byteCodeResult.indexColumns)) {
		rect.OffsetRect(0, h + 5);
		QWinCreater::createOrShowLabel(m_hWnd, createIdxForPerfLabel, text4, rect, clientRect, SS_LEFT | SS_CENTERIMAGE);
	}
}

void WhereOrderClauseAnalysisElem::createOrShowCheckBoxes(CRect & clientRect)
{
	auto & clauseColumns = clauseType == WHERE_CLAUSE ? byteCodeResult.whereColumns : byteCodeResult.orderColumns;
	if (isEqualColumns(clauseColumns, byteCodeResult.indexColumns)) {
		return ;
	}
	CRect rcLast = GdiPlusUtil::GetWindowRelativeRect(createIdxForPerfLabel.m_hWnd);
	int x = 10, y = rcLast.bottom + 5, w = 150, h = 20;
	CRect rect(x, y, x + w, y + h);
	for (auto & column : clauseColumns) {
		auto iter = std::find_if(tableColumnCheckBoxPtrs.begin(), tableColumnCheckBoxPtrs.end(), [&column](const auto & ptr) {
			if (!ptr || !ptr->IsWindow()) {
				return false;
			}
			CString str;
			ptr->GetWindowText(str);
			return column == str.GetString();
		});

		if (iter != tableColumnCheckBoxPtrs.end()) {
			QWinCreater::createOrShowCheckBox(m_hWnd, **iter, 0, column, rect, clientRect);
			rect.OffsetRect(w + 10, 0);
			continue;
		}
		CButton * columnCheckBoxPtr = new CButton();
		QWinCreater::createOrShowCheckBox(m_hWnd, *columnCheckBoxPtr, 0, column, rect, clientRect);
		tableColumnCheckBoxPtrs.push_back(columnCheckBoxPtr);
		rect.OffsetRect(w + 10, 0);
	}
	
}


void WhereOrderClauseAnalysisElem::createOrShowButtons(CRect & clientRect)
{
	if (tableColumnCheckBoxPtrs.empty()) {
		return;
	}
	auto & clauseColumns = clauseType == WHERE_CLAUSE ? byteCodeResult.whereColumns : byteCodeResult.orderColumns;
	if (isEqualColumns(clauseColumns, byteCodeResult.indexColumns)) {
		return ;
	}
	CRect rcLast = GdiPlusUtil::GetWindowRelativeRect(tableColumnCheckBoxPtrs.back()->m_hWnd);
	int x = clientRect.Width() - 20 - 180, y = rcLast.bottom + 5, w = 180, h = 30;
	CRect rect(x, y, x + w, y + h);
	createOrShowButton(m_hWnd, createIdxIndividButton, Config::ANALYSIS_CREATE_INDEX_INDIVID_BUTTON_ID, S(L"create-index-individ"), rect, clientRect);
	
	rect.OffsetRect(- w - 10, 0);
	createOrShowButton(m_hWnd, createIdxTogetherButton, Config::ANALYSIS_CREATE_INDEX_TOGETHER_BUTTON_ID, S(L"create-index-togeger"), rect, clientRect);
}

void WhereOrderClauseAnalysisElem::createOrShowButton(HWND hwnd, CButton & win, UINT id, std::wstring text, CRect rect, CRect &clientRect, DWORD exStyle)
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

void WhereOrderClauseAnalysisElem::clearTableColumnCheckBoxPtrs()
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


