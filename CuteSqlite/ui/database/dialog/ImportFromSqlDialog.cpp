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

 * @file   ImportFromSqlDialog.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-10-08
 *********************************************************************/
#include "stdafx.h"
#include "ImportFromSqlDialog.h"
#include "common/AppContext.h"
#include "utils/FontUtil.h"
#include "ui/common/message/QPopAnimate.h"
#include "ui/common/message/QMessageBox.h"

ImportFromSqlDialog::ImportFromSqlDialog(HWND parentHwnd, ImportDatabaseAdapter * adapter)
{
	ATLASSERT(parentHwnd != nullptr && adapter != nullptr);
	this->parentHwnd = parentHwnd;
	this->adapter = adapter;
	this->caption = SNT(L"database-import-from-sql");
	setFormSize(IMPORT_FROM_SQL_DIALOG_WIDTH, IMPORT_FROM_SQL_DIALOG_HEIGHT);
}


void ImportFromSqlDialog::createOrShowUI()
{
	CRect clientRect;
	GetClientRect(clientRect);

	createOrShowImportImage(importImage, clientRect);
	createOrShowImportTextLabel(importTextLabel, clientRect);
	createOrShowSelectDbElems(clientRect);
	createOrShowImportPathElems(clientRect);
	createOrShowAbortOnErrorElems(clientRect);
	createOrShowProcessBar(processBar, clientRect);

	QDialog::createOrShowUI();
	yesButton.SetWindowText(S(L"import").c_str());
}


void ImportFromSqlDialog::createOrShowImportImage(QStaticImage &win, CRect & clientRect)
{
	int x = 20, y = 15, w = 32, h = 32;
	CRect rect(x, y, x + w, y + h);
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_VISIBLE |WS_CLIPCHILDREN  | WS_CLIPSIBLINGS | SS_NOTIFY; // SS_NOTIFY - 表示static接受点击事件
		std::wstring imgDir = ResourceUtil::getProductImagesDir();
		std::wstring imgPath = imgDir + L"database\\dialog\\import-database.png";
		win.load(imgPath.c_str(), BI_PNG, true);
		win.Create(m_hWnd, rect, L"", dwStyle , 0);
		return;
	} else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
	}
}


void ImportFromSqlDialog::createOrShowImportTextLabel(CStatic &win, CRect & clientRect)
{
	std::wstring text = S(L"import-from-sql-text-description");
	std::wstring fmt = Lang::fontName();
	int fsize = Lang::fontSize(L"form-text-size");
	CSize size = FontUtil::measureTextSize(text.c_str(), fsize, false,  fmt.c_str());
	int x = 20 + 32 + 20, y = 25, w = clientRect.Width() - x - 20, h = size.cy + 2;
	CRect rect(x, y, x + w, y + h);
	createOrShowFormLabel(win, text, rect, clientRect, SS_LEFT, elemFont);
}


void ImportFromSqlDialog::createOrShowSelectDbElems(CRect & clientRect)
{
	int x = 20, y = 15 + 32 + 10 + 6, w = 200, h = 20;
	CRect rect(x, y, x + w, y + h);
	createOrShowFormLabel(selectDbLabel, S(L"select-database").append(L":"), rect, clientRect, SS_LEFT, elemFont);

	rect.OffsetRect(0, h + 5);
	createOrShowFormComboBox(selectDbComboBox, Config::IMPORT_TARGET_DB_COMBOBOX_ID,  L"", rect, clientRect);
}


void ImportFromSqlDialog::createOrShowImportPathElems(CRect & clientRect)
{
	int x = 20 , y = 15 + 32 + 10 + 6 + (20 + 5) * 2, w = 380, h = 20;
	CRect rect(x, y, x + w, y + h);
	createOrShowFormLabel(importPathLabel, S(L"open-the-file").append(L":"), rect, clientRect, SS_LEFT, elemFont);

	rect.OffsetRect(0, h + 5);
	createOrShowFormEdit(importPathEdit, Config::IMPORT_SOURCE_PATH_EDIT_ID, L"", L"", rect, clientRect, ES_LEFT | ES_AUTOHSCROLL, false);
	importPathEdit.SetLimitText(2048);

	rect.OffsetRect(w + 10, 0);	
	rect = { rect.left, rect.top, rect.left + 50, rect.bottom };
	createOrShowFormButton(openFileButton, Config::IMPORT_OPEN_FILE_BUTTON_ID, L"...", rect, clientRect);
}


void ImportFromSqlDialog::createOrShowAbortOnErrorElems(CRect & clientRect)
{
	int x = 20 , y = clientRect.bottom - QDIALOG_BUTTON_HEIGHT - 10, w = 250, h = 20;
	CRect rect(x, y, x + w, y + h);
	createOrShowFormCheckBox(abortOnErrorCheckbox, Config::IMPORT_ABORT_ON_ERROR_CHECKBOX_ID, S(L"abort-on-error-text"), rect, clientRect);
}

void ImportFromSqlDialog::createOrShowProcessBar(QProcessBar &win, CRect & clientRect)
{
	CRect formRect = getFormRect(clientRect);
	int x = formRect.left + 20, y = formRect.bottom - 80, w = formRect.Width() - 40, h = 20;
	CRect rect(x, y, x + w, y + h);

	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN  | WS_CLIPSIBLINGS ;
		UINT nID = 0;
		win.Create(m_hWnd, rect, L"", dwStyle , 0,  nID);
		return ;
	} else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
	}
}

void ImportFromSqlDialog::loadWindow()
{
	if (!isNeedReload) {
		return ;
	}

	isNeedReload = false;
	loadSelectDbComboBox();
	loadImportPathEdit();
	loadAbortOnErrorCheckBox();
}


void ImportFromSqlDialog::loadSelectDbComboBox()
{
	// db list
	UserDbList dbs = adapter->getDbs();
	selectDbComboBox.ResetContent();
	int n = static_cast<int>(dbs.size());
	int nSelItem = -1;
	for (int i = 0; i< n; i++) {
		auto item = dbs.at(i);
		int nItem = selectDbComboBox.AddString(item.name.c_str());
		selectDbComboBox.SetItemData(nItem, item.id);
		if (item.isActive) {
			nSelItem = i;
			settingService->setSysInit(L"import-selected-db-id", std::to_wstring(item.id));
		}
	}
	selectDbComboBox.SetCurSel(nSelItem);
}


void ImportFromSqlDialog::loadImportPathEdit()
{
	if (!importPathEdit.IsWindow()) {
		return ;
	}

	CString str;
	importPathEdit.GetWindowText(str);
	std::wstring importPath = str.GetString();
	if (str.IsEmpty()) {
		importPath = settingService->getSysInit(L"import-from-sql-path");
	}

	importPathEdit.SetWindowText(importPath.c_str());
}


void ImportFromSqlDialog::loadAbortOnErrorCheckBox()
{
	abortOnErrorCheckbox.SetCheck(1);
}

bool ImportFromSqlDialog::getSelUserDbId(uint64_t & userDbId)
{
	int nSelItem = selectDbComboBox.GetCurSel();
	if (nSelItem == -1) {
		userDbId = -1;
		return false;
	}
	userDbId = selectDbComboBox.GetItemData(nSelItem);
	return true;
}


bool ImportFromSqlDialog::getImportPath(std::wstring & importPath)
{
	CString str;
	importPathEdit.GetWindowText(str);
	if (str.IsEmpty()) {
		QPopAnimate::error(m_hWnd, S(L"import-path-empty-error"));
		importPathEdit.SetFocus();
		return false;
	}

	// the file not exists 	
	if (_waccess(str.GetString(), 0) != 0) {
		QPopAnimate::error(m_hWnd, S(L"import-file-not-exists"));
		importPathEdit.SetFocus();
		return false;
	}
	importPath = str.GetString();

	return true;
}


void ImportFromSqlDialog::saveImportPath(std::wstring & importPath)
{
	settingService->setSysInit(L"import-from-sql-path", importPath);
}

LRESULT ImportFromSqlDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	linePen.CreatePen(PS_SOLID, 1, lineColor);
	elemFont = FT(L"elem-size");

	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_IMPORT_PROCESS_ID);

	QDialog::OnInitDialog(uMsg, wParam, lParam, bHandled); 
	return 0;
}

LRESULT ImportFromSqlDialog::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	QDialog::OnDestroy(uMsg, wParam, lParam, bHandled);

	AppContext::getInstance()->unsuscribe(m_hWnd, Config::MSG_IMPORT_PROCESS_ID);

	if (!linePen.IsNull()) linePen.DeleteObject();
	if (elemFont) ::DeleteObject(elemFont);

	if (importImage.IsWindow()) importImage.DestroyWindow();
	if (importTextLabel.IsWindow()) importTextLabel.DestroyWindow();

	if (selectDbLabel.IsWindow()) selectDbLabel.DestroyWindow();
	if (selectDbComboBox.IsWindow()) selectDbComboBox.DestroyWindow();

	if (processBar.IsWindow()) processBar.DestroyWindow();

	if (importPathLabel.IsWindow()) importPathLabel.DestroyWindow();
	if (importPathEdit.IsWindow()) importPathEdit.DestroyWindow();
	if (openFileButton.IsWindow()) openFileButton.DestroyWindow();

	if (abortOnErrorCheckbox.IsWindow()) abortOnErrorCheckbox.DestroyWindow();

	return 0;
}

LRESULT ImportFromSqlDialog::OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// invisible then return 
	if (!wParam) { 
		return 0;
	}
	loadWindow();
	return 0;
}

void ImportFromSqlDialog::paintItem(CDC &dc, CRect &paintRect)
{
	int x = 20, y = 15 + 32 + 10, w = paintRect.Width() - 2 * 20, h = 1;

	HPEN oldPen = dc.SelectPen(linePen);
	dc.MoveTo(x, y);
	dc.LineTo(x + w, y);
	dc.SelectPen(oldPen);
}


LRESULT ImportFromSqlDialog::OnChangeSelectDbComboBox(UINT uNotifyCode, int nID, HWND hwnd)
{
	int nSelItem = selectDbComboBox.GetCurSel();
	uint64_t userDbId = static_cast<uint64_t>(selectDbComboBox.GetItemData(nSelItem));
	std::wstring exportSelectedDbId = settingService->getSysInit(L"export-selected-db-id");
	if (userDbId == std::stoull(exportSelectedDbId)) {
		return 0;
	}
	settingService->setSysInit(L"export-selected-db-id", std::to_wstring(userDbId));
	
	return 0;
}

void ImportFromSqlDialog::OnClickOpenFileButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	CString str;
	importPathEdit.GetWindowText(str);
	std::wstring exportPath = str;
	std::wstring fileName, fileDir;
	if (!str.IsEmpty()) {
		fileName = FileUtil::getFileName(exportPath);
		fileDir = FileUtil::getFileDir(exportPath);
	}
	wchar_t * szFilter = nullptr;
	wchar_t * defExt = nullptr;
	
	szFilter = L"SQL File(*.sql)\0*.sql\0All Files(*.*)\0*.*\0\0";
	defExt = L"sql";


	CFileDialog fileDlg(TRUE, defExt, fileName.c_str(), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilter, m_hWnd);
	//fileDlg.m_ofn.lpstrTitle = S(L"save-to-file").c_str();
	fileDlg.m_ofn.lpstrInitialDir = fileDir.c_str();
	//fileDlg.m_ofn.lpstrFilter = szFilter;
	CString path;
	if (IDOK == fileDlg.DoModal()) {
		exportPath = fileDlg.m_szFileName;
		importPathEdit.SetWindowText(exportPath.c_str());
	}
}

void ImportFromSqlDialog::OnClickAbortOnErrorCheckBox(UINT uNotifyCode, int nID, HWND hwnd)
{
	abortOnErrorCheckbox.SetCheck(1);
}

void ImportFromSqlDialog::OnClickYesButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	std::wstring importPath;
	if (!getImportPath(importPath)) {
		return ;
	}
	saveImportPath(importPath);

	// select user db id
	uint64_t userDbId;
	if (!getSelUserDbId(userDbId) || userDbId == -1) {
		return ;
	}

	isRunning = true;
	yesButton.EnableWindow(false);

	// Export objects(tables/views/triggers) to sql file
	if (adapter->importFromSql(userDbId, importPath)) {
		QPopAnimate::success(S(L"import-success-text"));
		yesButton.EnableWindow(true);
		noButton.SetWindowText(S(L"complete").c_str());
		return ;
	}

	yesButton.EnableWindow(true);
}

/**
 * Handle the message for export process.
 * 
 * @param uMsg - Config::MSG_IMPORT_DB_FROM_SQL_PROCESS_ID
 * @param wParam - 0 : Import in progress, 1:Import is complete 2:Import has error(s)
 * @param lParam - Percent of export progress
 * @param bHandled - not use
 * @return 0
 */
LRESULT ImportFromSqlDialog::OnProcessImport(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	Q_INFO(L"recieve MSG_IMPORT_DB_FROM_SQL_PROCESS_ID, isCompete:{},percent:{}", wParam, lParam);

	if (wParam == 1) { // 1 表示执行完成
		AppContext * appContext = AppContext::getInstance();
		processBar.run(100);
		isRunning = false;
	} else if (wParam == 0) { 
		// 进度条
		int percent = static_cast<int>(lParam);
		processBar.run(percent);
	} else if (wParam == 2) {
		QPopAnimate::error(m_hWnd, S(L"import-from-sql-error-text"));
		isRunning = false;
	} else if (wParam == 3) {
		QPopAnimate::error(m_hWnd, S(L"file-not-found"));
		isRunning = false;
	}
	return 0;
}
