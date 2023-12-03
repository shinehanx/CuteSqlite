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

 * @file   FirstPage.h
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-12-03
 *********************************************************************/
#pragma once
#include "ui/database/rightview/common/QTabPage.h"
#include "ui/common/button/QImageButton.h"
#include "common/Config.h"

class FirstPage : public QTabPage<FirstPage>
{
public:
	BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP_EX(FirstPage)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_SIZE(OnSize)
		MSG_WM_SHOWWINDOW(OnShowWindow)
		MSG_WM_PAINT(OnPaint)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		COMMAND_ID_HANDLER_EX(Config::ANALYSIS_SQL_LOG_BUTTON_ID, OnClickSqlLogButton)
		CHAIN_MSG_MAP(QPage)
		FORWARD_NOTIFICATIONS()
	END_MSG_MAP()

private:
	bool isNeedReload = true;
	
	COLORREF bkgColor = RGB(171, 171, 171);	
	COLORREF textColor = RGB(255, 255, 255);
	COLORREF sectionColor = RGB(255, 255, 255);
	CBrush bkgBrush;
	HFONT textFont = nullptr;
	HFONT sectionFont = nullptr;
	
	CRect perfAnalysisSectionRect;
	QImageButton sqlLogButton;

	void createOrShowUI();
	void createOrShowPerfAnalysisButton(CRect & clientRect);

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	int OnDestroy();
	void OnSize(UINT nType, CSize size);
	void OnShowWindow(BOOL bShow, UINT nStatus);
	void OnPaint(CDCHandle dc);
	BOOL OnEraseBkgnd(CDCHandle dc);

	LRESULT OnClickSqlLogButton(UINT uNotifyCode, int nID, HWND hwnd);
	void drawAnalysisText(CMemoryDC &mdc, CRect &clientRect);
	void drawPerfAnalysisSection(CMemoryDC &mdc, CRect &clientRect);

	void getPerfAnalysisSectionRect(CRect &clientRect, std::wstring &sectionText);
};
