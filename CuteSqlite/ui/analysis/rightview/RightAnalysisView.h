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
 *                         |      |-> SqlLogListBox
 *                         |               |-> SqlLogListItem
 *                         |-> PerfAnalysisPage
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
#include "ui/analysis/rightview/page/StoreAnalysisPage.h"
#include "ui/analysis/rightview/page/DbPragmaParamsPage.h"

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

		COMMAND_ID_HANDLER_EX(Config::ANALYSIS_ADD_SQL_TO_ANALYSIS_BUTTON_ID, OnClickAddSqlButton)
		COMMAND_ID_HANDLER_EX(Config::ANALYSIS_SQL_LOG_BUTTON_ID, OnClickSqlLogButton)
		// save
		COMMAND_ID_HANDLER_EX(Config::ANALYSIS_SAVE_BUTTON_ID, OnClickSaveButton)
		COMMAND_ID_HANDLER_EX(Config::ANALYSIS_SAVE_ALL_BUTTON_ID, OnClickSaveAllButton)

		MESSAGE_HANDLER_EX(Config::MSG_ANALYSIS_SQL_ID, OnHandleAnalysisSql)
		MESSAGE_HANDLER_EX(Config::MSG_DB_STORE_ANALYSIS_ID, OnHandleDbStoreAnalysis)
		MESSAGE_HANDLER_EX(Config::MSG_DB_PRAGMA_PARAMS_ID, OnHandleDbPragmaParams)
		MESSAGE_HANDLER_EX(Config::MSG_SHOW_SQL_LOG_PAGE_ID, OnHandleShowSqlLogPage)
		MESSAGE_HANDLER_EX(Config::MSG_ANALYSIS_ADD_PERF_REPORT_ID, OnHandleAnalysisAddPerfReport)
		MESSAGE_HANDLER_EX(Config::MSG_ANALYSIS_SAVE_PERF_REPORT_ID, OnHandleAnalysisSavePerfReport)
		MESSAGE_HANDLER_EX(Config::MSG_ANALYSIS_DROP_PERF_REPORT_ID, OnHandleAnalysisDropPerfReport)
		MESSAGE_HANDLER_EX(Config::MSG_DELETE_DATABASE_ID, OnHandleDeleteDatabase)

		NOTIFY_CODE_HANDLER (TBVN_TABCLOSEBTN, OnTabViewCloseBtn)

		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
private:
	bool isNeedReload = true;
	bool isInitedPages = false;

	COLORREF bkgColor = RGB(171, 171, 171);	
	COLORREF topbarColor = RGB(17, 24, 39);
	COLORREF topbarHoverColor = RGB(102, 102, 104);
	CBrush bkgBrush;	
	CBrush topbarBrush ;
	// add sql to analysis button
	QImageButton addSqlAnalysisButton;
	// sql log button
	QImageButton sqlLogButton;
	QImageButton saveButton;
	QImageButton saveAllButton;

	HACCEL m_hAccel = nullptr;

	QTabView tabView;
	FirstPage firstPage;
	SqlLogPage sqlLogPage;
	std::vector<PerfAnalysisPage *> perfAnalysisPagePtrs;
	std::vector<StoreAnalysisPage *> storeAnalysisPagePtrs;
	std::vector<DbPragmaParamsPage *> dbPragmaParamsPagePtrs;

	DatabaseService * databaseService = DatabaseService::getInstance();
	SqlLogService * sqlLogService = SqlLogService::getInstance();

	HICON firstIcon = nullptr;
	HICON sqlLogIcon = nullptr;
	HICON perfReportIcon = nullptr;
	HICON perfReportDirtyIcon = nullptr;
	HICON storeAnalysisIcon = nullptr;
	HICON dbPragmaParamIcon = nullptr;
	HICON dbQuikConfigIcon = nullptr;
	CImageList imageList;
	void createImageList();

	CRect getTopRect(CRect & clientRect);
	CRect getTabRect(CRect & clientRect);
	CRect getTabRect();

	void createOrShowUI();
	
	void createOrShowToolButtons(CRect & clientRect);
	void createOrShowAnalysisButtons(CRect & clientRect);
	void createOrShowSaveButtons(CRect & clientRect);

	void createOrShowTabView(QTabView &win, CRect & clientRect);
	void createOrShowFirstPage(FirstPage &win, CRect & clientRect, bool isAllowCreate = true);
	void createOrShowSqlLogPage(SqlLogPage &win, CRect & clientRect, bool isAllowCreate = true);
	void createOrShowPerfAnalysisPage(PerfAnalysisPage &win, CRect & clientRect, bool isAllowCreate = true);
	void createOrShowStoreAnalysisPage(StoreAnalysisPage &win, CRect & clientRect, bool isAllowCreate = true);
	void createOrShowDbPragmaParamsPage(DbPragmaParamsPage &win, CRect & clientRect, bool isAllowCreate = true);
		
	void loadWindow();
	void loadTabViewPages();

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnDestroy();
	void OnSize(UINT nType, CSize size);
	void OnShowWindow(BOOL bShow, UINT nStatus);
	void OnPaint(CDCHandle dc);
	BOOL OnEraseBkgnd(CDCHandle dc);
	HBRUSH OnCtlColorStatic(HDC hdc, HWND hwnd);

	LRESULT OnClickAddSqlButton(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnClickSqlLogButton(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnClickSaveButton(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnClickSaveAllButton(UINT uNotifyCode, int nID, HWND hwnd);

	LRESULT OnHandleAnalysisSql(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnHandleDbStoreAnalysis(UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT OnHandleDbPragmaParams(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnHandleShowSqlLogPage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnHandleAnalysisAddPerfReport(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnHandleAnalysisSavePerfReport(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnHandleAnalysisDropPerfReport(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnHandleDeleteDatabase(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnTabViewCloseBtn(int idCtrl, LPNMHDR pnmh, BOOL &bHandled);
	LRESULT closeTabViewPage(int nPage);
	void clearPerfAnalysisPagePtrs();
	void clearDbPragmaParamsPagePtrs();

	void doShowSqlLogPage();
	void addSqlToAnalysisPerfReport(const std::wstring & sql);
	void doAddDbStoreAnalysisPage(uint64_t userDbId);
	void doAddDbPragmaParamsPage(uint64_t userDbId);
};
