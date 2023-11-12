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

 * @file   TableTabView.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-10-10
 *********************************************************************/
#include "stdafx.h"
#include "TableTabView.h"
#include "utils/ResourceUtil.h"
#include "core/common/Lang.h"

BOOL TableTabView::PreTranslateMessage(MSG* pMsg)
{
	BOOL result = FALSE;
	if (tableColumnsPage.IsWindow() && tableColumnsPage.PreTranslateMessage(pMsg)) {
		return TRUE;
	}

	if (tableIndexesPage.IsWindow() && tableIndexesPage.PreTranslateMessage(pMsg)) {
		return TRUE;
	}

	if (tableForeinkeysPage.IsWindow() && tableForeinkeysPage.PreTranslateMessage(pMsg)) {
		return TRUE;
	}

	return false;
}

TableColumnsPage & TableTabView::getTableColumnsPage()
{
	ATLASSERT(tableColumnsPage.IsWindow());
	return tableColumnsPage;
}


TableIndexesPage & TableTabView::getTableIndexesPage()
{
	ATLASSERT(tableIndexesPage.IsWindow());
	return tableIndexesPage;
}


TableForeignkeysPage & TableTabView::getTableForeignkeysPage()
{
	ATLASSERT(tableForeinkeysPage.IsWindow());
	return tableForeinkeysPage;
}


void TableTabView::activePage(TableStructurePageType pageType)
{
	HWND activeHwnd = nullptr;
	CRect clientRect;
	GetClientRect(clientRect);
	supplier->setActivePageType(pageType);

	if (pageType == TABLE_COLUMNS_PAGE) {
		if (!tableColumnsPage.IsWindow()) {
			createOrShowTableColumnsPage(tableColumnsPage, clientRect);
			tabView.AddPage(tableColumnsPage.m_hWnd, StringUtil::blkToTail(S(L"table-columns")).c_str(), 0, &tableColumnsPage);
		}
		activeHwnd = tableColumnsPage.m_hWnd;
	} else if (pageType == TABLE_INDEXS_PAGE) {
		if (!tableIndexesPage.IsWindow()) {
			createOrShowTableIndexesPage(tableIndexesPage, clientRect);
			tabView.AddPage(tableIndexesPage.m_hWnd, StringUtil::blkToTail(S(L"table-indexes")).c_str(), 1, &tableIndexesPage);
		}
		activeHwnd = tableIndexesPage.m_hWnd;
	} else if (pageType == TABLE_FOREIGN_KEYS_PAGE) {
		if (!tableForeinkeysPage.IsWindow()) {
			createOrShowTableForeignkeysPage(tableForeinkeysPage, clientRect);
			tabView.AddPage(tableForeinkeysPage.m_hWnd, StringUtil::blkToTail(S(L"table-foreignkeys")).c_str(), 2, &tableForeinkeysPage);
		}
		activeHwnd = tableForeinkeysPage.m_hWnd;
	}
	if (!activeHwnd && !::IsWindow(activeHwnd)) {
		return;
	}
	
	int i = 0;
	int n = tabView.GetPageCount();
	for (int i = 0; i < n; i++) {
		if (tabView.GetPageHWND(i) == activeHwnd) {
			tabView.SetActivePage(i);
		}
	}
}

void TableTabView::createImageList()
{
	if (!imageList.IsNull()) {
		return;
	}
	std::wstring imgDir = ResourceUtil::getProductImagesDir();
	HINSTANCE ins = ModuleHelper::GetModuleInstance();
	
	columnIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\tab\\column.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	indexIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\tab\\index.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	foreignkeyIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\tab\\foreignkey.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);

	imageList.Create(16, 16, ILC_COLOR32, 0, 3);
	imageList.AddIcon(columnIcon); // 0 - table
	imageList.AddIcon(indexIcon); // 1 - index
	imageList.AddIcon(foreignkeyIcon); // 2 - foreign key
}

CRect TableTabView::getTabRect(CRect & clientRect)
{
	return { 0, 0, clientRect.right, tabView.m_cyTabHeight };
}

CRect TableTabView::getPageRect(CRect & clientRect)
{
	return { 0, tabView.m_cyTabHeight, clientRect.right, clientRect.bottom };
}


void TableTabView::createOrShowUI()
{
	CRect clientRect;
	GetClientRect(clientRect);
	createOrShowTabView(tabView, clientRect);
	createOrShowTableColumnsPage(tableColumnsPage, clientRect);
	createOrShowTableIndexesPage(tableIndexesPage, clientRect);
	createOrShowTableForeignkeysPage(tableForeinkeysPage, clientRect);
}


void TableTabView::createOrShowTabView(QTabView &win, CRect & clientRect)
{
	CRect rect = clientRect;
	if (IsWindow() && !win.IsWindow()) {
		win.Create(m_hWnd, rect, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, Config::DATABASE_TABLE_TAB_VIEW_ID);
		win.SetImageList(imageList);
		win.enableCloseBtn(false);
	}
	else if (IsWindow() && tabView.IsWindow()) {
		win.MoveWindow(rect);
		win.ShowWindow(true);
	}
}

void TableTabView::createOrShowTableColumnsPage(TableColumnsPage & win, CRect &clientRect)
{
	CRect pageRect = getPageRect(clientRect);
	int x = 1, y = pageRect.top + 1, w = pageRect.Width() - 2, h = pageRect.Height()  - 2;
	CRect rect(x, y, x + w, y + h);
	if (IsWindow() && !win.IsWindow()) {
		win.setup(supplier);
		win.Create(tabView.m_hWnd, rect, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0);
	} else if (IsWindow() && tabView.IsWindow()) {		
		win.MoveWindow(rect);
		win.ShowWindow(true);
	}
}


void TableTabView::createOrShowTableIndexesPage(TableIndexesPage & win, CRect &clientRect)
{
	CRect pageRect = getPageRect(clientRect);
	int x = 1, y = pageRect.top + 1, w = pageRect.Width() - 2, h = pageRect.Height()  - 2;
	CRect rect(x, y, x + w, y + h);
	if (IsWindow() && !win.IsWindow()) {
		win.setup(tableColumnsPage.getAdapter(), supplier);
		win.Create(tabView.m_hWnd, rect, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0);		
	} else if (IsWindow() && tabView.IsWindow()) {
		win.MoveWindow(rect);
		win.ShowWindow(true);
		if (win.getTblColumnsPageAdapter() == nullptr) {
			win.setTblColumnsPageAdapter(tableColumnsPage.getAdapter());
		}
		if (win.getSupplier() == nullptr) {
			win.setSupplier(supplier);
		}
	}
}


void TableTabView::createOrShowTableForeignkeysPage(TableForeignkeysPage & win, CRect &clientRect)
{
	CRect pageRect = getPageRect(clientRect);
	int x = 1, y = pageRect.top + 1, w = pageRect.Width() - 2, h = pageRect.Height()  - 2;
	CRect rect(x, y, x + w, y + h);
	if (IsWindow() && !win.IsWindow()) {
		win.setup(tableColumnsPage.getAdapter(), supplier);
		win.Create(tabView.m_hWnd, rect, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0);		
	} else if (IsWindow() && tabView.IsWindow()) {
		win.MoveWindow(rect);
		win.ShowWindow(true);
		if (win.getTblColumnsPageAdapter() == nullptr) {
			win.setTblColumnsPageAdapter(tableColumnsPage.getAdapter());
		}
		if (win.getSupplier() == nullptr) {
			win.setSupplier(supplier);
		}
	}
}

void TableTabView::loadWindow()
{
	if (!isNeedReload) {
		return;
	}

	isNeedReload = false;
	loadTabViewPages();
}


void TableTabView::loadTabViewPages()
{
	tabView.AddPage(tableColumnsPage.m_hWnd, StringUtil::blkToTail(S(L"table-columns")).c_str(), 0, &tableColumnsPage);
	tabView.AddPage(tableIndexesPage.m_hWnd, StringUtil::blkToTail(S(L"table-indexes")).c_str(), 1, &tableIndexesPage);
	tabView.AddPage(tableForeinkeysPage.m_hWnd, StringUtil::blkToTail(S(L"table-foreignkeys")).c_str(), 2, &tableForeinkeysPage);

	tabView.SetActivePage((int)supplier->getActivePageType());
}

int TableTabView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	bkgBrush = ::CreateSolidBrush(bkgColor);

	createImageList();

	return 0;
}

int TableTabView::OnDestroy()
{
	if (bkgBrush) ::DeleteObject(bkgBrush);

	if (tabView.IsWindow()) tabView.DestroyWindow();
	if (tableColumnsPage.IsWindow()) tableColumnsPage.DestroyWindow();
	if (tableIndexesPage.IsWindow()) tableIndexesPage.DestroyWindow();

	if (columnIcon) ::DeleteObject(columnIcon);
	if (indexIcon) ::DeleteObject(indexIcon);	
	return 0;
}

void TableTabView::OnSize(UINT nType, CSize size)
{
	createOrShowUI();
}

void TableTabView::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if (!bShow) {
		return;
	}
	loadWindow();
}

void TableTabView::OnPaint(CDCHandle dc)
{
	CPaintDC pdc(m_hWnd);
	pdc.FillRect(&(pdc.m_ps.rcPaint), bkgBrush);
}

BOOL TableTabView::OnEraseBkgnd(CDCHandle dc)
{
	return true;
}

LRESULT TableTabView::OnTableColumsChangePrimaryKey(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	::PostMessage(tableIndexesPage.m_hWnd, Config::MSG_TABLE_COLUMNS_CHANGE_PRIMARY_KEY_ID, wParam, lParam);
	return 0;
}

LRESULT TableTabView::OnTableColumsDeleteColumnName(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	::PostMessage(tableIndexesPage.m_hWnd, Config::MSG_TABLE_COLUMNS_DELETE_COLUMN_NAME_ID, wParam, lParam);
	::PostMessage(tableForeinkeysPage.m_hWnd, Config::MSG_TABLE_COLUMNS_DELETE_COLUMN_NAME_ID, wParam, lParam);
	return 0;
}
