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

 * @file   LeftTreeView.cpp
 * @brief  Left tree view for splitter
 * 
 * @author Xuehan Qin
 * @date   2023-05-19
 *********************************************************************/
#include "stdafx.h"
#include "LeftTreeView.h"
#include "utils/Log.h"
#include "core/common/Lang.h"
#include "core/common/exception/QRuntimeException.h"
#include "ui/common/message/QPopAnimate.h"
#include "ui/common/message/QMessageBox.h"

#define TREEVIEW_TOPBAR_HEIGHT 30
#define TREEVIEW_BUTTON_WIDTH 16
#define TREEVIEW_COMBOBOX_WIDTH 135
#define TREEVIEW_COMBOBOX_HEIGHT 20

CRect LeftTreeView::getTopRect(CRect & clientRect)
{
	return { 0, 0, clientRect.right, TREEVIEW_TOPBAR_HEIGHT };
}

CRect LeftTreeView::getTreeRect(CRect & clientRect)
{
	return { 0, TREEVIEW_TOPBAR_HEIGHT, clientRect.right, clientRect.bottom };
}

void LeftTreeView::createOrShowUI()
{
	CRect clientRect;
	GetClientRect(clientRect);
	createOrShowCreateDbButton(createDbButton, clientRect);
	createOrShowOpenDbButton(openDbButton, clientRect);
	createOrShowRefreshDbButton(refreshDbButton, clientRect);
	createOrShowDeleteDbButton(deleteDbButton, clientRect);
	createOrShowTreeView(treeView, clientRect);
	createOrShowSelectedDbComboBox(seletedDbComboBox, clientRect);
}


void LeftTreeView::createOrShowCreateDbButton(QImageButton & win, CRect &clientRect)
{
	CRect rect = { 10, 7, 10 + TREEVIEW_BUTTON_WIDTH, 7 + TREEVIEW_BUTTON_WIDTH };
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		std::wstring normalImagePath = ResourceUtil::getProductImagesDir() + L"database\\button\\create-button-normal.png";
		std::wstring pressedImagePath = ResourceUtil::getProductImagesDir() + L"database\\button\\create-button-pressed.png";
		win.SetIconPath(normalImagePath, pressedImagePath);
		win.SetBkgColors(RGB(238, 238, 238), RGB(238, 238, 238), RGB(238, 238, 238));
		win.Create(m_hWnd, rect, L"", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | BS_OWNERDRAW, 0, Config::DATABASE_CREATE_BUTTON_ID);
		win.SetToolTip(S(L"create-database"));
		return;
	}
	else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
	}
}

void LeftTreeView::createOrShowOpenDbButton(QImageButton & win, CRect &clientRect)
{
	int x = 10 + TREEVIEW_BUTTON_WIDTH + 7, y = 7, w = TREEVIEW_BUTTON_WIDTH, h = TREEVIEW_BUTTON_WIDTH;
	CRect rect(x, y, x + w, y + h);
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		std::wstring normalImagePath = ResourceUtil::getProductImagesDir() + L"database\\button\\open-button-normal.png";
		std::wstring pressedImagePath = ResourceUtil::getProductImagesDir() + L"database\\button\\open-button-pressed.png";
		win.SetIconPath(normalImagePath, pressedImagePath);
		win.SetBkgColors(RGB(238, 238, 238), RGB(238, 238, 238), RGB(238, 238, 238));
		win.Create(m_hWnd, rect, L"", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | BS_OWNERDRAW, 0, Config::DATABASE_OPEN_BUTTON_ID);
		win.SetToolTip(S(L"open-database"));
		return;
	}
	else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
	}
}

void LeftTreeView::createOrShowRefreshDbButton(QImageButton & win, CRect &clientRect)
{
	int x = 10 + (TREEVIEW_BUTTON_WIDTH + 7) * 2, y = 7, w = TREEVIEW_BUTTON_WIDTH, h = TREEVIEW_BUTTON_WIDTH;
	CRect rect(x, y, x + w, y + h);
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		std::wstring normalImagePath = ResourceUtil::getProductImagesDir() + L"database\\button\\refresh-button-normal.png";
		std::wstring pressedImagePath = ResourceUtil::getProductImagesDir() + L"database\\button\\refresh-button-pressed.png";
		win.SetIconPath(normalImagePath, pressedImagePath);
		win.SetBkgColors(RGB(238, 238, 238), RGB(238, 238, 238), RGB(238, 238, 238));
		win.Create(m_hWnd, rect, L"", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | BS_OWNERDRAW, 0, Config::DATABASE_REFRESH_BUTTON_ID);
		win.SetToolTip(S(L"refresh-database"));
		return;
	}
	else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
	}
}

void LeftTreeView::createOrShowDeleteDbButton(QImageButton & win, CRect &clientRect)
{
	int x = 10 + (TREEVIEW_BUTTON_WIDTH + 7) * 3 , y = 7, w = TREEVIEW_BUTTON_WIDTH, h = TREEVIEW_BUTTON_WIDTH;
	CRect rect(x, y, x + w, y + h);
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		std::wstring normalImagePath = ResourceUtil::getProductImagesDir() + L"database\\button\\delete-button-normal.png";
		std::wstring pressedImagePath = ResourceUtil::getProductImagesDir() + L"database\\button\\delete-button-pressed.png";
		win.SetIconPath(normalImagePath, pressedImagePath);
		win.SetBkgColors(RGB(238, 238, 238), RGB(238, 238, 238), RGB(238, 238, 238));
		win.Create(m_hWnd, rect, L"", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | BS_OWNERDRAW, 0, Config::DATABASE_DELETE_BUTTON_ID);
		win.SetToolTip(S(L"delete-database"));
		return;
	}
	else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
	}
}

void LeftTreeView::createOrShowTreeView(CTreeViewCtrlEx & win, CRect & clientRect)
{	
	CRect rect = getTreeRect(clientRect);
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		win.Create(m_hWnd, rect, L"", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | TVS_FULLROWSELECT 
			| TVS_LINESATROOT | TVS_SHOWSELALWAYS | TVS_HASBUTTONS , WS_EX_CLIENTEDGE, Config::DATABASE_TREEVIEW_ID);
		// create a singleton treeViewAdapter pointer
		treeViewAdapter = LeftTreeViewAdapter::getInstance(m_hWnd, &win); 
		return;
	}
	else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		// show first then move window
		win.ShowWindow(SW_SHOW);
		win.MoveWindow(&rect);
	}
}


void LeftTreeView::createOrShowSelectedDbComboBox(CComboBox & win, CRect & clientRect)
{
	int x = clientRect.right - 1 - TREEVIEW_COMBOBOX_WIDTH, y = 5, w = TREEVIEW_COMBOBOX_WIDTH, h = TREEVIEW_COMBOBOX_HEIGHT;
	CRect rect(x, y, x + w, y + h);
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		win.Create(m_hWnd, rect, L"", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | CBS_DROPDOWNLIST | CBS_HASSTRINGS, 
			0, Config::TREEVIEW_SELECTED_DB_COMBOBOX_ID);
		return;
	}
	else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		// show first then move window
		win.ShowWindow(SW_SHOW);
		win.MoveWindow(&rect);
	}
}

void LeftTreeView::loadWindow()
{
	treeViewAdapter->loadTreeView();
	loadComboBox();
}

void LeftTreeView::loadComboBox()
{
	// db list
	UserDbList dbs = treeViewAdapter->getDbs();
	seletedDbComboBox.ResetContent();
	int n = static_cast<int>(dbs.size());
	int nSelItem = -1;
	for (int i = 0; i< n; i++) {
		auto item = dbs.at(i);
		int nItem = seletedDbComboBox.AddString(item.name.c_str());
		seletedDbComboBox.SetItemData(nItem, item.id);
		if (item.isActive) {
			nSelItem = i;			
		}
	}
	seletedDbComboBox.SetCurSel(nSelItem);
}

void LeftTreeView::selectComboBox(uint64_t userDbId)
{
	int nSelItem = -1;
	for (int i = 0; i < seletedDbComboBox.GetCount(); i++) {
		uint64_t data = (uint64_t)seletedDbComboBox.GetItemData(i);
		if (data == userDbId) {
			nSelItem = i;
		}
	}
	if (nSelItem == -1) {
		return ;
	}
	seletedDbComboBox.SetCurSel(nSelItem);
}

int LeftTreeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	topbarBrush = ::CreateSolidBrush(topbarColor);
	bkgBrush = ::CreateSolidBrush(bkgColor);
	comboFont = FTB(L"combobox-size", true);
	return 0;
}

int LeftTreeView::OnDestroy()
{	
	if (topbarBrush) ::DeleteObject(topbarBrush);
	if (bkgBrush) ::DeleteObject(bkgBrush);	
	if (comboFont) ::DeleteObject(comboFont);

	if (treeView.IsWindow()) treeView.DestroyWindow();

	if (treeViewAdapter) delete treeViewAdapter;
	return 0;
}

void LeftTreeView::OnSize(UINT nType, CSize size)
{
	createOrShowUI();
}


void LeftTreeView::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if (!bShow) {
		return ;
	}
	loadWindow();
}

void LeftTreeView::OnPaint(CDCHandle dc)
{
	CPaintDC pdc(m_hWnd);
	CMemoryDC mdc(pdc, pdc.m_ps.rcPaint);
	CRect clientRect;
	GetClientRect(clientRect);


	CRect topRect = getTopRect(clientRect);
	mdc.FillRect(topRect, topbarBrush);

	CRect treeRect = getTreeRect(clientRect);
	mdc.FillRect(treeRect, bkgBrush);
}

BOOL LeftTreeView::OnEraseBkgnd(CDCHandle dc)
{
	return TRUE;
}

HBRUSH LeftTreeView::OnCtlColorListBox(HDC hdc, HWND hwnd)
{
	::SetTextColor(hdc, RGB(0, 0, 0)); //文本区域前景色
	::SetBkColor(hdc, RGB(153, 153, 153)); // 文本区域背景色
	::SelectObject(hdc, comboFont);
	return AtlGetStockBrush(WHITE_BRUSH);
}

HBRUSH LeftTreeView::OnCtlColorEdit(HDC hdc, HWND hwnd)
{
	::SetTextColor(hdc, RGB(0, 0, 0)); //文本区域前景色
	::SetBkColor(hdc, RGB(153, 153, 153)); // 文本区域背景色
	::SelectObject(hdc, comboFont);
	return AtlGetStockBrush(WHITE_BRUSH);
}

LRESULT LeftTreeView::OnClickCreateDbButton(UINT uNotifyCode, int nID, HWND hwnd)
{

	CFileDialog fileDialog(FALSE,  // TRUE-打开 false-另存为
		_T("*.db;*.sqlite;*.sqlite3;*.db3|*.*||"),
		_T(""),
		OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY,
		L"Sqlite File(*.db,*.sqlite,*.sqlite3,*.db3)|All File(*.*)",
		m_hWnd);

	if (fileDialog.DoModal() == IDOK) {
		std::wstring databasePath = fileDialog.m_szFileName;
		treeViewAdapter->createUserDatabase(databasePath);
		loadComboBox();
	}
	return 0; 
}

LRESULT LeftTreeView::OnClickOpenDbButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	CFileDialog fileDialog(TRUE,  // TRUE-打开 false-另存为
		_T("*.db;*.sqlite;*.sqlite3;*.db3|*.*||"),
		_T(""),
		OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY,
		L"Sqlite File(*.db,*.sqlite,*.sqlite3,*.db3)|All File(*.*)",
		m_hWnd);

	if (fileDialog.DoModal() == IDOK) {
		std::wstring databasePath = fileDialog.m_szFileName;		
		treeViewAdapter->openUserDatabase(databasePath);
		loadComboBox();
	}
	return 0;
}

LRESULT LeftTreeView::OnClickRefreshDbButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	treeViewAdapter->loadTreeView();
	loadComboBox();
	return 0;
}

LRESULT LeftTreeView::OnClickDeleteDbButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	treeViewAdapter->removeSeletedItem();
	loadComboBox();
	return 0;
}

/**
 * change selected treeview item .
 * 
 * @param wParam Treeview idCtrl
 * @param lParam structure pointer of LPNMTREEVIEW 
 * @param bHandled
 * @return 
 */
LRESULT LeftTreeView::OnChangeTreeViewItem(int wParam, LPNMHDR lParam, BOOL& bHandled)
{
	auto ptr = (LPNMTREEVIEW)lParam;
	HTREEITEM hSelTreeItem = ptr->itemNew.hItem;
	if (!hSelTreeItem) {
		return 0;
	}
	uint64_t userDbId = treeViewAdapter->getSeletedUserDbId();
	if (!userDbId) {
		return 0;
	}
	UserDbList dbs = treeViewAdapter->getDbs();
	auto itor = std::find_if(dbs.begin(), dbs.end(), [&userDbId, &dbs](UserDb item) {
		return item.id == userDbId && item.isActive == 1;
	});

	if (itor == dbs.end()) {
		treeViewAdapter->activeUserDatabase(userDbId);
		selectComboBox(userDbId);
	}

	
	return 0;
}
