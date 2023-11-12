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

 * @file   ResultTabView.h
 * @brief  TabView of the query results
 * 
 * @author Xuehan Qin
 * @date   2023-05-22
 *********************************************************************/
#pragma once
#include <atlwin.h>
#include <atlcrack.h>
#include <atltypes.h>
#include "common/Config.h"
#include "ui/common/tabview/QTabView.h"
#include "ui/database/rightview/page/result/ResultListPage.h"
#include "ui/database/rightview/page/result/ResultInfoPage.h"
#include "ui/database/rightview/page/result/ResultTableDataPage.h"
#include "ui/database/rightview/page/supplier/QueryPageSupplier.h"
#include "ui/database/rightview/page/result/TablePropertiesPage.h"

class ResultTabView : public CWindowImpl<ResultTabView>
{
public:
	BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP_EX(ResultTabView)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_SIZE(OnSize)
		MSG_WM_SHOWWINDOW(OnShowWindow)
		MSG_WM_PAINT(OnPaint)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	void setup(QueryPageSupplier * supplier);
	HWND getActiveResultListPageHwnd();
	

	void clearResultListPage();
	void clearMessage();
	ResultListPage * addResultToListPage(std::wstring & sql, int tabNo);	
	void removeResultListPageFrom(int nSelectSqlCount);

	bool execSqlToInfoPage(const std::wstring & sql);
	int getPageIndex(HWND hwnd);
	void setActivePage(int pageIndex);
	void activeResultInfoPage();

	bool isActiveTableDataPage();
	void activeTableDataPage();
	
	void loadTableDatas(std::wstring & table);

	void activeTablePropertiesPage();
	
	ResultInfo & getRuntimeResultInfo() { return runtimeResultInfo; }
private:
	bool isNeedReload = true;
	

	COLORREF bkgColor = RGB(255, 255, 255);
	HBRUSH bkgBrush = nullptr;

	QTabView tabView;
	std::vector<ResultListPage *> resultListPagePtrs;
	
	ResultInfo runtimeResultInfo;
	ResultInfoPage resultInfoPage;
	ResultTableDataPage resultTableDataPage;
	TablePropertiesPage tablePropertiesPage;

	CImageList imageList;
	HICON resultIcon = nullptr;
	HICON infoIcon = nullptr;
	HICON tableDataIcon = nullptr;
	HICON objectIcon = nullptr;
	HICON tablePropertiesIcon = nullptr;

	SqlService * sqlService = SqlService::getInstance();
	QueryPageSupplier * supplier = nullptr;
	DatabaseSupplier * databaseSupplier = DatabaseSupplier::getInstance();

	void createImageList();

	CRect getTabRect(CRect & clientRect);
	CRect getPageRect(CRect & clientRect);

	void resetRuntimeResultInfo();

	void createOrShowUI();
	void createOrShowTabView(QTabView &win, CRect & clientRect);
	void createOrShowFirstResultListPage(CRect &clientRect);
	void createOrShowResultInfoPage(ResultInfoPage & win, CRect &clientRect);
	void createOrShowResultTableDataPage(ResultTableDataPage & win, CRect &clientRect);
	void createOrShowTablePropertiesPage(TablePropertiesPage & win, CRect &clientRect);

	void loadWindow();
	void loadTabViewPages();

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	int OnDestroy();
	void OnSize(UINT nType, CSize size);
	void OnShowWindow(BOOL bShow, UINT nStatus);
	void OnPaint(CDCHandle dc);
	BOOL OnEraseBkgnd(CDCHandle dc);
};
