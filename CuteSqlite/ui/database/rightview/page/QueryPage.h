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

 * @file   QueryPage.h
 * @brief  Query data editor page for TabView
 * 
 * @author Xuehan Qin
 * @date   2023-05-22
 *********************************************************************/
#pragma once
#include <atlsplit.h>
#include "ui/common/page/QPage.h"
#include "ui/database/rightview/page/result/ResultTabView.h"
#include "ui/common/edit/QHelpEdit.h"
#include "ui/database/supplier/DatabaseSupplier.h"

class QueryPage : public QPage {
public:
	BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP_EX(QueryPage)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MESSAGE_HANDLER(Config::MSG_TREEVIEW_CLICK_ID, OnClickTreeview)
		MESSAGE_HANDLER(Config::MSG_TREEVIEW_DBCLICK_ID, OnDbClickTreeview)
		CHAIN_MSG_MAP(QPage)
		FORWARD_NOTIFICATIONS()
	END_MSG_MAP()

	void setup(const std::wstring & tplPath, const std::wstring & content = std::wstring());

	QHelpEdit & getSqlEditor();
	ResultTabView & getResultTabView();
	void execAndShow();
private:
	std::wstring tplPath;
	std::wstring content;

	bool isSpliterReload = true;
	CHorSplitterWindow splitter;// Horizontal splitter
	QHelpEdit sqlEditor;
	
	ResultTabView resultTabView;

	DatabaseSupplier * supplier = DatabaseSupplier::getInstance();
	
	virtual void createOrShowUI();
	virtual void loadWindow();
	void loadSqlEditor();

	void createOrShowSplitter(CHorSplitterWindow & win, CRect & clientRect);
	void createOrShowSqlEditor(QHelpEdit & win, CRect & clientRect);
	void createOrShowResultTabView(ResultTabView & win, CRect & clientRect);

	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual int OnDestroy();

	// 单击LeftTreeView::treeView的选中项，发送该消息，接收方wParam为CTreeViewCtrlEx *指针, lParam 是HTREEITEM指针，接收方通过lParam获得需要的数据
	LRESULT OnClickTreeview(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	// 双击LeftTreeView::treeView的选中项，发送该消息，接收方wParam为CTreeViewCtrlEx *指针, lParam 是HTREEITEM指针，接收方通过lParam获得需要的数据
	LRESULT OnDbClickTreeview(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);	
	
};
