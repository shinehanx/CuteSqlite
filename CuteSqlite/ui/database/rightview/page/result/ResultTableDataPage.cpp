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

 * @file   ResultTableDataPage.cpp
 * @brief  Select the table and then show the list of table data
 * 
 * @author Xuehan Qin
 * @date   2023-05-22
 *********************************************************************/

#include "stdafx.h"
#include "ResultTableDataPage.h"

ResultTableDataPage::ResultTableDataPage()
{
	settingPrefix = TABLE_DATA_SETTING_PREFIX;
}

void ResultTableDataPage::setup(std::wstring & table)
{
	this->table = table;
	// genderate query table sql
	this->sql.assign(L"SELECT * FROM ").append(table);

}

void ResultTableDataPage::loadTableDatas()
{
	if (table.empty() || sql.empty()) {
		return ;
	}
	loadListView();
}

void ResultTableDataPage::createOrShowUI()
{
	ResultListPage::createOrShowUI();

	CRect clientRect;
	GetClientRect(clientRect);

}

void ResultTableDataPage::loadWindow()
{
	ResultListPage::loadWindow();
}

/**
 * Create or show the list view 
 * Call by ResultListPage::createOrShowUI().
 * 
 * @param win
 * @param clientRect
 */
void ResultTableDataPage::createOrShowListView(CListViewCtrl & win, CRect & clientRect)
{
	CRect & rect = getLeftListRect(clientRect);
	if (IsWindow() && !win.IsWindow()) {
		win.Create(m_hWnd, rect,NULL, 
			WS_CHILD | WS_TABSTOP | WS_VISIBLE | LVS_ALIGNLEFT | LVS_REPORT | LVS_EDITLABELS | WS_BORDER | LVS_OWNERDATA , // | LVS_OWNERDATA
			0, Config::DATABASE_QUERY_LISTVIEW_ID );
		win.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER );
		win.SetImageList(imageList, LVSIL_SMALL);
		CHeaderCtrl header = win.GetHeader();
		header.SetImageList(imageList);
		adapter = new ResultListPageAdapter(m_hWnd, &win, TABLE_DATA);
	} else if (IsWindow() && win.IsWindow() && clientRect.Width() > 1) {
		win.MoveWindow(rect);
		win.ShowWindow(true);
	}
}



int ResultTableDataPage::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	bool ret = ResultListPage::OnCreate(lpCreateStruct);
	return ret;
}

int ResultTableDataPage::OnDestroy()
{
	bool ret = ResultListPage::OnDestroy();

	return ret;
}