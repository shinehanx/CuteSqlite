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

ExportAsSqlDialog::ExportAsSqlDialog(HWND parentHwnd, ExportDatabaseAdapter * adapter)
{
	ATLASSERT(parentHwnd != nullptr && adapter != nullptr);
	this->parentHwnd = parentHwnd;
	this->adapter = adapter;
	this->caption = S(L"export-database-as-sql");
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


	//options 	
	createOrShowSqlSettingsElems(clientRect);
	
}


void ExportAsSqlDialog::createOrShowExportImage(QStaticImage &win, CRect & clientRect)
{
	int x = 20, y = 15, w = 32, h = 32;
	CRect rect(x, y, x + w, y + h);
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_VISIBLE |WS_CLIPCHILDREN  | WS_CLIPSIBLINGS | SS_NOTIFY; // SS_NOTIFY - 表示static接受点击事件
		std::wstring imgDir = ResourceUtil::getProductImagesDir();
		std::wstring imgPath = imgDir + L"database\\dialog\\export.png";
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
	int x = 290, y = 15 + 32 + 10 + 6 + (20 + 5) * 3, w = 250, h = 100;
	CRect rect = { x, y, x + w, y + h };
	createOrShowFormGroupBox(structAndDataGroupBox, Config::EXPORT_AS_SQL_STRUCTURE_AND_DATA_GROUPBOX_ID, S(L"structure-and-data-settings"), rect, clientRect);

	rect.OffsetRect(0, h + 10);
	createOrShowFormGroupBox(insertStatementGroupBox, Config::EXPORT_AS_SQL_INSERT_STATEMENT_GROUPBOX_ID, S(L"insert-statement-settings"), rect, clientRect);

	rect.OffsetRect(0, h + 10);
	createOrShowFormGroupBox(tblStatementGroupBox, Config::EXPORT_AS_SQL_TBL_STATEMENT_GROUPBOX_ID, S(L"tbl-statement-settings"), rect, clientRect);
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

	rect.OffsetRect(w + 10, 0);	
	rect = { rect.left, rect.top, rect.left + 50, rect.bottom };
	createOrShowFormButton(openFileButton, Config::EXPORT_DB_AS_SQL_OPEN_FILE_BUTTON_ID, L"...", rect, clientRect);
}


void ExportAsSqlDialog::createOrShowDbObjectsElems(CRect & clientRect)
{
	int x = 20, y = 15 + 32 + 10 + 6 + (20 + 5) * 2, w = 250, h = 20;
	CRect rect(x, y, x + w, y + h);
	createOrShowFormLabel(selectObjectsLabel, S(L"database-objects").append(L":"), rect, clientRect, SS_LEFT, elemFont);

	rect.OffsetRect(0, h + 5);
	rect = {rect.left, rect.top, rect.right, rect.top + 300}; 
	createOrShowTreeView(dbObjectsTreeView, rect, clientRect);
}

void ExportAsSqlDialog::createOrShowTreeView(CTreeViewCtrlEx & win, CRect & rect, CRect & clientRect)
{	
	
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		win.Create(m_hWnd, rect, L"", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | TVS_FULLROWSELECT 
			| TVS_LINESATROOT | TVS_SHOWSELALWAYS | TVS_HASBUTTONS | TVS_CHECKBOXES, WS_EX_CLIENTEDGE, Config::DATABASE_OBJECTS_TREEVIEW_ID);
			//| TVS_LINESATROOT | TVS_HASBUTTONS , WS_EX_CLIENTEDGE, Config::DATABASE_TREEVIEW_ID);
		// create a singleton treeViewAdapter pointer
		objectsTreeViewAdapter = ObjectsTreeViewAdapter::getInstance(m_hWnd, &win);
		return;
	}
	else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		// show first then move window
		win.ShowWindow(SW_SHOW);
		win.MoveWindow(&rect);
	}
}

void ExportAsSqlDialog::createOrShowSqlSettingsElems(CRect & clientRect)
{
	bool isEnabled = settingService->getSysInit(L"export_fmt") == L"SQL" ? true : false;

	int x = 300, y = 15 + 32 + 10 + 6 + (20 + 5) * 3 + 20, w = 150, h = 20;
	CRect rect(x, y, x + w, y + h);
	createOrShowFormRadio(structureOnlyRadio, Config::EXPORT_SQL_STRUCTURE_ONLY_RADIO_ID, S(L"structure-only"), rect, clientRect);
	structureOnlyRadio.EnableWindow(isEnabled);

	rect.OffsetRect(0, h + 5);
	createOrShowFormRadio(dataOnlyRadio, Config::EXPORT_SQL_DATA_ONLY_RADIO_ID, S(L"data-only"), rect, clientRect);
	dataOnlyRadio.EnableWindow(isEnabled);

	rect.OffsetRect(0, h + 5);
	createOrShowFormRadio(structurAndDataRadio, Config::EXPORT_SQL_STRUCTURE_DATA_RADIO_ID, S(L"structure-and-data"), rect, clientRect);
	structurAndDataRadio.EnableWindow(isEnabled);
}

void ExportAsSqlDialog::loadWindow()
{
	if (!isNeedReload) {
		return ;
	}

	isNeedReload = false;	
	loadSelectDbComboBox();
	loadObjectsTreeView();
	loadSqlSettingsElems();	
	loadExportPathEdit();
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

void ExportAsSqlDialog::loadSqlSettingsElems()
{
	std::wstring dbExportAsSql = settingService->getSysInit(L"db_export_as_sql");
	for (auto ptr : sqlRadioPtrs) {
		if (ptr->m_hWnd == structureOnlyRadio.m_hWnd && dbExportAsSql == L"structure-only") {
			ptr->SetCheck(1);
		} else if (ptr->m_hWnd == dataOnlyRadio.m_hWnd && dbExportAsSql == L"data-only") {
			ptr->SetCheck(1);
		} else if (ptr->m_hWnd == structurAndDataRadio.m_hWnd && dbExportAsSql == L"structure-and-data") {
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
	exportPath = exportResultService->changeExportPathExt(exportPath, L"sql");	

	exportPathEdit.SetWindowText(exportPath.c_str());
}

void ExportAsSqlDialog::saveExportSqlParams(ExportSqlParams & params)
{
	settingService->setSysInit(L"db_export_as_sql", params.sqlSetting);
}


void ExportAsSqlDialog::saveExportPath(std::wstring & exportPath)
{
	settingService->setSysInit(L"db_export_as_sql_path", exportPath);
}

/**
 * Get export to SQL params.
 * 
 * @param params [out] the return params
 * @return 
 */
bool ExportAsSqlDialog::getExportSqlParams(ExportSqlParams & params)
{
	HWND selHwnd = getSelSqlRadioHwnd();
	if (selHwnd == structureOnlyRadio.m_hWnd) {
		params.sqlSetting = L"structure-only";
	} else if (selHwnd == dataOnlyRadio.m_hWnd) {
		params.sqlSetting = L"data-only";
	} else if (selHwnd == structurAndDataRadio.m_hWnd){
		params.sqlSetting = L"structure-and-data";
	} else {
		QPopAnimate::error(m_hWnd, S(L"export-sql-error"));
		structureOnlyRadio.SetFocus();
		return false;
	}
	return true;
}

HWND ExportAsSqlDialog::getSelSqlRadioHwnd()
{
	HWND selHwnd = nullptr;
	std::for_each(sqlRadioPtrs.begin(), sqlRadioPtrs.end(), [&selHwnd](CButton * ptr) {
		int checked = ptr->GetCheck();
		if (ptr->GetCheck()) {
			selHwnd = ptr->m_hWnd;
		}
	});
	return selHwnd;
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

	sqlRadioPtrs.push_back(&structureOnlyRadio);
	sqlRadioPtrs.push_back(&dataOnlyRadio);
	sqlRadioPtrs.push_back(&structurAndDataRadio);

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
	
	
	if (structAndDataGroupBox.IsWindow()) structAndDataGroupBox.DestroyWindow();
	
	if (structureOnlyRadio.IsWindow()) structureOnlyRadio.DestroyWindow();
	if (dataOnlyRadio.IsWindow()) dataOnlyRadio.DestroyWindow();
	if (structurAndDataRadio.IsWindow()) structurAndDataRadio.DestroyWindow();
	sqlRadioPtrs.clear();

	
	if (exportPathLabel.IsWindow()) exportPathLabel.DestroyWindow();
	if (exportPathEdit.IsWindow()) exportPathEdit.DestroyWindow();
	if (openFileButton.IsWindow()) openFileButton.DestroyWindow();
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


void ExportAsSqlDialog::OnClickExportSqlRadios(UINT uNotifyCode, int nID, HWND hwnd)
{
	HWND selHwnd = nullptr;
	std::for_each(sqlRadioPtrs.begin(), sqlRadioPtrs.end(), [&hwnd, &selHwnd](CButton * ptr) {
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

void ExportAsSqlDialog::OnClickYesButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	std::wstring exportPath;
	if (!getExportPath(exportPath)) {
		return ;
	}
	saveExportPath(exportPath);

	// todo ...

	EndDialog(1);
}

void ExportAsSqlDialog::OnClickNoButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	EndDialog(0);
}
