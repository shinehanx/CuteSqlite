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

 * @file   TableForeignkeysPage.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-10-10
 *********************************************************************/
#include "stdafx.h"
#include "TableForeignkeysPage.h"
#include "common/AppContext.h"
#include "utils/ResourceUtil.h"
#include "ui/common/QWinCreater.h"
#include "core/common/Lang.h"
#include "ui/database/rightview/page/table/dialog/TableForeignkeyColumnsDialog.h"
#include "ui/common/message/QPopAnimate.h"

BOOL TableForeignkeysPage::PreTranslateMessage(MSG* pMsg)
{
	if (listView.IsWindow() && listView.PreTranslateMessage(pMsg)) {
		return TRUE;
	}
	return FALSE;
}

void TableForeignkeysPage::setup(TableColumnsPageAdapter * tblColumnsPageAdapter, TableStructureSupplier * supplier)
{
	this->tblColumnsPageAdapter = tblColumnsPageAdapter;
	this->setSupplier(supplier);
}


TableForeignkeysPageAdapter * TableForeignkeysPage::getAdapter()
{
	ATLASSERT(adapter != nullptr);
	return adapter;
}


void TableForeignkeysPage::refreshDirtyAfterSave()
{
	listView.clearChangeVals();
	supplier->setFrkOrigDatas(supplier->getFrkRuntimeDatas());
	enableDataDirty();
}

void TableForeignkeysPage::createOrShowUI()
{
	QPage::createOrShowUI();

	CRect clientRect;
	GetClientRect(clientRect);
	createOrShowToolBarElems(clientRect);
	createOrShowListView(listView, clientRect);
}

void TableForeignkeysPage::createOrShowToolBarElems(CRect & clientRect)
{
	CRect topbarRect = getTopRect(clientRect);
	int x = 5, y = 5, w = PAGE_BUTTON_WIDTH, h = PAGE_BUTTON_HEIGHT;
	CRect rect(x, y, x + w, y + h);
	
	// create or show button
	std::wstring imgDir = ResourceUtil::getProductImagesDir();
	std::wstring normalImagePath, pressedImagePath;
	if (!newIndexButton.IsWindow()) {
		normalImagePath = imgDir + L"database\\list\\button\\column\\new-button-normal.png";
		pressedImagePath = imgDir + L"database\\list\\button\\column\\new-button-pressed.png";
		newIndexButton.SetIconPath(normalImagePath, pressedImagePath);
		newIndexButton.SetBkgColors(buttonColor, buttonColor, buttonColor);
	}
	QWinCreater::createOrShowButton(m_hWnd, newIndexButton, Config::TABLE_NEW_FOREIGNKEY_BUTTON_ID, L"", rect, clientRect);
	newIndexButton.SetToolTip(S(L"insert-new-foreignkey"));

	rect.OffsetRect(16 + 10, 0);
	if (!delIndexButton.IsWindow()) {
		normalImagePath = imgDir + L"database\\list\\button\\column\\del-button-normal.png";
		pressedImagePath = imgDir + L"database\\list\\button\\column\\del-button-pressed.png";
		delIndexButton.SetIconPath(normalImagePath, pressedImagePath);
		delIndexButton.SetBkgColors(buttonColor, buttonColor, buttonColor);
	}
	QWinCreater::createOrShowButton(m_hWnd, delIndexButton, Config::TABLE_DEL_FOREIGNKEY_BUTTON_ID, L"", rect, clientRect);
	delIndexButton.SetToolTip(S(L"delete-sel-foreignkey"));
}

void TableForeignkeysPage::createOrShowListView(QListViewCtrl & win, CRect & clientRect)
{
	CRect & rect = getPageRect(clientRect);
	if (IsWindow() && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_BORDER | LVS_ALIGNLEFT | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_OWNERDATA | LVS_OWNERDRAWFIXED;
		win.Create(m_hWnd, rect,NULL,dwStyle , // | LVS_OWNERDATA
			0, Config::DATABASE_TABLE_FOREIGNKEYS_LISTVIEW_ID );
		win.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER );
		win.setItemHeight(22);
		adapter = new TableForeignkeysPageAdapter(m_hWnd, &win, getSupplier());
	} else if (IsWindow() && win.IsWindow() && clientRect.Width() > 1) {
		win.MoveWindow(rect);
		win.ShowWindow(true);
		if (adapter == nullptr) {
			adapter = new TableForeignkeysPageAdapter(m_hWnd, &win, getSupplier());
		}
	}
}

void TableForeignkeysPage::loadWindow()
{
	QPage::loadWindow();

	if (!isNeedReload) {
		return;
	}
	isNeedReload = false;	
	loadListView();
}

void TableForeignkeysPage::loadListView()
{
	uint64_t userDbId = supplier->getRuntimeUserDbId();
	std::wstring tblName = supplier->getRuntimeTblName();
	std::wstring schema = supplier->getRuntimeSchema();
	rowCount = adapter->loadTblForeignkeysListView(userDbId, tblName, schema);
}


void TableForeignkeysPage::enableDataDirty()
{
	bool isDirty = adapter ? adapter->isDirty() : false;
	supplier->setIsDirty(!supplier->compareDatas());
	// class chain : TableForeignkeysPage(this) -> QTabView -> TableTabView
	HWND pHwnd = GetParent().GetParent().m_hWnd; // TableTabView
	::PostMessage(pHwnd, Config::MSG_TABLE_STRUCTURE_DIRTY_ID, WPARAM(m_hWnd), LPARAM(isDirty));

	// class chain : TableForeignkeysPage(this) -> QTabView -> TableTabView -> TableStructurePage -> QTabView(tabView) -> RightWorkView
	HWND pHwnd2 = GetParent().GetParent().GetParent().GetParent().GetParent().m_hWnd; // RightWorkView
	::PostMessage(pHwnd2, Config::MSG_TABLE_STRUCTURE_DIRTY_ID, WPARAM(m_hWnd), LPARAM(isDirty));
}

int TableForeignkeysPage::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	bool ret = QPage::OnCreate(lpCreateStruct);	
	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_DATA_HAS_CHANGED_ID);

	textFont = FT(L"form-text-size");
	return ret;
}

int TableForeignkeysPage::OnDestroy()
{
	bool ret = QPage::OnDestroy();
	AppContext::getInstance()->unsubscribe(m_hWnd, Config::MSG_DATA_HAS_CHANGED_ID);

	if (textFont) ::DeleteObject(textFont);

	if (newIndexButton.IsWindow()) newIndexButton.DestroyWindow();
	if (delIndexButton.IsWindow()) delIndexButton.DestroyWindow();

	if (listView.IsWindow()) listView.DestroyWindow();
	if (adapter) {
		delete adapter;
		adapter = nullptr;
	}

	return ret;
}

void TableForeignkeysPage::paintItem(CDC & dc, CRect & paintRect)
{

}


LRESULT TableForeignkeysPage::OnListViewItemChanged(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
	LPNMLISTVIEW nvListViewPtr = reinterpret_cast<LPNMLISTVIEW>(pnmh);
	if (nvListViewPtr->uOldState == 0 && nvListViewPtr->uNewState == 0) 
		return 0;         // No change 
	// Old check box state 
	BOOL bPrevState = (BOOL)(((nvListViewPtr->uOldState & LVIS_STATEIMAGEMASK) >> 12) - 1);     
	if (bPrevState < 0)   // On startup there 's no previous state   
		bPrevState = 0;   // so assign as false (unchecked) 
	// New check box state 
	BOOL bChecked = (BOOL)(((nvListViewPtr->uNewState & LVIS_STATEIMAGEMASK) >> 12) - 1);       
	if (bChecked < 0)   // On non-checkbox notifications assume false 
		bChecked = 0;   
	if (bPrevState == bChecked)   // No change in check box 
		return 0; 
	
	return 0;
}

LRESULT TableForeignkeysPage::OnDbClickListView(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
	NMITEMACTIVATE * aItem = (NMITEMACTIVATE *)pnmh; 
	
	subItemPos.first = aItem->iItem, subItemPos.second = aItem->iSubItem;

	if (aItem->iSubItem != 1) {
		return 0;
	}

	// show the editor
	listView.createOrShowEditor(subItemPos);
	return 0;
}


LRESULT TableForeignkeysPage::OnClickListView(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
	NMITEMACTIVATE * clickItem = (NMITEMACTIVATE *)pnmh; 
	if (clickItem->iSubItem == 2 || clickItem->iSubItem == 4) { // show button, 2 - Referencing columns  4 - Referenced columns
		int iItem = clickItem->iItem;
		int iSubItem = clickItem->iSubItem;
	
		CRect subItemRect, listWinRect;
		listView.GetSubItemRect(iItem, iSubItem, LVIR_BOUNDS, subItemRect);
		listView.GetWindowRect(listWinRect);
		int x = listWinRect.left + subItemRect.left, y = listWinRect.top + subItemRect.top,
			w = 20, h = subItemRect.Height() - 2;
		CRect rect(x, y, x + w, y + h);
		subItemRect.left = listWinRect.left + subItemRect.left;
		if (clickItem->iSubItem == 4 && adapter->getSubItemString(iItem, 3).empty()) {
			QPopAnimate::error(m_hWnd, S(L"select-referenced-table-first"));
			listView.activeSubItem(iItem, 3);
			return 0;
		}
		TableForeignkeyColumnsDialog columnsDialog(m_hWnd, tblColumnsPageAdapter, adapter, rect, iItem, iSubItem);
		if (columnsDialog.DoModal(m_hWnd) == Config::QDIALOG_YES_BUTTON_ID) {
			// send msg to TableStructurePage, class chain : TableForeinkeysPage($this)->QTabView($tabView)->TableTabView->TableStructurePage
			HWND pHwnd = GetParent().GetParent().GetParent().m_hWnd;
			::PostMessage(pHwnd, Config::MSG_TABLE_PREVIEW_SQL_ID, NULL, NULL);
		}
	}
	adapter->clickListViewSubItem(clickItem);
	listView.changeAllItemsCheckState();
	enableDataDirty();
	return 0;
}

HBRUSH TableForeignkeysPage::OnCtlColorStatic(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, topbarColor);
	::SelectObject(hdc, textFont);
	return topbarBrush.m_hBrush;
}

HBRUSH TableForeignkeysPage::OnCtlColorListBox(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, bkgColor);
	::SelectObject(hdc, textFont);
	return AtlGetStockBrush(WHITE_BRUSH);
}

HBRUSH TableForeignkeysPage::OnCtlColorEdit(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, bkgColor);
	::SelectObject(hdc, textFont);
	return AtlGetStockBrush(WHITE_BRUSH);
}

LRESULT TableForeignkeysPage::OnGetListViewData(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
	NMLVDISPINFO* plvdi = (NMLVDISPINFO*)pnmh;
	adapter->fillDataInListViewSubItem(plvdi);

	return 0;
}

LRESULT TableForeignkeysPage::OnPrepareListViewData(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
	auto pCachehint = (NMLVCACHEHINT *)pnmh;
	return 0;
}

LRESULT TableForeignkeysPage::OnFindListViewData(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
	LPNMLVFINDITEM  pnmfi = (LPNMLVFINDITEM)pnmh;

	auto iItem = pnmfi->iStart;
	if (-1 == iItem)
		return -1;

	auto count = rowCount;
	if (!count || count <= iItem)
		return -1;

	return 0;
}

LRESULT TableForeignkeysPage::OnListViewSubItemTextChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	const SubItemValues & changedVals = listView.getChangedVals();
	for (auto val : changedVals) {

		adapter->changeRuntimeDatasItem(val.iItem, val.iSubItem, val.origVal, val.newVal);
		adapter->invalidateSubItem(val.iItem, val.iSubItem);
	}
	// send msg to TableStructurePage, class chain : TableForeinkeysPage($this)->QTabView($tabView)->TableTabView->TableStructurePage
	HWND pHwnd = GetParent().GetParent().GetParent().m_hWnd;
	::PostMessage(pHwnd, Config::MSG_TABLE_PREVIEW_SQL_ID, NULL, NULL);
	enableDataDirty();
	return 0;
}

LRESULT TableForeignkeysPage::OnTableColumsDeleteColumnName(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	std::wstring columnName; 
	std::wstring * buff = (std::wstring *)lParam;
	if (buff) {
		columnName.assign(buff->c_str());
		delete buff; // // alloc memory in  TableColumnsPageAdapter::deleteSelColumns - columnName2
	}
	if (columnName.empty()) {
		Q_ERROR(L"columnName can't be empty");
		return 0;
	}
	adapter->deleteTableColumnName(columnName);

	// send msg to TableStructurePage, class chain : TableForeinkeysPage($this)->QTabView($tabView)->TableTabView->TableStructurePage
	HWND pHwnd = GetParent().GetParent().GetParent().m_hWnd;
	::PostMessage(pHwnd, Config::MSG_TABLE_PREVIEW_SQL_ID, NULL, NULL);
	enableDataDirty();
	return 0;
}


LRESULT TableForeignkeysPage::OnHandleDataHasChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return supplier->getIsDirty() ? 0 : 1;
}

LRESULT TableForeignkeysPage::OnClickNewForeignkeyButton(UINT uNotifyCode, int nID, HWND wndCtl)
{
	adapter->createNewForeignKey();
	
	// send msg to TableStructurePage, class chain : TableForeinkeysPage($this)->QTabView($tabView)->TableTabView->TableStructurePage
	HWND pHwnd = GetParent().GetParent().GetParent().m_hWnd;
	::PostMessage(pHwnd, Config::MSG_TABLE_PREVIEW_SQL_ID, NULL, NULL);
	enableDataDirty();
	return 0;
}

LRESULT TableForeignkeysPage::OnClickDelForeignkeyButton(UINT uNotifyCode, int nID, HWND wndCtl)
{
	adapter->deleteSelForeignKeys();
	// send msg to TableStructurePage, class chain : TableForeinkeysPage($this)->QTabView($tabView)->TableTabView->TableStructurePage
	HWND pHwnd = GetParent().GetParent().GetParent().m_hWnd;
	::PostMessage(pHwnd, Config::MSG_TABLE_PREVIEW_SQL_ID, NULL, NULL);
	enableDataDirty();
	return 0;
}