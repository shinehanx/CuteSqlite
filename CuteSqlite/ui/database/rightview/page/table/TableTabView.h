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
#include "ui/database/rightview/page/table/TableForeignkeysPage.h"
#include "ui/database/rightview/page/supplier/TableStructureSupplier.h"

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
		MESSAGE_HANDLER(Config::MSG_TABLE_COLUMNS_CHANGE_PRIMARY_KEY_ID, OnTableColumsChangePrimaryKey);		
		MESSAGE_HANDLER(Config::MSG_TABLE_COLUMNS_DELETE_COLUMN_NAME_ID, OnTableColumsDeleteColumnName);
		MESSAGE_HANDLER(Config::MSG_TABLE_STRUCTURE_DIRTY_ID, OnHandleTableStructDirty);
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
	
	~TableTabView();
	TableColumnsPage & getTableColumnsPage();
	TableIndexesPage & getTableIndexesPage();
	TableForeignkeysPage & getTableForeignkeysPage();

	TableStructureSupplier * getSupplier() const { return supplier; }
	void setSupplier(TableStructureSupplier * val) { supplier = val; }
	void activePage(TableStructurePageType pageType);
	void refreshDirtyAfterSave();
private:
	bool isNeedReload = true;

	COLORREF bkgColor = RGB(255, 255, 255);
	CBrush bkgBrush;

	QTabView tabView;
	TableColumnsPage tableColumnsPage;
	TableIndexesPage tableIndexesPage;
	TableForeignkeysPage tableForeinkeysPage;

	CImageList imageList;
	HICON columnIcon = nullptr;
	HICON indexIcon = nullptr;
	HICON foreignkeyIcon = nullptr;
	HICON columnDirtyIcon = nullptr;
	HICON indexDirtyIcon = nullptr;
	HICON foreignkeyDirtyIcon = nullptr;

	TableStructureSupplier * supplier = nullptr;

	void createImageList();

	CRect getTabRect(CRect & clientRect);
	CRect getPageRect(CRect & clientRect);

	void createOrShowUI();
	void createOrShowTabView(QTabView &win, CRect & clientRect);
	void createOrShowTableColumnsPage(TableColumnsPage & win, CRect &clientRect);
	void createOrShowTableIndexesPage(TableIndexesPage & win, CRect &clientRect);
	void createOrShowTableForeignkeysPage(TableForeignkeysPage & win, CRect &clientRect);

	void loadWindow();
	void loadTabViewPages();

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	int OnDestroy();
	void OnSize(UINT nType, CSize size);
	void OnShowWindow(BOOL bShow, UINT nStatus);
	void OnPaint(CDCHandle dc);
	BOOL OnEraseBkgnd(CDCHandle dc);

	LRESULT OnTableColumsChangePrimaryKey(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTableColumsDeleteColumnName(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnHandleTableStructDirty(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};
