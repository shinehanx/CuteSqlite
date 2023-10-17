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

 * @file   ResultTableDataPage.cpp
 * @brief  Select the table and then show the list of table data
 * 
 * @author Xuehan Qin
 * @date   2023-05-22
 *********************************************************************/

#include "stdafx.h"
#include "ResultTableDataPage.h"
#include <atlstr.h>
#include "utils/Log.h"
#include "core/common/Lang.h"
#include "ui/common/QWinCreater.h"

ResultTableDataPage::ResultTableDataPage()
{
	settingPrefix = TABLE_DATA_SETTING_PREFIX;
}

void ResultTableDataPage::setup(std::wstring & table)
{
	this->table = table;
	// genderate query table sql
	this->sql.assign(L"SELECT * FROM ").append(table);
	formViewReadOnly = false;
}

void ResultTableDataPage::loadTableDatas()
{
	if (table.empty() || sql.empty()) {
		return ;
	}
	saveLimitParams();
	loadListView();
}

void ResultTableDataPage::createOrShowUI()
{
	ResultListPage::createOrShowUI();

	CRect clientRect;
	GetClientRect(clientRect);
	
	//createOrShowEdits(clientRect);
}

void ResultTableDataPage::loadWindow()
{
	ResultListPage::loadWindow();

	if (!supplier || supplier->selectTable.empty()) {
		return ;
	}

	if (supplier->selectTable != table) {
		setup(supplier->selectTable);
		loadTableDatas();
	}	
}

/**
 * Create or show the list view 
 * Call by ResultListPage::createOrShowUI().
 * 
 * @param win
 * @param clientRect
 */
void ResultTableDataPage::createOrShowListView(QListViewCtrl & win, CRect & clientRect)
{
	CRect & rect = getLeftListRect(clientRect);
	if (IsWindow() && !win.IsWindow()) {
		// virtual listview must be have LVS_OWNERDATA style
		DWORD dwStyle = WS_CHILD | WS_TABSTOP | WS_VISIBLE | LVS_ALIGNLEFT | LVS_REPORT | LVS_SHOWSELALWAYS | WS_BORDER | LVS_OWNERDATA; 

		win.Create(m_hWnd, rect, NULL, dwStyle, 0, Config::DATABASE_QUERY_LISTVIEW_ID);
		win.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER );
		win.setItemHeight(22);
		adapter = new ResultListPageAdapter(m_hWnd, &win, TABLE_DATA);
	} else if (IsWindow() && win.IsWindow() && clientRect.Width() > 1) {
		win.MoveWindow(rect);
		win.ShowWindow(true);
	}
}

void ResultTableDataPage::createOrShowToolBarElems(CRect & clientRect)
{
	CRect topbarRect = getTopRect(clientRect);
	int x = 5, y = 5, w = PAGE_BUTTON_WIDTH, h = PAGE_BUTTON_HEIGHT;
	CRect rect(x, y, x + w, y + h);
	// first pane
	doCreateOrShowToolBarFirstPaneElems(rect, clientRect);

	// second pane
	doCreateOrShowToolBarSecondPaneElems(rect, clientRect);

	// third pane
	doCreateOrShowToolBarThirdPaneElems(rect, clientRect);
		
	// right pane
	doCreateOrShowToolBarRightPaneElems(rect, clientRect);
}

void ResultTableDataPage::doCreateOrShowToolBarSecondPaneElems(CRect &rect, CRect & clientRect)
{
	
	std::wstring imgDir = ResourceUtil::getProductImagesDir();
	// split
	CRect splitRect(rect.right + 2, rect.top, rect.right + 18, rect.bottom);
	if (!splitImage.IsWindow()) {
		std::wstring splitImagePath = imgDir + L"database\\list\\button\\split.png";
		splitImage.load(splitImagePath.c_str(), BI_PNG, true);
		splitImage.setBkgColor(topbarColor); 
	}
	QWinCreater::createOrShowImage(m_hWnd, splitImage, 0, splitRect, clientRect);

	rect.OffsetRect(16 + 22, 0);	
	std::wstring normalImagePath, pressedImagePath;
	if (!newRowButton.IsWindow()) {
		normalImagePath = imgDir + L"database\\list\\button\\new-row-button-normal.png";
		pressedImagePath = imgDir + L"database\\list\\button\\new-row-button-pressed.png";
		newRowButton.SetIconPath(normalImagePath, pressedImagePath);
		newRowButton.SetBkgColors(buttonColor, buttonColor, buttonColor);
	}
	QWinCreater::createOrShowButton(m_hWnd, newRowButton, Config::LISTVIEW_NEW_ROW_BUTTON_ID, L"", rect, clientRect);
	newRowButton.SetToolTip(S(L"new-row"));

	rect.OffsetRect(16 + 10, 0);
	if (!copyRowButton.IsWindow()) {
		normalImagePath = imgDir + L"database\\list\\button\\copy-row-button-normal.png";
		pressedImagePath = imgDir + L"database\\list\\button\\copy-row-button-pressed.png";
		copyRowButton.SetIconPath(normalImagePath, pressedImagePath);
		copyRowButton.SetBkgColors(buttonColor, buttonColor, buttonColor);
	}
	QWinCreater::createOrShowButton(m_hWnd, copyRowButton, Config::LISTVIEW_COPY_ROW_BUTTON_ID, L"", rect, clientRect);
	copyRowButton.SetToolTip(S(L"copy-row"));

	rect.OffsetRect(16 + 10, 0);
	if (!saveButton.IsWindow()) {
		normalImagePath = imgDir + L"database\\list\\button\\save-button-disabled.png";
		pressedImagePath = imgDir + L"database\\list\\button\\save-button-disabled.png";
		saveButton.SetIconPath(normalImagePath, pressedImagePath);
		saveButton.SetBkgColors(buttonColor, buttonColor, buttonColor);
	} 
	QWinCreater::createOrShowButton(m_hWnd, saveButton, Config::LISTVIEW_SAVE_BUTTON_ID, L"", rect, clientRect);
	saveButton.SetToolTip(S(L"save"));
	enableSaveButton();
	
	rect.OffsetRect(16 + 10, 0);
	bool enabledDelete;
	if (!deleteButton.IsWindow()) {
		normalImagePath = imgDir + L"database\\list\\button\\delete-button-normal.png";
		pressedImagePath = imgDir + L"database\\list\\button\\delete-button-pressed.png";
		deleteButton.SetIconPath(normalImagePath, pressedImagePath);
		deleteButton.SetBkgColors(buttonColor, buttonColor, buttonColor);
		enabledDelete = false;
	} else {
		enabledDelete = deleteButton.IsWindowEnabled();
	}
	QWinCreater::createOrShowButton(m_hWnd, deleteButton, Config::LISTVIEW_DELETE_BUTTON_ID, L"", rect, clientRect);
	deleteButton.SetToolTip(S(L"delete"));
	enableDeleteButton(enabledDelete);

	rect.OffsetRect(16 + 10, 0);
	if (!cancelButton.IsWindow()) {
		normalImagePath = imgDir + L"database\\list\\button\\cancel-button-normal.png";
		pressedImagePath = imgDir + L"database\\list\\button\\cancel-button-pressed.png";
		cancelButton.SetIconPath(normalImagePath, pressedImagePath);
		cancelButton.SetBkgColors(buttonColor, buttonColor, buttonColor);
	}
	QWinCreater::createOrShowButton(m_hWnd, cancelButton, Config::LISTVIEW_CANCEL_BUTTON_ID, L"", rect, clientRect);
	cancelButton.SetToolTip(S(L"cancel"));
	enableCancelButton();
}

void ResultTableDataPage::doCreateOrShowToolBarThirdPaneElems(CRect &rect, CRect & clientRect)
{
	// split
	std::wstring imgDir = ResourceUtil::getProductImagesDir();
	CRect splitRect(rect.right + 2, rect.top, rect.right + 18, rect.bottom);
	if (!splitImage2.IsWindow()) {
		std::wstring splitImagePath = imgDir + L"database\\list\\button\\split2.png";
		splitImage2.load(splitImagePath.c_str(), BI_PNG, true); 
		splitImage2.setBkgColor(topbarColor); 
	}
	QWinCreater::createOrShowImage(m_hWnd, splitImage2, 0, splitRect, clientRect);

	// show form checkbox
	rect.OffsetRect(16 + 30, 0);
	rect.InflateRect(0, 0, 50, 4);
	QWinCreater::createOrShowCheckBox(m_hWnd, formViewCheckBox, Config::LISTVIEW_FORMVIEW_CHECKBOX_ID, S(L"show-form-view"), rect, clientRect);
}


void ResultTableDataPage::enableSaveButton()
{
	bool enabled = adapter ? adapter->isDirty() : false;
	bool origEnabled = saveButton.IsWindowEnabled();
	if (origEnabled == enabled) {
		return ;
	}
	std::wstring normalImagePath, pressedImagePath;
	std::wstring imgDir = ResourceUtil::getProductImagesDir();
	if (enabled) {
		normalImagePath = imgDir + L"database\\list\\button\\save-button-normal.png";
		pressedImagePath = imgDir + L"database\\list\\button\\save-button-pressed.png";
	} else {
		normalImagePath = imgDir + L"database\\list\\button\\save-button-disabled.png";
		pressedImagePath = imgDir + L"database\\list\\button\\save-button-disabled.png";
	}
	saveButton.SetIconPath(normalImagePath, pressedImagePath);
	saveButton.EnableWindow(enabled);
}

void ResultTableDataPage::enableDeleteButton(bool enabled)
{
	bool origEnabled = deleteButton.IsWindowEnabled();
	if (origEnabled == enabled) {
		return ;
	}
	std::wstring normalImagePath, pressedImagePath;
	std::wstring imgDir = ResourceUtil::getProductImagesDir();
	if (enabled) {
		normalImagePath = imgDir + L"database\\list\\button\\delete-button-normal.png";
		pressedImagePath = imgDir + L"database\\list\\button\\delete-button-pressed.png";
	} else {
		normalImagePath = imgDir + L"database\\list\\button\\delete-button-disabled.png";
		pressedImagePath = imgDir + L"database\\list\\button\\delete-button-disabled.png";
	}
	deleteButton.SetIconPath(normalImagePath, pressedImagePath);
	deleteButton.EnableWindow(enabled);
}

void ResultTableDataPage::enableCancelButton()
{
	bool enabled = adapter ? adapter->isDirty() : false;
	bool origEnabled = cancelButton.IsWindowEnabled();
	if (origEnabled == enabled) {
		return ;
	}
	std::wstring normalImagePath, pressedImagePath;
	std::wstring imgDir = ResourceUtil::getProductImagesDir();
	if (enabled) {
		normalImagePath = imgDir + L"database\\list\\button\\cancel-button-normal.png";
		pressedImagePath = imgDir + L"database\\list\\button\\cancel-button-pressed.png";
	} else {
		normalImagePath = imgDir + L"database\\list\\button\\cancel-button-disabled.png";
		pressedImagePath = imgDir + L"database\\list\\button\\cancel-button-disabled.png";
	}
	cancelButton.SetIconPath(normalImagePath, pressedImagePath);
	cancelButton.EnableWindow(enabled);
}

int ResultTableDataPage::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	bool ret = ResultListPage::OnCreate(lpCreateStruct);
	return ret;
}

int ResultTableDataPage::OnDestroy()
{
	bool ret = ResultListPage::OnDestroy();
	if (splitImage.IsWindow()) splitImage.DestroyWindow();
	if (splitImage2.IsWindow()) splitImage2.DestroyWindow();

	if (newRowButton.IsWindow()) newRowButton.DestroyWindow();
	if (copyRowButton.IsWindow()) copyRowButton.DestroyWindow();
	if (saveButton.IsWindow()) saveButton.DestroyWindow();
	if (deleteButton.IsWindow()) deleteButton.DestroyWindow();
	if (cancelButton.IsWindow()) cancelButton.DestroyWindow();

	return ret;
}

void ResultTableDataPage::OnClickFilterButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	ResultListPage::OnClickFilterButton(uNotifyCode, nID, hwnd);
	enableSaveButton();
	enableCancelButton();
}

void ResultTableDataPage::OnClickRefreshButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	ResultListPage::OnClickRefreshButton(uNotifyCode, nID, hwnd);
	enableSaveButton();
	enableCancelButton();
}

LRESULT ResultTableDataPage::OnClickListView(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
	auto ret = ResultListPage::OnClickListView(idCtrl, pnmh, bHandled);
	// show toolbar buttons after selected items in ListView
	afterSelectedListView();
	return ret;
}

LRESULT ResultTableDataPage::OnClickListViewColumn(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	auto ret = ResultListPage::OnClickListViewColumn(uMsg, wParam, lParam, bHandled);
	// show toolbar buttons after selected items in ListView
	afterSelectedListView();
	return ret;
}

void ResultTableDataPage::afterSelectedListView()
{
	if (listView.GetSelectedCount()) {
		enableDeleteButton(true);
	}
	else {
		enableDeleteButton(false);
	}
}

LRESULT ResultTableDataPage::OnDbClickListView(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
	NMITEMACTIVATE * aItem = (NMITEMACTIVATE *)pnmh; 
	
	subItemPos.first = aItem->iItem, subItemPos.second = aItem->iSubItem;

	Q_INFO(L"OnDbClickListView, pdi.item.iItem.:{}, pdi.item.iSubItem:{}", aItem->iItem, aItem->iSubItem);
	
	// show the editor
	listView.createOrShowEditor(subItemPos);

	// show toolbar buttons after selected items in ListView
	afterSelectedListView();

	return 0;
}

LRESULT ResultTableDataPage::OnListViewSubItemTextChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SubItemValues changedVals = listView.getChangedVals();
	for (auto val : changedVals) {
		adapter->changeRuntimeDatasItem(val.iItem, val.iSubItem, val.origVal, val.newVal);
		adapter->invalidateSubItem(val.iItem, val.iSubItem);
		formView.setEditText(val.iSubItem - 1, val.newVal);
	}
	
	enableSaveButton();
	enableCancelButton();
	
	return 0;
}

LRESULT ResultTableDataPage::OnClickNewRowButton(UINT uNotifyCode, int nID, HWND wndCtl)
{
	adapter->createNewRow();
	enableSaveButton();
	enableCancelButton();
	return 0;
}

LRESULT ResultTableDataPage::OnClickCopyRowButton(UINT uNotifyCode, int nID, HWND wndCtl)
{
	adapter->copyNewRow();
	enableSaveButton();
	enableCancelButton();
	return 0;
}

LRESULT ResultTableDataPage::OnClickSaveButton(UINT uNotifyCode, int nID, HWND wndCtl)
{
	adapter->save();
	enableSaveButton();
	enableCancelButton();
	
	
	return 0;
}

LRESULT ResultTableDataPage::OnClickDeleteButton(UINT uNotifyCode, int nID, HWND wndCtl)
{
	adapter->remove();
	enableSaveButton();
	enableCancelButton();
	
	return 0;
}

LRESULT ResultTableDataPage::OnClickCancelButton(UINT uNotifyCode, int nID, HWND wndCtl)
{
	// 1.cancel the form view editor text change
	doCancelFormView();

	// 2.cancel listview.changeVals and cancel adapter.runtimeDatas and cancel adapter.runtimeNewRows
	adapter->cancel();
	
	enableSaveButton();
	enableCancelButton();	
	return 0;
}

void ResultTableDataPage::doCancelFormView()
{
	SubItemValues changedVals = listView.getChangedVals();
	for (auto val : changedVals) {
		formView.setEditText(val.iSubItem - 1, val.origVal);
	}
}
