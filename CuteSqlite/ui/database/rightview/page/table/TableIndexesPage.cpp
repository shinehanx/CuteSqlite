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

 * @file   TableIndexesPage.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-10-10
 *********************************************************************/
#include "stdafx.h"
#include "TableIndexesPage.h"
#include "common/AppContext.h"
#include "utils/ResourceUtil.h"
#include "ui/common/QWinCreater.h"
#include "core/common/Lang.h"
#include "ui/database/rightview/page/table/dialog/TableIndexColumnsDialog.h"

BOOL TableIndexesPage::PreTranslateMessage(MSG* pMsg)
{
	if (listView.IsWindow() && listView.PreTranslateMessage(pMsg)) {
		return TRUE;
	}
	return FALSE;
}

void TableIndexesPage::setup(TableColumnsPageAdapter * tblColumnsPageAdapter, TableStructureSupplier * supplier)
{
	this->tblColumnsPageAdapter = tblColumnsPageAdapter;
	this->setSupplier(supplier);
}


TableIndexesPageAdapter * TableIndexesPage::getAdapter()
{
	ATLASSERT(adapter != nullptr);
	return adapter;
}

void TableIndexesPage::createOrShowUI()
{
	QPage::createOrShowUI();

	CRect clientRect;
	GetClientRect(clientRect);
	createOrShowToolBarElems(clientRect);
	createOrShowListView(listView, clientRect);
}

void TableIndexesPage::createOrShowToolBarElems(CRect & clientRect)
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
	QWinCreater::createOrShowButton(m_hWnd, newIndexButton, Config::TABLE_NEW_INDEX_BUTTON_ID, L"", rect, clientRect);
	newIndexButton.SetToolTip(S(L"insert-new-column"));

	rect.OffsetRect(16 + 10, 0);
	if (!delIndexButton.IsWindow()) {
		normalImagePath = imgDir + L"database\\list\\button\\column\\del-button-normal.png";
		pressedImagePath = imgDir + L"database\\list\\button\\column\\del-button-pressed.png";
		delIndexButton.SetIconPath(normalImagePath, pressedImagePath);
		delIndexButton.SetBkgColors(buttonColor, buttonColor, buttonColor);
	}
	QWinCreater::createOrShowButton(m_hWnd, delIndexButton, Config::TABLE_DEL_INDEX_BUTTON_ID, L"", rect, clientRect);
	delIndexButton.SetToolTip(S(L"delete-sel-column"));
}

void TableIndexesPage::createOrShowListView(QListViewCtrl & win, CRect & clientRect)
{
	CRect & rect = getPageRect(clientRect);
	if (IsWindow() && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_BORDER | LVS_ALIGNLEFT | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_OWNERDATA | LVS_OWNERDRAWFIXED;
		win.Create(m_hWnd, rect,NULL,dwStyle , // | LVS_OWNERDATA
			0, Config::DATABASE_TABLE_INDEXES_LISTVIEW_ID );
		win.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER );
		win.setItemHeight(22);
		adapter = new TableIndexesPageAdapter(m_hWnd, &win, getSupplier());
	} else if (IsWindow() && win.IsWindow() && clientRect.Width() > 1) {
		win.MoveWindow(rect);
		win.ShowWindow(true);
	}
}

void TableIndexesPage::loadWindow()
{
	QPage::loadWindow();

	if (!isNeedReload) {
		return;
	}
	isNeedReload = false;	
	loadListView();
}

void TableIndexesPage::loadListView()
{
	rowCount = adapter->loadTblIndexesListView(runtimeUserDbId, runtimeSchema);
}

int TableIndexesPage::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	bool ret = QPage::OnCreate(lpCreateStruct);	
	textFont = FT(L"form-text-size");
	return ret;
}

int TableIndexesPage::OnDestroy()
{
	bool ret = QPage::OnDestroy();
	if (textFont) ::DeleteObject(textFont);

	if (newIndexButton.IsWindow()) newIndexButton.DestroyWindow();
	if (delIndexButton.IsWindow()) delIndexButton.DestroyWindow();
	if (upIndexButton.IsWindow()) upIndexButton.DestroyWindow();
	if (downIndexButton.IsWindow()) downIndexButton.DestroyWindow();

	if (listView.IsWindow()) listView.DestroyWindow();
	if (adapter) {
		delete adapter;
		adapter = nullptr;
	}

	return ret;
}

void TableIndexesPage::paintItem(CDC & dc, CRect & paintRect)
{

}


LRESULT TableIndexesPage::OnListViewItemChanged(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
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

LRESULT TableIndexesPage::OnDbClickListView(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
	NMITEMACTIVATE * aItem = (NMITEMACTIVATE *)pnmh; 
	
	subItemPos.first = aItem->iItem, subItemPos.second = aItem->iSubItem;

	if (aItem->iSubItem == 0 || aItem->iSubItem == 2 || aItem->iSubItem == 3) {
		return 0;
	}

	// show the editor
	listView.createOrShowEditor(subItemPos);
	return 0;
}


LRESULT TableIndexesPage::OnClickListView(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
	NMITEMACTIVATE * clickItem = (NMITEMACTIVATE *)pnmh; 
	if (clickItem->iSubItem == 2) { // button
		int iItem = clickItem->iItem;
		int iSubItem = clickItem->iSubItem;
	
		CRect subItemRect, listWinRect;
		listView.GetSubItemRect(iItem, iSubItem, LVIR_BOUNDS, subItemRect);
		listView.GetWindowRect(listWinRect);
		int x = listWinRect.left + subItemRect.left, y = listWinRect.top + subItemRect.top,
			w = 20, h = subItemRect.Height() - 2;
		CRect rect(x, y, x + w, y + h);
		subItemRect.left = listWinRect.left + subItemRect.left;
		TableIndexColumnsDialog columnsDialog(m_hWnd, tblColumnsPageAdapter, adapter, rect, iItem, iSubItem);

		columnsDialog.DoModal(m_hWnd);
		return 0;
	}
	adapter->clickListViewSubItem(clickItem);
	listView.changeAllItemsCheckState();
	return 0;
}

HBRUSH TableIndexesPage::OnCtlColorStatic(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, topbarColor);
	::SelectObject(hdc, textFont);
	return topbarBrush;
}

HBRUSH TableIndexesPage::OnCtlColorListBox(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, bkgColor);
	::SelectObject(hdc, textFont);
	return AtlGetStockBrush(WHITE_BRUSH);
}

HBRUSH TableIndexesPage::OnCtlColorEdit(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, bkgColor);
	::SelectObject(hdc, textFont);
	return AtlGetStockBrush(WHITE_BRUSH);
}

LRESULT TableIndexesPage::OnGetListViewData(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
	NMLVDISPINFO* plvdi = (NMLVDISPINFO*)pnmh;
	adapter->fillDataInListViewSubItem(plvdi);

	return 0;
}

LRESULT TableIndexesPage::OnPrepareListViewData(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
	auto pCachehint = (NMLVCACHEHINT *)pnmh;
	return 0;
}

LRESULT TableIndexesPage::OnFindListViewData(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
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

LRESULT TableIndexesPage::OnListViewSubItemTextChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SubItemValues changedVals = listView.getChangedVals();
	for (auto val : changedVals) {
		adapter->changeRuntimeDatasItem(val.iItem, val.iSubItem, val.origVal, val.newVal);
		adapter->invalidateSubItem(val.iItem, val.iSubItem);
	}
	// send msg to TableStructurePage, class chain : TableIndexesPage($this)->QTabView($tabView)->TableTabView->TableStructurePage
	HWND pHwnd = GetParent().GetParent().GetParent().m_hWnd;
	::PostMessage(pHwnd, Config::MSG_TABLE_PREVIEW_SQL_ID, NULL, NULL);
	return 0;
}

LRESULT TableIndexesPage::OnTableColumsChangePrimaryKey(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ColumnInfoList pkColumns = tblColumnsPageAdapter->getPrimaryKeyColumnInfoList();
	adapter->changePrimaryKey(pkColumns);
	
	// send msg to TableStructurePage, class chain : TableIndexesPage($this)->QTabView($tabView)->TableTabView->TableStructurePage
	HWND pHwnd = GetParent().GetParent().GetParent().m_hWnd;
	::PostMessage(pHwnd, Config::MSG_TABLE_PREVIEW_SQL_ID, NULL, NULL);
	return 0;
}


LRESULT TableIndexesPage::OnTableColumsChangeColumnName(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	std::wstring oldColumnName, newColumnName; 
	std::wstring * origBuff = (std::wstring *)wParam;
	std::wstring * newBuff = (std::wstring *)lParam;
	if (origBuff) {
		oldColumnName.assign(origBuff->c_str());
		delete origBuff;
	}

	if (newBuff) {
		newColumnName.assign(newBuff->c_str());
		delete newBuff;
	}
	
	Q_DEBUG(L"TableIndexesPage->changeColumnName arrive, wParam:{}, lParam:{}", oldColumnName, newColumnName);
	if (oldColumnName.empty() || newColumnName.empty() || oldColumnName == newColumnName) {
		Q_ERROR(L"oldColumnName or newColumnName can't be empty, oldColumnName:{}, newColumnName:{}", oldColumnName, newColumnName);
		return 0;
	}

	adapter->changeTableColumnName(oldColumnName, newColumnName);

	// send msg to TableStructurePage, class chain : TableIndexesPage($this)->QTabView($tabView)->TableTabView->TableStructurePage
	HWND pHwnd = GetParent().GetParent().GetParent().m_hWnd;
	::PostMessage(pHwnd, Config::MSG_TABLE_PREVIEW_SQL_ID, NULL, NULL);

	return 0;
}


LRESULT TableIndexesPage::OnTableColumsDeleteColumnName(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	std::wstring columnName; 
	std::wstring * buff = (std::wstring *)wParam;
	if (buff) {
		columnName.assign(buff->c_str());
		delete buff;
	}
	if (columnName.empty()) {
		Q_ERROR(L"columnName can't be empty");
		return 0;
	}
	adapter->deleteTableColumnName(columnName);

	// send msg to TableStructurePage, class chain : TableIndexesPage($this)->QTabView($tabView)->TableTabView->TableStructurePage
	HWND pHwnd = GetParent().GetParent().GetParent().m_hWnd;
	::PostMessage(pHwnd, Config::MSG_TABLE_PREVIEW_SQL_ID, NULL, NULL);
	return 0;
}

LRESULT TableIndexesPage::OnClickNewIndexButton(UINT uNotifyCode, int nID, HWND wndCtl)
{
	adapter->createNewIndex();
	
	// send msg to TableStructurePage, class chain : TableIndexesPage($this)->QTabView($tabView)->TableTabView->TableStructurePage
	HWND pHwnd = GetParent().GetParent().GetParent().m_hWnd;
	::PostMessage(pHwnd, Config::MSG_TABLE_PREVIEW_SQL_ID, NULL, NULL);
	return 0;
}

LRESULT TableIndexesPage::OnClickDelIndexButton(UINT uNotifyCode, int nID, HWND wndCtl)
{
	adapter->deleteSelIndexes();
	// send msg to TableStructurePage, class chain : TableIndexesPage($this)->QTabView($tabView)->TableTabView->TableStructurePage
	HWND pHwnd = GetParent().GetParent().GetParent().m_hWnd;
	::PostMessage(pHwnd, Config::MSG_TABLE_PREVIEW_SQL_ID, NULL, NULL);
	return 0;
}