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
#include "TableStructurePage.h"
#include <sstream>
#include <Scintilla/Scintilla.h>
#include <Scintilla/SciLexer.h>
#include "common/AppContext.h"
#include "core/common/Lang.h"
#include "ui/common/QWinCreater.h"
#include "ui/database/rightview/page/table/TableColumnsPage.h"
#include "ui/database/rightview/page/table/TableIndexesPage.h"
#include "ui/database/rightview/page/table/adapter/TableColumnsPageAdapter.h"
#include "ui/database/rightview/page/table/adapter/TableIndexesPageAdapter.h"

/**
 * Call the PreTranslateMessage(pMsg) function of sub element(s) class .
 * 
 * @param pMsg
 * @return 
 */
BOOL TableStructurePage::PreTranslateMessage(MSG* pMsg)
{
	BOOL result = FALSE;
	if (sqlPreviewEdit.IsWindow() && sqlPreviewEdit.PreTranslateMessage(pMsg)) {
		return TRUE;
	}

	if (tableTabView.IsWindow() && tableTabView.PreTranslateMessage(pMsg)) {
		return TRUE;
	}
	return FALSE;
}


CRect TableStructurePage::getEditorRect(CRect & clientRect)
{
	int x = 20, y = clientRect.bottom - 70 - 200, w = clientRect.Width() - 40, h = 200;
	return { x, y, x + w, y + h };
}

void TableStructurePage::createOrShowUI()
{
	QPage::createOrShowUI();

	CRect clientRect;
	GetClientRect(clientRect);

	// Create sub elements
	createOrShowTblNameElems(clientRect);
	createOrShowDatabaseElems(clientRect);
	createOrShowSchemaElems(clientRect);
	createOrShowTableTabView(tableTabView, clientRect);
	createOrShowSqlPreviewElems(clientRect);
	createOrShowButtons(clientRect);
}


void TableStructurePage::createOrShowTblNameElems(CRect & clientRect)
{
	int x = 20, y = 20, w = 80, h = 20;
	CRect rect(x, y, x + w, y + h);
	QWinCreater::createOrShowLabel(m_hWnd, tblNameLabel, S(L"tbl-name"), rect, clientRect);

	rect.OffsetRect(w + 5, 0);
	rect.right += 120;
	QWinCreater::createOrShowEdit(m_hWnd, tblNameEdit, Config::TABLE_TBL_NAME_EDIT_ID, L"", rect, clientRect, WS_CLIPCHILDREN | WS_CLIPSIBLINGS, false);
}


void TableStructurePage::createOrShowDatabaseElems(CRect & clientRect)
{
	int x = 20, y = 20 + 20 + 15, w = 80, h = 20;
	CRect rect(x, y, x + w, y + h);
	QWinCreater::createOrShowLabel(m_hWnd, databaseLabel, S(L"database"), rect, clientRect);

	rect.OffsetRect(w + 5, -5);
	rect.right += 120;
	QWinCreater::createOrShowComboBox(m_hWnd, databaseComboBox, Config::TABLE_DATABASE_COMBOBOX_ID, rect, clientRect);
}


void TableStructurePage::createOrShowSchemaElems(CRect & clientRect)
{
	int x = 20 + 80 + 5 + 200 + 20, y = 20 + 20 + 15, w = 80, h = 20;
	CRect rect(x, y, x + w, y + h);
	QWinCreater::createOrShowLabel(m_hWnd, schemaLabel, S(L"schema"), rect, clientRect);

	rect.OffsetRect(w + 5, -5);
	rect.right += 120;
	QWinCreater::createOrShowComboBox(m_hWnd, schemaComboBox, Config::TABLE_SCHEMA_COMBOBOX_ID, rect, clientRect);
}


void TableStructurePage::createOrShowTableTabView(TableTabView & win, CRect & clientRect)
{
	CRect editorRect = getEditorRect(clientRect);
	int x = 20, y = 20 + 20 + 15 + 20 + 20, w = clientRect.Width() - 40, h = clientRect.Height() - y - 70 - editorRect.Height() - 5;
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


void TableStructurePage::createOrShowSqlPreviewElems(CRect & clientRect)
{
	CRect rect = getEditorRect(clientRect);
	rect.DeflateRect(2, 2, 2, 2);
	createOrShowSqlEditor(sqlPreviewEdit, Config::TABLE_SQL_PREVIEW_EDIT_ID, rect, clientRect);
}

void TableStructurePage::createOrShowButtons(CRect & clientRect)
{
	int x = clientRect.right - (20 + 120) * 2, y = clientRect.bottom - 50, w = 120, h = 30;
	CRect rect(x, y, x + w, y + h);
	QWinCreater::createOrShowButton(m_hWnd, saveButton, Config::TABLE_SAVE_BUTTON_ID, S(L"save"), rect, clientRect);
	
	rect.OffsetRect(w + 20, 0);
	QWinCreater::createOrShowButton(m_hWnd, revertButton, Config::TABLE_REVERT_BUTTON_ID, S(L"revert"), rect, clientRect);
}

void TableStructurePage::loadWindow()
{
	if (!isNeedReload) {
		return;
	}

	isNeedReload = false;
	loadDatabaseComboBox();
	loadSchemaComboBox();
}


void TableStructurePage::loadDatabaseComboBox()
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


void TableStructurePage::loadSchemaComboBox()
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

int TableStructurePage::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_TABLE_PREVIEW_SQL_ID);
	bool ret = QPage::OnCreate(lpCreateStruct);
	textFont = FT(L"form-text-size");
	comboFont = FTB(L"combobox-size", true);
	btnFont = GDI_PLUS_FT(L"setting-button-size");
	editorBkgBrush = ::CreateSolidBrush(editorBkgColor); 

	saveButton.SetFontColors(RGB(0,0,0), RGB(0x12,0x96,0xdb), RGB(153,153,153));
	saveButton.SetBkgColors(RGB(238, 238, 238), RGB(238, 238, 238), RGB(238, 238, 238));

	revertButton.SetFontColors(RGB(0,0,0), RGB(0x12,0x96,0xdb), RGB(153,153,153));
	revertButton.SetBkgColors(RGB(238, 238, 238), RGB(238, 238, 238), RGB(238, 238, 238));

	return ret;
}

int TableStructurePage::OnDestroy()
{
	bool ret = QPage::OnDestroy();
	if (textFont) ::DeleteObject(textFont);
	if (comboFont) ::DeleteObject(comboFont);
	if (btnFont) ::DeleteObject(btnFont);
	if (editorBkgBrush) ::DeleteObject(editorBkgBrush);

	if (tblNameLabel.IsWindow()) tblNameLabel.DestroyWindow();
	if (tblNameEdit.IsWindow()) tblNameEdit.DestroyWindow();
	if (databaseLabel.IsWindow()) databaseLabel.DestroyWindow();
	if (databaseComboBox.IsWindow()) databaseComboBox.DestroyWindow();
	if (schemaLabel.IsWindow()) schemaLabel.DestroyWindow();
	if (schemaComboBox.IsWindow()) schemaComboBox.DestroyWindow();
	if (tableTabView.IsWindow()) tableTabView.DestroyWindow();
	if (saveButton.IsWindow()) saveButton.DestroyWindow();
	if (revertButton.IsWindow()) revertButton.DestroyWindow();
	if (sqlPreviewEdit.IsWindow()) sqlPreviewEdit.DestroyWindow();

	AppContext::getInstance()->unsuscribe(m_hWnd, Config::MSG_TABLE_PREVIEW_SQL_ID);
	return ret;
}


void TableStructurePage::paintItem(CDC & dc, CRect & paintRect)
{
	dc.FillRect(paintRect, bkgBrush);

	CRect editorRect = getEditorRect(paintRect);
	dc.FillRect(editorRect, editorBkgBrush);
}


void TableStructurePage::OnChangeTblNameEdit(UINT uNotifyCode, int nID, HWND hwnd)
{
	if (!sqlPreviewEdit.IsWindow()) {
		return;
	}
	previewRuntimeSql();
}


LRESULT TableStructurePage::OnPreviewSql(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	previewRuntimeSql();
	return 0;
}

HBRUSH TableStructurePage::OnCtlColorStatic(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, bkgColor);
	::SelectObject(hdc, textFont);
	return bkgBrush;
}

HBRUSH TableStructurePage::OnCtlColorEdit(HDC hdc, HWND hwnd)
{	
	::SetBkColor(hdc, bkgColor);	
	::SelectObject(hdc, textFont);
	return bkgBrush;
}

HBRUSH TableStructurePage::OnCtlColorListBox(HDC hdc, HWND hwnd)
{
	::SetTextColor(hdc, RGB(0, 0, 0)); // Text area foreground color
	::SetBkColor(hdc, RGB(153, 153, 153)); // Text area background color
	::SelectObject(hdc, comboFont);
	return AtlGetStockBrush(WHITE_BRUSH); 
}

void TableStructurePage::createOrShowSqlEditor(QHelpEdit & win, UINT id, CRect & rect, CRect & clientRect, DWORD exStyle)
{
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		win.setup(S(L"sql-preview"), std::wstring(L""));
		win.Create(m_hWnd, rect, L"", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, Config::TABLE_SQL_PREVIEW_EDIT_ID);		
		return;
	} else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
	}
}

void TableStructurePage::previewRuntimeSql()
{
	CString str;
	tblNameEdit.GetWindowText(str);
	std::wstring tblName(str.GetString());
	schemaComboBox.GetWindowText(str);
	std::wstring schema(str.GetString());

	// Generate "CREATE TABLE schema.table_name"
	std::wostringstream runtimeSql;
	runtimeSql << L"CREATE TABLE \"";
	if (schema.empty() || schema == L"main") {
		runtimeSql << tblName;
	} else {
		runtimeSql << schema << L"\".\"" << tblName;
	}
	runtimeSql << "\" (" << std::endl;

	std::wstring columnsSqlClause = generateColumnsClause();
	if (!columnsSqlClause.empty()) {
		runtimeSql << columnsSqlClause;
	}

	std::wstring indexesSqlClause = generateIndexesClause();
	if (!indexesSqlClause.empty()) {
		runtimeSql << L',' << std::endl << indexesSqlClause;
	}
	runtimeSql << std::endl << L"); " << std::endl;

	sqlPreviewEdit.setText(runtimeSql.str());
}

/**
 * Generate the columns clause for create table,
 * 
 * @return 
 */
std::wstring TableStructurePage::generateColumnsClause()
{
	ATLASSERT(tableTabView.IsWindow());
	return tableTabView.getTableColumnsPage().getAdapter()->genderateColumnsSqlClause();
}

std::wstring TableStructurePage::generateIndexesClause()
{
	ATLASSERT(tableTabView.IsWindow());
	bool hasAutoIncrement = tableTabView.getTableColumnsPage().getAdapter()->verifyExistsAutoIncrement();
	return tableTabView.getTableIndexesPage().getAdapter()->genderateIndexesSqlClause(hasAutoIncrement);
}
