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

 * @file   NewTablePage.cpp
 * @brief  The class provide create a new table 
 * 
 * @author Xuehan Qin
 * @date   2023-10-09
 *********************************************************************/
#include "stdafx.h"
#include "NewTablePage.h"
#include "common/AppContext.h"
#include "core/common/Lang.h"
#include "ui/common/QWinCreater.h"

/**
 * Call the PreTranslateMessage(pMsg) function of sub element(s) class .
 * 
 * @param pMsg
 * @return 
 */
BOOL NewTablePage::PreTranslateMessage(MSG* pMsg)
{
	if (tableTabView.IsWindow() && tableTabView.PreTranslateMessage(pMsg)) {
		return TRUE;
	}
	return FALSE;
}

void NewTablePage::createOrShowUI()
{
	QPage::createOrShowUI();

	CRect clientRect;
	GetClientRect(clientRect);

	// Create sub elements
	createTblNameElems(clientRect);
	createDatabaseElems(clientRect);
	createSchemaElems(clientRect);
	createOrShowTableTabView(tableTabView, clientRect);
	createOrShowButtons(clientRect);
}


void NewTablePage::createTblNameElems(CRect & clientRect)
{
	int x = 20, y = 20, w = 80, h = 20;
	CRect rect(x, y, x + w, y + h);
	QWinCreater::createOrShowLabel(m_hWnd, tblNameLabel, S(L"tbl-name"), rect, clientRect);

	rect.OffsetRect(w + 5, 0);
	rect.right += 120;
	QWinCreater::createOrShowEdit(m_hWnd, tblNameEdit, Config::NEW_TABLE_TBL_NAME_EDIT_ID, L"", rect, clientRect, WS_CLIPCHILDREN | WS_CLIPSIBLINGS, false);
}


void NewTablePage::createDatabaseElems(CRect & clientRect)
{
	int x = 20, y = 20 + 20 + 15, w = 80, h = 20;
	CRect rect(x, y, x + w, y + h);
	QWinCreater::createOrShowLabel(m_hWnd, databaseLabel, S(L"database"), rect, clientRect);

	rect.OffsetRect(w + 5, -5);
	rect.right += 120;
	QWinCreater::createOrShowComboBox(m_hWnd, databaseComboBox, Config::NEW_TABLE_DATABASE_COMBOBOX_ID, rect, clientRect);
}


void NewTablePage::createSchemaElems(CRect & clientRect)
{
	int x = 20 + 80 + 5 + 200 + 20, y = 20 + 20 + 15, w = 80, h = 20;
	CRect rect(x, y, x + w, y + h);
	QWinCreater::createOrShowLabel(m_hWnd, schemaLabel, S(L"schema"), rect, clientRect);

	rect.OffsetRect(w + 5, -5);
	rect.right += 120;
	QWinCreater::createOrShowComboBox(m_hWnd, schemaComboBox, Config::NEW_TABLE_SCHEMA_COMBOBOX_ID, rect, clientRect);
}


void NewTablePage::createOrShowTableTabView(TableTabView & win, CRect & clientRect)
{
	int x = 20, y = 20 + 20 + 15 + 20 + 20, w = clientRect.Width() - 40, h = clientRect.Height() - y - 90;
	CRect rect(x, y, x + w, y + h);
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		win.Create(m_hWnd, rect, L"", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
		
		// set the select userDbId
		int nSelItem = databaseComboBox.GetCurSel();
		uint64_t userDbId = (uint64_t)databaseComboBox.GetItemData(nSelItem);
		CString str;
		schemaComboBox.GetWindowText(str);
		std::wstring schema = str.GetString();
		win.setup(userDbId, schema);
		return;
	} else if (win.IsWindow()){
		win.MoveWindow(rect);
		win.ShowWindow(true);
	}
}


void NewTablePage::createOrShowButtons(CRect & clientRect)
{
	int x = clientRect.right - (20 + 120) * 2, y = clientRect.bottom - 50, w = 120, h = 30;
	CRect rect(x, y, x + w, y + h);
	QWinCreater::createOrShowButton(m_hWnd, saveButton, Config::TABLE_SAVE_BUTTON_ID, S(L"save"), rect, clientRect);
	
	rect.OffsetRect(w + 20, 0);
	QWinCreater::createOrShowButton(m_hWnd, revertButton, Config::TABLE_REVERT_BUTTON_ID, S(L"revert"), rect, clientRect);
}

void NewTablePage::loadWindow()
{
	if (!isNeedReload) {
		return;
	}

	isNeedReload = false;
	loadDatabaseComboBox();
	loadSchemaComboBox();
}


void NewTablePage::loadDatabaseComboBox()
{
	// db list
	UserDbList dbs = databaseService->getAllUserDbs();
	databaseComboBox.ResetContent();
	int n = static_cast<int>(dbs.size());
	int nSelItem = -1;
	for (int i = 0; i< n; i++) {
		auto item = dbs.at(i);
		int nItem = databaseComboBox.AddString(item.name.c_str());
		databaseComboBox.SetItemData(nItem, item.id);
		if (item.isActive) {
			nSelItem = i;
		}
	}
	databaseComboBox.SetCurSel(nSelItem);
}


void NewTablePage::loadSchemaComboBox()
{
	std::vector<std::wstring> schemas;
	schemas.push_back(L"main");
	schemas.push_back(L"temp");

	int n = static_cast<int>(schemas.size());
	int nSelItem = 0;
	for (int i = 0; i< n; i++) {
		auto item = schemas.at(i);
		int nItem = schemaComboBox.AddString(item.c_str());
		schemaComboBox.SetItemData(nItem, i);		
	}
	schemaComboBox.SetCurSel(nSelItem);
}

int NewTablePage::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	bool ret = QPage::OnCreate(lpCreateStruct);
	textFont = FT(L"form-text-size");
	comboFont = FTB(L"combobox-size", true);
	btnFont = GDI_PLUS_FT(L"setting-button-size");

	saveButton.SetFontColors(RGB(0,0,0), RGB(0x12,0x96,0xdb), RGB(153,153,153));
	saveButton.SetBkgColors(RGB(238, 238, 238), RGB(238, 238, 238), RGB(238, 238, 238));

	revertButton.SetFontColors(RGB(0,0,0), RGB(0x12,0x96,0xdb), RGB(153,153,153));
	revertButton.SetBkgColors(RGB(238, 238, 238), RGB(238, 238, 238), RGB(238, 238, 238));

	return ret;
}

int NewTablePage::OnDestroy()
{
	bool ret = QPage::OnDestroy();
	if (textFont) ::DeleteObject(textFont);
	if (comboFont) ::DeleteObject(comboFont);
	if (btnFont) ::DeleteObject(btnFont);

	if (tblNameLabel.IsWindow()) tblNameLabel.DestroyWindow();
	if (tblNameEdit.IsWindow()) tblNameEdit.DestroyWindow();
	if (databaseLabel.IsWindow()) databaseLabel.DestroyWindow();
	if (databaseComboBox.IsWindow()) databaseComboBox.DestroyWindow();
	if (schemaLabel.IsWindow()) schemaLabel.DestroyWindow();
	if (schemaComboBox.IsWindow()) schemaComboBox.DestroyWindow();
	if (tableTabView.IsWindow()) tableTabView.DestroyWindow();
	if (saveButton.IsWindow()) saveButton.DestroyWindow();
	if (revertButton.IsWindow()) revertButton.DestroyWindow();

	return ret;
}

void NewTablePage::paintItem(CDC & dc, CRect & paintRect)
{
	dc.FillRect(paintRect, bkgBrush);
}

HBRUSH NewTablePage::OnCtlColorStatic(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, bkgColor);
	::SelectObject(hdc, textFont);
	return bkgBrush;
}

HBRUSH NewTablePage::OnCtlColorEdit(HDC hdc, HWND hwnd)
{	
	::SetBkColor(hdc, bkgColor);	
	::SelectObject(hdc, textFont);
	return bkgBrush;
}

HBRUSH NewTablePage::OnCtlColorListBox(HDC hdc, HWND hwnd)
{
	::SetTextColor(hdc, RGB(0, 0, 0)); // Text area foreground color
	::SetBkColor(hdc, RGB(153, 153, 153)); // Text area background color
	::SelectObject(hdc, comboFont);
	return AtlGetStockBrush(WHITE_BRUSH);
}
