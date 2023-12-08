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
#include <core/common/exception/QSqlExecuteException.h>
#include <ui/common/message/QPopAnimate.h>

void TablePropertiesPage::setup(QueryPageSupplier * supplier)
{
	this->supplier = supplier;
}


void TablePropertiesPage::initScrollBar(CSize & clientSize)
{
	if (clientSize.cx == 0 || clientSize.cy == 0 || nHeightSum == 0) {
		return ;
	}
	int pageNums = nHeightSum % clientSize.cy ? 
		nHeightSum / clientSize.cy + 1 : nHeightSum / clientSize.cy;
	si.cbSize = sizeof(SCROLLINFO);   // setting the scrollbar

	// change 3 values(SIF_RANGE: si.nMin, si.nMax, si.nPage; SIF_PAGE:si.nPage; SIF_POS: si.nPos)
	si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS; 
	si.nMin = 0; // must set si.fMask = SIF_RANGE
	si.nMax = 100; // must set si.fMask = SIF_RANGE
	si.nPos = 0; // must set si.fMask = SIF_POS

	// must set si.fMask = SIF_PAGE
	si.nPage = si.nMax % pageNums ? 
		si.nMax / pageNums + 1 : si.nMax / pageNums;

	vScrollPages = si.nPage;
	::SetScrollInfo(m_hWnd, SB_VERT, &si, TRUE);
}

LRESULT TablePropertiesPage::OnVScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UINT nSBCode = LOWORD(wParam);
	int nPos = GetScrollPos(SB_VERT);
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	::GetScrollInfo(m_hWnd, SB_VERT, &si);
	iVscrollPos = si.nPos;


	switch (LOWORD(wParam))
	{
	case SB_LINEDOWN:
		si.nPos  += 1;
		break;
	case SB_LINEUP:
		si.nPos -= 1;
		break;
	case SB_PAGEDOWN:
		si.nPos += vScrollPages;
		break;
	case SB_PAGEUP:
		si.nPos -= vScrollPages;
		break;
	case SB_THUMBTRACK:
		si.nPos = si.nTrackPos ;
		break;
	}
	si.nPos = min(100 - vScrollPages, max(0, si.nPos));
	si.fMask = SIF_POS;
	::SetScrollInfo(m_hWnd, SB_VERT, &si, TRUE);
	::GetScrollInfo(m_hWnd, SB_VERT, &si);

	if (iVscrollPos != si.nPos) {
		::ScrollWindow(m_hWnd, 0, (nHeightSum / 100) * (iVscrollPos - si.nPos), nullptr, nullptr);
		Invalidate(true);
	}
	 return 0;
}


LRESULT TablePropertiesPage::OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int fwKeys = GET_KEYSTATE_WPARAM(wParam);
	short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
	
	if (zDelta == 120) { // 正向
		WPARAM newParam = MAKEWPARAM(SB_LINEUP, 0) ;
		OnVScroll(0, newParam, NULL, bHandled);
	}else if (zDelta == -120) { // 反向
		WPARAM newParam = MAKEWPARAM(SB_LINEDOWN, 0) ;
		OnVScroll(0, newParam, NULL, bHandled);
	}
	return 0;
}

LRESULT TablePropertiesPage::OnClickRefreshButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	nHeightSum = 0;

	CRect clientRect;
	GetClientRect(clientRect);
	
	si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS; 
	si.nMin = 0; // must set si.fMask = SIF_RANGE
	si.nMax = 100; // must set si.fMask = SIF_RANGE
	si.nPos = 0; // must set si.fMask = SIF_POS

	// must set si.fMask = SIF_PAGE
	si.nPage = 1;
	vScrollPages = si.nPage;
	::SetScrollInfo(m_hWnd, SB_VERT, &si, TRUE);

	createOrShowUI();
	Invalidate(true);
	return 0;
}


void TablePropertiesPage::createOrShowUI()
{
	QPage::createOrShowUI();

	nHeightSum = 0;

	CRect clientRect;
	GetClientRect(clientRect);
	createOrShowTitleElems(clientRect);
	createOrShowColumnsSectionElems(clientRect);
	createOrShowIndexesSectionElems(clientRect);
	createOrShowForeignKeySectionElems(clientRect);
	createOrShowDdlSectionElems(clientRect);

	// onSize will trigger init the v-scrollbar
	CSize size(clientRect.Width(), clientRect.Height());
	initScrollBar(size);
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
	UserDb userDb;
	try {
		userDb = databaseService->getUserDb(supplier->getRuntimeUserDbId());
	} catch (QSqlExecuteException &ex) {
		Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::report(ex);
		return;
	}
	std::wstring title = userDb.name;
	title.append(L".").append(supplier->getRuntimeTblName());
	createOrShowEdit(titleEdit, 0, title, rect, clientRect, 0);

	rect.OffsetRect(700 - rect.left - 60, 0);
	rect.right = rect.left + 60;
	QWinCreater::createOrShowButton(m_hWnd, refreshButton, Config::TABLE_PROPERTIES_REFRESH_BUTTON_ID, S(L"refresh-page"), rect, clientRect);
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
	destroyPtrs(primaryKeyPtrs);
	destroyPtrs(columnNamePtrs);
	destroyPtrs(columnTypePtrs);
	primaryBitmapPtrs.clear();

	CRect rcHeader0 = GdiPlusUtil::GetWindowRelativeRect(primaryKeyHeader.m_hWnd);
	CRect rcHeader1 = GdiPlusUtil::GetWindowRelativeRect(columnNameHeader.m_hWnd);
	CRect rcHeader2 = GdiPlusUtil::GetWindowRelativeRect(columnTypeHeader.m_hWnd);
	ColumnInfoList list;
	UserDb userDb;
	try {
		list = tableService->getUserColumns(supplier->getRuntimeUserDbId(), 
			supplier->getRuntimeTblName(), supplier->getRuntimeSchema(), false);
		userDb = databaseService->getUserDb(supplier->getRuntimeUserDbId());
	} catch (QSqlExecuteException &ex) {
		Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::report(ex);
		return;
	}
	int i = 0;
	int n = static_cast<int>(list.size());
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
				
		auto ptr0 = new CEdit();
		createOrShowEdit(*ptr0, 0, L"", rcHeader0, clientRect, 0);
		primaryKeyPtrs.push_back(ptr0);
		if (item.pk) {
			primaryBitmapPtrs.push_back({ ptr0, i}); // for draw primary key bitmap
			ptr0->ShowWindow(false);
		}

		auto ptr1 = new CEdit();
		createOrShowEdit(*ptr1, 0, field, rcHeader1, clientRect, 0);
		columnNamePtrs.push_back(ptr1);

		auto ptr2 = new CEdit();
		createOrShowEdit(*ptr2, 0, type, rcHeader2, clientRect, 0);
		columnTypePtrs.push_back(ptr2);

		i++;
	}
}

void TablePropertiesPage::drawPrimaryKey(CDC & dc, CRect & rect, int nItem)
{
	HBRUSH brush = bkgBrush.m_hBrush;
	if (nItem % 2) {
		brush = doubleRowBrush.m_hBrush;
	}
	dc.FillRect(rect, brush);
	int x = rect.left + (rect.Width() - 16) / 2, 
		y = rect.top + (rect.Height() - 16) / 2;
	dc.DrawIconEx(x, y, primaryIcon, 16, 16);
}

void TablePropertiesPage::createOrShowIndexesSectionElems(CRect & clientRect)
{
	int n = static_cast<int>(primaryKeyPtrs.size());
	CRect rcTop = n ? GdiPlusUtil::GetWindowRelativeRect(primaryKeyPtrs.at(n-1)->m_hWnd)
		: GdiPlusUtil::GetWindowRelativeRect(primaryKeyHeader.m_hWnd);
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
	destroyPtrs(idxPrimaryPtrs);
	destroyPtrs(idxNamePtrs);
	destroyPtrs(idxColumnsPtrs);
	destroyPtrs(idxTypePtrs);

	CRect rcHeader0 = GdiPlusUtil::GetWindowRelativeRect(idxPrimaryHeader.m_hWnd);
	CRect rcHeader1 = GdiPlusUtil::GetWindowRelativeRect(idxNameHeader.m_hWnd);
	CRect rcHeader2 = GdiPlusUtil::GetWindowRelativeRect(idxColumnsHeader.m_hWnd);
	CRect rcHeader3 = GdiPlusUtil::GetWindowRelativeRect(idxTypeHeader.m_hWnd);
	IndexInfoList list;
	UserDb userDb;
	try {
		list = tableService->getIndexInfoList(supplier->getRuntimeUserDbId(), supplier->getRuntimeTblName());
		userDb = databaseService->getUserDb(supplier->getRuntimeUserDbId());
	} catch (QSqlExecuteException &ex) {
		Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::report(ex);
		return;
	}
	
	int i = 0;
	int n = static_cast<int>(list.size());
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

		auto ptr0 = new CEdit();
		createOrShowEdit(*ptr0, 0, L"", rcHeader0, clientRect, 0);
		idxPrimaryPtrs.push_back(ptr0);
		if (item.pk) {
			primaryBitmapPtrs.push_back({ ptr0, i });
			ptr0->ShowWindow(false);
		}

		auto ptr1 = new CEdit();
		createOrShowEdit(*ptr1, 0, name, rcHeader1, clientRect, 0);
		idxNamePtrs.push_back(ptr1);

		auto ptr2 = new CEdit();
		createOrShowEdit(*ptr2, 0, columns, rcHeader2, clientRect, 0);
		idxColumnsPtrs.push_back(ptr2);

		auto ptr3 = new CEdit();
		createOrShowEdit(*ptr3, 0, type, rcHeader3, clientRect, 0);
		idxTypePtrs.push_back(ptr3);
		

		i++;
	}
}

void TablePropertiesPage::createOrShowForeignKeySectionElems(CRect & clientRect)
{
	int n = static_cast<int>(idxPrimaryPtrs.size());
	CRect rcTop = n ? GdiPlusUtil::GetWindowRelativeRect(idxPrimaryPtrs.at(n-1)->m_hWnd) 
		: GdiPlusUtil::GetWindowRelativeRect(idxPrimaryHeader.m_hWnd);
	int x = 20, y = rcTop.bottom + 30, w = 300, h = 20;
	CRect rect(x, y, x + w, y + h);
	// section
	createOrShowEdit(foreignKeySection, 0, S(L"table-foreignkeys"), rect, clientRect, 0);

	// header
	rect.OffsetRect(0, 20);
	rect.right = rect.left + 100;
	createOrShowEdit(frkNameHeader, 0, S(L"foreignkey-name"), rect, clientRect, 0);

	rect.OffsetRect(rect.Width() + 2, 0);
	rect.right = rect.left + 150;
	createOrShowEdit(frkColumnsHeader, 0, S(L"referencing-columns"), rect, clientRect, 0);

	rect.OffsetRect(rect.Width() + 2, 0);
	rect.right = rect.left + 100;
	createOrShowEdit(frkReferedTableHeader, 0, S(L"referenced-table"), rect, clientRect, 0);

	rect.OffsetRect(rect.Width() + 2, 0);
	rect.right = rect.left + 150;
	createOrShowEdit(frkReferedColumnsHeader, 0, S(L"referenced-columns"), rect, clientRect, 0);

	rect.OffsetRect(rect.Width() + 2, 0);
	rect.right = rect.left + 100;
	createOrShowEdit(frkOnUpdateHeader, 0, S(L"on-update"), rect, clientRect, 0);

	rect.OffsetRect(rect.Width() + 2, 0);
	rect.right = rect.left + 100;
	createOrShowEdit(frkOnDeleteHeader, 0, S(L"on-delete"), rect, clientRect, 0);

	createOrShowForeignKeyList(clientRect);
}

void TablePropertiesPage::createOrShowForeignKeyList(CRect & clientRect)
{
	destroyPtrs(frkNamePtrs);
	destroyPtrs(frkColumnsPtrs);
	destroyPtrs(frkReferedTablePtrs);
	destroyPtrs(frkReferedColumnsPtrs);
	destroyPtrs(frkOnUpdatePtrs);
	destroyPtrs(frkOnDeletePtrs);


	CRect rcHeader0 = GdiPlusUtil::GetWindowRelativeRect(frkNameHeader.m_hWnd);
	CRect rcHeader1 = GdiPlusUtil::GetWindowRelativeRect(frkColumnsHeader.m_hWnd);
	CRect rcHeader2 = GdiPlusUtil::GetWindowRelativeRect(frkReferedTableHeader.m_hWnd);
	CRect rcHeader3 = GdiPlusUtil::GetWindowRelativeRect(frkReferedColumnsHeader.m_hWnd);
	CRect rcHeader4 = GdiPlusUtil::GetWindowRelativeRect(frkOnUpdateHeader.m_hWnd);
	CRect rcHeader5 = GdiPlusUtil::GetWindowRelativeRect(frkOnDeleteHeader.m_hWnd);
	ForeignKeyList list;
	UserDb userDb;
	try {
		list = tableService->getForeignKeyList(supplier->getRuntimeUserDbId(), supplier->getRuntimeTblName());
		userDb = databaseService->getUserDb(supplier->getRuntimeUserDbId());
	} catch (QSqlExecuteException &ex) {
		Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::report(ex);
		return;
	}
	int i = 0;
	int n = static_cast<int>(list.size());
	std::wstring section = S(L"table-foreignkeys");
	section.append(L" (").append(std::to_wstring(n)).append(L")");
	foreignKeySection.SetWindowText(section.c_str());

	for (auto & item : list) {
		std::wstring & name = item.name;
		std::wstring & columns = item.columns;
		std::wstring & referedTable = item.referencedTable;
		std::wstring & referedColumns = item.referencedColumns;
		std::wstring & onUpdate = item.onUpdate;	
		std::wstring & onDelete = item.onDelete;
		
		rcHeader0.OffsetRect(0, rcHeader0.Height() + 2);
		rcHeader1.OffsetRect(0, rcHeader1.Height() + 2);
		rcHeader2.OffsetRect(0, rcHeader2.Height() + 2);
		rcHeader3.OffsetRect(0, rcHeader3.Height() + 2);
		rcHeader4.OffsetRect(0, rcHeader4.Height() + 2);
		rcHeader5.OffsetRect(0, rcHeader5.Height() + 2);

		auto ptr0 = new CEdit();
		createOrShowEdit(*ptr0, 0, name, rcHeader0, clientRect, 0);
		frkNamePtrs.push_back(ptr0);

		auto ptr1 = new CEdit();
		createOrShowEdit(*ptr1, 0, columns, rcHeader1, clientRect, 0);
		frkColumnsPtrs.push_back(ptr1);

		auto ptr2 = new CEdit();
		createOrShowEdit(*ptr2, 0, referedTable, rcHeader2, clientRect, 0);
		frkReferedTablePtrs.push_back(ptr2);

		auto ptr3 = new CEdit();
		createOrShowEdit(*ptr3, 0, referedColumns, rcHeader3, clientRect, 0);
		frkReferedColumnsPtrs.push_back(ptr3);

		auto ptr4 = new CEdit();
		createOrShowEdit(*ptr4, 0, onUpdate, rcHeader4, clientRect, 0);
		frkOnUpdatePtrs.push_back(ptr4);

		auto ptr5 = new CEdit();
		createOrShowEdit(*ptr5, 0, onDelete, rcHeader5, clientRect, 0);
		frkOnDeletePtrs.push_back(ptr5);
		
		i++;
	}
}

void TablePropertiesPage::createOrShowDdlSectionElems(CRect & clientRect)
{
	int n = static_cast<int>(frkNamePtrs.size());
	CRect rcTop = n ? GdiPlusUtil::GetWindowRelativeRect(frkNamePtrs.at(n-1)->m_hWnd) 
		: GdiPlusUtil::GetWindowRelativeRect(frkNameHeader.m_hWnd);
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
	UserTable userTable;
	UserIndexList userIndexList;
	try {
		userTable = tableService->getUserTable(supplier->getRuntimeUserDbId(), supplier->getRuntimeTblName());
		userIndexList = tableService->getUserIndexes(supplier->getRuntimeUserDbId(), supplier->getRuntimeTblName());
	}catch (QSqlExecuteException &ex) {
		Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::report(ex);
		return;
	}

	std::wstring sql = StringUtil::replace(userTable.sql, L"\r\n", L"\n");
	sql = StringUtil::replace(sql, L"\n", L"\r\n");
	sql.append(L";");
	for (auto userIndex : userIndexList) {
		if (userIndex.sql.empty()) {
			continue;
		}
		sql.append(L"\r\n").append(userIndex.sql).append(L";");
	}
	createOrShowEdit(ddlContent, 0, sql, rect, clientRect, ES_MULTILINE | ES_AUTOVSCROLL);
	
	CRect rcTail = GdiPlusUtil::GetWindowRelativeRect(ddlContent.m_hWnd);
	nHeightSum = rcTail.bottom + 150;
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
	int nPos = GetScrollPos(SB_VERT);
	int x = 20, y = 15 + 32 + 10 - nPos * cyChar, w = paintRect.Width() - 2 * 20, h = 1;

	HPEN oldPen = dc.SelectPen(linePen);
	dc.MoveTo(x, y);
	dc.LineTo(x + w, y);
	dc.SelectPen(oldPen);

	for (auto & pair : primaryBitmapPtrs) {
		if (pair.first && pair.first->IsWindow()) {
			CRect editRect = GdiPlusUtil::GetWindowRelativeRect(pair.first->m_hWnd);
			drawPrimaryKey(dc, editRect , pair.second);
		}		
	}
}

int TablePropertiesPage::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	bool ret = QPage::OnCreate(lpCreateStruct);
	linePen.CreatePen(PS_SOLID, 1, lineColor);
	textFont = FT(L"form-text-size");
	titleFont = FTB(L"properites-title-font", true);
	sectionFont = FTB(L"properites-section-font", true);
	headerBrush.CreateSolidBrush(headerBkgColor);
	doubleRowBrush.CreateSolidBrush(doubleRowColor);

	// Init the scrollbar params 
	HDC hdc  = ::GetDC(m_hWnd);
	::GetTextMetrics(hdc, &tm);
	cxChar = tm.tmAveCharWidth;
	cyChar = tm.tmHeight + tm.tmExternalLeading;
	::ReleaseDC(m_hWnd, hdc);

	std::wstring imgDir = ResourceUtil::getProductImagesDir();
	HINSTANCE ins = ModuleHelper::GetModuleInstance();
	primaryIcon = (HICON)::LoadImageW(ins, (imgDir + L"database\\page\\primary.ico").c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);

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
	if (!headerBrush.IsNull()) headerBrush.DeleteObject();
	if (doubleRowBrush) doubleRowBrush.DeleteObject();
	if (primaryIcon) ::DeleteObject(primaryIcon);


	if (titleImage.IsWindow()) titleImage.DestroyWindow();
	if (titleEdit.IsWindow()) titleEdit.DestroyWindow();
	if (refreshButton.IsWindow()) refreshButton.DestroyWindow();

	if (columnsSection.IsWindow()) columnsSection.DestroyWindow();
	if (columnNameHeader.IsWindow()) columnNameHeader.DestroyWindow();
	if (columnTypeHeader.IsWindow()) columnTypeHeader.DestroyWindow();
	destroyPtrs(primaryKeyPtrs);
	destroyPtrs(columnNamePtrs);
	destroyPtrs(columnTypePtrs);

	if (indexesSection.IsWindow()) indexesSection.DestroyWindow();
	if (idxPrimaryHeader.IsWindow()) idxPrimaryHeader.DestroyWindow();
	if (idxNameHeader.IsWindow()) idxNameHeader.DestroyWindow();
	if (idxColumnsHeader.IsWindow()) idxColumnsHeader.DestroyWindow();
	if (idxTypeHeader.IsWindow()) idxTypeHeader.DestroyWindow();
	destroyPtrs(idxPrimaryPtrs);
	destroyPtrs(idxNamePtrs);
	destroyPtrs(idxColumnsPtrs);
	destroyPtrs(idxTypePtrs);

	if (foreignKeySection.IsWindow()) foreignKeySection.DestroyWindow();
	if (frkNameHeader.IsWindow()) frkNameHeader.DestroyWindow();
	if (frkColumnsHeader.IsWindow()) frkColumnsHeader.DestroyWindow();
	if (frkReferedTableHeader.IsWindow()) frkReferedTableHeader.DestroyWindow();
	if (frkReferedColumnsHeader.IsWindow()) frkReferedColumnsHeader.DestroyWindow();
	if (frkOnUpdateHeader.IsWindow()) frkOnUpdateHeader.DestroyWindow();
	if (frkOnDeleteHeader.IsWindow()) frkOnDeleteHeader.DestroyWindow();
	destroyPtrs(frkNamePtrs);
	destroyPtrs(frkColumnsPtrs);
	destroyPtrs(frkReferedTablePtrs);
	destroyPtrs(frkReferedColumnsPtrs);
	destroyPtrs(frkOnUpdatePtrs);
	destroyPtrs(frkOnDeletePtrs);

	if (ddlSection.IsWindow()) ddlSection.DestroyWindow();
	if (ddlHeader.IsWindow()) ddlHeader.DestroyWindow();
	if (ddlContent.IsWindow()) ddlContent.DestroyWindow();

	return ret;
}


void TablePropertiesPage::destroyPtrs(std::vector<CEdit *> & ptrs)
{
	if (ptrs.empty()) {
		return;
	}

	for (auto ptr : ptrs) {
		if (ptr && ptr->IsWindow()) {
			ptr->DestroyWindow();
			delete ptr;
			ptr = nullptr;
		} else if (ptr) {
			delete ptr;
			ptr = nullptr;
		}
	}

	ptrs.clear();
}

HBRUSH TablePropertiesPage::OnCtlColorStatic(HDC hdc, HWND hwnd)
{
	size_t nSelItem = -1;
	HBRUSH brush;
	if (hwnd == titleEdit.m_hWnd) {
		::SetBkColor(hdc, bkgColor);
		::SelectObject(hdc, titleFont);
		brush = AtlGetStockBrush(WHITE_BRUSH);
	} else if (hwnd == columnsSection.m_hWnd 
		|| hwnd == indexesSection.m_hWnd
		|| hwnd == foreignKeySection.m_hWnd
		|| hwnd == ddlSection.m_hWnd) {
		::SetBkColor(hdc, bkgColor);
		::SelectObject(hdc, sectionFont);
		brush = AtlGetStockBrush(WHITE_BRUSH);
	} else if (hwnd == primaryKeyHeader.m_hWnd 
		|| hwnd == columnNameHeader.m_hWnd
		|| hwnd == columnTypeHeader.m_hWnd
		|| hwnd == idxPrimaryHeader
		|| hwnd == idxNameHeader
		|| hwnd == idxColumnsHeader
		|| hwnd == idxTypeHeader
		|| hwnd == ddlHeader
		|| hwnd == frkNameHeader
		|| hwnd == frkColumnsHeader
		|| hwnd == frkReferedTableHeader
		|| hwnd == frkReferedColumnsHeader
		|| hwnd == frkOnUpdateHeader
		|| hwnd == frkOnDeleteHeader) {
		::SetBkColor(hdc, headerBkgColor);
		::SelectObject(hdc, textFont);
		::SetTextColor(hdc, headerTextColor);
		brush = headerBrush.m_hBrush;
	} else if (((nSelItem = inEditArray(hwnd, primaryKeyPtrs)) != -1 && nSelItem % 2 )
		|| ((nSelItem = inEditArray(hwnd, columnNamePtrs)) != -1 && nSelItem % 2 )
		|| ((nSelItem = inEditArray(hwnd, columnTypePtrs)) != -1 && nSelItem % 2 )
		|| ((nSelItem = inEditArray(hwnd, idxPrimaryPtrs)) != -1 && nSelItem % 2 )
		|| ((nSelItem = inEditArray(hwnd, idxNamePtrs)) != -1 && nSelItem % 2 )
		|| ((nSelItem = inEditArray(hwnd, idxColumnsPtrs)) != -1 && nSelItem % 2 )
		|| ((nSelItem = inEditArray(hwnd, idxTypePtrs)) != -1 && nSelItem % 2 )
		|| ((nSelItem = inEditArray(hwnd, frkNamePtrs)) != -1 && nSelItem % 2 )
		|| ((nSelItem = inEditArray(hwnd, frkColumnsPtrs)) != -1 && nSelItem % 2 )
		|| ((nSelItem = inEditArray(hwnd, frkReferedTablePtrs)) != -1 && nSelItem % 2 )
		|| ((nSelItem = inEditArray(hwnd, frkReferedColumnsPtrs)) != -1 && nSelItem % 2 )
		|| ((nSelItem = inEditArray(hwnd, frkOnUpdatePtrs)) != -1 && nSelItem % 2 )
		|| ((nSelItem = inEditArray(hwnd, frkOnDeletePtrs)) != -1 && nSelItem % 2 )
		) {
		::SetBkColor(hdc, doubleRowColor);
		::SelectObject(hdc, textFont);
		brush = doubleRowBrush.m_hBrush;
	} else {
		::SetBkColor(hdc, bkgColor);
		::SelectObject(hdc, textFont);
		brush = AtlGetStockBrush(WHITE_BRUSH);
	}
	
	return brush;
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

