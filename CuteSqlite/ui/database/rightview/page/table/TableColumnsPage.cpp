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

 * @file   TableColumnsPage.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-10-10
 *********************************************************************/
#include "stdafx.h"
#include "TableColumnsPage.h"
#include "common/AppContext.h"
#include "utils/ResourceUtil.h"
#include "ui/common/QWinCreater.h"
#include "core/common/Lang.h"

BOOL TableColumnsPage::PreTranslateMessage(MSG* pMsg)
{
	if (listView.IsWindow() && listView.PreTranslateMessage(pMsg)) {
		return TRUE;
	}
	return FALSE;
}


void TableColumnsPage::setup(TableStructureSupplier * supplier)
{
	this->supplier = supplier;
}


TableColumnsPageAdapter * TableColumnsPage::getAdapter()
{
	ATLASSERT(adapter != nullptr);
	return adapter;
}

void TableColumnsPage::createOrShowUI()
{
	QPage::createOrShowUI();

	CRect clientRect;
	GetClientRect(clientRect);
	createOrShowToolBarElems(clientRect);
	createOrShowListView(listView, clientRect);
}

void TableColumnsPage::createOrShowToolBarElems(CRect & clientRect)
{
	CRect topbarRect = getTopRect(clientRect);
	int x = 5, y = 5, w = PAGE_BUTTON_WIDTH, h = PAGE_BUTTON_HEIGHT;
	CRect rect(x, y, x + w, y + h);
	
	// create or show button
	std::wstring imgDir = ResourceUtil::getProductImagesDir();
	std::wstring normalImagePath, pressedImagePath;
	if (!newColumnButton.IsWindow()) {
		normalImagePath = imgDir + L"database\\list\\button\\column\\new-button-normal.png";
		pressedImagePath = imgDir + L"database\\list\\button\\column\\new-button-pressed.png";
		newColumnButton.SetIconPath(normalImagePath, pressedImagePath);
		newColumnButton.SetBkgColors(buttonColor, buttonColor, buttonColor);
	}
	QWinCreater::createOrShowButton(m_hWnd, newColumnButton, Config::TABLE_NEW_COLUMN_BUTTON_ID, L"", rect, clientRect);
	newColumnButton.SetToolTip(S(L"insert-new-column"));

	rect.OffsetRect(16 + 10, 0);
	if (!delColumnButton.IsWindow()) {
		normalImagePath = imgDir + L"database\\list\\button\\column\\del-button-normal.png";
		pressedImagePath = imgDir + L"database\\list\\button\\column\\del-button-pressed.png";
		delColumnButton.SetIconPath(normalImagePath, pressedImagePath);
		delColumnButton.SetBkgColors(buttonColor, buttonColor, buttonColor);
	}
	QWinCreater::createOrShowButton(m_hWnd, delColumnButton, Config::TABLE_DEL_COLUMN_BUTTON_ID, L"", rect, clientRect);
	delColumnButton.SetToolTip(S(L"delete-sel-column"));

	rect.OffsetRect(16 + 10, 0);
	if (!upColumnButton.IsWindow()) {
		normalImagePath = imgDir + L"database\\list\\button\\column\\up-button-normal.png";
		pressedImagePath = imgDir + L"database\\list\\button\\column\\up-button-pressed.png";
		upColumnButton.SetIconPath(normalImagePath, pressedImagePath);
		upColumnButton.SetBkgColors(buttonColor, buttonColor, buttonColor);
	}
	QWinCreater::createOrShowButton(m_hWnd, upColumnButton, Config::TABLE_UP_COLUMN_BUTTON_ID, L"", rect, clientRect);
	upColumnButton.SetToolTip(S(L"move-up"));

	rect.OffsetRect(16 + 10, 0);
	if (!downColumnButton.IsWindow()) {
		normalImagePath = imgDir + L"database\\list\\button\\column\\down-button-normal.png";
		pressedImagePath = imgDir + L"database\\list\\button\\column\\down-button-pressed.png";
		downColumnButton.SetIconPath(normalImagePath, pressedImagePath);
		downColumnButton.SetBkgColors(buttonColor, buttonColor, buttonColor);
	}
	QWinCreater::createOrShowButton(m_hWnd, downColumnButton, Config::TABLE_DOWN_COLUMN_BUTTON_ID, L"", rect, clientRect);
	downColumnButton.SetToolTip(S(L"move-down"));
}

void TableColumnsPage::createOrShowListView(QListViewCtrl & win, CRect & clientRect)
{
	CRect & rect = getPageRect(clientRect);
	if (IsWindow() && !win.IsWindow()) {
		// Specify LVS_OWNERDATA style will be enabled virtual data list
		DWORD dwStyle = WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_BORDER | LVS_ALIGNLEFT | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_OWNERDATA | LVS_OWNERDRAWFIXED;
		DWORD dwExStyle = LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER  ;
		win.Create(m_hWnd, rect,NULL,dwStyle , 
			0, Config::DATABASE_TABLE_COLUMNS_LISTVIEW_ID );
		win.SetExtendedListViewStyle(dwExStyle);
		win.setItemHeight(22);
		adapter = new TableColumnsPageAdapter(m_hWnd, &win, supplier);
	} else if (IsWindow() && win.IsWindow() && clientRect.Width() > 1) {
		win.MoveWindow(rect);
		win.ShowWindow(true);
	}
}

void TableColumnsPage::loadWindow()
{
	QPage::loadWindow();

	if (!isNeedReload) {
		adapter->selectListViewItemForManage();
		return;
	}
	isNeedReload = false;
	loadListView();
	adapter->selectListViewItemForManage();
}

void TableColumnsPage::loadListView()
{
	rowCount = adapter->loadTblColumnsListView();
	
}

int TableColumnsPage::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	bool ret = QPage::OnCreate(lpCreateStruct);

	textFont = FT(L"form-text-size");

	return ret;
}

int TableColumnsPage::OnDestroy()
{
	bool ret = QPage::OnDestroy();
	if (textFont) ::DeleteObject(textFont);

	if (newColumnButton.IsWindow()) newColumnButton.DestroyWindow();
	if (delColumnButton.IsWindow()) delColumnButton.DestroyWindow();
	if (upColumnButton.IsWindow()) upColumnButton.DestroyWindow();
	if (downColumnButton.IsWindow()) downColumnButton.DestroyWindow();

	if (listView.IsWindow()) listView.DestroyWindow();

	if (imageList.IsNull()) imageList.Destroy();

	if (adapter) {
		delete adapter;
		adapter = nullptr;
	}
	return ret;
}

void TableColumnsPage::paintItem(CDC & dc, CRect & paintRect)
{

}


LRESULT TableColumnsPage::OnDbClickListView(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
	NMITEMACTIVATE * aItem = (NMITEMACTIVATE *)pnmh; 
	
	subItemPos.first = aItem->iItem, subItemPos.second = aItem->iSubItem;

	if (aItem->iSubItem == 0 || (aItem->iSubItem >= 2 && aItem->iSubItem <= 6)) {
		return 0;
	}

	// show the editor
	listView.createOrShowEditor(subItemPos);
	return 0;
}


LRESULT TableColumnsPage::OnClickListView(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
	NMITEMACTIVATE * aItem = (NMITEMACTIVATE *)pnmh; 
	adapter->clickListViewSubItem(aItem);
	listView.changeAllItemsCheckState();
	return 0;
}

HBRUSH TableColumnsPage::OnCtlColorStatic(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, topbarColor);
	::SelectObject(hdc, textFont);
	return topbarBrush.m_hBrush;
}

HBRUSH TableColumnsPage::OnCtlColorListBox(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, bkgColor);
	::SelectObject(hdc, textFont);
	return AtlGetStockBrush(WHITE_BRUSH);
}

HBRUSH TableColumnsPage::OnCtlColorEdit(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, bkgColor);
	::SelectObject(hdc, textFont);
	return AtlGetStockBrush(WHITE_BRUSH);
}

LRESULT TableColumnsPage::OnGetListViewData(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
	NMLVDISPINFO* plvdi = (NMLVDISPINFO*)pnmh;
	adapter->fillDataInListViewSubItem(plvdi);

	return 0;
}

LRESULT TableColumnsPage::OnPrepareListViewData(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
	auto pCachehint = (NMLVCACHEHINT *)pnmh;
	return 0;
}

LRESULT TableColumnsPage::OnFindListViewData(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
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

/**
 * This message handler function only provide to ComboBox text changed and editor text changed in the ListView,
 * not support checkBox
 * 
 * @param uMsg
 * @param wParam
 * @param lParam
 * @param bHandled
 * @return 
 */
LRESULT TableColumnsPage::OnListViewSubItemTextChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	const SubItemValues &changedVals = listView.getChangedVals();
	for (auto &val : changedVals) {
		//change the columns of TableIndexesPage that including this changed column name
		if (val.iSubItem == 1) { // 1 - column name
			auto & origName = supplier->getColsRuntimeDatas().at(val.iItem).name;
			auto & newName = val.newVal;
			if (origName != newName) {
				supplier->updateRelatedColumnsIfChangeColumnName(origName, newName);
			}			
		}

		adapter->changeRuntimeDatasItem(val.iItem, val.iSubItem, val.newVal);
		adapter->invalidateSubItem(val.iItem, val.iSubItem);
	}

	// send msg to TableStructurePage, class chain : TableColumnsPage($this)->QTabView($tabView)->TableTabView->TableStructurePage
	HWND pHwnd = GetParent().GetParent().GetParent().m_hWnd;
	::PostMessage(pHwnd, Config::MSG_TABLE_PREVIEW_SQL_ID, NULL, NULL);
	return 0;
}

LRESULT TableColumnsPage::OnListViewSubItemCheckBoxChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (wParam < 0 || lParam < 3 || lParam > 6) {
		return 0;
	}
	int iItem = static_cast<int>(wParam);
	int iSubItem = static_cast<int>(lParam);
	
	adapter->changeListViewCheckBox(iItem, iSubItem);
	// send msg to TableStructurePage, class chain : TableColumnsPage($this)->QTabView($tabView)->TableTabView->TableStructurePage
	HWND pHwnd = GetParent().GetParent().GetParent().m_hWnd;
	::PostMessage(pHwnd, Config::MSG_TABLE_PREVIEW_SQL_ID, NULL, NULL);
	return 0;
}

LRESULT TableColumnsPage::OnClickNewColumnButton(UINT uNotifyCode, int nID, HWND wndCtl)
{
	adapter->createNewColumn();
	return 0;
}

LRESULT TableColumnsPage::OnClickDelColumnButton(UINT uNotifyCode, int nID, HWND wndCtl)
{
	adapter->deleteSelColumns();
	return 0;
}

LRESULT TableColumnsPage::OnClickUpColumnButton(UINT uNotifyCode, int nID, HWND wndCtl)
{
	adapter->moveUpSelColumns();
	return 0;
}

LRESULT TableColumnsPage::OnClickDownColumnButton(UINT uNotifyCode, int nID, HWND wndCtl)
{
	adapter->moveDownSelColumns();
	return 0;
}
