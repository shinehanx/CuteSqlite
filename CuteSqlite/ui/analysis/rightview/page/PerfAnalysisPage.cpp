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
	this->sqlLogId = sqlLogId;
	this->sqlLog = sqlLogService->getSqlLog(sqlLogId);
}


uint64_t PerfAnalysisPage::getSqlLogId()
{
	return sqlLogId;
}

void PerfAnalysisPage::createOrShowUI()
{
	CRect clientRect;
	GetClientRect(clientRect);
}

int PerfAnalysisPage::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	bkgBrush.CreateSolidBrush(bkgColor);
	textFont = FT(L"analysis-text-size");
	sectionFont = FTB(L"analysis-section-size", true);
	return 0;
}

int PerfAnalysisPage::OnDestroy()
{
	if (!bkgBrush.IsNull()) bkgBrush.DeleteObject();
	if (!textFont) ::DeleteObject(textFont);
	if (!sectionFont) ::DeleteObject(sectionFont);

	
	return 0;
}

void PerfAnalysisPage::OnSize(UINT nType, CSize size)
{
	createOrShowUI();
}

void PerfAnalysisPage::OnShowWindow(BOOL bShow, UINT nStatus)
{

}

void PerfAnalysisPage::OnPaint(CDCHandle dc)
{
	CPaintDC pdc(m_hWnd);
	CMemoryDC mdc(pdc, pdc.m_ps.rcPaint);
	CRect clientRect;
	GetClientRect(clientRect);
	mdc.FillRect(&pdc.m_ps.rcPaint, bkgBrush.m_hBrush);
}

BOOL PerfAnalysisPage::OnEraseBkgnd(CDCHandle dc)
{
	return true;
}
