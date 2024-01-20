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

 * @file   RightAnalysisView.cpp
  * @Class Tree  RightAnalysisView
 *                 |->QTabView(tabView)
 *                         |-> SqlLogPage
 *                         |      |-> SqlLogListBox
 *                         |               |-> SqlLogListItem
 *                         |-> PerfAnalysisPage
 * 
 * 
 * 
 * @author Xuehan Qin
 * @date   2023-12-01
 *********************************************************************/
#include "stdafx.h"
#include "RightAnalysisView.h"
#include "utils/ResourceUtil.h"
#include "common/AppContext.h"
#include "core/common/Lang.h"
#include "utils/SqlUtil.h"
#include "ui/common/message/QPopAnimate.h"
#include "ui/common/QWinCreater.h"
#include "ui/analysis/rightview/dialog/AddSqlDialog.h"

#define RIGHT_ANALYSIS_VIEW_TOPBAR_HEIGHT 30
#define RIGHT_ANALYSIS_VIEW_BUTTON_WIDTH 16


BOOL RightAnalysisView::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST) {
		if (m_hAccel && ::TranslateAccelerator(m_hWnd, m_hAccel, pMsg)) {
			return TRUE;
		}
	}

	return FALSE;
}

void RightAnalysisView::createImageList()
{
	if (!imageList.IsNull()) {
		return;
	}
	std::wstring imgDir = ResourceUtil::getProductImagesDir();
	HINSTANCE ins = ModuleHelper::GetModuleInstance();

	firstIcon = (HICON)::LoadImageW(ins, (imgDir + L"analysis\\tab\\first.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	sqlLogIcon = (HICON)::LoadImageW(ins, (imgDir + L"analysis\\tree\\sql-log.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	perfReportIcon = (HICON)::LoadImageW(ins, (imgDir + L"analysis\\tree\\analysis-report.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	perfReportDirtyIcon = (HICON)::LoadImageW(ins, (imgDir + L"analysis\\tree\\analysis-report-dirty.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	storeAnalysisIcon = (HICON)::LoadImageW(ins, (imgDir + L"analysis\\tree\\store-analysis.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	dbPragmaParamIcon = (HICON)::LoadImageW(ins, (imgDir + L"analysis\\tree\\db-pragma.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	dbQuikConfigIcon = (HICON)::LoadImageW(ins, (imgDir + L"analysis\\tree\\db-quick-config.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);

	imageList.Create(16, 16, ILC_COLOR32, 8, 8);
	imageList.AddIcon(firstIcon); // 0 - first
	imageList.AddIcon(sqlLogIcon); // 1 - sql log
	imageList.AddIcon(perfReportIcon); // 2 - perf report
	imageList.AddIcon(perfReportDirtyIcon); // 3 - perf report not saved
	imageList.AddIcon(storeAnalysisIcon); // 4 - database store analysis
	imageList.AddIcon(dbPragmaParamIcon); // 5 - database PRAGMA params
	imageList.AddIcon(dbQuikConfigIcon); // 6 - database quick config params
}

CRect RightAnalysisView::getTopRect(CRect & clientRect)
{
	return { 0, 0, clientRect.right, RIGHT_ANALYSIS_VIEW_TOPBAR_HEIGHT };
}

CRect RightAnalysisView::getTabRect(CRect & clientRect)
{
	return { 0, RIGHT_ANALYSIS_VIEW_TOPBAR_HEIGHT, clientRect.right, clientRect.bottom };
}

CRect RightAnalysisView::getTabRect()
{
	CRect clientRect;
	GetClientRect(clientRect);
	return getTabRect(clientRect);
}


void RightAnalysisView::createOrShowUI()
{
	CRect clientRect;
	GetClientRect(clientRect);
	CRect tabRect = getTabRect(clientRect);

	createOrShowToolButtons(clientRect);

	createOrShowTabView(tabView, clientRect);	
	createOrShowFirstPage(firstPage, clientRect, !isInitedPages);
	
	isInitedPages = true;
}


void RightAnalysisView::createOrShowTabView(QTabView &win, CRect & clientRect)
{
	CRect rect = getTabRect(clientRect);
	if (IsWindow() && !win.IsWindow()) {
		win.Create(m_hWnd, rect, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, Config::DATABASE_WORK_TAB_VIEW_ID);
		win.SetTitleBarWindow(AppContext::getInstance()->getMainFrmHwnd());
		win.SetImageList(imageList);
	} else if (IsWindow() && tabView.IsWindow()) {
		win.MoveWindow(rect);
		win.ShowWindow(true);
	}
}


void RightAnalysisView::createOrShowFirstPage(FirstPage &win, CRect & clientRect, bool isAllowCreate /*= true*/)
{
	CRect tabRect = getTabRect(clientRect);
	int x = 1, y = tabView.m_cyTabHeight + 1, w = tabRect.Width() - 2, h = tabRect.Height() - tabView.m_cyTabHeight - 2;
	CRect rect(x, y, x + w, y + h);
	if (isAllowCreate && IsWindow() && !win.IsWindow()) {
		win.Create(tabView.m_hWnd, rect, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN , 0);
	} else if (IsWindow() && tabView.IsWindow() && win.IsWindow()) {
		win.MoveWindow(rect);
		win.ShowWindow(true);
	}
}

void RightAnalysisView::createOrShowSqlLogPage(SqlLogPage &win, CRect & clientRect, bool isAllowCreate /*= true*/)
{
	CRect tabRect = getTabRect(clientRect);
	int x = 1, y = tabView.m_cyTabHeight + 1, w = tabRect.Width() - 2, h = tabRect.Height() - tabView.m_cyTabHeight - 2;
	CRect rect(x, y, x + w, y + h);
	if (isAllowCreate && IsWindow() && !win.IsWindow()) {
		win.Create(tabView.m_hWnd, rect, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN , 0);		
	} else if (IsWindow() && tabView.IsWindow() && win.IsWindow()) {
		win.MoveWindow(rect);
		win.ShowWindow(true);
	}
}


void RightAnalysisView::createOrShowPerfAnalysisPage(PerfAnalysisPage &win, CRect & clientRect, bool isAllowCreate /*= true*/)
{
	CRect tabRect = getTabRect(clientRect);
	int x = 1, y = tabView.m_cyTabHeight + 1, w = tabRect.Width() - 2, h = tabRect.Height() - tabView.m_cyTabHeight - 2;
	CRect rect(x, y, x + w, y + h);
	if (isAllowCreate && IsWindow() && !win.IsWindow()) {
		win.Create(tabView.m_hWnd, rect, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN , 0);		
	} else if (IsWindow() && tabView.IsWindow() && win.IsWindow()) {
		win.MoveWindow(rect);
		win.ShowWindow(true);
	}
}


void RightAnalysisView::createOrShowStoreAnalysisPage(StoreAnalysisPage &win, CRect & clientRect, bool isAllowCreate /*= true*/)
{
	CRect tabRect = getTabRect(clientRect);
	int x = 1, y = tabView.m_cyTabHeight + 1, w = tabRect.Width() - 2, h = tabRect.Height() - tabView.m_cyTabHeight - 2;
	CRect rect(x, y, x + w, y + h);
	if (isAllowCreate && IsWindow() && !win.IsWindow()) {
		win.Create(tabView.m_hWnd, rect, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN , 0);		
	} else if (IsWindow() && tabView.IsWindow() && win.IsWindow()) {
		win.MoveWindow(rect);
		win.ShowWindow(true);
	}
}


void RightAnalysisView::createOrShowDbPragmaParamsPage(DbPragmaParamsPage &win, CRect & clientRect, bool isAllowCreate /*= true*/)
{
	CRect tabRect = getTabRect(clientRect);
	int x = 1, y = tabView.m_cyTabHeight + 1, w = tabRect.Width() - 2, h = tabRect.Height() - tabView.m_cyTabHeight - 2;
	CRect rect(x, y, x + w, y + h);
	if (isAllowCreate && IsWindow() && !win.IsWindow()) {
		win.Create(tabView.m_hWnd, rect, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN , 0);		
	} else if (IsWindow() && tabView.IsWindow() && win.IsWindow()) {
		win.MoveWindow(rect);
		win.ShowWindow(true);
	}
}

void RightAnalysisView::createOrShowToolButtons(CRect & clientRect)
{
	createOrShowAnalysisButtons(clientRect);
	createOrShowSaveButtons(clientRect);
}


void RightAnalysisView::createOrShowAnalysisButtons(CRect & clientRect)
{
	CRect topRect = getTopRect(clientRect);

	int x = 10, y = 7, w = RIGHT_ANALYSIS_VIEW_BUTTON_WIDTH, h = RIGHT_ANALYSIS_VIEW_BUTTON_WIDTH;

	// Exec sql buttons
	CRect rect(x, y, x + w, y + h);
	std::wstring imgDir = ResourceUtil::getProductImagesDir();
	std::wstring normalImagePath, pressedImagePath;
	if (!addSqlAnalysisButton.IsWindow()) {
		normalImagePath = imgDir + L"analysis\\toolbar\\add-sql-analysis-button-normal.png";
		pressedImagePath = imgDir + L"analysis\\toolbar\\add-sql-analysis-button-pressed.png";
		addSqlAnalysisButton.SetIconPath(normalImagePath, pressedImagePath);
		addSqlAnalysisButton.SetBkgColors(topbarColor, topbarHoverColor, topbarColor);
	}
	QWinCreater::createOrShowButton(m_hWnd, addSqlAnalysisButton, Config::ANALYSIS_ADD_SQL_TO_ANALYSIS_BUTTON_ID, L"", rect, clientRect);
	addSqlAnalysisButton.SetToolTip(S(L"add-sql-analysis"));

	rect.OffsetRect(w + 5, 0);
	if (!sqlLogButton.IsWindow()) {
		normalImagePath = imgDir + L"analysis\\toolbar\\sql-log-button-normal.png"; 
		pressedImagePath = imgDir + L"analysis\\toolbar\\sql-log-button-pressed.png";
		sqlLogButton.SetIconPath(normalImagePath, pressedImagePath);
		sqlLogButton.SetBkgColors(bkgColor, bkgColor, bkgColor);
	}
	QWinCreater::createOrShowButton(m_hWnd, sqlLogButton, Config::ANALYSIS_SQL_LOG_BUTTON_ID, L"", rect, clientRect, BS_OWNERDRAW);
	sqlLogButton.SetToolTip(S(L"open-sql-log"));
}

void RightAnalysisView::createOrShowSaveButtons(CRect & clientRect)
{
	CRect rect = GdiPlusUtil::GetWindowRelativeRect(sqlLogButton.m_hWnd);
	int w = rect.Width();
	std::wstring imgDir = ResourceUtil::getProductImagesDir();
	std::wstring normalImagePath,pressedImagePath;

	rect.OffsetRect(w + 40, 0); 
	if (!saveButton.IsWindow()) {
		normalImagePath = imgDir + L"analysis\\toolbar\\save-button-normal.png";
		pressedImagePath = imgDir + L"analysis\\toolbar\\save-button-pressed.png";
		saveButton.SetIconPath(normalImagePath, pressedImagePath);
		saveButton.SetBkgColors(topbarColor ,topbarHoverColor, topbarColor);
	}
	QWinCreater::createOrShowButton(m_hWnd, saveButton, Config::ANALYSIS_SAVE_BUTTON_ID, L"", rect, clientRect);
	saveButton.SetToolTip(S(L"save-current"));

	rect.OffsetRect(w + 10, 0); 
	if (!saveAllButton.IsWindow()) {
		normalImagePath = imgDir + L"analysis\\toolbar\\save-all-button-normal.png";
		pressedImagePath = imgDir + L"analysis\\toolbar\\save-all-button-pressed.png";
		saveAllButton.SetIconPath(normalImagePath, pressedImagePath);
		saveAllButton.SetBkgColors(topbarColor ,topbarHoverColor, topbarColor);
	}
	QWinCreater::createOrShowButton(m_hWnd, saveAllButton, Config::ANALYSIS_SAVE_ALL_BUTTON_ID, L"", rect, clientRect);
	saveAllButton.SetToolTip(S(L"save-all"));
}

void RightAnalysisView::loadWindow()
{
	if (!isNeedReload) {
		return ;
	}

	isNeedReload = false;
	loadTabViewPages();
}

void RightAnalysisView::loadTabViewPages()
{
 	tabView.AddPage(firstPage.m_hWnd, StringUtil::blkToTail(S(L"home")).c_str(), 0, firstPage.m_hWnd);
	tabView.SetActivePage(0);
// 	databaseSupplier->activeTabPageHwnd = firstQueryPage->m_hWnd;
}

int RightAnalysisView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	HINSTANCE ins = ModuleHelper::GetModuleInstance();
	m_hAccel = ::LoadAccelerators(ins, MAKEINTRESOURCE(RIGHT_ANALYSIS_VIEW_ACCEL));

	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_SHOW_SQL_LOG_PAGE_ID);
	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_ANALYSIS_SQL_ID);
	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_DB_STORE_ANALYSIS_ID);
	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_DB_PRAGMA_PARAMS_ID);
	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_ANALYSIS_ADD_PERF_REPORT_ID);
	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_ANALYSIS_SAVE_PERF_REPORT_ID);
	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_ANALYSIS_DROP_PERF_REPORT_ID);

	bkgBrush.CreateSolidBrush(bkgColor);
	topbarBrush.CreateSolidBrush(topbarColor);
	createImageList();
	return 0;
}

void RightAnalysisView::OnDestroy()
{
	AppContext::getInstance()->unsubscribe(m_hWnd, Config::MSG_SHOW_SQL_LOG_PAGE_ID);
	AppContext::getInstance()->unsubscribe(m_hWnd, Config::MSG_ANALYSIS_SQL_ID);
	AppContext::getInstance()->unsubscribe(m_hWnd, Config::MSG_DB_STORE_ANALYSIS_ID);
	AppContext::getInstance()->unsubscribe(m_hWnd, Config::MSG_DB_PRAGMA_PARAMS_ID);
	AppContext::getInstance()->unsubscribe(m_hWnd, Config::MSG_ANALYSIS_ADD_PERF_REPORT_ID);
	AppContext::getInstance()->unsubscribe(m_hWnd, Config::MSG_ANALYSIS_SAVE_PERF_REPORT_ID);
	AppContext::getInstance()->unsubscribe(m_hWnd, Config::MSG_ANALYSIS_DROP_PERF_REPORT_ID);

	if (!bkgBrush.IsNull()) bkgBrush.DeleteObject();
	if (!topbarBrush.IsNull()) topbarBrush.DeleteObject();
	if (firstIcon) ::DeleteObject(firstIcon);
	if (sqlLogIcon) ::DeleteObject(sqlLogIcon);
	if (perfReportIcon) ::DeleteObject(perfReportIcon);
	if (perfReportDirtyIcon) ::DeleteObject(perfReportDirtyIcon);
	if (storeAnalysisIcon) ::DeleteObject(storeAnalysisIcon);
	if (dbPragmaParamIcon) ::DeleteObject(dbPragmaParamIcon);
	if (dbQuikConfigIcon) ::DeleteObject(dbQuikConfigIcon);
	if (!imageList.IsNull()) imageList.Destroy();

	if (firstPage.IsWindow()) firstPage.DestroyWindow();
	if (sqlLogPage.IsWindow()) sqlLogPage.DestroyWindow();
	clearPerfAnalysisPagePtrs();

	if (tabView.IsWindow()) tabView.DestroyWindow();
	clearDbPragmaParamsPagePtrs();
}

void RightAnalysisView::OnSize(UINT nType, CSize size)
{
	createOrShowUI();
}

void RightAnalysisView::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if (!bShow) {
		return ;
	}
	loadWindow();
}

void RightAnalysisView::OnPaint(CDCHandle dc)
{
	CPaintDC pdc(m_hWnd);
	CMemoryDC mdc(pdc, pdc.m_ps.rcPaint);
	CRect clientRect;
	GetClientRect(clientRect);

	CRect topRect = getTopRect(clientRect);
	mdc.FillRect(topRect, topbarBrush.m_hBrush);

	CRect tabRect = getTabRect(clientRect);
	mdc.FillRect(tabRect, bkgBrush.m_hBrush);
}

LRESULT RightAnalysisView::OnTabViewCloseBtn(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
	int nPage = static_cast<int>(pnmh->idFrom);
	return closeTabViewPage(nPage); 
}

LRESULT RightAnalysisView::closeTabViewPage(int nPage)
{
	HWND pageHwnd = tabView.GetPageHWND(nPage);
	if (pageHwnd == firstPage.m_hWnd) {
		return 1;  // 1 - cancel close
	}

	if (pageHwnd == sqlLogPage.m_hWnd) {
		if (sqlLogPage.IsWindow()) {
			sqlLogPage.DestroyWindow();
		}
		return 0;
	}

	for (auto iter = perfAnalysisPagePtrs.begin(); iter != perfAnalysisPagePtrs.end(); iter++) {
		auto & ptr = *iter;
		if (ptr->m_hWnd == pageHwnd) {
			if (ptr && ptr->IsWindow()) {
				ptr->DestroyWindow();
				ptr->m_hWnd = NULL;
				delete ptr;
				ptr = nullptr;
			}
			perfAnalysisPagePtrs.erase(iter);
			break;
		}
	}

	for (auto iter = storeAnalysisPagePtrs.begin(); iter != storeAnalysisPagePtrs.end(); iter++) {
		auto & ptr = *iter;
		if (ptr->m_hWnd == pageHwnd) {
			if (ptr && ptr->IsWindow()) {
				ptr->DestroyWindow();
				ptr->m_hWnd = NULL;
				delete ptr;
				ptr = nullptr;
			}
			storeAnalysisPagePtrs.erase(iter);
			break;
		}
	}

	for (auto iter = dbPragmaParamsPagePtrs.begin(); iter != dbPragmaParamsPagePtrs.end(); iter++) {
		auto & ptr = *iter;
		if (ptr->m_hWnd == pageHwnd) {
			if (ptr && ptr->IsWindow()) {
				ptr->DestroyWindow();
				ptr->m_hWnd = NULL;
				delete ptr;
				ptr = nullptr;
			}
			dbPragmaParamsPagePtrs.erase(iter);
			break;
		}
	}

	return 0;  // 0 - force close
}


void RightAnalysisView::clearPerfAnalysisPagePtrs()
{
	for (auto ptr : perfAnalysisPagePtrs) {
		if (ptr && ptr->IsWindow()) {
			ptr->DestroyWindow();
			ptr->m_hWnd = NULL;
		}
		if (ptr) {
			delete ptr;
			ptr = nullptr;
		}
	}
	perfAnalysisPagePtrs.clear();
}


void RightAnalysisView::clearDbPragmaParamsPagePtrs()
{
	for (auto ptr : dbPragmaParamsPagePtrs) {
		if (ptr && ptr->IsWindow()) {
			ptr->DestroyWindow();
			ptr->m_hWnd = NULL;
		}
		if (ptr) {
			delete ptr;
			ptr = nullptr;
		}
	}
	dbPragmaParamsPagePtrs.clear();
}

BOOL RightAnalysisView::OnEraseBkgnd(CDCHandle dc)
{
	return TRUE;
}

HBRUSH RightAnalysisView::OnCtlColorStatic(HDC hdc, HWND hwnd)
{
	return bkgBrush.m_hBrush;
}


LRESULT RightAnalysisView::OnHandleAnalysisSql(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	uint64_t userDbId = static_cast<uint64_t>(wParam);
	uint64_t sqlLogId = static_cast<uint64_t>(lParam);
	if (!sqlLogId) {
		Q_ERROR(L"analysis failed, sqlLogId is empty");
		QPopAnimate::error(S(L"sql-log-id-empty"));
		return 0;
	}
	SqlLog sqlLog;
	try {
		sqlLog = sqlLogService->getSqlLog(sqlLogId);
	} catch (QRuntimeException &ex) {
		Q_ERROR(L"analysis failed, get sql log has error, code:{}, msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::report(ex);
		return 0;
	}
	if (sqlLog.sql.empty()) {
		Q_ERROR(L"analysis failed, sql log not found in database, sqlLogId:{}", sqlLogId);
		QPopAnimate::error(S(L"sql-log-not-found"));
		return 0;
	}

	if (!SqlUtil::isSelectSql(sqlLog.sql)) {
		QPopAnimate::error(S(L"not-select-sql"));
		return 0;
	}
	CRect clientRect;
	GetClientRect(clientRect);
	CRect tabRect = getTabRect(clientRect);
	
	for (auto ptr : perfAnalysisPagePtrs) {
		if (ptr->getSqlLogId() == sqlLogId) {
			int n = tabView.GetPageCount();
			for (int i = 0; i < n; i++) {
				if (tabView.GetPageHWND(i) != ptr->m_hWnd) {
					continue;
				}
				tabView.SetActivePage(i);
				return 0;
			}
		}
	}
	
	PerfAnalysisPage * newPage = new PerfAnalysisPage(sqlLogId);
	createOrShowPerfAnalysisPage(*newPage, clientRect, true);
	perfAnalysisPagePtrs.push_back(newPage);
	std::wstring title = S(L"sql-perf-report-prefix");
	title.append(std::to_wstring(sqlLogId)); 
	int nImage = 3;
	if (newPage->isSaved()) {
		nImage = 2;
	}
	tabView.AddPage(newPage->m_hWnd, StringUtil::blkToTail(title).c_str(), nImage, newPage->m_hWnd);
	
	return 0;
}


LRESULT RightAnalysisView::OnHandleDbStoreAnalysis(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	uint64_t userDbId = static_cast<uint64_t>(wParam);
	doAddDbStoreAnalysisPage(userDbId);
	return 0;
}


void RightAnalysisView::doAddDbStoreAnalysisPage(uint64_t userDbId)
{
	CRect clientRect;
	GetClientRect(clientRect);
	CRect tabRect = getTabRect(clientRect);

	UserDb userDb;
	try {
		userDb = databaseService->getUserDb(userDbId);
	}
	catch (QRuntimeException &ex) {
		Q_ERROR(L"analysis failed, get sql log has error, code:{}, msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::report(ex);
		return ;
	}

	for (auto ptr : storeAnalysisPagePtrs) {
		if (ptr->getUserDbId() == userDbId) {
			int n = tabView.GetPageCount();
			for (int i = 0; i < n; i++) {
				if (tabView.GetPageHWND(i) != ptr->m_hWnd) {
					continue;
				}
				tabView.SetActivePage(i);
				return ;
			}
		}
	}

	StoreAnalysisPage * newPage = new StoreAnalysisPage(userDbId);
	createOrShowStoreAnalysisPage(*newPage, clientRect, true);
	storeAnalysisPagePtrs.push_back(newPage);
	std::wstring title = S(L"store-analysis-prefix");
	title.append(userDb.name);
	int nImage = 4;
	tabView.AddPage(newPage->m_hWnd, StringUtil::blkToTail(title).c_str(), nImage, newPage->m_hWnd);
}


void RightAnalysisView::doAddDbPragmaParamsPage(uint64_t userDbId)
{
	CRect clientRect;
	GetClientRect(clientRect);
	CRect tabRect = getTabRect(clientRect);

	UserDb userDb;
	try {
		userDb = databaseService->getUserDb(userDbId);
	} catch (QRuntimeException &ex) {
		Q_ERROR(L"analysis failed, get sql log has error, code:{}, msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::report(ex);
		return ;
	}

	for (auto ptr : dbPragmaParamsPagePtrs) {
		if (ptr->getUserDbId() == userDbId) {
			int n = tabView.GetPageCount();
			for (int i = 0; i < n; i++) {
				if (tabView.GetPageHWND(i) != ptr->m_hWnd) {
					continue;
				}
				tabView.SetActivePage(i);
				return ;
			}
		}
	}

	DbPragmaParamsPage * newPage = new DbPragmaParamsPage(userDbId);
	createOrShowDbPragmaParamsPage(*newPage, clientRect, true);
	dbPragmaParamsPagePtrs.push_back(newPage);
	std::wstring title = userDb.name;
	title.append(L" : ").append(S(L"db-pragma-params"));
	int nImage = 5;
	tabView.AddPage(newPage->m_hWnd, StringUtil::blkToTail(title).c_str(), nImage, newPage->m_hWnd);
}

LRESULT RightAnalysisView::OnHandleDbPragmaParams(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	uint64_t userDbId = static_cast<uint64_t>(wParam);
	doAddDbPragmaParamsPage(userDbId);
	return 0;
}

LRESULT RightAnalysisView::OnHandleShowSqlLogPage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	doShowSqlLogPage();
	return 0;
}


LRESULT RightAnalysisView::OnClickAddSqlButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	std::wstring sql;
	addSqlToAnalysisPerfReport(sql);
	return 0;
}


LRESULT RightAnalysisView::OnClickSqlLogButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	doShowSqlLogPage();
	return 0;
}


LRESULT RightAnalysisView::OnClickSaveButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	int activePage = tabView.GetActivePage();
	HWND activePageHwnd = tabView.GetPageHWND(activePage);
	for (auto ptr : perfAnalysisPagePtrs) {
		if (ptr && ptr->IsWindow() && ptr->m_hWnd == activePageHwnd) {
			ptr->save();
			return 0;
		}
	}
	return 0;
}


LRESULT RightAnalysisView::OnClickSaveAllButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	for (auto ptr : perfAnalysisPagePtrs) {
		if (ptr && ptr->IsWindow()) {
			ptr->save();
		}
	}
	return 0;
}

LRESULT RightAnalysisView::OnHandleAnalysisAddPerfReport(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	std::wstring sql;
	if (wParam) {
		sql = *(std::wstring *)wParam;
	}
	addSqlToAnalysisPerfReport(sql);
	return 0;
}


LRESULT RightAnalysisView::OnHandleAnalysisSavePerfReport(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HWND hwnd = (HWND)wParam;
	uint64_t sqlLogId = static_cast<uint64_t>(lParam);
	if (!hwnd || !sqlLogId) {
		return 0;
	}

	int pageCount = tabView.GetPageCount();
	for (int i = 0; i < pageCount; ++i) {
		HWND pageHwnd = tabView.GetPageHWND(i);
		if (pageHwnd == hwnd) {
			tabView.SetPageImage(i, 2);
			break;
		}
	}
	size_t n = perfAnalysisPagePtrs.size();
	for (int i = 0; i < n; ++i) {
		auto ptr = perfAnalysisPagePtrs.at(i);
		if (ptr->m_hWnd == hwnd) {
			uint64_t sqlLogId = ptr->getSqlLogId();
		}
	}
	return 0;
}


LRESULT RightAnalysisView::OnHandleAnalysisDropPerfReport(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	uint64_t sqlLogId = static_cast<uint64_t>(lParam);
	if (!sqlLogId) {
		return 0;
	}

	size_t n = perfAnalysisPagePtrs.size();
	int nPageCount = tabView.GetPageCount();
	size_t nSelPage = -1;
	for (auto & iter = perfAnalysisPagePtrs.begin(); iter != perfAnalysisPagePtrs.end(); iter++) {
		auto ptr = *iter;
		if (ptr->getSqlLogId() != sqlLogId) {
			continue;
		}

		bool isFound = false;
		for (int j = 0; j < nPageCount; ++j) {
			if (ptr->m_hWnd == tabView.GetPageHWND(j)) {
				tabView.RemovePage(j);
				isFound = true;
				break;
			}
		}

		if (ptr && ptr->IsWindow()) {
			ptr->DestroyWindow();
			ptr->m_hWnd = nullptr;
			delete ptr;
			ptr = nullptr;
		}
		perfAnalysisPagePtrs.erase(iter);
		break;
	}
	
	return 0;
}

void RightAnalysisView::doShowSqlLogPage()
{
	CRect clientRect;
	GetClientRect(clientRect);
	CRect tabRect = getTabRect(clientRect);

	createOrShowSqlLogPage(sqlLogPage, clientRect, true);
	int n = tabView.GetPageCount();
	bool found = false;
	for (int i = 0; i < n; i++) {
		HWND pageHwnd = tabView.GetPageHWND(i);
		if (pageHwnd == sqlLogPage.m_hWnd) {
			tabView.SetActivePage(i);
			found = true;
			break;
		}
	}
	if (!found) {
		std::wstring title = StringUtil::blkToTail(S(L"sql-log"));
		tabView.AddPage(sqlLogPage.m_hWnd, title.c_str(), 1, sqlLogPage.m_hWnd);
	}
}

void RightAnalysisView::addSqlToAnalysisPerfReport(const std::wstring & sql)
{
	AddSqlDialog dialog(m_hWnd, sql);

	if (dialog.DoModal(m_hWnd) == Config::QDIALOG_YES_BUTTON_ID) {
	
	}
}

