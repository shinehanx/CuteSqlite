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

 * @file   ResultPropertiesPage.cpp
 * @brief  Execute sql statement and show the info of query result
 * 
 * @author Xuehan Qin
 * @date   2023-11-06
 *********************************************************************/

#include "stdafx.h"
#include "TablePropertiesPage.h"
#include "ui/common/QWinCreater.h"
#include "core/common/Lang.h"
#include "common/AppContext.h"

void TablePropertiesPage::setup(QueryPageSupplier * supplier)
{
	this->supplier = supplier;
}

void TablePropertiesPage::clear()
{
	titleEdit.Clear();
}

void TablePropertiesPage::createOrShowUI()
{
	QPage::createOrShowUI();

	CRect clientRect;
	GetClientRect(clientRect);
	createOrShowTitleElems(clientRect);
	createOrShowColumnsSectionElems(clientRect);
	createOrShowIndexesSectionElems(clientRect);
	createOrShowDdlSectionElems(clientRect);
}

void TablePropertiesPage::loadWindow()
{
	QPage::loadWindow();
}

void TablePropertiesPage::createOrShowTitleElems(CRect & clientRect)
{
	int x = 20, y = 15, w = 32, h = 32;
	CRect rect(x, y, x + w, y + h);
	createOrShowImage(titleImage, rect, clientRect);

	rect.OffsetRect(32 + 10, 5);
	rect.right = rect.left + 300;

	UserDb userDb = databaseService->getUserDb(supplier->getRuntimeUserDbId());
	std::wstring title = userDb.name;
	title.append(L".").append(supplier->getRuntimeTblName());
	createOrShowEdit(titleEdit, 0, title, rect, clientRect, 0);
	titleEdit.BringWindowToTop();
}

void TablePropertiesPage::createOrShowColumnsSectionElems(CRect & clientRect)
{
	CRect rcTop = GdiPlusUtil::GetWindowRelativeRect(titleEdit.m_hWnd);
	int x = 20, y = rcTop.bottom + 30, w = 300, h = 20;
	CRect rect(x, y, x + w, y + h);
	// section
	createOrShowEdit(columnsSection, 0, S(L"columns"), rect, clientRect, 0);

	// header
	rect.OffsetRect(0, 20);
	rect.right = rect.left + 100;
	createOrShowEdit(primaryKeyHeader, 0, L"", rect, clientRect, 0);

	rect.OffsetRect(rect.Width() + 2, 0);
	rect.right = rect.left + 200;
	createOrShowEdit(columnNameHeader, 0, S(L"columns"), rect, clientRect, 0);

	rect.OffsetRect(rect.Width() + 2, 0);
	rect.right = rect.left + 400;
	createOrShowEdit(columnTypeHeader, 0, S(L"data-type"), rect, clientRect, 0);

	// columns
	createOrShowColumnsList(clientRect);
}

void TablePropertiesPage::createOrShowColumnsList(CRect & clientRect)
{
	CRect rcHeader0 = GdiPlusUtil::GetWindowRelativeRect(primaryKeyHeader.m_hWnd);
	CRect rcHeader1 = GdiPlusUtil::GetWindowRelativeRect(columnNameHeader.m_hWnd);
	CRect rcHeader2 = GdiPlusUtil::GetWindowRelativeRect(columnTypeHeader.m_hWnd);
	ColumnInfoList list = tableService->getUserColumns(supplier->getRuntimeUserDbId(), supplier->getRuntimeTblName());
	int i = 0;
	int n = static_cast<int>(columnNamePtrs.size());
	UserDb userDb = databaseService->getUserDb(supplier->getRuntimeUserDbId());
	std::wstring section = S(L"columns");
	section.append(L" (").append(std::to_wstring(n)).append(L")");
	columnsSection.SetWindowText(section.c_str());

	for (ColumnInfo item : list) {
		std::wstring field = item.name;
		std::wstring type = item.type;
		type.append(L", ").append(item.notnull ? L"NOT NULL" : L"NULL");
		
		rcHeader0.OffsetRect(0, rcHeader0.Height() + 2);
		rcHeader1.OffsetRect(0, rcHeader1.Height() + 2);
		rcHeader2.OffsetRect(0, rcHeader2.Height() + 2);
		if (i < n && columnNamePtrs.at(i) && columnNamePtrs.at(i)->IsWindow()) {
			auto ptr = primaryKeyPtrs.at(i);
			ptr->MoveWindow(rcHeader0);	

			ptr = columnNamePtrs.at(i);
			ptr->MoveWindow(rcHeader1);	

			ptr = columnTypePtrs.at(i);
			ptr->MoveWindow(rcHeader2);	
		} else {
			auto ptr0 = new CEdit();
			createOrShowEdit(*ptr0, 0, L"", rcHeader0, clientRect, 0);
			primaryKeyPtrs.push_back(ptr0);

			auto ptr1 = new CEdit();
			createOrShowEdit(*ptr1, 0, field, rcHeader1, clientRect, 0);
			columnNamePtrs.push_back(ptr1);

			auto ptr2 = new CEdit();
			createOrShowEdit(*ptr2, 0, type, rcHeader2, clientRect, 0);
			columnTypePtrs.push_back(ptr2);
		}

		i++;
	}
}


void TablePropertiesPage::createOrShowIndexesSectionElems(CRect & clientRect)
{
	int n = static_cast<int>(primaryKeyPtrs.size());
	CRect rcTop = GdiPlusUtil::GetWindowRelativeRect(primaryKeyPtrs.at(n-1)->m_hWnd);
	int x = 20, y = rcTop.bottom + 30, w = 300, h = 20;
	CRect rect(x, y, x + w, y + h);
	// section
	createOrShowEdit(indexesSection, 0, S(L"indexes"), rect, clientRect, 0);

	// header
	rect.OffsetRect(0, 20);
	rect.right = rect.left + 50;
	createOrShowEdit(idxPrimaryHeader, 0, L"", rect, clientRect, 0);

	rect.OffsetRect(rect.Width() + 2, 0);
	rect.right = rect.left + 150;
	createOrShowEdit(idxNameHeader, 0, S(L"index-name"), rect, clientRect, 0);

	rect.OffsetRect(rect.Width() + 2, 0);
	rect.right = rect.left + 250;
	createOrShowEdit(idxColumnsHeader, 0, S(L"columns"), rect, clientRect, 0);

	rect.OffsetRect(rect.Width() + 2, 0);
	rect.right = rect.left + 250;
	createOrShowEdit(idxTypeHeader, 0, S(L"index-type"), rect, clientRect, 0);

	createOrShowIndexesList(clientRect);
}

void TablePropertiesPage::createOrShowIndexesList(CRect & clientRect)
{
	CRect rcHeader0 = GdiPlusUtil::GetWindowRelativeRect(idxPrimaryHeader.m_hWnd);
	CRect rcHeader1 = GdiPlusUtil::GetWindowRelativeRect(idxNameHeader.m_hWnd);
	CRect rcHeader2 = GdiPlusUtil::GetWindowRelativeRect(idxColumnsHeader.m_hWnd);
	CRect rcHeader3 = GdiPlusUtil::GetWindowRelativeRect(idxTypeHeader.m_hWnd);
	IndexInfoList list = tableService->getIndexInfoList(supplier->getRuntimeUserDbId(), supplier->getRuntimeTblName());
	int i = 0;
	int n = static_cast<int>(idxTypePtrs.size());
	UserDb userDb = databaseService->getUserDb(supplier->getRuntimeUserDbId());
	std::wstring section = S(L"indexes");
	section.append(L" (").append(std::to_wstring(n)).append(L")");
	indexesSection.SetWindowText(section.c_str());

	for (auto & item : list) {
		std::wstring & name = item.name;
		std::wstring & columns = item.columns;
		std::wstring & type = item.type;		
		
		rcHeader0.OffsetRect(0, rcHeader0.Height() + 2);
		rcHeader1.OffsetRect(0, rcHeader1.Height() + 2);
		rcHeader2.OffsetRect(0, rcHeader2.Height() + 2);
		rcHeader3.OffsetRect(0, rcHeader3.Height() + 2);
		if (i < n && idxTypePtrs.at(i) && idxTypePtrs.at(i)->IsWindow()) {
			auto ptr = idxPrimaryPtrs.at(i);
			ptr->MoveWindow(rcHeader0);	

			ptr = idxNamePtrs.at(i);
			ptr->MoveWindow(rcHeader1);	

			ptr = idxColumnsPtrs.at(i);
			ptr->MoveWindow(rcHeader2);	

			ptr = idxTypePtrs.at(i);
			ptr->MoveWindow(rcHeader3);	
		} else {
			auto ptr0 = new CEdit();
			createOrShowEdit(*ptr0, 0, L"", rcHeader0, clientRect, 0);
			idxPrimaryPtrs.push_back(ptr0);

			auto ptr1 = new CEdit();
			createOrShowEdit(*ptr1, 0, name, rcHeader1, clientRect, 0);
			idxNamePtrs.push_back(ptr1);

			auto ptr2 = new CEdit();
			createOrShowEdit(*ptr2, 0, columns, rcHeader2, clientRect, 0);
			idxColumnsPtrs.push_back(ptr2);

			auto ptr3 = new CEdit();
			createOrShowEdit(*ptr3, 0, type, rcHeader3, clientRect, 0);
			idxTypePtrs.push_back(ptr3);
		}

		i++;
	}
}

void TablePropertiesPage::createOrShowDdlSectionElems(CRect & clientRect)
{
	int n = static_cast<int>(idxPrimaryPtrs.size());
	CRect rcTop = GdiPlusUtil::GetWindowRelativeRect(idxPrimaryPtrs.at(n-1)->m_hWnd);
	int x = 20, y = rcTop.bottom + 30, w = 300, h = 20;
	CRect rect(x, y, x + w, y + h);
	// section
	createOrShowEdit(ddlSection, 0, L"DDL", rect, clientRect, 0);

	// header
	rect.OffsetRect(0, 20);
	rect.right = rect.left + 700;
	createOrShowEdit(ddlHeader, 0, L"CREATE TABLE", rect, clientRect, 0);

	rect.OffsetRect(0, rect.Height() + 10);
	rect.bottom = rect.top + 700;
	UserTable userTable = tableService->getUserTable(supplier->getRuntimeUserDbId(), supplier->getRuntimeTblName());

	std::wstring sql = StringUtil::replace(userTable.sql, L"\r\n", L"\n");
	sql = StringUtil::replace(sql, L"\n", L"\r\n");
	createOrShowEdit(ddlContent, 0, sql, rect, clientRect, ES_MULTILINE | ES_AUTOVSCROLL);
}

void TablePropertiesPage::createOrShowEdit(WTL::CEdit & win, UINT id, std::wstring text, CRect rect, CRect &clientRect, DWORD exStyle)
{
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_VISIBLE |WS_TABSTOP;
		if (exStyle) {
			dwStyle |= exStyle;
		}
		win.Create(m_hWnd, rect, text.c_str(), dwStyle , 0, id);
		win.SetReadOnly(TRUE);
		win.SetWindowText(text.c_str());

		return;
	} else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
		win.SetWindowText(text.c_str());
	}
}

void TablePropertiesPage::createOrShowImage(QStaticImage &win, CRect & rect, CRect & clientRect)
{
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_VISIBLE |WS_CLIPCHILDREN  | WS_CLIPSIBLINGS | SS_NOTIFY; // SS_NOTIFY - 表示static接受点击事件
		std::wstring imgDir = ResourceUtil::getProductImagesDir();
		std::wstring imgPath = imgDir + L"database\\page\\table-properties.png";
		win.load(imgPath.c_str(), BI_PNG, true);
		win.Create(m_hWnd, rect, L"", dwStyle , 0);
		return;
	} else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
	}
}

void TablePropertiesPage::paintItem(CDC & dc, CRect & paintRect)
{
	dc.FillRect(paintRect, bkgColor);

	int x = 20, y = 15 + 32 + 10, w = paintRect.Width() - 2 * 20, h = 1;

	HPEN oldPen = dc.SelectPen(linePen);
	dc.MoveTo(x, y);
	dc.LineTo(x + w, y);
	dc.SelectPen(oldPen);
}

int TablePropertiesPage::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	bool ret = QPage::OnCreate(lpCreateStruct);
	linePen.CreatePen(PS_SOLID, 1, lineColor);
	textFont = FT(L"form-text-size");
	titleFont = FTB(L"properites-title-font", true);
	sectionFont = FTB(L"properites-section-font", true);
	headerBrush = ::CreateSolidBrush(headerBkgColor);
	doubleRowBrush = ::CreateSolidBrush(doubleRowColor);

	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_EXEC_SQL_RESULT_MESSAGE_ID);
	return ret;
}

int TablePropertiesPage::OnDestroy()
{
	bool ret = QPage::OnDestroy();
	if (!linePen.IsNull()) linePen.DeleteObject();
	if (textFont) ::DeleteObject(textFont);
	if (titleFont) ::DeleteObject(titleFont);
	if (sectionFont) ::DeleteObject(sectionFont);
	if (headerBrush) ::DeleteObject(headerBrush);
	if (doubleRowBrush) ::DeleteObject(doubleRowBrush);


	if (titleEdit.IsWindow()) titleEdit.DestroyWindow();
	return ret;
}


HBRUSH TablePropertiesPage::OnCtlColorStatic(HDC hdc, HWND hwnd)
{
	size_t nSelItem = -1;
	if (hwnd == titleEdit.m_hWnd) {
		::SetBkColor(hdc, bkgColor);
		::SelectObject(hdc, titleFont);
		return AtlGetStockBrush(WHITE_BRUSH);
	} else if (hwnd == columnsSection.m_hWnd 
		|| hwnd == indexesSection.m_hWnd
		|| hwnd == ddlSection.m_hWnd) {
		::SetBkColor(hdc, bkgColor);
		::SelectObject(hdc, sectionFont);
		return AtlGetStockBrush(WHITE_BRUSH);
	} else if (hwnd == primaryKeyHeader.m_hWnd 
		|| hwnd == columnNameHeader.m_hWnd
		|| hwnd == columnTypeHeader.m_hWnd
		|| hwnd == idxPrimaryHeader
		|| hwnd == idxNameHeader
		|| hwnd == idxColumnsHeader
		|| hwnd == idxTypeHeader
		|| hwnd == ddlHeader) {
		::SetBkColor(hdc, headerBkgColor);
		::SelectObject(hdc, textFont);
		::SetTextColor(hdc, headerTextColor);
		return headerBrush;
	} else if ((nSelItem = inEditArray(hwnd, primaryKeyPtrs)) != -1 && nSelItem % 2 ) {
		::SetBkColor(hdc, doubleRowColor);
		::SelectObject(hdc, textFont);
		return doubleRowBrush;
	} else if ((nSelItem = inEditArray(hwnd, columnNamePtrs)) != -1 && nSelItem % 2 ) {
		::SetBkColor(hdc, doubleRowColor);
		::SelectObject(hdc, textFont);
		return doubleRowBrush;
	} else if ((nSelItem = inEditArray(hwnd, columnTypePtrs)) != -1 && nSelItem % 2 ) {
		::SetBkColor(hdc, doubleRowColor);
		::SelectObject(hdc, textFont);
		return doubleRowBrush;
	} else if ((nSelItem = inEditArray(hwnd, idxPrimaryPtrs)) != -1 && nSelItem % 2 ) {
		::SetBkColor(hdc, doubleRowColor);
		::SelectObject(hdc, textFont);
		return doubleRowBrush;
	} else if ((nSelItem = inEditArray(hwnd, idxNamePtrs)) != -1 && nSelItem % 2 ) {
		::SetBkColor(hdc, doubleRowColor);
		::SelectObject(hdc, textFont);
		return doubleRowBrush;
	} else if ((nSelItem = inEditArray(hwnd, idxColumnsPtrs)) != -1 && nSelItem % 2 ) {
		::SetBkColor(hdc, doubleRowColor);
		::SelectObject(hdc, textFont);
		return doubleRowBrush;
	} else if ((nSelItem = inEditArray(hwnd, idxTypePtrs)) != -1 && nSelItem % 2 ) {
		::SetBkColor(hdc, doubleRowColor);
		::SelectObject(hdc, textFont);
		return doubleRowBrush;
	}
	else {
		::SetBkColor(hdc, bkgColor);
		::SelectObject(hdc, textFont);
		return AtlGetStockBrush(WHITE_BRUSH);
	}
	
	
}

HBRUSH TablePropertiesPage::OnCtlColorEdit(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, bkgColor);
	::SelectObject(hdc, textFont);
	return AtlGetStockBrush(WHITE_BRUSH);
}

/**
 * found in array.
 * 
 * @param hwnd
 * @param ptrs
 * @return 
 */
size_t TablePropertiesPage::inEditArray(HWND hwnd, const std::vector<CEdit *> & ptrs)
{
	size_t n = ptrs.size();

	size_t nSelItem = -1;
	for (size_t i = 0; i < n; i++) {
		if (ptrs.at(i)->m_hWnd == hwnd) {
			return i;
		}
	}

	return -1;
}


