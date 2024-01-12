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

 * @file   FirstPage.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-12-03
 *********************************************************************/
#include "stdafx.h"
#include "FirstPage.h"
#include "core/common/Lang.h"
#include "utils/ResourceUtil.h"
#include "ui/common/QWinCreater.h"
#include "utils/StringUtil.h"
#include "common/AppContext.h"


void FirstPage::createOrShowUI()
{
	CRect clientRect;
	GetClientRect(clientRect);

	createOrShowPerfAnalysisButton(clientRect);

}


void FirstPage::createOrShowPerfAnalysisButton(CRect & clientRect)
{
	getPerfAnalysisSectionRect(clientRect, S(L"perf-analysis-section-text"));
	int x = perfAnalysisSectionRect.right + 2,
		y = perfAnalysisSectionRect.top + 5,
		w = 16, h = 16;
	CRect rect(x, y, x + w, y + h);
	std::wstring imgDir = ResourceUtil::getProductImagesDir();
	std::wstring normalImagePath, pressedImagePath;

	if (!addSqlAnalysisButton.IsWindow()) {
		normalImagePath = imgDir + L"analysis\\button\\add-sql-analysis-button-normal.png";
		pressedImagePath = imgDir + L"analysis\\button\\add-sql-analysis-button-pressed.png"; 
		addSqlAnalysisButton.SetIconPath(normalImagePath, pressedImagePath); 
		addSqlAnalysisButton.SetBkgColors(bkgColor, bkgColor, bkgColor);
	}
	QWinCreater::createOrShowButton(m_hWnd, addSqlAnalysisButton, Config::ANALYSIS_ADD_SQL_TO_ANALYSIS_BUTTON_ID, L"", rect, clientRect, BS_OWNERDRAW);
	addSqlAnalysisButton.SetToolTip(S(L"add-sql-analysis"));

	rect.OffsetRect(w + 5, 0);
	if (!sqlLogButton.IsWindow()) {
		normalImagePath = imgDir + L"analysis\\button\\sql-log-button-normal.png";
		pressedImagePath = imgDir + L"analysis\\button\\sql-log-button-pressed.png";
		sqlLogButton.SetIconPath(normalImagePath, pressedImagePath); 
		sqlLogButton.SetBkgColors(bkgColor, bkgColor, bkgColor);
	}
	QWinCreater::createOrShowButton(m_hWnd, sqlLogButton, Config::ANALYSIS_SQL_LOG_BUTTON_ID, L"", rect, clientRect, BS_OWNERDRAW);
	sqlLogButton.SetToolTip(S(L"open-sql-log"));
}

int FirstPage::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	bkgBrush.CreateSolidBrush(bkgColor);
	textFont = FT(L"analysis-text-size");
	sectionFont = FTB(L"analysis-section-size", true);
	return 0;
}

int FirstPage::OnDestroy()
{
	if (!bkgBrush.IsNull()) bkgBrush.DeleteObject();
	if (!textFont) ::DeleteObject(textFont);
	if (!sectionFont) ::DeleteObject(sectionFont);

	if (sqlLogButton.IsWindow()) sqlLogButton.DestroyWindow();
	return 0;
}

void FirstPage::OnSize(UINT nType, CSize size)
{
	createOrShowUI();
}

void FirstPage::OnShowWindow(BOOL bShow, UINT nStatus)
{

}

void FirstPage::OnPaint(CDCHandle dc)
{
	CPaintDC pdc(m_hWnd);
	CMemoryDC mdc(pdc, pdc.m_ps.rcPaint);
	CRect clientRect;
	GetClientRect(clientRect);
	mdc.FillRect(&pdc.m_ps.rcPaint, bkgBrush.m_hBrush);

	drawAnalysisText(mdc, clientRect);
	drawPerfAnalysisSection(mdc, clientRect);
}

void FirstPage::drawAnalysisText(CMemoryDC &mdc, CRect &clientRect)
{
	int x = 20, y = clientRect.top + 20, w = clientRect.Width(), h = 40;
	// y = topbarRect.top;
	CRect rect(x, y, x + w, y + h);
	int oldMode = mdc.SetBkMode(TRANSPARENT);
	HFONT oldFont = mdc.SelectFont(textFont);
	COLORREF oldColor = mdc.SetTextColor(textColor);
	std::wstring analysisText = S(L"analysis-text1");
	mdc.DrawText(analysisText.c_str(), static_cast<int>(analysisText.size()), rect, DT_LEFT | DT_VCENTER);

	rect.OffsetRect(0, rect.Height());
	analysisText = S(L"analysis-text2");
	mdc.DrawText(analysisText.c_str(), static_cast<int>(analysisText.size()), rect, DT_LEFT | DT_VCENTER);

	rect.OffsetRect(0, rect.Height());
	analysisText = S(L"analysis-text3");
	mdc.DrawText(analysisText.c_str(), static_cast<int>(analysisText.size()), rect, DT_LEFT | DT_VCENTER);

	mdc.SetTextColor(oldColor);
	mdc.SelectFont(oldFont);
	mdc.SetBkMode(oldMode);
}

void FirstPage::drawPerfAnalysisSection(CMemoryDC &mdc, CRect &clientRect)
{
	int oldMode = mdc.SetBkMode(TRANSPARENT);
	HFONT oldFont = mdc.SelectFont(sectionFont);
	COLORREF oldColor = mdc.SetTextColor(sectionColor);	
	std::wstring sectionText = S(L"perf-analysis-section-text");
	getPerfAnalysisSectionRect(clientRect, sectionText);

	mdc.DrawText(sectionText.c_str(), static_cast<int>(sectionText.size()), perfAnalysisSectionRect, DT_LEFT | DT_VCENTER);

	mdc.SetTextColor(oldColor);
	mdc.SelectFont(oldFont);
	mdc.SetBkMode(oldMode);
}


void FirstPage::getPerfAnalysisSectionRect(CRect &clientRect, std::wstring &sectionText)
{
	int x = 20, y = 50 + 3 * 40 + 20, w = clientRect.Width() - 40, h = 40;
	CSize size = StringUtil::getTextSize(sectionText.c_str(), sectionFont);
	w = size.cx + 10;
	perfAnalysisSectionRect = { x, y, x + w, y + h };
}

BOOL FirstPage::OnEraseBkgnd(CDCHandle dc)
{
	return true;
}

LRESULT FirstPage::OnClickSqlLogButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	AppContext::getInstance()->dispatch(Config::MSG_SHOW_SQL_LOG_PAGE_ID);
	return 0;
}

LRESULT FirstPage::OnClickAddSqlButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	AppContext::getInstance()->dispatch(Config::MSG_ADD_SQL_TO_ANALYSIS_ID);
	return 0;
}
