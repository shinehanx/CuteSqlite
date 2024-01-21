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

 * @file   LeftNavigationView.h
 * @brief  Performance analysis 
 * 
 * @author Xuehan Qin
 * @date   2023-12-01
 *********************************************************************/
#pragma once
#include <atlwin.h>
#include <atlcrack.h>
#include <atltypes.h>
#include "ui/common/treeview/QTreeViewCtrl.h"
#include "ui/analysis/leftview/adapter/LeftNaigationViewAdapter.h"
#include "common/Config.h"

class LeftNavigationView : public CWindowImpl<LeftNavigationView>
{
public:
	BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP_EX(LeftNavigationView)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_SIZE(OnSize)
		MSG_WM_SHOWWINDOW(OnShowWindow)
		MSG_WM_PAINT(OnPaint)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		NOTIFY_HANDLER(Config::ANALYSIS_NAVIGATION_TREEVIEW_ID, NM_DBLCLK, OnDbClickTreeViewItem)
		NOTIFY_HANDLER(Config::ANALYSIS_NAVIGATION_TREEVIEW_ID, NM_RCLICK, OnRightClickTreeViewItem)

		COMMAND_ID_HANDLER_EX(Config::ANALYSIS_OPEN_PERF_REPORT_MENU_ID, OnClickOpenPerfReportMenu)
		COMMAND_ID_HANDLER_EX(Config::ANALYSIS_DROP_PERF_REPORT_MENU_ID, OnClickDropPerfReportMenu)

		MESSAGE_HANDLER_EX(Config::MSG_ANALYSIS_SQL_ID, OnHandleAnalysisSql)
		MESSAGE_HANDLER_EX(Config::MSG_ANALYSIS_SAVE_PERF_REPORT_ID, OnHandleAnalysisSavePerfReport)
		MESSAGE_HANDLER_EX(Config::MSG_ADD_DATABASE_ID, OnHandleAddDatabase)
		MESSAGE_HANDLER_EX(Config::MSG_DELETE_DATABASE_ID, OnHandleDeleteDatabase)
		MESSAGE_HANDLER_EX(Config::MSG_ANALYSIS_DIRTY_DB_PRAGMAS_ID, OnHandleDirtyDbPragmas)
		MESSAGE_HANDLER_EX(Config::MSG_ANALYSIS_DIRTY_DB_QUICK_CONFIG_ID, OnHandleDirtyDbQuickConfig)
	END_MSG_MAP()
private:
	bool isNeedReload = true;
	COLORREF bkgColor = RGB(255, 255, 255);
	COLORREF topbarColor = RGB(17, 24, 39);
	COLORREF titleColor = RGB(255, 255, 255);
	CBrush bkgBrush;
	CBrush topbarBrush;
	HFONT titleFont = nullptr;

	HACCEL m_hAccel = nullptr;
	QTreeViewCtrl navigationTreeView;

	LeftNaigationViewAdapter * adapter = nullptr;
	SqlLogService * sqlLogService = SqlLogService::getInstance();

	CRect getTopRect(CRect & clientRect);
	CRect getTreeRect(CRect & clientRect);
	
	void createOrShowUI();
	void createOrShowNavigationTreeView(QTreeViewCtrl & win, CRect & clientRect);

	void loadWindow();

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnDestroy();
	void OnSize(UINT nType, CSize size);
	void OnShowWindow(BOOL bShow, UINT nStatus);
	void OnPaint(CDCHandle dc);
	BOOL OnEraseBkgnd(CDCHandle dc);
	// double click treeview item .
	LRESULT OnDbClickTreeViewItem(int wParam, LPNMHDR lParam, BOOL& bHandled);
	LRESULT OnRightClickTreeViewItem(int wParam, LPNMHDR lParam, BOOL& bHandled);

	LRESULT OnHandleAnalysisSql(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnHandleAnalysisSavePerfReport(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnHandleAddDatabase(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnHandleDeleteDatabase(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnHandleDirtyDbPragmas(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnHandleDirtyDbQuickConfig(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void OnClickOpenPerfReportMenu(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickDropPerfReportMenu(UINT uNotifyCode, int nID, HWND hwnd);
};
