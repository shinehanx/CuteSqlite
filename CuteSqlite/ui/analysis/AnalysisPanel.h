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

 * @file   AnalysisPanel.h
 * @brief  Performance analysis and performance optimization
 * 
 * @author Xuehan Qin
 * @date   2023-12-01
 *********************************************************************/
#pragma once
#include <atlwin.h>
#include <atlcrack.h>
#include <atltypes.h>
#include <atlsplit.h>
#include "common/Config.h"
#include "ui/analysis/leftview/LeftNavigationView.h"
#include "ui/analysis/rightview/RightAnalysisView.h"

class AnalysisPanel :public CWindowImpl<AnalysisPanel>
{
public:
	const Config::PanelId panelId = Config::ANALYSIS_PANEL;

	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP_EX(AnalysisPanel)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_SIZE(OnSize)		
		MSG_WM_SHOWWINDOW(OnShowWindow)
		MSG_WM_PAINT(OnPaint)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
	END_MSG_MAP()

private:
	bool isNeededReload = true;
	COLORREF bkgColor = RGB(255, 255, 255);
	CBrush bkgBrush;

	CSplitterWindow splitter;
	LeftNavigationView leftView;
	RightAnalysisView rightView;

	void createOrShowUI();
	void createOrShowSplitter(CSplitterWindow & win , CRect & clientRect);
	void createOrShowLeftView(LeftNavigationView & win, CRect & clientRect);
	void createOrShowRightView(RightAnalysisView & win, CRect & clientRect);

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnDestroy();
	void OnSize(UINT nType, CSize size);
	void OnShowWindow(BOOL bShow, UINT nStatus);
	void OnPaint(CDCHandle dc);
	BOOL OnEraseBkgnd(CDCHandle dc);

};
