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

 * @file   TableIndexColumnsDialog.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-10-16
 *********************************************************************/
#include "stdafx.h"
#include "TableIndexColumnsDialog.h"

TableIndexColumnsDialog::TableIndexColumnsDialog(
	HWND parentHwnd, TableColumnsPageAdapter * columnPageAdapter, 
	TableIndexesPageAdapter * indexPageAdapter, CRect btnRect, int iItem, int iSubItem)
{
	this->parentHwnd = parentHwnd;
	this->iItem = iItem;
	this->iSubItem = iSubItem;
	this->btnRect = btnRect;
	this->columnPageAdapter = columnPageAdapter;
	this->indexPageAdapter = indexPageAdapter;

	caption = S(L"columns");
	formWidth = 500;
	formHeight = 350;
}


void TableIndexColumnsDialog::createOrShowUI()
{
	initWindowRect();
	QDialog::createOrShowUI();
	CRect clientRect;
	GetClientRect(clientRect);
	createOrShowLeftListBox(clientRect);
	createOrShowRightListBox(clientRect);
	createOrShowButtons(clientRect);
}


void TableIndexColumnsDialog::initWindowRect()
{
	int x = btnRect.left - 5, y = btnRect.bottom,
		w = formWidth, 
		h = formHeight;

	::SetWindowPos(m_hWnd, GetParent().m_hWnd,  x, y, w, h, SWP_SHOWWINDOW);
}

void TableIndexColumnsDialog::createOrShowLeftListBox(CRect & clientRect)
{
	int x = 20, y = 5, w = 150, h = 20;
	CRect rect(x, y, x + w, y + h);
	createOrShowFormLabel(leftLabel, S(L"to-be-selected"), rect, clientRect, SS_LEFT);

	rect.OffsetRect(0, h + 10);
	rect.bottom += 200;
	createOrShowFormListBox(leftListBox, Config::TABLE_INDEX_TO_BE_SELECTED_COLUMNS_LISTBOX_ID, rect, clientRect);
}


void TableIndexColumnsDialog::createOrShowRightListBox(CRect & clientRect)
{
	int x = 270, y = 5, w = 150, h = 20;
	CRect rect(x, y, x + w, y + h);
	createOrShowFormLabel(rightLabel, S(L"selected"), rect, clientRect, SS_LEFT);

	rect.OffsetRect(0, h + 10);
	rect.bottom += 200;
	createOrShowFormListBox(rightListBox, Config::TABLE_INDEX_SELECTED_COLUMNS_LISTBOX_ID, rect, clientRect);
}


void TableIndexColumnsDialog::createOrShowButtons(CRect & clientRect)
{
	// left/right
	int x = 205, y = 5 + 20 + 10 + (200 - 50) / 2, w = 30, h = 20;
	CRect rect(x, y, x + w, y + h);
	createOrShowFormButton(moveRightButton, Config::TABLE_INDEX_MOVE_RIGHT_BUTTON_ID,  L">>", rect, clientRect);
	rect.OffsetRect(0, h + 10);
	createOrShowFormButton(moveLeftButton, Config::TABLE_INDEX_MOVE_LEFT_BUTTON_ID,  L"<<", rect, clientRect);

	// up/down
	x = 205 + 30 + 35 + 150 + 10, y = 5 + 20 + 10 + (200 - 50) / 2;
	rect = { x, y, x + w, y + h };
	createOrShowFormButton(moveUpButton, Config::TABLE_INDEX_MOVE_UP_BUTTON_ID,  S(L"up"), rect, clientRect);
	rect.OffsetRect(0, h + 10);
	createOrShowFormButton(moveDownButton, Config::TABLE_INDEX_MOVE_DOWN_BUTTON_ID,  S(L"down"), rect, clientRect);
}

void TableIndexColumnsDialog::loadWindow()
{
	if (!isNeedReload) {
		return ;
	}

	isNeedReload = false;
	loadLeftListBox();
	loadRightListBox();
}


void TableIndexColumnsDialog::loadLeftListBox()
{
	leftListBox.ResetContent();

	std::wstring columns = indexPageAdapter->getSubItemString(iItem, iSubItem);
	std::vector<std::wstring> selColumnNames = StringUtil::split(columns, L",");

	const std::vector<std::wstring> columnNames = columnPageAdapter->getAllColumnNames(selColumnNames);
	for (auto columnName : columnNames) {
		leftListBox.AddString(columnName.c_str());
	}
}


void TableIndexColumnsDialog::loadRightListBox()
{
	rightListBox.ResetContent();

	std::wstring columns = indexPageAdapter->getSubItemString(iItem, iSubItem);
	std::vector<std::wstring> selColumnNames = StringUtil::split(columns, L",");
	for (auto columnName : selColumnNames) {
		rightListBox.AddString(columnName.c_str());
	}
}

LRESULT TableIndexColumnsDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	QDialog::OnInitDialog(uMsg, wParam, lParam, bHandled); 

	loadWindow();
	return 0;
}

LRESULT TableIndexColumnsDialog::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	QDialog::OnDestroy(uMsg, wParam, lParam, bHandled); 

	// todo
	if (leftLabel.IsWindow()) leftLabel.DestroyWindow();
	if (rightLabel.IsWindow()) rightLabel.DestroyWindow();
	if (leftListBox.IsWindow()) leftListBox.DestroyWindow();
	if (rightListBox.IsWindow()) rightListBox.DestroyWindow();
	if (moveLeftButton.IsWindow()) moveLeftButton.DestroyWindow();
	if (moveRightButton.IsWindow()) moveRightButton.DestroyWindow();
	if (moveUpButton.IsWindow()) moveUpButton.DestroyWindow();
	if (moveDownButton.IsWindow()) moveDownButton.DestroyWindow();
	return 0;
}


void TableIndexColumnsDialog::OnClickMoveRightButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	int nItemCount = leftListBox.GetCount();
	if (!nItemCount) {
		return;
	}

	int nSelItem = leftListBox.GetCurSel();
	if (nSelItem == -1) {
		leftListBox.SetCurSel(0);
		return;
	}
	
	wchar_t cch[256] = {0};
	leftListBox.GetText(nSelItem, cch);

	// add to right ListBox
	rightListBox.AddString(cch);

	// delte from left ListBox
	leftListBox.DeleteString(nSelItem);

	if (nSelItem < nItemCount) {
		leftListBox.SetCurSel(nSelItem);
	}
}


void TableIndexColumnsDialog::OnClickMoveLeftButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	int nItemCount = rightListBox.GetCount();
	if (!nItemCount) {
		return;
	}
	int nSelItem = rightListBox.GetCurSel();
	if (nSelItem == -1) {
		rightListBox.SetCurSel(0);
		return;
	}
	
	wchar_t cch[256] = {0};
	rightListBox.GetText(nSelItem, cch);

	// add to right ListBox
	leftListBox.AddString(cch);

	// delte from left ListBox
	rightListBox.DeleteString(nSelItem);

	if (nSelItem < nItemCount) {
		rightListBox.SetCurSel(nSelItem);
	}
}


void TableIndexColumnsDialog::OnClickMoveUpButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	int nItemCount = rightListBox.GetCount();
	if (!nItemCount) {
		return;
	}
	int nSelItem = rightListBox.GetCurSel();
	if (nSelItem == -1) {
		rightListBox.SetCurSel(0);
		return;
	}

	if (nSelItem == 0) {
		return;
	}

	wchar_t cch[256] = {0};
	rightListBox.GetText(nSelItem, cch);
	rightListBox.DeleteString(nSelItem);
	rightListBox.InsertString(nSelItem - 1, cch);

	rightListBox.SetCurSel(nSelItem - 1);
}


void TableIndexColumnsDialog::OnClickMoveDownButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	int nItemCount = rightListBox.GetCount();
	if (!nItemCount) {
		return;
	}
	int nSelItem = rightListBox.GetCurSel();
	if (nSelItem == -1) {
		rightListBox.SetCurSel(0);
		return;
	}

	if (nSelItem == nItemCount - 1) {
		return;
	}

	wchar_t cch[256] = {0};
	rightListBox.GetText(nSelItem + 1, cch);
	rightListBox.DeleteString(nSelItem + 1);
	rightListBox.InsertString(nSelItem, cch);

	rightListBox.SetCurSel(nSelItem + 1);
}

void TableIndexColumnsDialog::OnClickYesButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	
	int nItemCount = rightListBox.GetCount();
	if (!nItemCount) {
		Q_ERROR(S(L"no-columns-selected"));
		rightListBox.SetFocus();
		return;
	}
	std::wstring selColumns;
	for (int i = 0; i < nItemCount; i++) {
		wchar_t cch[256] = {0};
		rightListBox.GetText(i, cch);
		selColumns.append(cch).append(L",");
	}
	selColumns = StringUtil::cutLastChar(selColumns);
	indexPageAdapter->changeColumnText(iItem, iSubItem, selColumns);
	EndDialog(0);
}
