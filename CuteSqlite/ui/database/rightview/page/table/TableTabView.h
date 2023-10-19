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

 * @file   TableTabView.h
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-10-10
 *********************************************************************/
#pragma once
#include <atlwin.h>
#include <atlcrack.h>
#include <atltypes.h>
#include "common/Config.h"
#include "ui/common/tabview/QTabView.h"
#include "ui/database/rightview/page/table/TableColumnsPage.h"
#include "ui/database/rightview/page/table/TableIndexesPage.h"

class TableTabView : public CWindowImpl<TableTabView>
{
public:
	BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP_EX(TableTabView)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_SIZE(OnSize)
		MSG_WM_SHOWWINDOW(OnShowWindow)
		MSG_WM_PAINT(OnPaint)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	void setup(uint64_t userDbId, const std::wstring & schema = L"");

	TableColumnsPage & getTableColumnsPage();
	TableIndexesPage & getTableIndexesPage();
private:
	bool isNeedReload = true;

	uint64_t userDbId = 0;
	std::wstring schema;

	COLORREF bkgColor = RGB(255, 255, 255);
	HBRUSH bkgBrush = nullptr;

	QTabView tabView;
	TableColumnsPage tableColumnsPage;
	TableIndexesPage tableIndexesPage;

	CImageList imageList;
	HBITMAP columnBitmap = nullptr;
	HBITMAP indexBitmap = nullptr;

	void createImageList();

	CRect getTabRect(CRect & clientRect);
	CRect getPageRect(CRect & clientRect);

	void createOrShowUI();
	void createOrShowTabView(QTabView &win, CRect & clientRect);
	void createOrShowTableColumnsPage(TableColumnsPage & win, CRect &clientRect);
	void createOrShowTableIndexesPage(TableIndexesPage & win, CRect &clientRect);

	void loadWindow();
	void loadTabViewPages();

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	int OnDestroy();
	void OnSize(UINT nType, CSize size);
	void OnShowWindow(BOOL bShow, UINT nStatus);
	void OnPaint(CDCHandle dc);
	BOOL OnEraseBkgnd(CDCHandle dc);
};
