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
 * @brief  table join elem UI used for joins clause analysis in PerAnalysisPage
 * 
 * @author Xuehan Qin
 * @date   2023-12-08
 *********************************************************************/
#include "stdafx.h"
#include "TableJoinAnalysisElem.h"
#include "core/common/Lang.h"
#include "ui/common/QWinCreater.h"
#include "utils/StringUtil.h"
#include "ui/common/message/QPopAnimate.h"
#include "common/AppContext.h"

TableJoinAnalysisElem::TableJoinAnalysisElem(const std::wstring &_selectColumnStr, const ByteCodeResults & _byteCodeResults)
	: selectColumnStr(_selectColumnStr), byteCodeResults(_byteCodeResults)
{
	
}

int TableJoinAnalysisElem::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	bkgBrush.CreateSolidBrush(bkgColor);
	textFont = FT(L"form-text-size");
	return 0;
}

void TableJoinAnalysisElem::OnDestroy()
{
	if (!bkgBrush.IsNull()) bkgBrush.DeleteObject();
	if (textFont) ::DeleteObject(textFont);

	if (tableJoinLabel.IsWindow()) tableJoinLabel.DestroyWindow();
	if (pseudocodeEdit.IsWindow()) pseudocodeEdit.DestroyWindow();
	if (cycleCountLabel.IsWindow()) cycleCountLabel.DestroyWindow();
	if (optimizeAdviceLabel.IsWindow()) optimizeAdviceLabel.DestroyWindow();
	
}

void TableJoinAnalysisElem::OnSize(UINT nType, CSize size)
{
	if (size.cx == 0 || size.cy == 0) {
		return;
	}
	createOrShowUI();
}

void TableJoinAnalysisElem::OnShowWindow(BOOL bShow, UINT nStatus)
{

}

void TableJoinAnalysisElem::OnPaint(CDCHandle dc)
{
	CPaintDC pdc(m_hWnd);
	pdc.FillRect(&pdc.m_ps.rcPaint, bkgBrush.m_hBrush);
}

BOOL TableJoinAnalysisElem::OnEraseBkgnd(CDCHandle dc)
{
	return true;
}

HBRUSH TableJoinAnalysisElem::OnCtlStaticColor(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, bkgColor);	
	::SelectObject(hdc, textFont);
	if (hwnd == optimizeAdviceLabel.m_hWnd) {
		::SetTextColor(hdc, hintColor);
	} else {
		::SetTextColor(hdc,textColor);
	}	
	return bkgBrush.m_hBrush;
}


void TableJoinAnalysisElem::createOrShowUI()
{
	CRect clientRect;
	GetClientRect(clientRect);

	createOrShowLabelsAndEdits(clientRect);
}

void TableJoinAnalysisElem::createOrShowLabelsAndEdits(CRect & clientRect)
{
	int x = 5, y = 0, w = clientRect.Width() - 10, h = 20;
	CRect rect(x, y, x + w, y + h);

	std::wstring text1 = S(L"query-optimizer-pseudocode");
	std::wstring text2 = S(L"cycle-count");
	std::wstring text3 = S(L"optimize-cycle-count");

	QWinCreater::createOrShowLabel(m_hWnd, tableJoinLabel, text1, rect, clientRect, SS_LEFT | SS_CENTERIMAGE);

	rect.OffsetRect(0, h + 5);
	rect.bottom = rect.top + 80;
	std::wstring  pseudocode = getTableJoinPseudocode();
	QWinCreater::createOrShowEdit(m_hWnd, pseudocodeEdit, 0, pseudocode, rect, clientRect, textFont,  WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL);

	rect.OffsetRect(0, rect.Height() + 5);
	rect.bottom = rect.top + 20;
	uint64_t cycleCount = getTableJoinCycleCount();
	text2.append(L":").append(std::to_wstring(cycleCount));
	QWinCreater::createOrShowLabel(m_hWnd, cycleCountLabel, text2, rect, clientRect, SS_LEFT | SS_CENTERIMAGE);

	rect.OffsetRect(0, rect.Height() + 5);
	QWinCreater::createOrShowLabel(m_hWnd, optimizeAdviceLabel, text2, rect, clientRect, SS_LEFT | SS_CENTERIMAGE);
}


std::wstring TableJoinAnalysisElem::getTableJoinPseudocode()
{
	std::wstring  pseudocode;
	int i = 0;
	for (auto & byteCodeResult : byteCodeResults) {
		if (byteCodeResult.type != L"table") {
			continue;
		}
		
		if (i > 0) {
			pseudocode.append(L"\r\n");
			for (int j = 0; j < i; j++) {
				pseudocode.append(L"  ");
			}			
		}
		pseudocode.append(L"FOREACH \"").append(byteCodeResult.name).append(L"\" WHERE ");
		std::wstring whereExpressStr = StringUtil::implode(byteCodeResult.whereExpresses, L", ");
		pseudocode.append(whereExpressStr).append(L" DO:");
		i++;
	}

	// fields
	if (i > 0) {
		pseudocode.append(L"\r\n");
		for (int ii = 0; ii <= i; ii++) {
			pseudocode.append(L"  ");
		}
		pseudocode.append(L"RETURN ").append(selectColumnStr);
	}
	// ending
	for (int j = i; j > 0; j--) {
		pseudocode.append(L"\r\n");
		for (int k = 0; k < j; k++) {
			if (k > 0) pseudocode.append(L"  ");
		}
		pseudocode.append(L"END");
	}

	return pseudocode;
}

uint64_t TableJoinAnalysisElem::getTableJoinCycleCount()
{
	uint64_t total = 1024;
	return total;
}

