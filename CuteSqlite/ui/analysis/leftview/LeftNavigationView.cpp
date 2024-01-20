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

 * @file   LeftNavigationView.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-12-01
 *********************************************************************/
#include "stdafx.h"
#include "LeftNavigationView.h"
#include "common/Config.h"
#include "common/AppContext.h"
#include "core/common/Lang.h"
#include "utils/Log.h"
#include "utils/SqlUtil.h"
#include "core/common/exception/QSqlExecuteException.h"
#include "ui/common/message/QPopAnimate.h"
#include <ui/common/message/QMessageBox.h>

#define NAVIGATION_TOPBAR_HEIGHT 30

BOOL LeftNavigationView::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST) {
		if (m_hAccel && ::TranslateAccelerator(m_hWnd, m_hAccel, pMsg)) {
			return TRUE;
		}
	}
	return FALSE;
}

CRect LeftNavigationView::getTopRect(CRect & clientRect)
{
	return { 0, 0, clientRect.right, NAVIGATION_TOPBAR_HEIGHT };
}


CRect LeftNavigationView::getTreeRect(CRect & clientRect)
{
	return { 0, NAVIGATION_TOPBAR_HEIGHT, clientRect.right, clientRect.bottom };
}

void LeftNavigationView::createOrShowUI()
{
	CRect clientRect;
	GetClientRect(clientRect);

	createOrShowNavigationTreeView(navigationTreeView, clientRect);
}

void LeftNavigationView::createOrShowNavigationTreeView(QTreeViewCtrl & win, CRect & clientRect)
{
	CRect rect = getTreeRect(clientRect);
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		win.Create(m_hWnd, rect, L"", WS_CHILD | WS_VISIBLE  | WS_CLIPSIBLINGS | TVS_FULLROWSELECT 
			| TVS_LINESATROOT | TVS_SHOWSELALWAYS | TVS_HASBUTTONS | TVS_INFOTIP , WS_EX_CLIENTEDGE, Config::ANALYSIS_NAVIGATION_TREEVIEW_ID);
		
		// create a singleton adapter pointer
		adapter = new LeftNaigationViewAdapter(m_hWnd, &win);
		
		return;
	}
	else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		// show first then move window
		win.ShowWindow(SW_SHOW);
		win.MoveWindow(&rect);
	}
}


void LeftNavigationView::loadWindow()
{
	if (!isNeedReload) {
		return;
	}
	isNeedReload = false;

	adapter->loadTreeView();
}

int LeftNavigationView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	HINSTANCE ins = ModuleHelper::GetModuleInstance();
	m_hAccel = ::LoadAccelerators(ins, MAKEINTRESOURCE(LEFT_NAVIGATION_VIEW_ACCEL));

	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_ANALYSIS_SQL_ID);
	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_ANALYSIS_SAVE_PERF_REPORT_ID);

	bkgBrush.CreateSolidBrush(bkgColor);
	topbarBrush.CreateSolidBrush(topbarColor);
	titleFont = FTB(L"panel-list-header-size", true);
	return 0;
}

void LeftNavigationView::OnDestroy()
{
	AppContext::getInstance()->unsubscribe(m_hWnd, Config::MSG_ANALYSIS_SQL_ID);
	AppContext::getInstance()->unsubscribe(m_hWnd, Config::MSG_ANALYSIS_SAVE_PERF_REPORT_ID);

	if (!bkgBrush.IsNull()) bkgBrush.DeleteObject();
	if (!topbarBrush.IsNull()) topbarBrush.DeleteObject();
	if (!titleFont) ::DeleteObject(titleFont);

	if (adapter) {
		delete adapter;
		adapter = nullptr;
	}
}

void LeftNavigationView::OnSize(UINT nType, CSize size)
{
	createOrShowUI();
}

void LeftNavigationView::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if (!bShow) {
		return ;
	}
	
	loadWindow();
}

void LeftNavigationView::OnPaint(CDCHandle dc)
{
	CPaintDC pdc(m_hWnd);
	CMemoryDC mdc(pdc, pdc.m_ps.rcPaint);
	CRect clientRect;
	GetClientRect(clientRect);


	CRect topRect = getTopRect(clientRect);
	mdc.FillRect(topRect, topbarBrush.m_hBrush);

	CRect treeRect = getTreeRect(clientRect);
	mdc.FillRect(treeRect, bkgBrush.m_hBrush);

	CRect rect(10, 5, topRect.right - 10, NAVIGATION_TOPBAR_HEIGHT - 5);
	int oldMode = mdc.SetBkMode(TRANSPARENT);
	HFONT oldFont = mdc.SelectFont(titleFont);
	COLORREF oldColor = mdc.SetTextColor(titleColor);
	std::wstring text = S(L"analysis");
	mdc.DrawText(text.c_str(), static_cast<int>(text.size()), rect, DT_LEFT | DT_VCENTER);
	mdc.SetTextColor(oldColor);
	mdc.SelectFont(oldFont);
	mdc.SetBkMode(oldMode);
}

BOOL LeftNavigationView::OnEraseBkgnd(CDCHandle dc)
{
	return true;
}

LRESULT LeftNavigationView::OnDbClickTreeViewItem(int wParam, LPNMHDR lParam, BOOL& bHandled)
{
	Q_DEBUG(L"LeftNavigationView::OnDbClickTreeViewItem");
	auto ptr = (LPNMTREEVIEW)lParam;
	HTREEITEM hSelTreeItem = ptr->itemNew.hItem;
	if (!hSelTreeItem) {
		return 0;
	}

	CTreeItem treeItem = navigationTreeView.GetSelectedItem();
	int nImage = -1, nSeletedImage = -1;
	bool ret = treeItem.GetImage(nImage, nSeletedImage);

	if (nImage == 4) { // 4 - sql log
		AppContext::getInstance()->dispatch(Config::MSG_SHOW_SQL_LOG_PAGE_ID);
	} else if (nImage == 5 || nImage == 6) { // 5 - perf analysis report; 6 - perf analysis not save report
		uint64_t sqlLogId = static_cast<uint64_t>(treeItem.GetData());
		try {
			auto & sqlLog = sqlLogService->getSqlLog(sqlLogId);
			if (!sqlLog.id) {
				return 0;
			}
			AppContext::getInstance()->dispatch(Config::MSG_ANALYSIS_SQL_ID, WPARAM(sqlLog.userDbId), LPARAM(sqlLogId));
		} catch (QSqlExecuteException & ex) {
			QPopAnimate::report(ex);
			return 0;
		}
	} else if (nImage == 7) {
		uint64_t userDbId = static_cast<uint64_t>(treeItem.GetData());
		if (!userDbId) {
			return 0;
		}
		AppContext::getInstance()->dispatch(Config::MSG_DB_STORE_ANALYSIS_ID, WPARAM(userDbId));
	} else if (nImage == 9) {
		uint64_t userDbId = static_cast<uint64_t>(treeItem.GetData());
		if (!userDbId) {
			return 0;
		}
		AppContext::getInstance()->dispatch(Config::MSG_DB_PRAGMA_PARAMS_ID, WPARAM(userDbId));
	} else if (nImage == 10) {
		uint64_t userDbId = static_cast<uint64_t>(treeItem.GetData());
		if (!userDbId) {
			return 0;
		}
		AppContext::getInstance()->dispatch(Config::MSG_DB_QUICK_CONFIG_PARAMS_ID, WPARAM(userDbId));
	}
	
	return 0;
}


LRESULT LeftNavigationView::OnRightClickTreeViewItem(int wParam, LPNMHDR lParam, BOOL& bHandled)
{
	CPoint pt; 
	::GetCursorPos(&pt);
	CPoint pt2 = pt;
	navigationTreeView.ScreenToClient(&pt2);
	UINT uFlag;
	CTreeItem selItem = navigationTreeView.HitTest(pt2, &uFlag);

	if (!selItem.IsNull()) {
		navigationTreeView.SelectItem(selItem.m_hTreeItem);

		int nImage = -1, nSeletedImage = -1;
		bool ret = selItem.GetImage(nImage, nSeletedImage);
	
		if (nImage == 5 || nImage == 6) { // 5
			adapter->popupPerfReportMenu(pt);
		}
	}
	return 0;
}

/**
 * Handle Config::MSG_ANALYSIS_SQL_ID message from SqlLogPage
 * 
 * @param uMsg
 * @param wParam - userDbId
 * @param lParam - sqlLogId
 * @return 
 */
LRESULT LeftNavigationView::OnHandleAnalysisSql(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	uint64_t userDbId = static_cast<uint64_t>(wParam);
	uint64_t sqlLogId = static_cast<uint64_t>(lParam);
	
	adapter->addPerfAnalysisReport(userDbId, sqlLogId);

	return 0;
}

LRESULT LeftNavigationView::OnHandleAnalysisSavePerfReport(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	uint64_t sqlLogId = static_cast<uint64_t>(lParam);
	adapter->savePerfAnalysisReport(sqlLogId);

	return 0;
}


void LeftNavigationView::OnClickOpenPerfReportMenu(UINT uNotifyCode, int nID, HWND hwnd)
{
	CTreeItem reportTreeItem = navigationTreeView.GetSelectedItem();
	if (reportTreeItem.IsNull()) {
		return;
	}
	uint64_t sqlLogId = static_cast<uint64_t>(reportTreeItem.GetData());
	adapter->openPerfAnalysisReport(sqlLogId);
}

void LeftNavigationView::OnClickDropPerfReportMenu(UINT uNotifyCode, int nID, HWND hwnd)
{
	CTreeItem reportTreeItem = navigationTreeView.GetSelectedItem();
	if (reportTreeItem.IsNull()) {
		return;
	}
	if (QMessageBox::confirm(m_hWnd, S(L"drop-perf-report-confirm-text")) != Config::CUSTOMER_FORM_YES_BUTTON_ID) {
		return ;
	}

	uint64_t sqlLogId = static_cast<uint64_t>(reportTreeItem.GetData());
	adapter->dropPerfAnalysisReport(sqlLogId);

	navigationTreeView.DeleteItem(reportTreeItem);
}

