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
#include "utils/FontUtil.h"

ExportResultDialog::ExportResultDialog(HWND parentHwnd, ResultListPageAdapter * adapter)
{
	ATLASSERT(parentHwnd != nullptr && adapter != nullptr);
	this->parentHwnd = parentHwnd;
	this->adapter = adapter;
	this->caption = S(L"export-result-as");
	setFormSize(670, 500);
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
	createOrShowCsvOptionsElems(clientRect);
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
	int x = 20, y = 88, w = 200, h = 350;
	CRect rect(x, y, x + w, y + h);
	createOrShowFormGroupBox(csvGroupBox, Config::EXPORT_TO_CSV_GROUPBOX_ID, S(L"csv-option"), rect, clientRect);

	int x2 = x + 5, y2 = y + 15, w2 = 190, h2 = 100;
	CRect rect2(x2, y2, x2 + w2, y2 + h2);
	createOrShowFormGroupBox(csvFieldGroupBox, Config::EXPORT_TO_CSV_FEILD_SET_GROUPBOX_ID, S(L"csv-field-option"), rect2, clientRect);
	y2 += h2 + 5, h2 = 40;
	rect2 = { x2, y2, x2 + w2, y2 + h2 };
	createOrShowFormGroupBox(csvLineGroupBox, Config::EXPORT_TO_CSV_LINE_SET_GROUPBOX_ID, S(L"csv-line-option"), rect2, clientRect);

	x += w + 10, h = 150;
	rect = { x, y, x + w, y + h };
	createOrShowFormGroupBox(excelGroupBox, Config::EXPORT_TO_EXCEL_GROUPBOX_ID, S(L"excel-option"), rect, clientRect);

	y += h + 5, h = 200;
	rect = { x, y, x + w, y + h };
	createOrShowFormGroupBox(sqlGroupBox, Config::EXPORT_TO_SQL_GROUPBOX_ID, S(L"sql-option"), rect, clientRect);

	x += w + 10, y = y - 150 - 5, h = 350;
	rect = { x, y, x + w, y + h };
	createOrShowFormGroupBox(exportFieldsGroupBox, Config::EXPORT_FEILDS_GROUPBOX_ID, S(L"export-fields-option"), rect, clientRect); 
}

/**
 * Elecment create or show in CSV options groupbox.
 * 
 * @param clientRect 
 */
void ExportResultDialog::createOrShowCsvOptionsElems(CRect & clientRect)
{
	// cvs options - Fields
	int x = 30, y = 120, w = 100, h = 20;
	CRect rect(x, y, x + w, y + h);
	createOrShowFormLabel(csvFieldTerminatedByLabel, S(L"terminated-by").append(L":"), rect, clientRect, SS_RIGHT, elemFont);
	int x2 = x + w + 5, y2 = y,  w2 = 30, h2 = h;
	CRect rect2(x2, y2, x2 + w2, y2 + h2);
	createOrShowFormEdit(csvFieldTerminatedByEdit,Config::EXPORT_CSV_FIELD_TERMINAATED_BY_EDIT_ID, L"", L"", rect2, clientRect, ES_LEFT, true);

	rect.OffsetRect(0, h + 5);
	createOrShowFormLabel(csvFieldEnclosedByLabel, S(L"enclosed-by").append(L":"), rect, clientRect, SS_RIGHT, elemFont);
	rect2.OffsetRect(0, h + 5);
	createOrShowFormEdit(csvFieldEnclosedByEdit,Config::EXPORT_CSV_FIELD_ENCLOSED_BY_EDIT_ID, L"", L"", rect2, clientRect, ES_LEFT, true);

	rect.OffsetRect(0, h + 5);
	createOrShowFormLabel(csvFieldEscapedByLabel, S(L"escaped-by").append(L":"), rect, clientRect, SS_RIGHT, elemFont);
	rect2.OffsetRect(0, h + 5);
	createOrShowFormEdit(csvFieldEscapedByEdit, Config::EXPORT_CSV_FIELD_ESCAPED_BY_EDIT_ID, L"", L"", rect2, clientRect, ES_LEFT, true);

	// cvs options - Lines
	y = 203 + 20;
	rect = { x, y, x + w, y + h };
	createOrShowFormLabel(csvLineTerminatedByLabel, S(L"terminated-by").append(L":"), rect, clientRect, SS_RIGHT, elemFont);
	y2 = y;
	rect2 = { x2, y2, x2 + w2, y2 + h2 };
	createOrShowFormEdit(csvLineTerminatedByEdit, Config::EXPORT_CSV_LINE_TERMINAATED_BY_EDIT_ID, L"", L"", rect2, clientRect, ES_LEFT, true);

	// cvs options - Add column names on top
	int x3 = 25, y3 = 88 + 15 + 100 + 5 + 40, w3 = 200 - 10, h3 = 20;
	CRect rect3 = { x3, y3, x3 + w3, y3 + h3 };
	createOrShowFormCheckBox(columnNameCheckBox, Config::EXPORT_CSV_COLUMN_NAME_CHECKBOX_ID, S(L"add-column-name"), rect3, clientRect);
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

	return 0;
}

void ExportResultDialog::paintItem(CDC &dc, CRect &paintRect)
{
	int x = 20, y = 15 + 32 + 10, w = paintRect.Width() - 2 * 20, h = 1;

	HPEN oldPen = dc.SelectPen(linePen);
	dc.MoveTo(x, y);
	dc.LineTo(x + w, y);
	dc.SelectPen(oldPen);
}

void ExportResultDialog::OnClickRadios(UINT uNotifyCode, int nID, HWND hwnd)
{
	std::for_each(radioPtrs.begin(), radioPtrs.end(), [&hwnd](CButton * ptr) {
		int checked = ptr->GetCheck();
		if (ptr->m_hWnd == hwnd) {
			if (!checked) {
				ptr->SetCheck(!checked);
			}
		}else {
			ptr->SetCheck(0);
		}
	});
}
