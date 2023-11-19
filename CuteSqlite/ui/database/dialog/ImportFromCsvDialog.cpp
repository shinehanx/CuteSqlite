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

 * @file   ImportFromCsvDialog.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-10-08
 *********************************************************************/
#include "stdafx.h"
#include "ImportFromCsvDialog.h"
#include "common/AppContext.h"
#include "utils/FontUtil.h"
#include "ui/common/message/QPopAnimate.h"
#include "ui/common/message/QMessageBox.h"
#include "ui/common/QWinCreater.h"
#include "ui/database/leftview/dialog/PreviewSqlDialog.h"

ImportFromCsvDialog::ImportFromCsvDialog(HWND parentHwnd) 
{
	ATLASSERT(parentHwnd != nullptr);
	this->parentHwnd = parentHwnd;
	this->caption = S(L"database-import-from-sql");
	setFormSize(IMPORT_FROM_CSV_DIALOG_WIDTH, IMPORT_FROM_CSV_DIALOG_HEIGHT);
}


void ImportFromCsvDialog::createOrShowUI()
{	
	CRect clientRect;
	GetClientRect(clientRect);

	createOrShowImportImage(importImage, clientRect);
	createOrShowImportTextLabel(importTextLabel, clientRect);
	createOrShowImportPathElems(clientRect);
	createOrShowTargetElems(clientRect);
	createOrShowGroupBoxes(clientRect);	
	createOrShowListViews(clientRect);
	createOrShowCsvSettingsElems(clientRect);
	createOrShowProcessBar(processBar, clientRect);
	
	QDialog::createOrShowUI();
	yesButton.SetWindowText(S(L"import").c_str());
	createOrShowPreviewSqlButton(clientRect);
}


void ImportFromCsvDialog::createOrShowImportImage(QStaticImage &win, CRect & clientRect)
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


void ImportFromCsvDialog::createOrShowImportTextLabel(CStatic &win, CRect & clientRect)
{
	std::wstring text = S(L"import-from-csv-text-description");
	std::wstring fmt = Lang::fontName();
	int fsize = Lang::fontSize(L"form-text-size");
	CSize size = FontUtil::measureTextSize(text.c_str(), fsize, false,  fmt.c_str());
	int x = 20 + 32 + 20, y = 25, w = clientRect.Width() - x - 20, h = size.cy + 2;
	CRect rect(x, y, x + w, y + h);
	createOrShowFormLabel(win, text, rect, clientRect, SS_LEFT, elemFont);
}

void ImportFromCsvDialog::createOrShowImportPathElems(CRect & clientRect)
{
	int x = 25 , y = 15 + 32 + 10 + 10, w = 340, h = 20;
	CRect rect(x, y, x + w, y + h);
	createOrShowFormLabel(importPathLabel, S(L"source-csv-file").append(L":"), rect, clientRect, SS_LEFT, elemFont);

	rect.OffsetRect(0, h + 5);
	createOrShowFormEdit(importPathEdit, Config::IMPORT_SOURCE_PATH_EDIT_ID, L"", L"", rect, clientRect, ES_LEFT, false);
	importPathEdit.SetLimitText(1024);

	rect.OffsetRect(w + 5, 0);	
	rect = { rect.left, rect.top, rect.left + 30, rect.bottom };
	createOrShowFormButton(openFileButton, Config::IMPORT_OPEN_FILE_BUTTON_ID, L"...", rect, clientRect);
}

void ImportFromCsvDialog::createOrShowTargetElems(CRect & clientRect)
{
	int x = 430, y = 15 + 32 + 10 + 10, w = 170, h = 20;
	CRect rect1(x, y, x + w, y + h);
	createOrShowFormLabel(targetDbLabel, S(L"target-database").append(L":"), rect1, clientRect, SS_LEFT, elemFont);

	CRect rect2 = rect1;
	rect2.OffsetRect(0, h + 5);
	createOrShowFormComboBox(targetDbComboBox, Config::IMPORT_TARGET_DB_COMBOBOX_ID,  L"", rect2, clientRect);

	rect1.OffsetRect(w + 10, 0);
	createOrShowFormLabel(targetTblLabel, S(L"target-table").append(L":"), rect1, clientRect, SS_LEFT, elemFont);

	rect2.OffsetRect(w + 10, 0);
	createOrShowFormComboBox(targetTblComboBox, Config::IMPORT_TARGET_TBL_COMBOBOX_ID,  L"", rect2, clientRect);
}


void ImportFromCsvDialog::createOrShowGroupBoxes(CRect & clientRect)
{
	int x = 20 , y = 15 + 32 + 10 , w = clientRect.Width() - 40, h = 70;
	CRect rect = { x, y, x + w, y + h };
	createOrShowFormGroupBox(topGroupBox, 0, L"", rect, clientRect);

	x = 20, y = rect.bottom + 10, w = 420, h = 300;
	CRect rect1 = { x, y, x + w, y + h };
	createOrShowFormGroupBox(columnsSettingsGroupBox, 0, S(L"columns-settings"), rect1, clientRect);
	

	CRect rect2 = rect1;
	rect2.OffsetRect(w + 20, 0);
	rect2.right = clientRect.right - 20;
	createOrShowFormGroupBox(csvSettingsGroupBox, 0, S(L"csv-settings"), rect2, clientRect);

	x = 20, y = rect2.bottom + 10, w = clientRect.right - 50, h = 150;
	CRect rect3 = { x, y, x + w, y + h };
	createOrShowFormGroupBox(csvDatasGroupBox, 0, S(L"csv-data"), rect3, clientRect);
}

void ImportFromCsvDialog::createOrShowListViews(CRect & clientRect)
{
	CRect rect1 = GdiPlusUtil::GetWindowRelativeRect(columnsSettingsGroupBox);
	rect1.DeflateRect(10, 20, 10, 10);
	createOrShowListView(columnListView, Config::IMPORT_COLUMN_LISTVIEW_ID, rect1, clientRect);

	CRect rect2 = GdiPlusUtil::GetWindowRelativeRect(csvDatasGroupBox);
	rect2.DeflateRect(10, 20, 10, 10);
	createOrShowListView(dataListView, Config::IMPORT_DATA_LISTVIEW_ID, rect2, clientRect);
}

void ImportFromCsvDialog::createOrShowListView(QListViewCtrl & win, UINT id, CRect & rect, CRect & clientRect)
{
	if (IsWindow() && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_BORDER | LVS_ALIGNLEFT | LVS_REPORT  | LVS_OWNERDATA | LVS_OWNERDRAWFIXED;
		win.enabledCheckBox(false);
		win.Create(m_hWnd, rect,NULL,dwStyle , // | LVS_OWNERDATA
			0, id);
		win.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
		win.setItemHeight(22);
	} else if (IsWindow() && win.IsWindow() && clientRect.Width() > 1) {
		win.MoveWindow(rect);
		win.ShowWindow(true);
	}
}


void ImportFromCsvDialog::createOrShowCsvSettingsElems(CRect & clientRect)
{
	CRect rcGroupBox = GdiPlusUtil::GetWindowRelativeRect(csvSettingsGroupBox);
	rcGroupBox.DeflateRect(10, 30, 10, 10);


	// cvs options - Add column names on top
	int x = rcGroupBox.left, y = rcGroupBox.top, w = rcGroupBox.right - 20, h = 20;
	CRect rect(x, y, x + w, y + h);	
	createOrShowFormCheckBox(csvColumnNameCheckBox, Config::CSV_COLUMN_NAME_CHECKBOX_ID, S(L"column-name-on-top"), rect, clientRect);

	rect.OffsetRect(0, h + 10);
	rect.right = rect.left + 200;
	createOrShowFormLabel(csvFieldTerminatedByLabel, S(L"field-terminated-by").append(L":"), rect, clientRect, SS_LEFT, elemFont);

	int x2 = rcGroupBox.right - 60, y2 = rect.top,  w2 = 60, h2 = h;
	CRect rect2(x2, y2, x2 + w2, y2 + h2); 
	bool isReadOnly = false;
	createOrShowFormComboBox(csvFieldTerminatedByComboBox,Config::CSV_FIELD_TERMINAATED_BY_COMBOBOX_ID, L"", rect2, clientRect);

	rect.OffsetRect(0, h + 10);
	createOrShowFormLabel(csvFieldEnclosedByLabel, S(L"field-enclosed-by").append(L":"), rect, clientRect, SS_LEFT, elemFont);
	rect2.OffsetRect(0, h + 10);
	createOrShowFormEdit(csvFieldEnclosedByEdit,Config::CSV_FIELD_ENCLOSED_BY_EDIT_ID, L"", L"", rect2, clientRect, ES_LEFT, isReadOnly);

	rect.OffsetRect(0, h + 10);
	createOrShowFormLabel(csvFieldEscapedByLabel, S(L"field-escaped-by").append(L":"), rect, clientRect, SS_LEFT, elemFont);
	rect2.OffsetRect(0, h + 10);
	createOrShowFormEdit(csvFieldEscapedByEdit, Config::CSV_FIELD_ESCAPED_BY_EDIT_ID, L"", L"", rect2, clientRect, ES_LEFT, isReadOnly);

	// csv options - Lines
	rect.OffsetRect(0, h + 10);
	createOrShowFormLabel(csvLineTerminatedByLabel, S(L"line-terminated-by").append(L":"), rect, clientRect, SS_LEFT, elemFont);
	rect2.OffsetRect(0, h + 10);
	createOrShowFormComboBox(csvLineTerminatedByComboBox, Config::CSV_LINE_TERMINAATED_BY_COMBOBOX_ID, L"", rect2, clientRect);

	rect.OffsetRect(0, h + 10);
	createOrShowFormLabel(csvNullAsKeywordLabel, S(L"csv-null-as-keyword").append(L":"), rect, clientRect, SS_LEFT, elemFont);
	rect2.OffsetRect(0, h + 10);
	createOrShowFormComboBox(csvNullAsKeywordComboBox, Config::CSV_NULL_AS_KEYWORD_COMBOBOX_ID, L"", rect2, clientRect);

	rect.OffsetRect(0, h + 10);
	createOrShowFormLabel(csvCharsetLabel, S(L"encoding").append(L":"), rect, clientRect, SS_LEFT, elemFont); 
	rect2.OffsetRect(0, h + 10);
	createOrShowFormComboBox(csvCharsetComboBox, Config::CSV_CHARSET_COMBOBOX_ID, L"", rect2, clientRect); 
}

void ImportFromCsvDialog::createOrShowProcessBar(QProcessBar &win, CRect & clientRect)
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


void ImportFromCsvDialog::createOrShowPreviewSqlButton(CRect & clientRect)
{
	CRect rcBtn = GdiPlusUtil::GetWindowRelativeRect(yesButton.m_hWnd);
	int x = 20, y = rcBtn.top, w = rcBtn.Width(), h = rcBtn.Height();
	CRect rect(x, y, x + w, y + h);
	createOrShowFormButton(previewSqlButton, Config::PREVIEW_SQL_BUTTON_ID, S(L"preview-sql"), rect, clientRect);
}

void ImportFromCsvDialog::loadWindow()
{
	if (!isNeedReload) {
		return ;
	}

	isNeedReload = false;
	
	// [notice] Must load settings first, and then load others
	loadCsvSettingsElems();

	loadImportPathEdit();	
	loadTargetDbComboBox();
	loadTargetTblComboBox();
	loadColumnsListView();
}


void ImportFromCsvDialog::loadTargetDbComboBox()
{
	// db list
	UserDbList dbs = adapter->getDbs();
	targetDbComboBox.ResetContent();
	int n = static_cast<int>(dbs.size());
	int nSelItem = -1;
	for (int i = 0; i< n; i++) {
		auto item = dbs.at(i);
		int nItem = targetDbComboBox.AddString(item.name.c_str());
		targetDbComboBox.SetItemData(nItem, item.id);
		if (item.isActive) {
			nSelItem = i;
		}
	}
	targetDbComboBox.SetCurSel(nSelItem);
}

void ImportFromCsvDialog::loadTargetTblComboBox()
{
	UserTableStrings userTables = tableService->getUserTableStrings(supplier->getRuntimeUserDbId());
	targetTblComboBox.ResetContent();
	int n = static_cast<int>(userTables.size());
	int nSelItem = 0;
	for (int i = 0; i< n; i++) {
		auto item = userTables.at(i);
		int nItem = targetTblComboBox.AddString(item.c_str());
		if (item == supplier->getRuntimeTblName()) {
			nSelItem = i;
		}
	}
	targetTblComboBox.SetCurSel(nSelItem);

	wchar_t str[256] = { 0 };
	targetTblComboBox.GetLBText(nSelItem, str);
	supplier->setRuntimeTblName(str);
	columnRowCount = adapter->loadCsvFileToColumnListView(supplier->importPath, columnListView);
	columnRowCount = adapter->loadCsvFileToDataListView(supplier->importPath, dataListView);
}


void ImportFromCsvDialog::loadColumnsListView()
{
	adapter->loadHeaderForColumnListView(columnListView);
}


void ImportFromCsvDialog::loadCsvSettingsElems()
{
	if (csvColumnNameCheckBox.IsWindow()) {
		std::wstring val = settingService->getSysInit(L"csv-column-name");
		val = val.empty() ? L"1" : val;
		if (val == L"1") {
			csvColumnNameCheckBox.SetCheck(1);
		} else {
			csvColumnNameCheckBox.SetCheck(0);
		}
		supplier->csvColumnNameOnTop = csvColumnNameCheckBox.GetCheck();
	}

	if (csvFieldTerminatedByComboBox.IsWindow()) {		
		std::wstring val = settingService->getSysInit(L"csv-field-terminated-by");
		val = val.empty() ? QSupplier::csvFieldSeperators[0] : val;
		QWinCreater::loadComboBox(csvFieldTerminatedByComboBox, QSupplier::csvFieldSeperators, 5, val);
		supplier->csvFieldTerminateBy = val;
	}

	if (csvFieldEnclosedByEdit.IsWindow()) {
		std::wstring val = settingService->getSysInit(L"csv-field-enclosed-by");
		csvFieldEnclosedByEdit.SetWindowText(val.c_str());  
		supplier->csvFieldEnclosedBy = val;
	}

	if (csvFieldEscapedByEdit.IsWindow()) {
		std::wstring val = settingService->getSysInit(L"csv-field-escaped-by");
		csvFieldEscapedByEdit.SetWindowText(val.c_str()); 
		supplier->csvFieldEscapedBy = val;
	}

	if (csvLineTerminatedByComboBox.IsWindow()) {
		std::wstring val = settingService->getSysInit(L"csv-line-terminated-by");
		val = val.empty() ? QSupplier::csvLineSeperators[1] : val;
		QWinCreater::loadComboBox(csvLineTerminatedByComboBox,  QSupplier::csvLineSeperators, 3, val);
		supplier->csvLineTerminatedBy = val;
	}

	if (csvNullAsKeywordComboBox.IsWindow()) {
		std::wstring val = settingService->getSysInit(L"csv-null-as-keyword");
		val = val.empty() ? QSupplier::csvNullAsKeywords[0] : val;
		QWinCreater::loadComboBox(csvNullAsKeywordComboBox, QSupplier::csvNullAsKeywords, 2, val);
		supplier->csvNullAsKeyword = val;
	}
	
	if (csvCharsetComboBox.IsWindow()) {
		std::wstring val = settingService->getSysInit(L"csv-charset");		
		val = val.empty() ? QSupplier::csvEncodings[0] : val;
		QWinCreater::loadComboBox(csvCharsetComboBox, QSupplier::csvEncodings, 2, val);
		supplier->csvCharset = val;
	}
}

void ImportFromCsvDialog::saveImportSettings()
{
	settingService->setSysInit(L"import-from-csv-path", supplier->importPath);
	settingService->setSysInit(L"csv-column-name", std::to_wstring(supplier->csvColumnNameOnTop));
	settingService->setSysInit(L"csv-field-terminated-by", supplier->csvFieldTerminateBy);
	settingService->setSysInit(L"csv-field-enclosed-by", supplier->csvFieldEnclosedBy);
	settingService->setSysInit(L"csv-field-escaped-by", supplier->csvFieldEscapedBy);
	settingService->setSysInit(L"csv-line-terminated-by", supplier->csvLineTerminatedBy);
	settingService->setSysInit(L"csv-null-as-keyword", supplier->csvNullAsKeyword);
	settingService->setSysInit(L"csv-charset", supplier->csvCharset);
}

void ImportFromCsvDialog::loadImportPathEdit()
{
	if (!importPathEdit.IsWindow()) {
		return ;
	}

	CString str;
	importPathEdit.GetWindowText(str);
	supplier->importPath = str.GetString();
	if (str.IsEmpty()) {
		supplier->importPath = settingService->getSysInit(L"import-from-csv-path");
	}

	importPathEdit.SetWindowText(supplier->importPath.c_str());
}

bool ImportFromCsvDialog::getSelUserDbId(uint64_t & userDbId)
{
	int nSelItem = targetDbComboBox.GetCurSel();
	if (nSelItem == -1) {
		userDbId = -1;
		return false;
	}
	userDbId = targetDbComboBox.GetItemData(nSelItem);
	return true;
}


bool ImportFromCsvDialog::getImportPath(std::wstring & importPath)
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


LRESULT ImportFromCsvDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	linePen.CreatePen(PS_SOLID, 1, lineColor);
	elemFont = FT(L"elem-size");

	supplier = new ImportFromCsvSupplier(databaseSupplier);
	adapter = new ImportFromCsvAdapter(m_hWnd, supplier);

	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_IMPORT_PROCESS_ID);
	QDialog::OnInitDialog(uMsg, wParam, lParam, bHandled); 
	return 0;
}

LRESULT ImportFromCsvDialog::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	QDialog::OnDestroy(uMsg, wParam, lParam, bHandled);

	AppContext::getInstance()->unsuscribe(m_hWnd, Config::MSG_IMPORT_PROCESS_ID);

	if (!linePen.IsNull()) linePen.DeleteObject();
	if (elemFont) ::DeleteObject(elemFont);

	if (importImage.IsWindow()) importImage.DestroyWindow();
	if (importTextLabel.IsWindow()) importTextLabel.DestroyWindow();

	if (importPathLabel.IsWindow()) importPathLabel.DestroyWindow();
	if (importPathEdit.IsWindow()) importPathEdit.DestroyWindow();
	if (openFileButton.IsWindow()) openFileButton.DestroyWindow();

	if (targetDbLabel.IsWindow()) targetDbLabel.DestroyWindow();
	if (targetDbComboBox.IsWindow()) targetDbComboBox.DestroyWindow();
	if (targetTblLabel.IsWindow()) targetTblLabel.DestroyWindow();
	if (targetTblComboBox.IsWindow()) targetTblComboBox.DestroyWindow();

	if (topGroupBox.IsWindow()) topGroupBox.DestroyWindow();
	if (csvSettingsGroupBox.IsWindow()) csvSettingsGroupBox.DestroyWindow();
	if (columnsSettingsGroupBox.IsWindow()) columnsSettingsGroupBox.DestroyWindow();
	if (csvDatasGroupBox.IsWindow()) csvDatasGroupBox.DestroyWindow();

	if (columnListView.IsWindow()) columnListView.DestroyWindow();
	if (dataListView.IsWindow()) dataListView.DestroyWindow();

	if (csvFieldTerminatedByLabel.IsWindow()) csvFieldTerminatedByLabel.DestroyWindow();
	if (csvFieldTerminatedByComboBox.IsWindow()) csvFieldTerminatedByComboBox.DestroyWindow();
	if (csvFieldEnclosedByLabel.IsWindow()) csvFieldEnclosedByLabel.DestroyWindow();
	if (csvFieldEnclosedByEdit.IsWindow()) csvFieldEnclosedByEdit.DestroyWindow();
	if (csvLineTerminatedByLabel.IsWindow()) csvLineTerminatedByLabel.DestroyWindow();
	if (csvLineTerminatedByComboBox.IsWindow()) csvLineTerminatedByComboBox.DestroyWindow();
	if (csvFieldEscapedByLabel.IsWindow()) csvFieldEscapedByLabel.DestroyWindow();
	if (csvFieldEscapedByEdit.IsWindow()) csvFieldEscapedByEdit.DestroyWindow();
	if (csvNullAsKeywordLabel.IsWindow()) csvNullAsKeywordLabel.DestroyWindow();
	if (csvNullAsKeywordComboBox.IsWindow()) csvNullAsKeywordComboBox.DestroyWindow();
	if (csvCharsetLabel.IsWindow()) csvCharsetLabel.DestroyWindow();
	if (csvCharsetComboBox.IsWindow()) csvCharsetComboBox.DestroyWindow();
	if (csvColumnNameCheckBox.IsWindow()) csvColumnNameCheckBox.DestroyWindow();

	if (processBar.IsWindow()) processBar.DestroyWindow();
	if (previewSqlButton.IsWindow()) previewSqlButton.DestroyWindow();
	

	if (supplier) {
		delete supplier;
		supplier = nullptr;
	}
	if (adapter) {
		delete adapter;
		adapter = nullptr;
	}
	return 0;
}

LRESULT ImportFromCsvDialog::OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// invisible then return 
	if (!wParam) { 
		return 0;
	}
	loadWindow();
	return 0;
}

void ImportFromCsvDialog::paintItem(CDC &dc, CRect &paintRect)
{
	int x = 20, y = 15 + 32 + 10, w = paintRect.Width() - 2 * 20, h = 1;

	HPEN oldPen = dc.SelectPen(linePen);
	dc.MoveTo(x, y);
	dc.LineTo(x + w, y);
	dc.SelectPen(oldPen);
}


LRESULT ImportFromCsvDialog::OnChangeTargetDbComboBox(UINT uNotifyCode, int nID, HWND hwnd)
{
	int nSelItem = targetDbComboBox.GetCurSel();
	uint64_t userDbId = static_cast<uint64_t>(targetDbComboBox.GetItemData(nSelItem));
	std::wstring importSelectedDbId = settingService->getSysInit(L"import-selected-db-id");
	if (importSelectedDbId.empty() || userDbId == std::stoull(importSelectedDbId)) {
		return 0;
	}
	settingService->setSysInit(L"export-selected-db-id", importSelectedDbId);
	supplier->setRuntimeUserDbId(userDbId);
	loadTargetTblComboBox();
	return 0;
}


LRESULT ImportFromCsvDialog::OnChangeTargetTblComboBox(UINT uNotifyCode, int nID, HWND hwnd)
{
	int nSelItem = targetTblComboBox.GetCurSel();
	wchar_t str[256] = {0};
	targetTblComboBox.GetLBText(nSelItem, str);
	std::wstring selectTbl(str);

	if (selectTbl.empty() || supplier->getRuntimeTblName() == str) {
		return 0;
	}
	supplier->setRuntimeTblName(selectTbl);
	columnRowCount = adapter->loadCsvFileToColumnListView(supplier->importPath, columnListView);

	return 0;
}

void ImportFromCsvDialog::OnClickOpenFileButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	CString str;
	importPathEdit.GetWindowText(str);
	std::wstring importPath = str;
	std::wstring fileName, fileDir;
	if (!str.IsEmpty()) {
		fileName = FileUtil::getFileName(importPath);
		fileDir = FileUtil::getFileDir(importPath);
	}
	wchar_t * szFilter = nullptr;
	wchar_t * defExt = nullptr;
	
	szFilter = L"CSV File(*.csv)\0*.csv\0All Files(*.*)\0*.*\0\0";
	defExt = L"sql";


	CFileDialog fileDlg(TRUE, defExt, fileName.c_str(), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilter, m_hWnd);
	//fileDlg.m_ofn.lpstrTitle = S(L"save-to-file").c_str();
	fileDlg.m_ofn.lpstrInitialDir = fileDir.c_str();
	//fileDlg.m_ofn.lpstrFilter = szFilter;
	CString path;
	if (IDOK == fileDlg.DoModal()) {
		importPath = fileDlg.m_szFileName;
		importPathEdit.SetWindowText(importPath.c_str());
		supplier->importPath = importPath;

		columnRowCount = adapter->loadCsvFileToColumnListView(importPath, columnListView);
		dataRowCount = adapter->loadCsvFileToDataListView(importPath, dataListView);
	}
}


void ImportFromCsvDialog::OnClickCsvColumnNameCheckBox(UINT uNotifyCode, int nID, HWND hwnd)
{
	int newCheck = !csvColumnNameCheckBox.GetCheck();
	csvColumnNameCheckBox.SetCheck(newCheck);
	supplier->csvColumnNameOnTop = newCheck;
	columnRowCount = adapter->loadCsvFileToColumnListView(supplier->importPath, columnListView);
	dataRowCount = adapter->loadCsvFileToDataListView(supplier->importPath, dataListView);
}


void ImportFromCsvDialog::OnChangeCsvFieldTerminatedByComboBox(UINT uNotifyCode, int nID, HWND hwnd)
{
	wchar_t cch[8] = { 0 };
	int nSelItem = csvFieldTerminatedByComboBox.GetCurSel();
	csvFieldTerminatedByComboBox.GetLBText(nSelItem, cch);
	supplier->csvFieldTerminateBy = cch;
	if (supplier->csvFieldTerminateBy.empty()) {
		return ;
	}
	columnRowCount = adapter->loadCsvFileToColumnListView(supplier->importPath, columnListView);
	dataRowCount = adapter->loadCsvFileToDataListView(supplier->importPath, dataListView);
}


void ImportFromCsvDialog::OnChangeCsvFieldEnclosedByEdit(UINT uNotifyCode, int nID, HWND hwnd)
{
	CString str;
	csvFieldEnclosedByEdit.GetWindowText(str);
	supplier->csvFieldEnclosedBy = str.GetString();
	if (supplier->csvFieldEnclosedBy.empty()) {
		return ;
	}
	columnRowCount = adapter->loadCsvFileToColumnListView(supplier->importPath, columnListView);
	dataRowCount = adapter->loadCsvFileToDataListView(supplier->importPath, dataListView);
}


void ImportFromCsvDialog::OnChangeCsvFieldEscapedByEdit(UINT uNotifyCode, int nID, HWND hwnd)
{
	CString str;
	csvFieldEscapedByEdit.GetWindowText(str);
	supplier->csvFieldEscapedBy = str.GetString();
	if (supplier->csvFieldEscapedBy.empty()) {
		return ;
	}
	columnRowCount = adapter->loadCsvFileToColumnListView(supplier->importPath, columnListView);
	dataRowCount = adapter->loadCsvFileToDataListView(supplier->importPath, dataListView);
}


void ImportFromCsvDialog::OnChangeCsvLineTerminatedByComboBox(UINT uNotifyCode, int nID, HWND hwnd)
{
	wchar_t cch[8] = { 0 };
	int nSelItem = csvLineTerminatedByComboBox.GetCurSel();
	csvLineTerminatedByComboBox.GetLBText(nSelItem, cch);
	supplier->csvLineTerminatedBy = cch;
	if (supplier->csvLineTerminatedBy.empty()) {
		return ;
	}
	columnRowCount = adapter->loadCsvFileToColumnListView(supplier->importPath, columnListView);
	dataRowCount = adapter->loadCsvFileToDataListView(supplier->importPath, dataListView);
}


void ImportFromCsvDialog::OnChangeCsvNullAsKeyWordComboBox(UINT uNotifyCode, int nID, HWND hwnd)
{
	wchar_t cch[8] = { 0 };
	int nSelItem = csvNullAsKeywordComboBox.GetCurSel();
	csvNullAsKeywordComboBox.GetLBText(nSelItem, cch);
	supplier->csvNullAsKeyword = cch;
	if (supplier->csvNullAsKeyword.empty()) {
		return ;
	}
	columnRowCount = adapter->loadCsvFileToColumnListView(supplier->importPath, columnListView);
	dataRowCount = adapter->loadCsvFileToDataListView(supplier->importPath, dataListView);
}


void ImportFromCsvDialog::OnChangeCsvCharsetComboBox(UINT uNotifyCode, int nID, HWND hwnd)
{
	wchar_t cch[8] = { 0 };
	int nSelItem = csvCharsetComboBox.GetCurSel();
	csvCharsetComboBox.GetLBText(nSelItem, cch);
	supplier->csvCharset = cch;
	if (supplier->csvCharset.empty()) {
		return ;
	}
	columnRowCount = adapter->loadCsvFileToColumnListView(supplier->importPath, columnListView);
	dataRowCount = adapter->loadCsvFileToDataListView(supplier->importPath, dataListView);
}

void ImportFromCsvDialog::OnClickPreviewSqlButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	std::wstring sql = adapter->getPreviewSql();
	std::wstring title = L"Import to " + supplier->getRuntimeTblName();
	PreviewSqlDialog dialog(m_hWnd, sql, title);
	dialog.DoModal(m_hWnd);
	return;
}

void ImportFromCsvDialog::OnClickYesButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	std::wstring importPath;
	if (!getImportPath(importPath)) {
		return ;
	}
	saveImportSettings();

	// select user db id
	uint64_t userDbId;
	if (!getSelUserDbId(userDbId) || userDbId == -1) {
		return ;
	}

	isRunning = true;
	yesButton.EnableWindow(false);

	// Export objects(tables/views/triggers) to sql file
	if (!adapter->importFromRuntimeSqlList()) {
		yesButton.EnableWindow(true);
		return ;
	}

	QPopAnimate::success(S(L"import-success-text"));		
	noButton.SetWindowText(S(L"complete").c_str());
	yesButton.EnableWindow(true);

	AppContext::getInstance()->dispatch(Config::MSG_REFRESH_SAME_TABLE_DATA_ID, WPARAM(supplier->getRuntimeUserDbId()), LPARAM(&supplier->getRuntimeTblName()));
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
LRESULT ImportFromCsvDialog::OnProcessImport(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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
		processBar.error(S(L"import-error-text"));
		isRunning = false;
	} else if (wParam == 3) {
		processBar.error(S(L"import-error-text"));
		isRunning = false;
	}
	return 0;
}

LRESULT ImportFromCsvDialog::OnGetListViewData(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
	NMLVDISPINFO* plvdi = (NMLVDISPINFO*)pnmh;

	if (plvdi->hdr.hwndFrom == columnListView.m_hWnd) {
		adapter->fillSubItemForColumnListView(plvdi, columnListView);
	} else if (plvdi->hdr.hwndFrom == dataListView.m_hWnd) {
		adapter->fillSubItemForDataListView(plvdi, dataListView);
	}
	
	return 0;
}

LRESULT ImportFromCsvDialog::OnPrepareListViewData(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
	auto pCachehint = (NMLVCACHEHINT *)pnmh;
	return 0;
}

LRESULT ImportFromCsvDialog::OnFindListViewData(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
	LPNMLVFINDITEM  pnmfi = (LPNMLVFINDITEM)pnmh;

	auto iItem = pnmfi->iStart;
	if (-1 == iItem)
		return -1;

	int count = 0;
	if (pnmfi->hdr.hwndFrom == columnListView.m_hWnd) {
		count = columnRowCount;
	}else if (pnmfi->hdr.hwndFrom == dataListView.m_hWnd) {
		count = dataRowCount;
	}
	if (!count || count <= iItem)
		return -1;

	return 0;
}

LRESULT ImportFromCsvDialog::OnClickColumnListView(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
	NMITEMACTIVATE * clickItem = (NMITEMACTIVATE *)pnmh; 
	adapter->clickListViewSubItem(clickItem, columnListView);
	return 0;
}

LRESULT ImportFromCsvDialog::OnListViewSubItemTextChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	const SubItemValues & changedVals = columnListView.getChangedVals();
	for (auto val : changedVals) {
		// check if duplicated define Primary Key
		if (val.iSubItem == 2 && val.newVal == supplier->getTblRuntimeColumns().at(val.iItem)) {
			continue;
		}
		supplier->getTblRuntimeColumns().at(val.iItem) = val.newVal;
		CRect subItemRect;
		columnListView.GetSubItemRect(val.iItem, val.iSubItem, LVIR_BOUNDS, subItemRect);
		columnListView.InvalidateRect(subItemRect, false);		
	}
	return 0;
}

