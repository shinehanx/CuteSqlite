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
 * 
 * @Class Tree  RightAnalysisView
 *                 |->QTabView(tabView)
 *                         |-> SqlLogPage
 *                               |-> SqlLogListBox
 *                                        |-> SqlLogListItem
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
#include "ui/analysis/rightview/page/FirstPage.h"
#include "ui/analysis/rightview/page/PerfAnalysisPage.h"

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

		MESSAGE_HANDLER_EX(Config::MSG_ANALYSIS_SQL_ID, OnHandleAnalysisSql)
		MESSAGE_HANDLER_EX(Config::MSG_SHOW_SQL_LOG_PAGE_ID, OnHandleShowSqlLogPage)

		NOTIFY_CODE_HANDLER (TBVN_TABCLOSEBTN, OnTabViewCloseBtn)

		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
private:
	bool isNeedReload = true;
	bool isInitedPages = false;

	COLORREF bkgColor = RGB(171, 171, 171);	
	COLORREF topbarColor = RGB(17, 24, 39);
	CBrush bkgBrush;	
	CBrush topbarBrush ;

	QTabView tabView;
	FirstPage firstPage;
	SqlLogPage sqlLogPage;
	std::vector<PerfAnalysisPage *> perfAnalysisPagePtrs;

	SqlLogService * sqlLogService = SqlLogService::getInstance();

	HICON firstIcon = nullptr;
	HICON sqlLogIcon = nullptr;
	HICON perfReportIcon = nullptr;
	CImageList imageList;
	void createImageList();

	CRect getTopRect(CRect & clientRect);
	CRect getTabRect(CRect & clientRect);
	CRect getTabRect();

	void createOrShowUI();
	void createOrShowTabView(QTabView &win, CRect & clientRect);
	void createOrShowFirstPage(FirstPage &win, CRect & clientRect, bool isAllowCreate = true);
	void createOrShowSqlLogPage(SqlLogPage &win, CRect & clientRect, bool isAllowCreate = true);
	void createOrShowPerfAnalysisPage(PerfAnalysisPage &win, CRect & clientRect, bool isAllowCreate = true);

	void loadWindow();
	void loadTabViewPages();

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnDestroy();
	void OnSize(UINT nType, CSize size);
	void OnShowWindow(BOOL bShow, UINT nStatus);
	void OnPaint(CDCHandle dc);
	BOOL OnEraseBkgnd(CDCHandle dc);
	HBRUSH OnCtlColorStatic(HDC hdc, HWND hwnd);

	LRESULT OnHandleAnalysisSql(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnHandleShowSqlLogPage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnTabViewCloseBtn(int idCtrl, LPNMHDR pnmh, BOOL &bHandled);
	LRESULT closeTabViewPage(int nPage);
	void clearPerfAnalysisPagePtrs();
};
