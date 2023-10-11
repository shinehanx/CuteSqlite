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
	if (tableColumnsPage.IsWindow() && tableColumnsPage.PreTranslateMessage(pMsg)) {
		return TRUE;
	}

	return false;
}


void TableTabView::setup(uint64_t userDbId, const std::wstring & schema)
{
	this->userDbId = userDbId;
	this->schema = schema;
}

void TableTabView::createImageList()
{
	if (!imageList.IsNull()) {
		return;
	}
	std::wstring imgDir = ResourceUtil::getProductImagesDir();
	HINSTANCE ins = ModuleHelper::GetModuleInstance();
	
	columnBitmap = (HBITMAP)::LoadImageW(ins, (imgDir + L"database\\tab\\column.bmp").c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	indexBitmap = (HBITMAP)::LoadImageW(ins, (imgDir + L"database\\tab\\index.bmp").c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	imageList.Create(16, 16, ILC_COLOR32, 0, 4);
	imageList.Add(columnBitmap); // 0 - table
	imageList.Add(indexBitmap); // 1 - index
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
}


void TableTabView::createOrShowTabView(QTabView &win, CRect & clientRect)
{
	CRect rect = clientRect;
	if (IsWindow() && !win.IsWindow()) {
		win.Create(m_hWnd, rect, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, Config::DATABASE_TABLE_TAB_VIEW_ID);
		win.SetImageList(imageList);
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
		win.Create(tabView.m_hWnd, rect, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0);
		win.setup(userDbId, schema);
	} else if (IsWindow() && tabView.IsWindow()) {
		
		win.MoveWindow(rect);
		win.ShowWindow(true);
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
	tabView.AddPage(tableColumnsPage.m_hWnd, S(L"table-columns").c_str(), 0, &tableColumnsPage);
	//tabView.AddPage(tableIndexesPage.m_hWnd, S(L"index-columns").c_str(), 1, &tableIndexesPage);
	tabView.SetActivePage(0);
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

	if (columnBitmap) ::DeleteObject(columnBitmap);
	if (indexBitmap) ::DeleteObject(indexBitmap);	
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
