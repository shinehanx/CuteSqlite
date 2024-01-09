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
#include "ui/common/message/QPopAnimate.h"
#include "common/AppContext.h"
#include "ui/common/message/QMessageBox.h"

ResultTableDataPage::ResultTableDataPage()
{
	settingPrefix = TABLE_DATA_SETTING_PREFIX;
}

void ResultTableDataPage::setup(std::wstring & table)
{
	this->table = table;
	// generate query table sql
	if (!table.empty()) {
		this->sql.assign(L"SELECT ROWID AS _ct_sqlite_rowid,* FROM \"").append(table).append(L"\"");
	}
	
	formViewReadOnly = false;
}

void ResultTableDataPage::setup(QueryPageSupplier *supplier, std::wstring sql)
{
	ResultListPage::setup(supplier, sql);
}

void ResultTableDataPage::loadTableDatas()
{
	if (table.empty() || sql.empty()) {
		return ;
	}
	if (adapter->isDirty()) {
		if (QMessageBox::confirm(m_hWnd, S(L"save-if-data-has-changed"), S(L"save"), S(L"cancel")) 
			== Config::CUSTOMER_FORM_YES_BUTTON_ID) {
			adapter->save();
			enableToolButtonsAndDirty();
		} else {
			return ;
		}
	}
	saveLimitParams();
	loadListView();
	isNeedReload = false;
	enableToolButtonsAndDirty();
}

void ResultTableDataPage::enableToolButtonsAndDirty()
{
	enableSaveButton();
	enableCancelButton();
	enableDataDirty();
	enableDeleteButton();
}

void ResultTableDataPage::createOrShowUI()
{
	ResultListPage::createOrShowUI();

	CRect clientRect;
	GetClientRect(clientRect);
}

void ResultTableDataPage::loadWindow()
{
	ResultListPage::loadWindow();

	if (!databaseSupplier || databaseSupplier->selectedTable.empty()) {
		return ;
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
		DWORD dwStyle = WS_CHILD | WS_TABSTOP | WS_VISIBLE | LVS_ALIGNLEFT | LVS_REPORT | LVS_SHOWSELALWAYS | WS_BORDER | LVS_OWNERDATA | LVS_OWNERDRAWFIXED; 
		
		win.Create(m_hWnd, rect, NULL, dwStyle, 0, Config::DATABASE_QUERY_LISTVIEW_ID);
		win.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER | LVS_EX_CHECKBOXES);
		win.setItemHeight(22);
		adapter = new ResultListPageAdapter(m_hWnd, &win, QUERY_TABLE_DATA);
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
	newRowButton.SetToolTip(S(L"new-row-tip"));

	rect.OffsetRect(16 + 10, 0);
	if (!copyRowButton.IsWindow()) {
		normalImagePath = imgDir + L"database\\list\\button\\copy-row-button-normal.png";
		pressedImagePath = imgDir + L"database\\list\\button\\copy-row-button-pressed.png";
		copyRowButton.SetIconPath(normalImagePath, pressedImagePath);
		copyRowButton.SetBkgColors(buttonColor, buttonColor, buttonColor);
	}
	QWinCreater::createOrShowButton(m_hWnd, copyRowButton, Config::LISTVIEW_COPY_ROW_BUTTON_ID, L"", rect, clientRect);
	copyRowButton.SetToolTip(S(L"copy-row-tip"));

	rect.OffsetRect(16 + 10, 0);
	if (!saveButton.IsWindow()) {
		normalImagePath = imgDir + L"database\\list\\button\\save-button-disabled.png";
		pressedImagePath = imgDir + L"database\\list\\button\\save-button-disabled.png";
		saveButton.SetIconPath(normalImagePath, pressedImagePath);
		saveButton.SetBkgColors(buttonColor, buttonColor, buttonColor);
	} 
	QWinCreater::createOrShowButton(m_hWnd, saveButton, Config::LISTVIEW_SAVE_BUTTON_ID, L"", rect, clientRect);
	saveButton.SetToolTip(S(L"save-tip"));
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
	deleteButton.SetToolTip(S(L"delete-tip"));
	enableDeleteButtonState(enabledDelete);

	rect.OffsetRect(16 + 10, 0);
	if (!cancelButton.IsWindow()) {
		normalImagePath = imgDir + L"database\\list\\button\\cancel-button-normal.png";
		pressedImagePath = imgDir + L"database\\list\\button\\cancel-button-pressed.png";
		cancelButton.SetIconPath(normalImagePath, pressedImagePath);
		cancelButton.SetBkgColors(buttonColor, buttonColor, buttonColor);
	}
	QWinCreater::createOrShowButton(m_hWnd, cancelButton, Config::LISTVIEW_CANCEL_BUTTON_ID, L"", rect, clientRect);
	cancelButton.SetToolTip(S(L"cancel-tip"));
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
	rect.InflateRect(0, 0, 80, 4);
	QWinCreater::createOrShowCheckBox(m_hWnd, formViewCheckBox, Config::LISTVIEW_FORMVIEW_CHECKBOX_ID, S(L"show-form-view"), rect, clientRect);
}


void ResultTableDataPage::enableDataDirty()
{
	bool isDirty = adapter ? adapter->isDirty() : false;
	supplier->setIsDirty(isDirty);
	// class chain : ResultTableDataPage(this) -> QTabView -> ResultTabView
	HWND pHwnd = GetParent().GetParent().m_hWnd; // ResultTabView
	::PostMessage(pHwnd, Config::MSG_DATA_DIRTY_ID, WPARAM(m_hWnd), LPARAM(isDirty));

	// class chain : ResultTableDataPage(this) -> QTabView -> ResultTabView -> CHorSplitterWindow -> QueryPage -> QTabView(tabView) -> RightWorkView
	HWND rightWorkViewHwnd = GetParent().GetParent().GetParent().GetParent().GetParent().GetParent().m_hWnd; // RightWorkView
	// class chain : ResultTableDataPage(this) -> QTabView -> ResultTabView -> CHorSplitterWindow -> QueryPage
	HWND queryPageHwnd = GetParent().GetParent().GetParent().GetParent().m_hWnd; //queryPage
	::PostMessage(rightWorkViewHwnd, Config::MSG_DATA_DIRTY_ID, WPARAM(queryPageHwnd), LPARAM(isDirty));
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

void ResultTableDataPage::enableDeleteButton()
{
	if (listView.GetSelectedCount()) {
		enableDeleteButtonState(true);
	} else {
		enableDeleteButtonState(false);
	}
}

void ResultTableDataPage::enableDeleteButtonState(bool enabled)
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

void ResultTableDataPage::enableSelectAll()
{
	listView.changeAllItemsCheckState();
}

int ResultTableDataPage::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	bool ret = ResultListPage::OnCreate(lpCreateStruct);
	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_DATA_HAS_CHANGED_ID);
	return ret;
}

int ResultTableDataPage::OnDestroy()
{
	bool ret = ResultListPage::OnDestroy();
	AppContext::getInstance()->unsubscribe(m_hWnd, Config::MSG_DATA_HAS_CHANGED_ID);

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
	// 1. save the changes first
	if (adapter->isDirty()) {
		if (QMessageBox::confirm(m_hWnd, S(L"save-if-data-has-changed"), S(L"save"), S(L"cancel")) 
			== Config::CUSTOMER_FORM_YES_BUTTON_ID) {
			adapter->save();
			enableToolButtonsAndDirty();
		} else {
			return ;
		}
	}
	ResultListPage::OnClickFilterButton(uNotifyCode, nID, hwnd);
	enableToolButtonsAndDirty();
}

void ResultTableDataPage::OnClickRefreshButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	// 1. save the changes first
	if (adapter->isDirty()) {
		if (QMessageBox::confirm(m_hWnd, S(L"save-if-data-has-changed"), S(L"save"), S(L"cancel")) 
			== Config::CUSTOMER_FORM_YES_BUTTON_ID) {
			adapter->save();
			enableToolButtonsAndDirty();
		} else {
			return ;
		}
	}

	ResultListPage::OnClickRefreshButton(uNotifyCode, nID, hwnd);
	enableToolButtonsAndDirty();
}

LRESULT ResultTableDataPage::OnClickListView(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
	auto ret = ResultListPage::OnClickListView(idCtrl, pnmh, bHandled);
	
	// Clicked the next row of bottom item , will insert a new column row
	NMITEMACTIVATE * clickItem = (NMITEMACTIVATE *)pnmh; 
	CPoint pt = clickItem->ptAction;
	CRect lastRowRect;
	listView.GetItemRect(listView.GetItemCount() - 1, lastRowRect, LVIR_BOUNDS);
	lastRowRect.OffsetRect(0, lastRowRect.Height());
	if (lastRowRect.PtInRect(pt)) {
		adapter->createNewRow();
		enableSaveButton();
		enableCancelButton();
		enableDataDirty();
	}
	// show toolbar buttons after selected items in ListView
	enableDeleteButton();
	return ret;
}

LRESULT ResultTableDataPage::OnClickListViewHeader(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
	LRESULT ret = ResultListPage::OnClickListViewHeader(idCtrl, pnmh, bHandled);
	enableToolButtonsAndDirty();
	return ret;
}


LRESULT ResultTableDataPage::OnDbClickListView(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
	NMITEMACTIVATE * aItem = (NMITEMACTIVATE *)pnmh; 
	
	subItemPos.first = aItem->iItem, subItemPos.second = aItem->iSubItem;
	if (aItem->iSubItem == 0) {
		bHandled = 0;
		return 0;
	}

	Q_INFO(L"OnDbClickListView, pdi.item.iItem.:{}, pdi.item.iSubItem:{}", aItem->iItem, aItem->iSubItem);
	
	// show the editor
	listView.createOrShowEditor(subItemPos);

	// show toolbar buttons after selected items in ListView
	enableDeleteButton();

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
	enableDataDirty();

	return 0;
}

LRESULT ResultTableDataPage::OnListViewItemCheckBoxChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	enableDeleteButton();
	return 0;
}

LRESULT ResultTableDataPage::OnHandleDataHasChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return supplier->getIsDirty() ? 0 : 1;
}

LRESULT ResultTableDataPage::OnClickNewRowButton(UINT uNotifyCode, int nID, HWND wndCtl)
{
	adapter->createNewRow();
	enableToolButtonsAndDirty();
	enableSelectAll();
	return 0;
}

LRESULT ResultTableDataPage::OnClickCopyRowButton(UINT uNotifyCode, int nID, HWND wndCtl)
{
	adapter->copyNewRow();
	enableSaveButton();
	enableCancelButton();
	enableDataDirty();
	return 0;
}

LRESULT ResultTableDataPage::OnClickSaveButton(UINT uNotifyCode, int nID, HWND wndCtl)
{
	save();
	return 0;
}

void ResultTableDataPage::save()
{
	adapter->save();
	enableSaveButton();
	enableCancelButton();
	enableDataDirty();
	clearFormView();
}

LRESULT ResultTableDataPage::OnClickDeleteButton(UINT uNotifyCode, int nID, HWND wndCtl)
{
	if (adapter->remove()) {
		enableToolButtonsAndDirty();
		clearFormView();
		QPopAnimate::success(S(L"delete-success-text"));
		listView.changeAllItemsCheckState();
	}
	
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
	enableDataDirty();
	return 0;
}

void ResultTableDataPage::doCancelFormView()
{
	SubItemValues changedVals = listView.getChangedVals();
	for (auto val : changedVals) {
		formView.setEditText(val.iSubItem - 1, val.origVal);
	}
}
