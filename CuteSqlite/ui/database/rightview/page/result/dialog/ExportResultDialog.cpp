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

 * @file   ExportResultDialog.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-06-07
 *********************************************************************/
#include "stdafx.h"
#include "ExportResultDialog.h"
#include <string>
#include "utils/FontUtil.h"
#include "ui/common/message/QPopAnimate.h"

ExportResultDialog::ExportResultDialog(HWND parentHwnd, ResultListPageAdapter * adapter)
{
	ATLASSERT(parentHwnd != nullptr && adapter != nullptr);
	this->parentHwnd = parentHwnd;
	this->adapter = adapter;
	this->caption = S(L"export-result-as");
	setFormSize(670, 480);
}


void ExportResultDialog::createOrShowUI()
{
	QDialog::createOrShowUI();
	CRect clientRect;
	GetClientRect(clientRect);

	createOrShowExportImage(exportImage, clientRect);
	createOrShowExportTextLabel(exportTextLabel, clientRect);
	createOrShowRadios(clientRect);
	createOrShowGroupBoxes(clientRect);

	//options 
	createOrShowCsvSettingsElems(clientRect);
	createOrShowExcelSettingsElems(clientRect);
	createOrShowSqlSettingsElems(clientRect);
	createOrShowSelectFieldsElems(clientRect);
	createOrShowExportPathElems(clientRect);
}


void ExportResultDialog::createOrShowExportImage(QStaticImage &win, CRect & clientRect)
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


void ExportResultDialog::createOrShowExportTextLabel(CStatic &win, CRect & clientRect)
{
	std::wstring text = S(L"export-text-description");
	std::wstring fmt = Lang::fontName();
	int fsize = Lang::fontSize(L"form-text-size"); 
	CSize size = FontUtil::measureTextSize(text.c_str(), fsize, false,  fmt.c_str());
	int x = 20 + 32 + 20, y = 25, w = clientRect.Width() - x - 20, h = size.cy + 2;
	CRect rect(x, y, x + w, y + h);
	createOrShowFormLabel(win, text, rect, clientRect, SS_LEFT, elemFont);
}


void ExportResultDialog::createOrShowRadios(CRect & clientRect)
{
	int x = 20, y = 15 + 32 + 10 + 6, w = 80, h = 20;
	CRect rect(x, y, x + w, y + h);
	createOrShowFormRadio(csvRadio, Config::EXPORT_TO_CSV_RADIO_ID, L"CSV", rect, clientRect);
	

	rect.OffsetRect(w + 10, 0);
	createOrShowFormRadio(jsonRadio, Config::EXPORT_TO_JSON_RADIO_ID, L"JSON", rect, clientRect);

	rect.OffsetRect(w + 10, 0);
	createOrShowFormRadio(htmlRadio, Config::EXPORT_TO_HTML_RADIO_ID, L"HTML", rect, clientRect);

	rect.OffsetRect(w + 10, 0);
	createOrShowFormRadio(xmlRadio, Config::EXPORT_TO_XML_RADIO_ID, L"XML", rect, clientRect);

	rect.OffsetRect(w + 10, 0);
	createOrShowFormRadio(excelXmlRadio, Config::EXPORT_TO_EXCEL_XML_RADIO_ID, L"Excel XML", rect, clientRect);

	rect.OffsetRect(w + 10, 0);
	createOrShowFormRadio(sqlRadio, Config::EXPORT_TO_SQL_RADIO_ID, L"SQL", rect, clientRect);	
}


void ExportResultDialog::createOrShowGroupBoxes(CRect & clientRect)
{
	int x = 20, y = 88, w = 200, h = 240;
	CRect rect(x, y, x + w, y + h);
	createOrShowFormGroupBox(csvGroupBox, Config::EXPORT_TO_CSV_GROUPBOX_ID, S(L"csv-settings"), rect, clientRect);

	int x2 = x + 5, y2 = y + 15, w2 = 190, h2 = 100;
	CRect rect2(x2, y2, x2 + w2, y2 + h2);
	createOrShowFormGroupBox(csvFieldGroupBox, Config::EXPORT_TO_CSV_FEILD_SET_GROUPBOX_ID, S(L"csv-field-settings"), rect2, clientRect);
	y2 += h2 + 5, h2 = 40;
	rect2 = { x2, y2, x2 + w2, y2 + h2 };
	createOrShowFormGroupBox(csvLineGroupBox, Config::EXPORT_TO_CSV_LINE_SET_GROUPBOX_ID, S(L"csv-line-settings"), rect2, clientRect);
	y2 += h2 + 5, h2 = 40;
	rect2 = { x2, y2, x2 + w2, y2 + h2 };
	createOrShowFormGroupBox(csvCharsetGroupBox, Config::EXPORT_TO_CSV_CHARSET_SET_GROUPBOX_ID, S(L"csv-charset-settings"), rect2, clientRect);

	x += w + 10, h = 100;
	rect = { x, y, x + w, y + h };
	createOrShowFormGroupBox(excelGroupBox, Config::EXPORT_TO_EXCEL_GROUPBOX_ID, S(L"excel-settings"), rect, clientRect);

	y += h + 5, h = 135;
	rect = { x, y, x + w, y + h };
	createOrShowFormGroupBox(sqlGroupBox, Config::EXPORT_TO_SQL_GROUPBOX_ID, S(L"sql-settings"), rect, clientRect);

	x += w + 10, y = y - 100 - 5, h = 240;
	rect = { x, y, x + w, y + h };
	createOrShowFormGroupBox(exportFieldsGroupBox, Config::EXPORT_FEILDS_GROUPBOX_ID, S(L"export-fields-settings"), rect, clientRect); 
}

/**
 * Elecment create or show in CSV options groupbox.
 * 
 * @param clientRect 
 */
void ExportResultDialog::createOrShowCsvSettingsElems(CRect & clientRect)
{
	// cvs options - Fields
	int x = 30, y = 120, w = 100, h = 20;
	CRect rect(x, y, x + w, y + h);
	createOrShowFormLabel(csvFieldTerminatedByLabel, S(L"terminated-by").append(L":"), rect, clientRect, SS_RIGHT, elemFont);
	int x2 = x + w + 5, y2 = y,  w2 = 40, h2 = h;
	CRect rect2(x2, y2, x2 + w2, y2 + h2);

	bool isReadOnly = true;
	createOrShowFormEdit(csvFieldTerminatedByEdit,Config::EXPORT_CSV_FIELD_TERMINAATED_BY_EDIT_ID, L"", L"", rect2, clientRect, ES_LEFT, isReadOnly);

	rect.OffsetRect(0, h + 5);
	createOrShowFormLabel(csvFieldEnclosedByLabel, S(L"enclosed-by").append(L":"), rect, clientRect, SS_RIGHT, elemFont);
	rect2.OffsetRect(0, h + 5);
	createOrShowFormEdit(csvFieldEnclosedByEdit,Config::EXPORT_CSV_FIELD_ENCLOSED_BY_EDIT_ID, L"", L"", rect2, clientRect, ES_LEFT, isReadOnly);

	rect.OffsetRect(0, h + 5);
	createOrShowFormLabel(csvFieldEscapedByLabel, S(L"escaped-by").append(L":"), rect, clientRect, SS_RIGHT, elemFont);
	rect2.OffsetRect(0, h + 5);
	createOrShowFormEdit(csvFieldEscapedByEdit, Config::EXPORT_CSV_FIELD_ESCAPED_BY_EDIT_ID, L"", L"", rect2, clientRect, ES_LEFT, isReadOnly);

	// cvs options - Lines
	y = 203 + 20;
	rect = { x, y, x + w, y + h };
	createOrShowFormLabel(csvLineTerminatedByLabel, S(L"terminated-by").append(L":"), rect, clientRect, SS_RIGHT, elemFont);
	y2 = y;
	rect2 = { x2, y2, x2 + w2, y2 + h2 };
	createOrShowFormEdit(csvLineTerminatedByEdit, Config::EXPORT_CSV_LINE_TERMINAATED_BY_EDIT_ID, L"", L"", rect2, clientRect, ES_LEFT, isReadOnly);

	y += 30 + 15;
	rect = { x, y, x + w, y + h };
	createOrShowFormLabel(csvCharsetLabel, S(L"csv-charset").append(L":"), rect, clientRect, SS_RIGHT, elemFont);
	y2 = y;
	rect2 = { x2, y2, x2 + w2, y2 + h2 };
	createOrShowFormEdit(csvCharsetEdit, Config::EXPORT_CSV_CHARSET_EDIT_ID, L"UTF-8", L"", rect2, clientRect, ES_LEFT, isReadOnly);

	// cvs options - Add column names on top
	int x3 = 25, y3 = 88 + 15 + 100 + 5 + 40 + 20 + 30 , w3 = 200 - 10, h3 = 20;
	CRect rect3 = { x3, y3, x3 + w3, y3 + h3 };
	createOrShowFormCheckBox(csvColumnNameCheckBox, Config::EXPORT_CSV_COLUMN_NAME_CHECKBOX_ID, S(L"add-column-name"), rect3, clientRect);

	bool isEndabled = settingService->getSysInit(L"export_fmt") == L"CSV" ? true : false;
	csvFieldTerminatedByLabel.EnableWindow(isEndabled);
	csvFieldTerminatedByEdit.EnableWindow(isEndabled);
	
	csvFieldEnclosedByLabel.EnableWindow(isEndabled);
	csvFieldEnclosedByEdit.EnableWindow(isEndabled);

	csvFieldEscapedByLabel.EnableWindow(isEndabled);
	csvFieldEscapedByEdit.EnableWindow(isEndabled);

	csvLineTerminatedByLabel.EnableWindow(isEndabled);
	csvLineTerminatedByEdit.EnableWindow(isEndabled);

	csvColumnNameCheckBox.EnableWindow(isEndabled);	
}


void ExportResultDialog::createOrShowExcelSettingsElems(CRect & clientRect)
{
	bool isReadOnly = settingService->getSysInit(L"export_fmt") == L"EXCEL" ? false : true;
	bool isEndabled = !isReadOnly;

	// cvs options - Fields
	int x = 235, y = 120, w = 120, h = 20;
	CRect rect(x, y, x + w, y + h);
	createOrShowFormLabel(excelComlumnMaxSizeLabel, S(L"excel-column-max-size").append(L":"), rect, clientRect, SS_RIGHT, elemFont);
	int x2 = x + w + 5, y2 = y,  w2 = 40, h2 = h;
	CRect rect2(x2, y2, x2 + w2, y2 + h2);
	createOrShowFormEdit(excelComlumnMaxSizeEdit,Config::EXPORT_EXCEL_COLMUMN_MAX_SIZE_EDIT_ID, L"", L"", rect2, clientRect, ES_LEFT, isReadOnly);

	rect.OffsetRect(0, h + 5);
	createOrShowFormLabel(excelDecimalPlacesLabel, S(L"excel-decinmal-places").append(L":"), rect, clientRect, SS_RIGHT, elemFont);
	rect2.OffsetRect(0, h + 5);
	createOrShowFormEdit(excelDecimalPlacesEdit,Config::EXPORT_EXCEL_DECIMAL_PLACES_EDIT_ID, L"", L"", rect2, clientRect, ES_LEFT, isReadOnly);

	excelComlumnMaxSizeLabel.EnableWindow(isEndabled);
	excelComlumnMaxSizeEdit.EnableWindow(isEndabled);

	excelDecimalPlacesLabel.EnableWindow(isEndabled);
	excelDecimalPlacesEdit.EnableWindow(isEndabled);
}


void ExportResultDialog::createOrShowSqlSettingsElems(CRect & clientRect)
{
	bool isEnabled = settingService->getSysInit(L"export_fmt") == L"SQL" ? true : false;

	int x = 260, y = 220, w = 140, h = 20;
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


void ExportResultDialog::createOrShowSelectFieldsElems(CRect & clientRect)
{
	int x = 450, y = 120, w = 180, h = 150;
	CRect rect(x, y, x + w, y + h);
	createOrShowFormListBox(selectFieldsListBox, Config::EXPORT_SELECT_FIELDS_LISTBOX_ID, rect, clientRect);

	y += h + 10, w = 85, h = 30;
	rect = { x, y, x + w, y + h };
	createOrShowFormButton(selectAllButton, Config::EXPORT_SELECT_ALL_FIELDS_BUTTON_ID, S(L"select-all"), rect, clientRect);

	rect.OffsetRect(w + 10, 0);
	createOrShowFormButton(deselectAllButton, Config::EXPORT_DESELECT_ALL_FIELDS_BUTTON_ID, S(L"deselect-all"), rect, clientRect);
}


void ExportResultDialog::createOrShowExportPathElems(CRect & clientRect)
{
	int x = 20, y = 350, w = 80, h = 20;
	CRect rect(x, y, x + w, y + h);
	createOrShowFormLabel(exportPathLabel, S(L"save-to-file").append(L":"), rect, clientRect, SS_LEFT, elemFont);

	x += w + 10, w = clientRect.Width() - x - 60;
	rect = { x, y, x + w, y + h };
	createOrShowFormEdit(exportPathEdit,Config::EXPORT_PATH_EDIT_ID, L"", L"", rect, clientRect, ES_LEFT, false);

	x += w + 20, w = 20;
	rect = { x, y, x + w, y + h };
	createOrShowFormButton(openFileButton, Config::EXPORT_OPEN_FILE_BUTTON_ID, L"...", rect, clientRect);
}


void ExportResultDialog::loadWindow()
{
	if (!isNeedReload) {
		return ;
	}

	isNeedReload = false;
	loadExportFmtElems();
	loadCsvSettingsElems();
	loadExcelSettingsElems();
	loadSqlSettingsElems();
	loadSelectFieldsListBox();
	loadExportPathEdit();
}


void ExportResultDialog::loadExportFmtElems()
{
	std::wstring exportFmt = settingService->getSysInit(L"export_fmt");
	for (auto ptr : radioPtrs) {
		if (ptr->m_hWnd == csvRadio.m_hWnd && exportFmt == L"CSV") {
			ptr->SetCheck(1);
		} else if (ptr->m_hWnd == jsonRadio.m_hWnd && exportFmt == L"JSON") {
			ptr->SetCheck(1);
		} else if (ptr->m_hWnd == htmlRadio.m_hWnd && exportFmt == L"HTML") {
			ptr->SetCheck(1);
		} else if (ptr->m_hWnd == xmlRadio.m_hWnd && exportFmt == L"XML") {
			ptr->SetCheck(1);
		} else if (ptr->m_hWnd == excelXmlRadio.m_hWnd && exportFmt == L"EXCEL") {
			ptr->SetCheck(1);
		} else if (ptr->m_hWnd == sqlRadio.m_hWnd && exportFmt == L"SQL") {
			ptr->SetCheck(1);
		} else {
			ptr->SetCheck(0);
		}
	}
	
}

void ExportResultDialog::loadCsvSettingsElems()
{
	bool isEnabled = getSelExportFmtHwnd() == csvRadio.m_hWnd;

	if (csvFieldTerminatedByEdit.IsWindow()) {
		std::wstring val = settingService->getSysInit(L"csv_field_terminaated_by");
		csvFieldTerminatedByEdit.SetWindowText(val.c_str()); 

		csvFieldTerminatedByLabel.EnableWindow(isEnabled);
		csvFieldTerminatedByEdit.EnableWindow(isEnabled);
	}

	if (csvFieldEnclosedByEdit.IsWindow()) {
		std::wstring val = settingService->getSysInit(L"csv_field_enclosed_by");
		csvFieldEnclosedByEdit.SetWindowText(val.c_str()); 
		
		csvFieldEnclosedByLabel.EnableWindow(isEnabled);
		csvFieldEnclosedByEdit.EnableWindow(isEnabled);
	}

	if (csvFieldEscapedByEdit.IsWindow()) {
		std::wstring val = settingService->getSysInit(L"csv_field_escaped_by");
		csvFieldEscapedByEdit.SetWindowText(val.c_str()); 

		csvFieldEscapedByLabel.EnableWindow(isEnabled);
		csvFieldEscapedByEdit.EnableWindow(isEnabled);
	}

	if (csvLineTerminatedByEdit.IsWindow()) {
		std::wstring val = settingService->getSysInit(L"csv_line_terminaated_by");
		csvLineTerminatedByEdit.SetWindowText(val.c_str()); 
		
		csvLineTerminatedByLabel.EnableWindow(isEnabled);
		csvLineTerminatedByEdit.EnableWindow(isEnabled);
	}

	if (csvCharsetEdit.IsWindow()) {
		std::wstring val = settingService->getSysInit(L"csv_charset");
		csvCharsetEdit.SetWindowText(val.c_str()); 
		
		csvCharsetLabel.EnableWindow(isEnabled);
		csvCharsetEdit.EnableWindow(isEnabled);
	}

	if (csvColumnNameCheckBox.IsWindow()) {
		std::wstring val = settingService->getSysInit(L"csv_column_name");
		if (val == L"true") {
			csvColumnNameCheckBox.SetCheck(1);
		}else {
			csvColumnNameCheckBox.SetCheck(0);
		}
		csvColumnNameCheckBox.EnableWindow(isEnabled);
	}
}


void ExportResultDialog::loadExcelSettingsElems()
{
	bool isEnabled = getSelExportFmtHwnd() == excelXmlRadio.m_hWnd;
	if (excelComlumnMaxSizeEdit.IsWindow()) {
		std::wstring val = settingService->getSysInit(L"excel_colmumn_max_size");
		excelComlumnMaxSizeEdit.SetWindowText(val.c_str());
		excelComlumnMaxSizeEdit.SetReadOnly(!isEnabled);

		excelComlumnMaxSizeLabel.EnableWindow(isEnabled);
		excelComlumnMaxSizeEdit.EnableWindow(isEnabled);
	}

	if (excelDecimalPlacesEdit.IsWindow()) {
		std::wstring val = settingService->getSysInit(L"excel_decimal_places");
		excelDecimalPlacesEdit.SetWindowText(val.c_str());
		excelDecimalPlacesEdit.SetReadOnly(!isEnabled);

		excelDecimalPlacesLabel.EnableWindow(isEnabled);
		excelDecimalPlacesEdit.EnableWindow(isEnabled);
	}
}


void ExportResultDialog::loadSqlSettingsElems()
{
	bool isEnabled = getSelExportFmtHwnd() == sqlRadio.m_hWnd;

	std::wstring sqlExport = settingService->getSysInit(L"sql_export");
	for (auto ptr : sqlRadioPtrs) {
		if (ptr->m_hWnd == structureOnlyRadio.m_hWnd && sqlExport == L"structure-only") {
			ptr->SetCheck(1);
		} else if (ptr->m_hWnd == dataOnlyRadio.m_hWnd && sqlExport == L"data-only") {
			ptr->SetCheck(1);
		} else if (ptr->m_hWnd == structurAndDataRadio.m_hWnd && sqlExport == L"structure-and-data") {
			ptr->SetCheck(1);
		} else {
			ptr->SetCheck(0);
		}
		ptr->EnableWindow(isEnabled);
	}
}


void ExportResultDialog::loadSelectFieldsListBox()
{
	selectFieldsListBox.ResetContent();
	auto columns = adapter->getRuntimeColumns();
	if (columns.empty()) {
		return;
	}
	for (auto column : columns) {
		selectFieldsListBox.AddString(column.c_str());
	}
	selectFieldsListBox.SelItemRange(true, 0, selectFieldsListBox.GetCount()-1);

}


void ExportResultDialog::loadExportPathEdit()
{
	if (!exportPathEdit.IsWindow()) {
		return ;
	}

	HWND hwnd = getSelExportFmtHwnd();
	CString str;
	exportPathEdit.GetWindowText(str);
	std::wstring exportPath = str.GetString();
	if (str.IsEmpty()) {
		exportPath = settingService->getSysInit(L"export_path"); 		
	} else if (hwnd == csvRadio.m_hWnd) {
		exportPath = exportResultService->changeExportPathExt(exportPath, L"csv");
	} else if (hwnd == jsonRadio.m_hWnd) {
		exportPath = exportResultService->changeExportPathExt(exportPath, L"json");
	} else if (hwnd == htmlRadio.m_hWnd) {
		exportPath = exportResultService->changeExportPathExt(exportPath, L"html");
	} else if (hwnd == xmlRadio.m_hWnd) {
		exportPath = exportResultService->changeExportPathExt(exportPath, L"xml");
	} else if (hwnd == excelXmlRadio.m_hWnd) {
		exportPath = exportResultService->changeExportPathExt(exportPath, L"xls"); 
	} else if (hwnd == sqlRadio.m_hWnd) {
		exportPath = exportResultService->changeExportPathExt(exportPath, L"sql");
	}

	exportPathEdit.SetWindowText(exportPath.c_str());
}

/**
 * Get the window hadler(HWND) of selected export format radio.
 * 
 * @return selected radio hwnd
 */
HWND ExportResultDialog::getSelExportFmtHwnd()
{
	HWND selHwnd = nullptr;
	std::for_each(radioPtrs.begin(), radioPtrs.end(), [&selHwnd](CButton * ptr) {
		int checked = ptr->GetCheck();
		if (ptr->GetCheck()) {
			selHwnd = ptr->m_hWnd;
		}
	});
	return selHwnd;
}


HWND ExportResultDialog::getSelSqlRadioHwnd()
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
 * Get export to csv params.
 * 
 * @param exportCsvParam [out] the return params
 * @return 
 */
bool ExportResultDialog::getExportCsvParams(ExportCsvParams & params)
{
	CString str;
	csvFieldTerminatedByEdit.GetWindowText(str);
	if (str.IsEmpty()) {
		QPopAnimate::error(m_hWnd, S(L"terminated-by-error"));
		csvFieldTerminatedByEdit.SetFocus();
		return false;
	}
	params.csvFieldTerminatedBy = str.GetString();

	csvFieldEnclosedByEdit.GetWindowText(str);
	if (str.IsEmpty()) {
		QPopAnimate::error(m_hWnd, S(L"enclosed-by-error"));
		csvFieldEnclosedByEdit.SetFocus();
		return false;
	}
	params.csvFieldEnclosedBy = str.GetString();

	csvFieldEscapedByEdit.GetWindowText(str);
	if (str.IsEmpty()) {
		QPopAnimate::error(m_hWnd, S(L"escaped-by-error"));
		csvFieldEscapedByEdit.SetFocus();
		return false;
	}
	params.csvFieldEscapedBy = str.GetString();

	csvLineTerminatedByEdit.GetWindowText(str);
	if (str.IsEmpty()) {
		QPopAnimate::error(m_hWnd, S(L"line-terminated-by-error"));
		csvLineTerminatedByEdit.SetFocus();
		return false;
	}
	std::wstring lineTerminated(str.GetString());
	lineTerminated = StringUtil::replace(lineTerminated, L"\\r\\n", L"\r\n");
	lineTerminated = StringUtil::replace(lineTerminated, L"\\r", L"\r");
	lineTerminated = StringUtil::replace(lineTerminated, L"\\n", L"\n");
	params.csvLineTerminatedBy = lineTerminated;

	params.hasColumnOnTop = csvColumnNameCheckBox.GetCheck() ? true : false;

	return true;
}

/**
 * Get export to Excel XML params.
 * 
 * @param params [out] the return params
 * @return 
 */
bool ExportResultDialog::getExportExcelParams(ExportExcelParams & params)
{
	CString str;
	excelComlumnMaxSizeEdit.GetWindowText(str);
	if (str.IsEmpty()) {
		QPopAnimate::error(m_hWnd, S(L"excel-column-max-size-error"));
		excelComlumnMaxSizeEdit.SetFocus();
		return false;
	}
	params.excelComlumnMaxSize = std::stoi(str.GetString());

	excelDecimalPlacesEdit.GetWindowText(str);
	if (str.IsEmpty()) {
		QPopAnimate::error(m_hWnd, S(L"excel-decinmal-places-error"));
		excelDecimalPlacesEdit.SetFocus();
		return false;
	}
	params.excelDecimalPlaces = std::stoi(str.GetString());

	return true;
}

/**
 * Get export to SQL params.
 * 
 * @param params [out] the return params
 * @return 
 */
bool ExportResultDialog::getExportSqlParams(ExportSqlParams & params)
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

/**
 * Get export to SQL params.
 * 
 * @param params [out] the return params
 * @return 
 */
bool ExportResultDialog::getExportSelectedColumns(ExportSelectedColumns & params)
{
	params.clear();
	if (selectFieldsListBox.GetCount() == 0 || selectFieldsListBox.GetSelCount() == 0) {
		QPopAnimate::error(m_hWnd, S(L"selected-fields-error"));
		selectFieldsListBox.SetFocus();
		return false;
	}
	int selIndexes[1024];
	int n = selectFieldsListBox.GetSelItems(1024, selIndexes);
	
	for (int i = 0; i < n; i++) {
		int selIndex = selIndexes[i];
		std::wstring fieldName = adapter->getRuntimeColumns().at(selIndex);
		params.push_back(fieldName);
	}
	return true;
}

/**
 * Get export to SQL params.
 * 
 * @param params [out] the return params
 * @return 
 */
bool ExportResultDialog::getExportPath(std::wstring & exportPath)
{
	CString str;
	exportPathEdit.GetWindowText(str);
	if (str.IsEmpty()) {
		QPopAnimate::error(m_hWnd, S(L"export-path-error"));
		exportPathEdit.SetFocus();
		return false;
	}
	exportPath = str.GetString();
	return true;
}


LRESULT ExportResultDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	linePen.CreatePen(PS_SOLID, 1, lineColor);
	elemFont = FT(L"elem-size");

	radioPtrs.push_back(&csvRadio);
	radioPtrs.push_back(&jsonRadio);
	radioPtrs.push_back(&xmlRadio);
	radioPtrs.push_back(&htmlRadio);
	radioPtrs.push_back(&excelXmlRadio); 
	radioPtrs.push_back(&sqlRadio);

	sqlRadioPtrs.push_back(&structureOnlyRadio);
	sqlRadioPtrs.push_back(&dataOnlyRadio);
	sqlRadioPtrs.push_back(&structurAndDataRadio);

	QDialog::OnInitDialog(uMsg, wParam, lParam, bHandled); 
	return 0;
}

LRESULT ExportResultDialog::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	QDialog::OnDestroy(uMsg, wParam, lParam, bHandled);

	if (!linePen.IsNull()) linePen.DeleteObject();
	if (elemFont) ::DeleteObject(elemFont);

	if (exportImage.IsWindow()) exportImage.DestroyWindow();
	if (exportTextLabel.IsWindow()) exportTextLabel.DestroyWindow();
	
	if (csvRadio.IsWindow()) csvRadio.DestroyWindow();
	if (jsonRadio.IsWindow()) jsonRadio.DestroyWindow();
	if (htmlRadio.IsWindow()) htmlRadio.DestroyWindow();
	if (xmlRadio.IsWindow()) xmlRadio.DestroyWindow();
	if (excelXmlRadio.IsWindow()) excelXmlRadio.DestroyWindow();
	if (sqlRadio.IsWindow()) sqlRadio.DestroyWindow();
	radioPtrs.clear();

	if (csvGroupBox.IsWindow()) csvGroupBox.DestroyWindow();
	if (csvFieldGroupBox.IsWindow()) csvFieldGroupBox.DestroyWindow();
	if (csvLineGroupBox.IsWindow()) csvLineGroupBox.DestroyWindow();
	if (csvCharsetGroupBox.IsWindow()) csvCharsetGroupBox.DestroyWindow();
	if (excelGroupBox.IsWindow()) excelGroupBox.DestroyWindow();
	if (sqlGroupBox.IsWindow()) sqlGroupBox.DestroyWindow();
	if (exportFieldsGroupBox.IsWindow()) exportFieldsGroupBox.DestroyWindow();

	if (csvFieldTerminatedByLabel.IsWindow()) csvFieldTerminatedByLabel.DestroyWindow();
	if (csvFieldTerminatedByEdit.IsWindow()) csvFieldTerminatedByEdit.DestroyWindow();
	if (csvFieldEnclosedByLabel.IsWindow()) csvFieldEnclosedByLabel.DestroyWindow();
	if (csvFieldEnclosedByEdit.IsWindow()) csvFieldEnclosedByEdit.DestroyWindow();
	if (csvFieldEscapedByLabel.IsWindow()) csvFieldEscapedByLabel.DestroyWindow();
	if (csvFieldEscapedByEdit.IsWindow()) csvFieldEscapedByEdit.DestroyWindow();
	if (csvLineTerminatedByLabel.IsWindow()) csvLineTerminatedByLabel.DestroyWindow();
	if (csvLineTerminatedByEdit.IsWindow()) csvLineTerminatedByEdit.DestroyWindow();
	if (csvCharsetLabel.IsWindow()) csvCharsetLabel.DestroyWindow();
	if (csvCharsetEdit.IsWindow()) csvCharsetEdit.DestroyWindow();

	if (csvColumnNameCheckBox.IsWindow()) csvColumnNameCheckBox.DestroyWindow();

	if (excelComlumnMaxSizeLabel.IsWindow()) excelComlumnMaxSizeLabel.DestroyWindow();
	if (excelComlumnMaxSizeEdit.IsWindow()) excelComlumnMaxSizeEdit.DestroyWindow();
	if (excelDecimalPlacesLabel.IsWindow()) excelDecimalPlacesLabel.DestroyWindow();
	if (excelDecimalPlacesEdit.IsWindow()) excelDecimalPlacesEdit.DestroyWindow();

	if (structureOnlyRadio.IsWindow()) structureOnlyRadio.DestroyWindow();
	if (dataOnlyRadio.IsWindow()) dataOnlyRadio.DestroyWindow();
	if (structurAndDataRadio.IsWindow()) structurAndDataRadio.DestroyWindow();
	sqlRadioPtrs.clear();

	if (selectFieldsListBox.IsWindow()) selectFieldsListBox.DestroyWindow();
	if (selectAllButton.IsWindow()) selectAllButton.DestroyWindow();
	if (deselectAllButton.IsWindow()) deselectAllButton.DestroyWindow();
	if (exportPathLabel.IsWindow()) exportPathLabel.DestroyWindow();
	if (exportPathEdit.IsWindow()) exportPathEdit.DestroyWindow();
	if (openFileButton.IsWindow()) openFileButton.DestroyWindow();
	return 0;
}


LRESULT ExportResultDialog::OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// invisible then return 
	if (!wParam) { 
		return 0;
	}
	loadWindow();
	return 0;
}

void ExportResultDialog::paintItem(CDC &dc, CRect &paintRect)
{
	int x = 20, y = 15 + 32 + 10, w = paintRect.Width() - 2 * 20, h = 1;

	HPEN oldPen = dc.SelectPen(linePen);
	dc.MoveTo(x, y);
	dc.LineTo(x + w, y);

	y= 340;
	dc.MoveTo(x, y);
	dc.LineTo(x + w, y);

	y= 380;
	dc.MoveTo(x, y);
	dc.LineTo(x + w, y);
	dc.SelectPen(oldPen);
}

void ExportResultDialog::OnClickExportFmtRadios(UINT uNotifyCode, int nID, HWND hwnd)
{
	HWND selHwnd = nullptr;
	std::for_each(radioPtrs.begin(), radioPtrs.end(), [&hwnd, &selHwnd](CButton * ptr) {
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

	// reload the elements value and set enabled/disabled 
	loadCsvSettingsElems();
	loadExcelSettingsElems();
	loadSqlSettingsElems();
	loadExportPathEdit();
}

void ExportResultDialog::OnClickSelectAllFieldsButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	if (selectFieldsListBox.GetCount() == 0) {
		return;
	}

	selectFieldsListBox.SelItemRange(true, 0, selectFieldsListBox.GetCount() - 1);
}

void ExportResultDialog::OnClickDeselectAllFieldsButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	if (selectFieldsListBox.GetCount() == 0) {
		return;
	}

	selectFieldsListBox.SelItemRange(false, 0, selectFieldsListBox.GetCount() - 1);
}

void ExportResultDialog::OnClickYesButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	ExportSelectedColumns selectedColumns;
	if (!getExportSelectedColumns(selectedColumns) || selectedColumns.empty()) {
		return ;
	}

	std::wstring exportPath;
	if (!getExportPath(exportPath)) {
		return ;
	}

	
	HWND selHwnd = getSelExportFmtHwnd();
	DataList datas = adapter->getRuntimeDatas();
	Columns columns = adapter->getRuntimeColumns();
	if (selHwnd == csvRadio.m_hWnd) {
		ExportCsvParams csvParams;
		if (!getExportCsvParams(csvParams)) {
			return ;
		}
		exportResultService->exportToCsv(exportPath, columns, selectedColumns, datas, csvParams);
	} else if (selHwnd == jsonRadio.m_hWnd) { 
		exportResultService->exportToJson(exportPath, columns, selectedColumns, datas);
	} else if (selHwnd == htmlRadio.m_hWnd) { 
		exportResultService->exportToHtml(exportPath, columns, selectedColumns, datas);
	} else if (selHwnd == xmlRadio.m_hWnd) { 
		exportResultService->exportToXml(exportPath, columns, selectedColumns, datas);
	} else if (selHwnd == excelXmlRadio.m_hWnd) {
		ExportExcelParams excelParams;
		if (!getExportExcelParams(excelParams)) {
			return ;
		}
		exportResultService->exportToExcelXml(exportPath, columns, selectedColumns, datas, excelParams);
	} else if (selHwnd == sqlRadio.m_hWnd) {
		ExportSqlParams sqlParams;
		if (!getExportSqlParams(sqlParams)) {
			
			return ;
		}
		UserTableStrings tbls = adapter->getRuntimeTables();
		if (tbls.empty() || tbls.size() > 1) {
			QPopAnimate::error(m_hWnd, S(L"sql-notsupport-multitable-query-error"));
			sqlRadio.SetFocus();
			return ;
		}

		UserTable userTable = adapter->getRuntimeUserTable(tbls.at(0));
		exportResultService->exportToSql(exportPath, userTable, columns, selectedColumns, datas, sqlParams);
	}


	EndDialog(Config::QDIALOG_YES_BUTTON_ID);
}

void ExportResultDialog::OnClickNoButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	EndDialog(Config::QDIALOG_NO_BUTTON_ID);
}
