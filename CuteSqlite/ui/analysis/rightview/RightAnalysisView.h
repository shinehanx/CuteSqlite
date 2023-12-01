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

 * @file   RightAnalysisView.h
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-12-01
 *********************************************************************/
#pragma once
#include <atlwin.h>
#include <atlcrack.h>
#include <atltypes.h>
#include "ui/common/tabview/QTabView.h"
#include "ui/analysis/rightview/page/SqlLogPage.h"

class RightAnalysisView : public CWindowImpl<RightAnalysisView>
{
public:
	BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP_EX(LeftTreeView)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_SIZE(OnSize)
		MSG_WM_SHOWWINDOW(OnShowWindow)
		MSG_WM_PAINT(OnPaint)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MSG_WM_CTLCOLORSTATIC(OnCtlColorStatic)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
private:
	bool isNeedReload = true;
	
	COLORREF bkgColor = RGB(255, 255, 255);	
	COLORREF textColor = RGB(255, 255, 255);
	CBrush bkgBrush;

	COLORREF topbarColor = RGB(17, 24, 39);
	CBrush topbarBrush ;
	HFONT textFont;

	QTabView tabView;
	SqlLogPage sqlLogPage;

	HICON sqlLogIcon = nullptr;
	CImageList imageList;
	void createImageList();

	CRect getTopRect(CRect & clientRect);
	CRect getTabRect(CRect & clientRect);
	CRect getTabRect();

	void createOrShowUI();
	void createOrShowTabView(QTabView &win, CRect & clientRect);
	void createOrShowSqlLogPage(SqlLogPage &win, CRect & clientRect, bool isAllowCreate = true);

	void loadWindow();
	void loadTabViewPages();

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnDestroy();
	void OnSize(UINT nType, CSize size);
	void OnShowWindow(BOOL bShow, UINT nStatus);
	void OnPaint(CDCHandle dc);
	BOOL OnEraseBkgnd(CDCHandle dc);
	HBRUSH OnCtlColorStatic(HDC hdc, HWND hwnd);
};
