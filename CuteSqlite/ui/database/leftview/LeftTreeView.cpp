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
#include "common/AppContext.h"
#include "core/common/Lang.h"
#include "core/common/exception/QRuntimeException.h"
#include "ui/common/message/QPopAnimate.h"
#include "ui/common/message/QMessageBox.h"
#include "ui/database/dialog/ExportAsSqlDialog.h"
#include "ui/database/dialog/ImportFromSqlDialog.h"

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
	createOrShowSelectedDbComboBox(selectedDbComboBox, clientRect);
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

void LeftTreeView::createOrShowTreeView(QTreeViewCtrl & win, CRect & clientRect)
{	
	CRect rect = getTreeRect(clientRect);
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		win.Create(m_hWnd, rect, L"", WS_CHILD | WS_VISIBLE  | WS_CLIPSIBLINGS | TVS_FULLROWSELECT 
			| TVS_LINESATROOT | TVS_SHOWSELALWAYS | TVS_HASBUTTONS | TVS_INFOTIP , WS_EX_CLIENTEDGE, Config::DATABASE_TREEVIEW_ID);
			//| TVS_LINESATROOT | TVS_HASBUTTONS , WS_EX_CLIENTEDGE, Config::DATABASE_TREEVIEW_ID);
		// create a singleton treeViewAdapter pointer
		treeViewAdapter = LeftTreeViewAdapter::getInstance(m_hWnd, &win); 
		databaseMenuAdapter = DatabaseMenuAdapter::getInstance(m_hWnd, &win); 
		tableMenuAdapter = TableMenuAdapter::getInstance(m_hWnd, &win); 
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
	if (!isNeedReload) {
		return;
	}
	isNeedReload = false;
	treeViewAdapter->loadTreeView();
	loadComboBox();
}

void LeftTreeView::loadComboBox()
{
	// db list
	UserDbList dbs = treeViewAdapter->getDbs();
	selectedDbComboBox.ResetContent();
	int n = static_cast<int>(dbs.size());
	int nSelItem = -1;
	for (int i = 0; i< n; i++) {
		auto item = dbs.at(i);
		int nItem = selectedDbComboBox.AddString(item.name.c_str());
		selectedDbComboBox.SetItemData(nItem, item.id);
		if (item.isActive) {
			nSelItem = i;
			AppContext::getInstance()->appendMainFrmCaption(item.path);
		}
	}
	selectedDbComboBox.SetCurSel(nSelItem);
}

void LeftTreeView::selectComboBox(uint64_t userDbId)
{
	int nCurSelItem = selectedDbComboBox.GetCurSel();
	uint64_t selUserDbId = (uint64_t)selectedDbComboBox.GetItemData(nCurSelItem);
	if (userDbId == selUserDbId) {
		return;
	}

	int nSelItem = -1;
	for (int i = 0; i < selectedDbComboBox.GetCount(); i++) {
		uint64_t data = (uint64_t)selectedDbComboBox.GetItemData(i);
		if (data == userDbId) {
			nSelItem = i;
		}
	}
	if (nSelItem == -1) {
		return ;
	}
	selectedDbComboBox.SetCurSel(nSelItem);
	
	// change the main caption
	UserDb userDb = databaseService->getUserDb(userDbId);
	AppContext::getInstance()->appendMainFrmCaption(userDb.path);
}

int LeftTreeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	topbarBrush = ::CreateSolidBrush(topbarColor);
	bkgBrush = ::CreateSolidBrush(bkgColor);
	comboFont = FTB(L"combobox-size", true);
	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_LEFTVIEW_REFRESH_DATABASE);
	return 0;
}

int LeftTreeView::OnDestroy()
{	
	AppContext::getInstance()->unsuscribe(m_hWnd, Config::MSG_LEFTVIEW_REFRESH_DATABASE);

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
	::SetTextColor(hdc, RGB(0, 0, 0)); // Text area foreground color
	::SetBkColor(hdc, RGB(153, 153, 153)); // Text area background color
	::SelectObject(hdc, comboFont);
	return AtlGetStockBrush(WHITE_BRUSH);
}

HBRUSH LeftTreeView::OnCtlColorEdit(HDC hdc, HWND hwnd)
{
	::SetTextColor(hdc, RGB(0, 0, 0)); // Text area foreground color
	::SetBkColor(hdc, RGB(153, 153, 153)); // Text area background color
	::SelectObject(hdc, comboFont);
	return AtlGetStockBrush(WHITE_BRUSH);
}


/**
 * change selected treeview item .
 * 
 * @param wParam Treeview idCtrl
 * @param lParam structure pointer of LPNMTREEVIEW 
 * @param bHandled
 * @return 
 */
LRESULT LeftTreeView::OnChangedTreeViewItem(int wParam, LPNMHDR lParam, BOOL& bHandled)
{
	// if refresh,then exit imediatly
	if (refreshLock) {
		return 0;
	}

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
	auto itor = std::find_if(dbs.begin(), dbs.end(), [&userDbId, &dbs](UserDb & item) {
		return item.id == userDbId && item.isActive == 1;
	});

	if (itor == dbs.end()) {
		treeViewAdapter->activeUserDatabase(userDbId);
		selectComboBox(userDbId);
	}

	CTreeItem treeItem = treeViewAdapter->getSeletedItem();
	int nImage = -1, nSeletedImage = -1;
	bool ret = treeItem.GetImage(nImage, nSeletedImage);

	if (nImage == 0 || nImage == 2 || nImage == 3 || nImage == 4 || nImage == 5 || nImage == 6) {
		AppContext::getInstance()->dispatch(Config::MSG_TREEVIEW_CLICK_ID, WPARAM(treeViewAdapter), (LPARAM)hSelTreeItem);
	}
	
	return 0;
}

LRESULT LeftTreeView::OnExpendingTreeViewItem(int wParam, LPNMHDR lParam, BOOL& bHandled)
{
	auto ptr = (LPNMTREEVIEW)lParam;
	HTREEITEM hSelTreeItem = ptr->itemNew.hItem;
	CTreeItem treeItem(hSelTreeItem, &treeView);

	int nImage = -1, nSeletedImage = -1;
	bool ret = treeItem.GetImage(nImage, nSeletedImage);
	if (nImage != 0) {
		return 0;
	}
	uint64_t userDbId = static_cast<uint64_t>(treeItem.GetData());
	UserDb userDb;
	userDb.id = userDbId;
	HTREEITEM hTablesFolderItem = treeViewAdapter->getFolderItem(hSelTreeItem, S(L"tables"));
	HTREEITEM hViewsFolderItem = treeViewAdapter->getFolderItem(hSelTreeItem, S(L"views"));
	HTREEITEM hTriggersFolderItem = treeViewAdapter->getFolderItem(hSelTreeItem, S(L"triggers"));

	// if folder item has children, then it is loaded before
	if ((hTablesFolderItem && treeView.ItemHasChildren(hTablesFolderItem)) 
		|| (hViewsFolderItem && treeView.ItemHasChildren(hViewsFolderItem))
		|| (hTriggersFolderItem && treeView.ItemHasChildren(hTriggersFolderItem)) ) {
		return 0;
	}

	// reload
	treeViewAdapter->loadTablesForTreeView(hTablesFolderItem, userDb);
	treeViewAdapter->loadViewsForTreeView(hViewsFolderItem, userDb);
	treeViewAdapter->loadTriggersForTreeView(hTriggersFolderItem, userDb);

	treeView.Expand(hTablesFolderItem);
	return 0;
}

/**
 * Show the tooltip in the CTreeItem.
 * @reference url - https://learn.microsoft.com/zh-cn/windows/win32/api/commctrl/ns-commctrl-nmtvgetinfotipa
 * 
 * @param wParam - not use
 * @param lParam - NMTVGETINFOTIP struct pointer
 * @param bHandled
 * @return 
 */
LRESULT LeftTreeView::OnShowTreeViewItemToolTip(int wParam, LPNMHDR lParam, BOOL& bHandled)
{
	LPNMTVGETINFOTIP lpGetInfoTip = (LPNMTVGETINFOTIP)lParam;
	int nImage = -1, nSelImage = -1;
	treeView.GetItemImage(lpGetInfoTip->hItem, nImage, nSelImage);
	// Database item : nImage == 0
	if (nImage != 0) { 
		return 0;
	}
	uint64_t userDbId = treeView.GetItemData(lpGetInfoTip->hItem);
	if (!userDbId) {
		return 0;
	}
	UserDb userDb = databaseService->getUserDb(userDbId);
	if (!userDb.id || userDb.path.empty()) {
		return 0;
	}
	wchar_t cch[1025] = {0};
	wmemcpy_s(cch, 1024, userDb.path.c_str(), userDb.path.size());
	lpGetInfoTip->pszText = cch;
	return 0;
}

LRESULT LeftTreeView::OnDbClickTreeViewItem(int wParam, LPNMHDR lParam, BOOL& bHandled)
{
	Q_INFO(L"LeftTreeView::OnDbClickTreeViewItem");
	auto ptr = (LPNMTREEVIEW)lParam;
	HTREEITEM hSelTreeItem = ptr->itemNew.hItem;
	if (!hSelTreeItem) {
		return 0;
	}

	CTreeItem treeItem = treeViewAdapter->getSeletedItem();
	int nImage = -1, nSeletedImage = -1;
	bool ret = treeItem.GetImage(nImage, nSeletedImage);

	if (nImage == 0 || nImage == 2 || nImage == 3 || nImage == 4 || nImage == 5 || nImage == 6) {
		AppContext::getInstance()->dispatch(Config::MSG_TREEVIEW_DBCLICK_ID, WPARAM(treeViewAdapter), (LPARAM)hSelTreeItem);
	}
	
	return 0;
}

LRESULT LeftTreeView::OnRightClickTreeViewItem(int wParam, LPNMHDR lParam, BOOL &bHandled)
{
	Q_INFO(L"LeftTreeView::OnRightClickTreeViewItem");

	CPoint pt; 
	::GetCursorPos(&pt);
	CPoint pt2 = pt;
	treeView.ScreenToClient(&pt2);
	UINT uFlag;
	CTreeItem selItem = treeView.HitTest(pt2, &uFlag);

	if (!selItem.IsNull()) {
		treeView.SelectItem(selItem.m_hTreeItem);

		int nImage = -1, nSeletedImage = -1;
		bool ret = selItem.GetImage(nImage, nSeletedImage);
	
		if (nImage == 0) {
			databaseMenuAdapter->popupMenu(pt);
		}else if (nImage == 1) {
			CTreeItem pTreeItem = treeView.GetParentItem(selItem.m_hTreeItem);
			int npImage = -1, npSelImage = -1;
			bool ret = pTreeItem.GetImage(npImage, npSelImage);
			if (npImage == 0) {
				databaseMenuAdapter->popupMenu(pt);
			}			
		}
	}
	
	
	return 0;
}

LRESULT LeftTreeView::OnChangeSelectDbComboBox(UINT uNotifyCode, int nID, HWND hwnd)
{
	int nSelItem = selectedDbComboBox.GetCurSel();
	uint64_t userDbId = static_cast<uint64_t>(selectedDbComboBox.GetItemData(nSelItem));

	treeViewAdapter->selectItem(userDbId);

	// set main caption text, such as "CuteSqlite - f:\path1\path2\xxx.db"
	std::wstring caption = AppContext::getInstance()->getMainFrmCaption();
	UserDb userDb = databaseService->getUserDb(userDbId);
	AppContext::getInstance()->appendMainFrmCaption(userDb.path);

	return 0;
}

LRESULT LeftTreeView::OnClickCreateDatabaseButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	doCreateDatabase();
	return 0; 
}

LRESULT LeftTreeView::OnClickOpenDatabaseButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	doOpenDatabase();
	return 0;
}

LRESULT LeftTreeView::OnClickRefreshDatabaseButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	doRefreshDatabase();
	return 0;
}

LRESULT LeftTreeView::OnClickDeleteDatabaseButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	doDeleteDatabase();

	return 0;
}

void LeftTreeView::OnClickCreateDatabaseMenu(UINT uNotifyCode, int nID, HWND hwnd)
{
	doCreateDatabase();
}

void LeftTreeView::OnClickOpenDatabaseMenu(UINT uNotifyCode, int nID, HWND hwnd)
{
	doOpenDatabase();
}

void LeftTreeView::OnClickRefreshDatabaseMenu(UINT uNotifyCode, int nID, HWND hwnd)
{
	doRefreshDatabase();
}

void LeftTreeView::OnClickDeleteDatabaseMenu(UINT uNotifyCode, int nID, HWND hwnd)
{
	doDeleteDatabase();
}

void LeftTreeView::OnClickCopyDatabaseMenu(UINT uNotifyCode, int nID, HWND hwnd)
{
	doCopyDatabase();
}

void LeftTreeView::OnClickExportAsSqlMenu(UINT uNotifyCode, int nID, HWND hwnd)
{
	doExportAsSql();
}

void LeftTreeView::OnClickImportFromSqlMenu(UINT uNotifyCode, int nID, HWND hwnd)
{
	doImportFromSql();
}

void LeftTreeView::OnClickNewTableMenu(UINT uNotifyCode, int nID, HWND hwnd)
{
	doNewTable();
}

void LeftTreeView::OnClickNewViewMenu(UINT uNotifyCode, int nID, HWND hwnd)
{
	doNewView();
}

void LeftTreeView::OnClickNewTriggerMenu(UINT uNotifyCode, int nID, HWND hwnd)
{
	doNewTrigger();
}

LRESULT LeftTreeView::OnRefreshDatabase(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	doRefreshDatabase();
	return 0;
}

void LeftTreeView::doCreateDatabase()
{
	wchar_t * szFilter = L"Sqlite File(*.db)\0*.db\0Sqlite File(*.db3)\0*.db3\0Sqlite File(*.sqlite)\0*.sqlite\0Sqlite File(*.sqlite3)\0*.sqlite3\0All Files(*.*)\0*.*\0\0";
	wchar_t * defExt = L"db";

	CFileDialog fileDlg(FALSE, defExt, L"NewDatabase", OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilter, m_hWnd);

	if (fileDlg.DoModal() == IDOK) {
		std::wstring databasePath = fileDlg.m_szFileName;
		treeViewAdapter->createUserDatabase(databasePath);
		loadComboBox();
	}
}

void LeftTreeView::doOpenDatabase()
{
	wchar_t * szFilter = L"Sqlite File(*.db)\0*.db\0Sqlite File(*.db3)\0*.db3\0Sqlite File(*.sqlite)\0*.sqlite\0Sqlite File(*.sqlite3)\0*.sqlite3\0All Files(*.*)\0*.*\0\0";
	wchar_t * defExt = L"db";

	CFileDialog fileDlg(TRUE, defExt, L"", OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilter, m_hWnd);

	if (fileDlg.DoModal() == IDOK) {
		std::wstring databasePath = fileDlg.m_szFileName;
		treeViewAdapter->openUserDatabase(databasePath);
		loadComboBox();
		
	}
}

void LeftTreeView::doCopyDatabase()
{
	wchar_t * szFilter = L"Sqlite File(*.db)\0*.db\0Sqlite File(*.db3)\0*.db3\0Sqlite File(*.sqlite)\0*.sqlite\0Sqlite File(*.sqlite3)\0*.sqlite3\0All Files(*.*)\0*.*\0\0";
	wchar_t * defExt = L"db";

	CFileDialog fileDlg(FALSE, defExt, L"CopyDatabase", OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilter, m_hWnd);

	if (fileDlg.DoModal() == IDOK) {
		std::wstring databasePath = fileDlg.m_szFileName;
		treeViewAdapter->copyUserDatabase(databasePath);
		loadComboBox();
	}
}

void LeftTreeView::doRefreshDatabase()
{
	refreshLock = true;
	treeViewAdapter->loadTreeView();
	refreshLock = false;
	loadComboBox();
}

void LeftTreeView::doDeleteDatabase()
{
	treeViewAdapter->removeSeletedItem();
	loadComboBox();
}

void LeftTreeView::doExportAsSql()
{
	auto adapter = ExportDatabaseAdapter::getInstance(m_hWnd, nullptr);
	ExportAsSqlDialog exportAsSqlDialog(m_hWnd, adapter);
	exportAsSqlDialog.DoModal(m_hWnd);
}

void LeftTreeView::doImportFromSql()
{
	auto adapter = ImportDatabaseAdapter::getInstance(m_hWnd, nullptr);
	ImportFromSqlDialog dialog(m_hWnd, adapter);
	dialog.DoModal(m_hWnd);
}

void LeftTreeView::doNewTable()
{
	AppContext::getInstance()->dispatch(Config::MSG_NEW_TABLE_ID, NULL, NULL);
}

void LeftTreeView::doNewView()
{
	AppContext::getInstance()->dispatch(Config::MSG_NEW_VIEW_ID, NULL, NULL);
}

void LeftTreeView::doNewTrigger()
{
	AppContext::getInstance()->dispatch(Config::MSG_NEW_TRIGGER_ID, NULL, NULL);
}
