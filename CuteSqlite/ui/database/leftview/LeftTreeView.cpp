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
#include "ui/database/dialog/ImportFromCsvDialog.h"

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
		treeViewAdapter = new LeftTreeViewAdapter(m_hWnd, &win); 
		databaseMenuAdapter =  new DatabaseMenuAdapter(m_hWnd, &win); 
		tableMenuAdapter = new TableMenuAdapter(m_hWnd, &win);
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
		win.Create(m_hWnd, rect, L"", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | CBS_DROPDOWNLIST | CBS_HASSTRINGS | CBS_AUTOHSCROLL, 
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
	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_LEFTVIEW_REFRESH_DATABASE_ID);
	importDatabaseAdapter = new ImportDatabaseAdapter(m_hWnd, nullptr);
	exportDatabaseAdapter = new ExportDatabaseAdapter(m_hWnd, nullptr);
	return 0;
}

int LeftTreeView::OnDestroy()
{	
	AppContext::getInstance()->unsuscribe(m_hWnd, Config::MSG_LEFTVIEW_REFRESH_DATABASE_ID);

	if (topbarBrush) ::DeleteObject(topbarBrush);
	if (bkgBrush) ::DeleteObject(bkgBrush);	
	if (comboFont) ::DeleteObject(comboFont);

	if (createDbButton.IsWindow()) createDbButton.DestroyWindow();
	if (openDbButton.IsWindow()) openDbButton.DestroyWindow();
	if (refreshDbButton.IsWindow()) refreshDbButton.DestroyWindow();
	if (deleteDbButton.IsWindow()) deleteDbButton.DestroyWindow();
	if (selectedDbComboBox.IsWindow()) selectedDbComboBox.DestroyWindow();
	if (treeView.IsWindow()) treeView.DestroyWindow();

	if (treeViewAdapter) {
		delete treeViewAdapter;
		treeViewAdapter = nullptr;
	}
	if (databaseMenuAdapter) {
		delete databaseMenuAdapter;
		databaseMenuAdapter = nullptr;
	}
	if (tableMenuAdapter) {
		delete tableMenuAdapter;
		tableMenuAdapter = nullptr;
	}

	if (exportDatabaseAdapter) {
		delete exportDatabaseAdapter;
		exportDatabaseAdapter = nullptr;
	}

	if (importDatabaseAdapter) {
		delete importDatabaseAdapter;
		importDatabaseAdapter = nullptr;
	}
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

	//  When tree item changing, init the runtime data of DatabaseSupplier object .
	treeViewAdapter->initDatabaseSupplier();

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
	treeViewAdapter->expandTreeItem(ptr);
	bHandled = true;
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
	
		if (nImage == 0) { // 0 - database
			databaseMenuAdapter->popupMenu(pt);
		} else if (nImage == 2) { // 2 - table
			tableMenuAdapter->popupMenu(pt); 
		} else if (nImage == 3) { // 3 - field / column
			tableMenuAdapter->popupColumnsMenu(pt, true);
		} else if (nImage == 4) { // 4 - index
			tableMenuAdapter->popupIndexesMenu(pt, true);
		} else if (nImage == 5) { // 5 - view
			databaseMenuAdapter->popupViewsMenu(pt, true);
		} else if (nImage == 6) { // 6 - trigger
			databaseMenuAdapter->popupTriggersMenu(pt, true);
		}  else if (nImage == 1) { // 1 - folder
			CTreeItem pTreeItem = treeView.GetParentItem(selItem.m_hTreeItem);
			int npImage = -1, npSelImage = -1;
			bool ret = pTreeItem.GetImage(npImage, npSelImage);
			wchar_t * cch = nullptr;
			std::wstring folder;
			selItem.GetText(cch);
			if (cch) {
				folder.assign(cch);
			}
			::SysFreeString(cch);

			if (npImage == 0 && folder == S(L"tables")) { // 0 - parent is database,  folder="Tables"
				databaseMenuAdapter->popupMenu(pt);
			} else if (npImage == 0 && folder == S(L"views")) { // 0 - parent is database,  folder="Views"
				databaseMenuAdapter->popupViewsMenu(pt, false);
			} else if (npImage == 0 && folder == S(L"triggers")) { // 0 - parent is database,  folder="Trigger"
				databaseMenuAdapter->popupTriggersMenu(pt, false);
			} else if (npImage == 2 && folder == S(L"columns")) { // 2 - parent is table,  folder="Columns"
				tableMenuAdapter->popupColumnsMenu(pt, false);
			} else if (npImage == 2 && folder == S(L"indexes")) { // 2 - parent is table,  folder="Indexes"
				tableMenuAdapter->popupIndexesMenu(pt, false);
			}
		}
	}
	return 0;
}

LRESULT LeftTreeView::OnChangeSelectDbComboBox(UINT uNotifyCode, int nID, HWND hwnd)
{
	int nSelItem = selectedDbComboBox.GetCurSel();
	uint64_t userDbId = static_cast<uint64_t>(selectedDbComboBox.GetItemData(nSelItem));

	treeViewAdapter->selectDbTreeItem(userDbId);

	// set main caption text, such as "CuteSqlite - f:\path1\path2\xxx.db"
	std::wstring caption = AppContext::getInstance()->getMainFrmCaption();
	UserDb userDb = databaseService->getUserDb(userDbId);
	AppContext::getInstance()->appendMainFrmCaption(userDb.path);
	databaseSupplier->setSeletedUserDbId(userDbId);
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

void LeftTreeView::OnClickManageIndexMenu(UINT uNotifyCode, int nID, HWND hwnd)
{
	tableMenuAdapter->manageIndex();
}

void LeftTreeView::OnClickManageForeignKeyMenu(UINT uNotifyCode, int nID, HWND hwnd)
{
	tableMenuAdapter->manageForeignKey();
}

void LeftTreeView::OnClickPropertiesMenu(UINT uNotifyCode, int nID, HWND hwnd)
{
	tableMenuAdapter->showProperties();
}

void LeftTreeView::OnClickManageColumnsMenu(UINT uNotifyCode, int nID, HWND hwnd)
{
	tableMenuAdapter->manageColumn();
}

void LeftTreeView::OnClickDropColumnMenu(UINT uNotifyCode, int nID, HWND hwnd)
{
	tableMenuAdapter->dropColumn();
}

void LeftTreeView::OnClickDropIndexMenu(UINT uNotifyCode, int nID, HWND hwnd)
{
	tableMenuAdapter->dropIndex();
}

void LeftTreeView::OnClickNewViewMenu(UINT uNotifyCode, int nID, HWND hwnd)
{
	doNewView();
}

void LeftTreeView::OnClickNewTriggerMenu(UINT uNotifyCode, int nID, HWND hwnd)
{
	doNewTrigger();
}

void LeftTreeView::OnClickOpenViewMenu(UINT uNotifyCode, int nID, HWND hwnd)
{
	AppContext::getInstance()->dispatch(Config::MSG_OPEN_VIEW_ID, NULL, NULL);
}

void LeftTreeView::OnClickDropViewMenu(UINT uNotifyCode, int nID, HWND hwnd)
{
	databaseMenuAdapter->dropView();
}

void LeftTreeView::OnClickOpenTriggerMenu(UINT uNotifyCode, int nID, HWND hwnd)
{
	AppContext::getInstance()->dispatch(Config::MSG_OPEN_TRIGGER_ID, NULL, NULL);
}

void LeftTreeView::OnClickDropTriggerMenu(UINT uNotifyCode, int nID, HWND hwnd)
{
	databaseMenuAdapter->dropTrigger();
}

void LeftTreeView::OnClickNewTableMenu(UINT uNotifyCode, int nID, HWND hwnd)
{
	doNewTable();
}

void LeftTreeView::OnClickAlterTableMenu(UINT uNotifyCode, int nID, HWND hwnd)
{
	doAlterTable();
}

void LeftTreeView::OnClickRenameTableMenu(UINT uNotifyCode, int nID, HWND hwnd)
{
	tableMenuAdapter->renameTable();
}

void LeftTreeView::OnClickTruncateTableMenu(UINT uNotifyCode, int nID, HWND hwnd)
{
	tableMenuAdapter->truncateTable();
}

void LeftTreeView::OnClickDropTableMenu(UINT uNotifyCode, int nID, HWND hwnd)
{
	tableMenuAdapter->dropTable();
}

void LeftTreeView::OnClickCopyTableMenu(UINT uNotifyCode, int nID, HWND hwnd)
{
	if (tableMenuAdapter->copyTable()) {
		doRefreshDatabase();
	}
}

void LeftTreeView::OnClickShardingTableMenu(UINT uNotifyCode, int nID, HWND hwnd)
{
	tableMenuAdapter->shardingTable();
}

void LeftTreeView::OnClickExportTableMenu(UINT uNotifyCode, int nID, HWND hwnd)
{
	tableMenuAdapter->exportTable();
}

void LeftTreeView::OnClickImportFromCsvMenu(UINT uNotifyCode, int nID, HWND hwnd)
{
	doImportFromCsv();
}

void LeftTreeView::OnClickOpenTableMenu(UINT uNotifyCode, int nID, HWND hwnd)
{	
	tableMenuAdapter->openTable();
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
	treeViewAdapter->removeSeletedDbTreeItem();
	loadComboBox();
}

void LeftTreeView::doExportAsSql()
{
	ExportAsSqlDialog exportAsSqlDialog(m_hWnd, exportDatabaseAdapter);
	exportAsSqlDialog.DoModal(m_hWnd);
}

void LeftTreeView::doImportFromSql()
{
	ImportFromSqlDialog dialog(m_hWnd, importDatabaseAdapter);
	dialog.DoModal(m_hWnd);
}

void LeftTreeView::doImportFromCsv()
{
	ImportFromCsvDialog dialog(m_hWnd);
	dialog.DoModal(m_hWnd);
}

void LeftTreeView::doNewView()
{
	AppContext::getInstance()->dispatch(Config::MSG_NEW_VIEW_ID, NULL, NULL);
}

void LeftTreeView::doNewTrigger()
{
	AppContext::getInstance()->dispatch(Config::MSG_NEW_TRIGGER_ID, NULL, NULL);
}

void LeftTreeView::doNewTable()
{
	AppContext::getInstance()->dispatch(Config::MSG_NEW_TABLE_ID, NULL, NULL);
}

void LeftTreeView::doAlterTable()
{
	AppContext::getInstance()->dispatch(Config::MSG_ALTER_TABLE_ID, TABLE_COLUMNS_PAGE, NULL);
}
