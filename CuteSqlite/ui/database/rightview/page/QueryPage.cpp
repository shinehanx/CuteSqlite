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

 * @file   QueryPage.cpp
 * @brief  Query data editor page for TabView
 * 
 * @author Xuehan Qin
 * @date   2023-05-22
 *********************************************************************/
#include "stdafx.h"
#include "QueryPage.h"
#include <string>
#include <atlctrls.h>
#include "utils/SqlUtil.h"
#include "utils/Log.h"
#include "common/AppContext.h"
#include "core/common/Lang.h"
#include "ui/common/message/QPopAnimate.h"
#include "ui/database/leftview/adapter/LeftTreeViewAdapter.h"

BOOL QueryPage::PreTranslateMessage(MSG* pMsg)
{
	if (sqlEditor.IsWindow() && sqlEditor.PreTranslateMessage(pMsg)) {
		return TRUE;
	}

	if (resultTabView.IsWindow() && resultTabView.PreTranslateMessage(pMsg)) {
		return TRUE;
	}

	return FALSE;
}

QHelpEdit & QueryPage::getSqlEditor()
{
	return sqlEditor;
}

ResultTabView & QueryPage::getResultTabView()
{
	return resultTabView;
}

void QueryPage::execAndShow()
{
	std::wstring sqls = getSqlEditor().getText();
	if (sqls.empty()) {
		QPopAnimate::warn(m_hWnd, S(L"no-sql-statement"));
		sqlEditor.focus();
		return;
	}

	supplier->splitToSqlVector(sqls);
	resultTabView.clearResultListPage();
	resultTabView.clearMessage();
	std::vector<std::wstring> & sqlVector = supplier->sqlVector;
	int n = static_cast<int>(sqlVector.size());
	for (int i = 0; i < n; i++) {
		auto sql = sqlVector.at(i);
		if (SqlUtil::isSelectSql(sql)) {
			resultTabView.addResultListPage(sql, i+1);
		}
	}
	if (n) {
		resultTabView.setActivePage(0);
	}
}

void QueryPage::createOrShowUI()
{
	QPage::createOrShowUI();

	CRect clientRect;
	GetClientRect(clientRect);
	createOrShowSplitter(splitter, clientRect);
	createOrShowSqlEditor(sqlEditor, clientRect);
	createOrShowResultTabView(resultTabView, clientRect);

	if (isNeedReload) {
		isNeedReload = false;
		splitter.SetSplitterPane(0, sqlEditor, true);
		splitter.SetSplitterPane(1, resultTabView, false);
	}
}


void QueryPage::loadWindow()
{
	QPage::loadWindow();

}


void QueryPage::createOrShowSplitter(CHorSplitterWindow & win, CRect & clientRect)
{
	CRect & rect = clientRect;
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		win.Create(m_hWnd, rect, L"", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
		win.m_cxySplitBar = 2;
		return;
	}
	else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		// show first then move window
		win.ShowWindow(SW_SHOW);
		win.MoveWindow(&rect);
		if (win.GetSplitterPos() == 4)
			win.SetSplitterPos(clientRect.Height() * 1 / 3);
	}
}


void QueryPage::createOrShowSqlEditor(QHelpEdit & win, CRect & clientRect)
{
	CRect rect(0, 0, 1, 1);
	Q_INFO(L"QueryPage,clientRect.w{}:{},clientRect.h:{}", clientRect.Width(), clientRect.Height());
	if (::IsWindow(splitter.m_hWnd) && !win.IsWindow()) {
		win.setup(S(L"query-page-help"), std::wstring(L""));
		win.Create(splitter.m_hWnd, rect, L"", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, Config::DATABASE_QUERY_EDITOR_ID);
		return;
	}
}


void QueryPage::createOrShowResultTabView(ResultTabView & win, CRect & clientRect)
{
	CRect rect(0, 0, 1, 1);
	if (::IsWindow(splitter.m_hWnd) && !win.IsWindow()) {
		win.Create(splitter.m_hWnd, rect, L"", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
		return;
	}
}

int QueryPage::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	bool ret = QPage::OnCreate(lpCreateStruct);
	AppContext::getInstance()->subscribe(m_hWnd,  Config::MSG_TREEVIEW_CLICK_ID);
	AppContext::getInstance()->subscribe(m_hWnd,  Config::MSG_TREEVIEW_DBCLICK_ID);
	return ret;
}

int QueryPage::OnDestroy()
{
	AppContext::getInstance()->unsuscribe(m_hWnd,  Config::MSG_TREEVIEW_CLICK_ID);
	AppContext::getInstance()->unsuscribe(m_hWnd,  Config::MSG_TREEVIEW_DBCLICK_ID);

	bool ret = QPage::OnDestroy();
	if (sqlEditor.IsWindow()) sqlEditor.DestroyWindow();
	if (resultTabView.IsWindow()) resultTabView.DestroyWindow();
	if (splitter.IsWindow()) splitter.DestroyWindow();

	return ret;
}

LRESULT QueryPage::OnClickTreeview(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LeftTreeViewAdapter * treeViewAdapter = (LeftTreeViewAdapter *)wParam;
	HTREEITEM hSelTreeItem = (HTREEITEM)lParam;
	if (!hSelTreeItem) {
		return 0;
	}

	CTreeItem treeItem = treeViewAdapter->getSeletedItem();
	wchar_t cch[512] = { 0 };
	bool ret = treeItem.GetText(cch, 512);
	if (!ret) {
		return 0;
	}

	int nImage = -1, nSeletedImage = -1;
	ret = treeItem.GetImage(nImage, nSeletedImage);
	if (!ret || nImage == -1) {
		return 0;
	}

	std::wstring selItemText(cch);
	if (nImage == 2) { // 2 - table
		supplier->selectTable = selItemText;
	}

	if (resultTabView.isActiveTableDataPage()) {
		resultTabView.loadTableDatas(supplier->selectTable);
	}
	return 0;
}

/**
 * 双击LeftTreeView::treeView的选中项，发送该消息，接收方wParam为CTreeViewCtrlEx *指针, lParam 是HTREEITEM指针，接收方通过lParam获得需要的数据.
 * 
 * @param uMsg
 * @param wParam pointer of CTreeViewCtrlEx *
 * @param lParam pointer of HTREEITEM
 * @param bHandled
 * @return 
 */
LRESULT QueryPage::OnDbClickTreeview(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LeftTreeViewAdapter * treeViewAdapter = (LeftTreeViewAdapter *)wParam;
	HTREEITEM hSelTreeItem = (HTREEITEM)lParam;
	if (!hSelTreeItem) {
		return 0;
	}

	CTreeItem treeItem = treeViewAdapter->getSeletedItem();
	wchar_t cch[512] = { 0 };
	bool ret = treeItem.GetText(cch, 512);
	if (!ret) {
		return 0;
	}

	int nImage = -1, nSeletedImage = -1;
	ret = treeItem.GetImage(nImage, nSeletedImage);
	if (!ret || nImage == -1) {
		return 0;
	}

	std::wstring selItemText(cch);
	if (nImage == 3) { // 3 - column
		std::wstring fieldName = SqlUtil::getColumnName(selItemText);
		sqlEditor.replaceSelText(fieldName);
	} else {
		sqlEditor.replaceSelText(selItemText);
	}
	


	return 0;
}
