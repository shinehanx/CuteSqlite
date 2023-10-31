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

 * @file   ExportAsSqlDialog.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-06-07
 *********************************************************************/
#include "stdafx.h"
#include "ExportAsSqlDialog.h"
#include <string>
#include "utils/FontUtil.h"
#include "ui/common/message/QPopAnimate.h"
#include "ui/common/message/QMessageBox.h"
#include "common/AppContext.h"

ExportAsSqlDialog::ExportAsSqlDialog(HWND parentHwnd, ExportDatabaseAdapter * adapter)
{
	ATLASSERT(parentHwnd != nullptr && adapter != nullptr);
	this->parentHwnd = parentHwnd;
	this->adapter = adapter;
	this->caption = S(L"database-export-as-sql");
	setFormSize(EXPORT_AS_SQL_DIALOG_WIDTH, EXPORT_AS_SQL_DIALOG_HEIGHT);
}


void ExportAsSqlDialog::createOrShowUI()
{
	QDialog::createOrShowUI();
	CRect clientRect;
	GetClientRect(clientRect);

	createOrShowExportImage(exportImage, clientRect);
	createOrShowExportTextLabel(exportTextLabel, clientRect);
	createOrShowGroupBoxes(clientRect);

	// selected database
	createOrShowSelectDbElems(clientRect);

	// Export path
	createOrShowExportPathElems(clientRect);

	// Database objects
	createOrShowDbObjectsElems(clientRect);


	// structure and data settings 	
	createOrShowStructureAndDataSettingsElems(clientRect);

	// insert statement settings
	createOrShowInsertStatementSettingsElems(clientRect);
	
	// table statement settings
	createOrShowTblStatementSettingsElems(clientRect);
	
	// process bar 
	createOrShowProcessBar(processBar, clientRect);

	yesButton.SetWindowText(S(L"export").c_str());
}


void ExportAsSqlDialog::createOrShowExportImage(QStaticImage &win, CRect & clientRect)
{
	int x = 20, y = 15, w = 32, h = 32;
	CRect rect(x, y, x + w, y + h);
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_VISIBLE |WS_CLIPCHILDREN  | WS_CLIPSIBLINGS | SS_NOTIFY; // SS_NOTIFY - 表示static接受点击事件
		std::wstring imgDir = ResourceUtil::getProductImagesDir();
		std::wstring imgPath = imgDir + L"database\\dialog\\export-database.png";
		win.load(imgPath.c_str(), BI_PNG, true);
		win.Create(m_hWnd, rect, L"", dwStyle , 0);
		return;
	} else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
	}
}


void ExportAsSqlDialog::createOrShowExportTextLabel(CStatic &win, CRect & clientRect)
{
	std::wstring text = S(L"export-as-sql-text-description");
	std::wstring fmt = Lang::fontName();
	int fsize = Lang::fontSize(L"form-text-size"); 
	CSize size = FontUtil::measureTextSize(text.c_str(), fsize, false,  fmt.c_str());
	int x = 20 + 32 + 20, y = 25, w = clientRect.Width() - x - 20, h = size.cy + 2;
	CRect rect(x, y, x + w, y + h);
	createOrShowFormLabel(win, text, rect, clientRect, SS_LEFT, elemFont);
}

void ExportAsSqlDialog::createOrShowGroupBoxes(CRect & clientRect)
{
	int x = 290, y = 15 + 32 + 10 + 6 + (20 + 5) * 3, w = 310, h = 100;
	CRect rect = { x, y, x + w, y + h };
	createOrShowFormGroupBox(structAndDataGroupBox, Config::EXPORT_AS_SQL_STRUCTURE_AND_DATA_GROUPBOX_ID, S(L"structure-and-data-settings"), rect, clientRect);

	rect.OffsetRect(0, h + 10);
	rect.bottom -= 10;
	createOrShowFormGroupBox(insertStatementGroupBox, Config::EXPORT_AS_SQL_INSERT_STATEMENT_GROUPBOX_ID, S(L"insert-statement-settings"), rect, clientRect);

	rect.OffsetRect(0, rect.Height() + 10);
	createOrShowFormGroupBox(tblStatementGroupBox, Config::EXPORT_AS_SQL_TBL_STATEMENT_GROUPBOX_ID, S(L"tbl-statement-settings"), rect, clientRect);
}


void ExportAsSqlDialog::createOrShowProcessBar(QProcessBar &win, CRect & clientRect)
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

void ExportAsSqlDialog::createOrShowSelectDbElems(CRect & clientRect)
{
	int x = 20, y = 15 + 32 + 10 + 6, w = 250, h = 20;
	CRect rect(x, y, x + w, y + h);
	createOrShowFormLabel(selectDbLabel, S(L"select-database").append(L":"), rect, clientRect, SS_LEFT, elemFont);

	rect.OffsetRect(0, h + 5);
	createOrShowFormComboBox(selectDbComboBox, Config::EXPORT_DB_AS_SQL_SELECT_DB_COMBOBOX_ID,  L"", rect, clientRect);
}

void ExportAsSqlDialog::createOrShowExportPathElems(CRect & clientRect)
{
	int x = 20 + 250 + 20, y = 15 + 32 + 10 + 6, w = 250, h = 20;
	CRect rect(x, y, x + w, y + h);
	createOrShowFormLabel(exportPathLabel, S(L"save-to-file").append(L":"), rect, clientRect, SS_LEFT, elemFont);

	rect.OffsetRect(0, h + 5);
	createOrShowFormEdit(exportPathEdit, Config::EXPORT_DB_AS_SQL_PATH_EDIT_ID, L"", L"", rect, clientRect, ES_LEFT, false);
	exportPathEdit.SetLimitText(1024);

	rect.OffsetRect(w + 10, 0);	
	rect = { rect.left, rect.top, rect.left + 50, rect.bottom };
	createOrShowFormButton(openFileButton, Config::EXPORT_DB_AS_SQL_OPEN_FILE_BUTTON_ID, L"...", rect, clientRect);
}


void ExportAsSqlDialog::createOrShowDbObjectsElems(CRect & clientRect)
{
	int x = 20, y = 15 + 32 + 10 + 6 + (20 + 5) * 2, w = 250, h = 20;
	CRect rect(x, y, x + w, y + h);
	createOrShowFormLabel(selectObjectsLabel, S(L"database-objects").append(L":"), rect, clientRect, SS_LEFT, elemFont);

	// object(s) tree view
	rect.OffsetRect(0, h + 5);
	rect = {rect.left, rect.top, rect.right, rect.top + 300}; 
	createOrShowTreeView(dbObjectsTreeView, rect, clientRect);

	// select all button
	rect.OffsetRect(0, rect.Height() + 5);
	rect = {rect.left, rect.top, rect.left + 70, rect.top + 28};
	createOrShowFormButton(selectAllButton, Config::EXPORT_DB_AS_SQL_SELECT_ALL_OBJECTS_BUTTON_ID, S(L"select-all"), rect, clientRect);

	// deselect all button
	rect.OffsetRect(rect.Width() + 20, 0);
	createOrShowFormButton(deselectAllButton, Config::EXPORT_DB_AS_SQL_DESELECT_ALL_OBJECTS_BUTTON_ID, S(L"deselect-all"), rect, clientRect);
}

void ExportAsSqlDialog::createOrShowTreeView(CTreeViewCtrlEx & win, CRect & rect, CRect & clientRect)
{	
	
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		win.Create(m_hWnd, rect, L"", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | TVS_FULLROWSELECT 
			| TVS_LINESATROOT | TVS_SHOWSELALWAYS | TVS_HASBUTTONS | TVS_CHECKBOXES, WS_EX_CLIENTEDGE, Config::DATABASE_OBJECTS_TREEVIEW_ID);
			//| TVS_LINESATROOT | TVS_HASBUTTONS , WS_EX_CLIENTEDGE, Config::DATABASE_TREEVIEW_ID);
		// create a singleton treeViewAdapter pointer
		objectsTreeViewAdapter = new ObjectsTreeViewAdapter(m_hWnd, &win);
		return;
	}
	else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		// show first then move window
		win.ShowWindow(SW_SHOW);
		win.MoveWindow(&rect);
	}
}

void ExportAsSqlDialog::createOrShowStructureAndDataSettingsElems(CRect & clientRect)
{	
	int x = 300, y = 15 + 32 + 10 + 6 + (20 + 5) * 3 + 20, w = 150, h = 20;
	CRect rect(x, y, x + w, y + h);
	createOrShowFormRadio(structureOnlyRadio, Config::STRUCTURE_ONLY_RADIO_ID, S(L"structure-only"), rect, clientRect);

	rect.OffsetRect(0, h + 5);
	createOrShowFormRadio(dataOnlyRadio, Config::DATA_ONLY_RADIO_ID, S(L"data-only"), rect, clientRect);

	rect.OffsetRect(0, h + 5);
	createOrShowFormRadio(structureAndDataRadio, Config::STRUCTURE_DATA_RADIO_ID, S(L"structure-and-data"), rect, clientRect);
}


void ExportAsSqlDialog::createOrShowInsertStatementSettingsElems(CRect & clientRect)
{
	int x = 300, y = 15 + 32 + 10 + 6 + (20 + 5) * 3 + 100 + 10 + 20, w = 250, h = 20;
	CRect rect(x, y, x + w, y + h);
	createOrShowFormCheckBox(retainColumnCheckbox, Config::EXPORT_RETAIN_COLUMN_CHECKBOX_ID, S(L"retain-column-text"), rect, clientRect);

	rect.OffsetRect(0, h + 5);
	createOrShowFormCheckBox(containMultiRowsCheckbox, Config::EXPORT_CONTAIN_MULTI_ROWS_CHECKBOX_ID, S(L"contain-multi-rows-text"), rect, clientRect);
}


void ExportAsSqlDialog::createOrShowTblStatementSettingsElems(CRect & clientRect)
{
	int x = 300, y = 15 + 32 + 10 + 6 + (20 + 5) * 3 + 100 + 10 + 90 + 10 + 20, w = 300, h = 20;
	CRect rect(x, y, x + w, y + h);
	createOrShowFormRadio(retainTableRadio, Config::EXPORT_RETAIN_TABLE_RADIO_ID, S(L"retain-table-text"), rect, clientRect);

	rect.OffsetRect(0, h + 5);
	createOrShowFormRadio(overrideTableRadio, Config::EXPORT_OVERRIDE_TABLE_RADIO_ID, S(L"override-table-text"), rect, clientRect);
}

void ExportAsSqlDialog::loadWindow()
{
	if (!isNeedReload) {
		return ;
	}

	isNeedReload = false;	
	loadSelectDbComboBox();	
	loadExportPathEdit();
	loadObjectsTreeView();

	loadStructureAndDataSettingsElems();	
	loadInsertStatementSettingsElems();	
	loadTblStatementSettingsElems();	
}


void ExportAsSqlDialog::loadSelectDbComboBox()
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
			settingService->setSysInit(L"export_selected_db_id", std::to_wstring(item.id));
		}
	}
	selectDbComboBox.SetCurSel(nSelItem);
	
}


void ExportAsSqlDialog::loadObjectsTreeView()
{
	objectsTreeViewAdapter->loadTreeView();
}

void ExportAsSqlDialog::loadStructureAndDataSettingsElems()
{
	std::wstring structureAndDataSetting = settingService->getSysInit(L"export_as_sql_structure_and_data");
	for (auto ptr : structureAndDataRadioPtrs) {
		if (ptr->m_hWnd == structureOnlyRadio.m_hWnd && structureAndDataSetting == L"structure-only") {
			ptr->SetCheck(1);
		} else if (ptr->m_hWnd == dataOnlyRadio.m_hWnd && structureAndDataSetting == L"data-only") {
			ptr->SetCheck(1);
		} else if (ptr->m_hWnd == structureAndDataRadio.m_hWnd && structureAndDataSetting == L"structure-and-data") {
			ptr->SetCheck(1);
		} else {
			ptr->SetCheck(0);
		}	
	}
}


void ExportAsSqlDialog::loadInsertStatementSettingsElems()
{
	std::wstring retainColumn = settingService->getSysInit(L"export_as_sql_retain_column");	
	retainColumn == L"1" ? retainColumnCheckbox.SetCheck(1): retainColumnCheckbox.SetCheck(0);

	std::wstring containMultiRow = settingService->getSysInit(L"export_as_sql_contain_multi_rows");	
	containMultiRow == L"1" ? containMultiRowsCheckbox.SetCheck(1): containMultiRowsCheckbox.SetCheck(0);
}


void ExportAsSqlDialog::loadTblStatementSettingsElems()
{
	std::wstring tblStatementSetting = settingService->getSysInit(L"export_as_sql_tbl_statement");
	for (auto ptr : tblStatementRadioPtrs) {
		if (ptr->m_hWnd == retainTableRadio.m_hWnd && tblStatementSetting == L"retain-table") {
			ptr->SetCheck(1);
		} else if (ptr->m_hWnd == overrideTableRadio.m_hWnd && tblStatementSetting == L"override-table") {
			ptr->SetCheck(1);
		} else {
			ptr->SetCheck(0);
		}	
	}
}

void ExportAsSqlDialog::loadExportPathEdit()
{
	if (!exportPathEdit.IsWindow()) {
		return ;
	}

	CString str;
	exportPathEdit.GetWindowText(str);
	std::wstring exportPath = str.GetString();
	if (str.IsEmpty()) {
		exportPath = settingService->getSysInit(L"export_as_sql_path");
	}

	exportPathEdit.SetWindowText(exportPath.c_str());
}

void ExportAsSqlDialog::saveExportStructureAndDataSettings(StructAndDataParams & params)
{
	settingService->setSysInit(L"export_as_sql_structure_and_data", params.sqlSetting);
}


void ExportAsSqlDialog::saveInsertStatementSettings(InsertStatementParams & params)
{
	settingService->setSysInit(L"export_as_sql_retain_column", params.retainColumn ? L"1" : L"0");
	settingService->setSysInit(L"export_as_sql_contain_multi_rows", params.multiRows ? L"1" : L"0");
}


void ExportAsSqlDialog::saveTblStatementSettings(TblStatementParams & params)
{
	settingService->setSysInit(L"export_as_sql_tbl_statement", params.param);
}

void ExportAsSqlDialog::saveExportPath(std::wstring & exportPath)
{
	settingService->setSysInit(L"export_as_sql_path", exportPath);
}

/**
 * Get params of structure and data settings.
 * 
 * @param params [out] the return params
 * @return 
 */
bool ExportAsSqlDialog::getExportStructureAndDataParams(StructAndDataParams & params)
{
	HWND selHwnd = getSelStructureAndDataRadioHwnd();
	if (selHwnd == structureOnlyRadio.m_hWnd) {
		params.sqlSetting = L"structure-only";
	} else if (selHwnd == dataOnlyRadio.m_hWnd) {
		params.sqlSetting = L"data-only";
	} else if (selHwnd == structureAndDataRadio.m_hWnd){
		params.sqlSetting = L"structure-and-data";
	} else {
		QPopAnimate::error(m_hWnd, S(L"export-sql-error"));
		structureOnlyRadio.SetFocus();
		return false;
	}
	return true;
}


void ExportAsSqlDialog::getExportInsertStatementParams(InsertStatementParams & params)
{	
	params.retainColumn = retainColumnCheckbox.GetCheck();
	params.multiRows = containMultiRowsCheckbox.GetCheck();	
}


bool ExportAsSqlDialog::getExportTblStatementParams(TblStatementParams & params)
{
	HWND selHwnd = getSelTblStatementRadioHwnd();
	if (selHwnd == retainTableRadio.m_hWnd) {
		params.param = L"retain-table";
	} else if (selHwnd == overrideTableRadio.m_hWnd) {
		params.param = L"override-table";
	} else {
		QPopAnimate::error(m_hWnd, S(L"export-sql-error"));
		retainTableRadio.SetFocus();
		return false;
	}
	return true;
}

HWND ExportAsSqlDialog::getSelStructureAndDataRadioHwnd()
{
	HWND selHwnd = nullptr;
	std::for_each(structureAndDataRadioPtrs.begin(), structureAndDataRadioPtrs.end(), [&selHwnd](CButton * ptr) {
		int checked = ptr->GetCheck();
		if (ptr->GetCheck()) {
			selHwnd = ptr->m_hWnd;
		}
	});
	return selHwnd;
}


HWND ExportAsSqlDialog::getSelTblStatementRadioHwnd()
{
	HWND selHwnd = nullptr;
	std::for_each(tblStatementRadioPtrs.begin(), tblStatementRadioPtrs.end(), [&selHwnd](CButton * ptr) {
		int checked = ptr->GetCheck();
		if (ptr->GetCheck()) {
			selHwnd = ptr->m_hWnd;
		}
	});
	return selHwnd;
}

bool ExportAsSqlDialog::getSelUserDbId(uint64_t & userDbId)
{
	int nSelItem = selectDbComboBox.GetCurSel();
	if (nSelItem == -1) {
		userDbId = -1;
		return false;
	}
	userDbId = selectDbComboBox.GetItemData(nSelItem);
	return true;
}


/**
 * Get export path string
 * 
 * @param params [out] the return params
 * @return 
 */
bool ExportAsSqlDialog::getExportPath(std::wstring & exportPath)
{
	CString str;
	exportPathEdit.GetWindowText(str);
	if (str.IsEmpty()) {
		QPopAnimate::error(m_hWnd, S(L"export-path-error"));
		exportPathEdit.SetFocus();
		return false;
	}

	// the file exists 
	exportPath = str.GetString();
	if (_waccess(str.GetString(), 0) == 0) {
		std::wstring msg = StringUtil::replace(S(L"export-file-exists-prompt"), L"{export-path}", exportPath);
		if (QMessageBox::confirm(m_hWnd, msg) != Config::CUSTOMER_FORM_YES_BUTTON_ID) {
			return false;
		}
	}

	return true;
}

LRESULT ExportAsSqlDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	linePen.CreatePen(PS_SOLID, 1, lineColor);
	elemFont = FT(L"elem-size");

	structureAndDataRadioPtrs.push_back(&structureOnlyRadio);
	structureAndDataRadioPtrs.push_back(&dataOnlyRadio);
	structureAndDataRadioPtrs.push_back(&structureAndDataRadio);

	tblStatementRadioPtrs.push_back(&retainTableRadio);
	tblStatementRadioPtrs.push_back(&overrideTableRadio);
	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_EXPORT_DB_AS_SQL_PROCESS_ID);

	QDialog::OnInitDialog(uMsg, wParam, lParam, bHandled); 
	return 0;
}

LRESULT ExportAsSqlDialog::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	QDialog::OnDestroy(uMsg, wParam, lParam, bHandled);

	if (!linePen.IsNull()) linePen.DeleteObject();
	if (elemFont) ::DeleteObject(elemFont);

	if (exportImage.IsWindow()) exportImage.DestroyWindow();
	if (exportTextLabel.IsWindow()) exportTextLabel.DestroyWindow();

	if (selectDbLabel.IsWindow()) selectDbLabel.DestroyWindow();
	if (selectDbComboBox.IsWindow()) selectDbComboBox.DestroyWindow();

	if (selectObjectsLabel.IsWindow()) selectObjectsLabel.DestroyWindow();
	if (dbObjectsTreeView.IsWindow()) dbObjectsTreeView.DestroyWindow();	
	if (selectAllButton.IsWindow()) selectAllButton.DestroyWindow();	
	if (deselectAllButton.IsWindow()) deselectAllButton.DestroyWindow();	
	
	if (structAndDataGroupBox.IsWindow()) structAndDataGroupBox.DestroyWindow();
	if (insertStatementGroupBox.IsWindow()) insertStatementGroupBox.DestroyWindow();
	if (tblStatementGroupBox.IsWindow()) tblStatementGroupBox.DestroyWindow();
	
	if (structureOnlyRadio.IsWindow()) structureOnlyRadio.DestroyWindow();
	if (dataOnlyRadio.IsWindow()) dataOnlyRadio.DestroyWindow();
	if (structureAndDataRadio.IsWindow()) structureAndDataRadio.DestroyWindow();
	structureAndDataRadioPtrs.clear();
	
	if (retainColumnCheckbox.IsWindow()) retainColumnCheckbox.DestroyWindow();
	if (containMultiRowsCheckbox.IsWindow()) containMultiRowsCheckbox.DestroyWindow();

	if (retainTableRadio.IsWindow()) retainTableRadio.DestroyWindow();
	if (overrideTableRadio.IsWindow()) overrideTableRadio.DestroyWindow();
	tblStatementRadioPtrs.clear();

	if (processBar.IsWindow()) processBar.DestroyWindow();

	if (exportPathLabel.IsWindow()) exportPathLabel.DestroyWindow();
	if (exportPathEdit.IsWindow()) exportPathEdit.DestroyWindow();
	if (openFileButton.IsWindow()) openFileButton.DestroyWindow();

	if (objectsTreeViewAdapter) {
		delete objectsTreeViewAdapter;
		objectsTreeViewAdapter = nullptr;
	}
	AppContext::getInstance()->unsuscribe(m_hWnd, Config::MSG_EXPORT_DB_AS_SQL_PROCESS_ID);
	return 0;
}


LRESULT ExportAsSqlDialog::OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// invisible then return 
	if (!wParam) { 
		return 0;
	}
	loadWindow();
	return 0;
}


LRESULT ExportAsSqlDialog::OnCheckObjectsTreeViewItem(LPNMHDR pnmh)
{
	CPoint pt;
    GetCursorPos(&pt);
    ScreenToClient(&pt);
    MapWindowPoints(dbObjectsTreeView.m_hWnd, &pt, 1);

	UINT flag = TVHT_ONITEM;
    HTREEITEM hCurrentItem = dbObjectsTreeView.HitTest(pt, &flag);
    if(hCurrentItem) {
        dbObjectsTreeView.SelectItem(hCurrentItem);
        if(flag&(TVHT_ONITEMSTATEICON)) {
            objectsTreeViewAdapter->checkToTree(hCurrentItem, !dbObjectsTreeView.GetCheckState(hCurrentItem));
        }
    }

	return 0;
}

void ExportAsSqlDialog::paintItem(CDC &dc, CRect &paintRect)
{
	int x = 20, y = 15 + 32 + 10, w = paintRect.Width() - 2 * 20, h = 1;

	HPEN oldPen = dc.SelectPen(linePen);
	dc.MoveTo(x, y);
	dc.LineTo(x + w, y);
	dc.SelectPen(oldPen);
}


LRESULT ExportAsSqlDialog::OnChangeSelectDbComboBox(UINT uNotifyCode, int nID, HWND hwnd)
{
	int nSelItem = selectDbComboBox.GetCurSel();
	uint64_t userDbId = static_cast<uint64_t>(selectDbComboBox.GetItemData(nSelItem));

	objectsTreeViewAdapter->selectItem(userDbId);

	return 0;
}

void ExportAsSqlDialog::OnClickStructureAndDataRadios(UINT uNotifyCode, int nID, HWND hwnd)
{
	HWND selHwnd = nullptr;
	std::for_each(structureAndDataRadioPtrs.begin(), structureAndDataRadioPtrs.end(), [&hwnd, &selHwnd](CButton * ptr) {
		int checked = ptr->GetCheck();
		if (ptr->m_hWnd == hwnd) {
			if (!checked) {
				ptr->SetCheck(!checked);
			}
			selHwnd = hwnd;
		}else {
			ptr->SetCheck(0);
		}
	});
}


void ExportAsSqlDialog::OnClickInsertStatemenCheckBox(UINT uNotifyCode, int nID, HWND hwnd)
{
	if (hwnd == retainColumnCheckbox.m_hWnd) {
		retainColumnCheckbox.SetCheck(!retainColumnCheckbox.GetCheck());
	} else if (hwnd == containMultiRowsCheckbox.m_hWnd) {
		containMultiRowsCheckbox.SetCheck(!containMultiRowsCheckbox.GetCheck());
	}
}

void ExportAsSqlDialog::OnClickTblStatementRadios(UINT uNotifyCode, int nID, HWND hwnd)
{
	HWND selHwnd = nullptr;
	std::for_each(tblStatementRadioPtrs.begin(), tblStatementRadioPtrs.end(), [&hwnd, &selHwnd](CButton * ptr) {
		int checked = ptr->GetCheck();
		if (ptr->m_hWnd == hwnd) {
			if (!checked) {
				ptr->SetCheck(!checked);
			}
			selHwnd = hwnd;
		}else {
			ptr->SetCheck(0);
		}
	});
}

void ExportAsSqlDialog::OnClickOpenFileButton(UINT uNotifyCode, int nID, HWND btnHwnd)
{
	CString str;
	exportPathEdit.GetWindowText(str);
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


	CFileDialog fileDlg(FALSE, defExt, fileName.c_str(), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, m_hWnd);
	//fileDlg.m_ofn.lpstrTitle = S(L"save-to-file").c_str();
	fileDlg.m_ofn.lpstrInitialDir = fileDir.c_str();
	//fileDlg.m_ofn.lpstrFilter = szFilter;
	CString path;
	if (IDOK == fileDlg.DoModal()) {
		exportPath = fileDlg.m_szFileName;
		exportPathEdit.SetWindowText(exportPath.c_str());
	}
}


void ExportAsSqlDialog::OnClickSelectAllButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	objectsTreeViewAdapter->selectAll(TRUE);
}


void ExportAsSqlDialog::OnClickDeSelectAllButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	objectsTreeViewAdapter->selectAll(FALSE);
}

void ExportAsSqlDialog::OnClickYesButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	std::wstring exportPath;
	if (!getExportPath(exportPath)) {
		return ;
	}
	saveExportPath(exportPath);

	// select user db id
	uint64_t userDbId;
	if (!getSelUserDbId(userDbId) || userDbId == -1) {
		return ;
	}
	UserTableList tblList;
	UserViewList viewList;
	UserViewList triggerList;
	isRunning = true;
	yesButton.EnableWindow(false);
	int percent = 0;
	objectsTreeViewAdapter->getSelUserTableList(userDbId, tblList);
	objectsTreeViewAdapter->getSelUserViewList(userDbId, viewList);
	objectsTreeViewAdapter->getSelUserTriggerList(userDbId, triggerList);
	
	StructAndDataParams structAndDataParams;
	InsertStatementParams insertStatementParams;
	TblStatementParams tblStatementParams;
	// struct and data settings
	if (!getExportStructureAndDataParams(structAndDataParams)) {
		isRunning = false;
		yesButton.EnableWindow(TRUE);
		return ;
	}
	// insert statement settings
	getExportInsertStatementParams(insertStatementParams);
	if (!getExportTblStatementParams(tblStatementParams)) {
		yesButton.EnableWindow(TRUE);
		return ;
	}

	// Export objects(tables/views/triggers) to sql file
	adapter->exportObjectsToSql(userDbId, exportPath, tblList, viewList, triggerList, 
		structAndDataParams, insertStatementParams, tblStatementParams);

	// save the settings to sys_init table
	saveExportStructureAndDataSettings(structAndDataParams);
	saveInsertStatementSettings(insertStatementParams);
	saveTblStatementSettings(tblStatementParams);

	yesButton.EnableWindow(TRUE);
}

void ExportAsSqlDialog::OnClickNoButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	EndDialog(0);
}

/**
 * Handle the message for export process.
 * 
 * @param uMsg - Config::MSG_EXPORT_DB_AS_SQL_PROCESS_ID
 * @param wParam - 0 : Export in progress, 1:export is complete 2:Export has error(s)
 * @param lParam - Percent of export progress
 * @param bHandled - not use
 * @return 0
 */
LRESULT ExportAsSqlDialog::OnProcessExport(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	Q_DEBUG(L"recieve MSG_EXPORT_DB_AS_SQL_PROCESS_ID, isCompete:{},percent:{}", wParam, lParam);

	if (wParam == 1) { // 1 表示执行完成
		AppContext * appContext = AppContext::getInstance();
		processBar.run(100);
		isRunning = false;
	} else if (wParam == 0) { 
		// 进度条
		int percent = static_cast<int>(lParam);
		processBar.run(percent);
	} else if (wParam == 2) {
		QPopAnimate::error(m_hWnd, S(L"export-as-sql-error-text"));
		isRunning = false;
	} else if (wParam == 3) {
		QPopAnimate::error(m_hWnd, S(L"file-not-found"));
		isRunning = false;
	}
	return 0;
}
