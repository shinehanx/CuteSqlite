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

 * @file   ResultListPage.cpp
 * @brief  Execute sql statement and show the list of query result
 * 
 * @author Xuehan Qin
 * @date   2023-05-22
 *********************************************************************/

#include "stdafx.h"
#include "ResultListPage.h"
#include "core/common/Lang.h"
#include "ui/common/message/QPopAnimate.h"
#include "ui/common/QWinCreater.h"

void ResultListPage::setup(std::wstring & sql)
{
	this->sql = sql;
}

void ResultListPage::createOrShowUI()
{
	QPage::createOrShowUI();

	CRect clientRect;
	GetClientRect(clientRect);
	createOrShowToolBarElements(clientRect);
	createOrShowListView(listView, clientRect);
}

/**
 * create or show page toolbar buttons and other elments.
 * 
 * @param clientRect The page's client rect
 */
void ResultListPage::createOrShowToolBarElements(CRect & clientRect)
{
	CRect topbarRect = getTopRect(clientRect);
	int x = 5, y = 5, w = PAGE_BUTTON_WIDTH, h = PAGE_BUTTON_HEIGHT;
	CRect rect(x, y, x + w, y + h);
	// create or show button
	std::wstring imgDir = ResourceUtil::getProductImagesDir();
	std::wstring normalImagePath = imgDir + L"database\\list\\button\\export-button-normal.png";
	std::wstring pressedImagePath = imgDir + L"database\\list\\button\\export-button-pressed.png";
	exportButton.SetIconPath(normalImagePath, pressedImagePath);
	exportButton.SetBkgColors(buttonColor, buttonColor, buttonColor);
	QWinCreater::createOrShowButton(m_hWnd, exportButton,  Config::LISTVIEW_EXPORT_BUTTON_ID, L"", rect, clientRect);
	exportButton.SetToolTip(S(L"export-result-as"));

	rect.OffsetRect(16 + 10, 0);
	rect.InflateRect(0, 0, 8, 0); 
	normalImagePath = imgDir + L"database\\list\\button\\copy-button-normal.png";
	pressedImagePath = imgDir + L"database\\list\\button\\copy-button-pressed.png";
	copyButton.SetIconPath(normalImagePath, pressedImagePath);
	copyButton.SetBkgColors(buttonColor, buttonColor, buttonColor);
	copyButton.SetToolTip(S(L"copy"));
	QWinCreater::createOrShowButton(m_hWnd, copyButton, Config::LISTVIEW_COPY_BUTTON_ID, L"", rect, clientRect);
	copyButton.SetToolTip(S(L"copy-result-data"));

	rect.OffsetRect(24 + 10, 0);
	rect.InflateRect(0, 2, 80, -2); 
	QWinCreater::createOrShowComboBox(m_hWnd, readWriteComboBox, Config::LISTVIEW_READ_WRITE_COMBOBOX_ID, rect, clientRect);
	readWriteComboBox.SetFont(FT(L"combobox-size"));

	rect.OffsetRect(120 + 20, 0);
	rect.InflateRect(0, 0, 20, 4);
	formViewCheckBox.setBkgColor(buttonColor);
	formViewCheckBox.setFontSize(Lang::fontSize(L"checkbox-size")); 
	QWinCreater::createOrShowCheckBox(m_hWnd, formViewCheckBox, Config::LISTVIEW_FORMVIEW_CHECKBOX_ID, S(L"show-form-view"), rect, clientRect);
		
	rect.MoveToX(topbarRect.right - 300);
	rect.InflateRect(0, -2, -100, -2);
	normalImagePath = imgDir + L"database\\list\\button\\filter-button-normal.png"; 
	pressedImagePath = imgDir + L"database\\list\\button\\filter-button-pressed.png"; 
	filterButton.SetIconPath(normalImagePath, pressedImagePath);
	filterButton.SetBkgColors(buttonColor, buttonColor, buttonColor);
	QWinCreater::createOrShowButton(m_hWnd, filterButton, Config::LISTVIEW_FILTER_BUTTON_ID, L"", rect, clientRect);

	rect.OffsetRect(16 + 10, 0);
	normalImagePath = imgDir + L"database\\list\\button\\refresh-button-normal.png";
	pressedImagePath = imgDir + L"database\\list\\button\\refresh-button-pressed.png";
	refreshButton.SetIconPath(normalImagePath, pressedImagePath);
	refreshButton.SetBkgColors(buttonColor, buttonColor, buttonColor);
	QWinCreater::createOrShowButton(m_hWnd, refreshButton, Config::LISTVIEW_REFRESH_BUTTON_ID, L"", rect, clientRect);
}

void ResultListPage::loadWindow()
{
	QPage::loadWindow();

	if (!isNeedReload || sql.empty()) {
		return;
	}
	isNeedReload = false;

	uint64_t userDbId = supplier->getSeletedUserDbId();
	if (!userDbId) {
		QPopAnimate::warn(m_hWnd, S(L"no-select-userdb"));
		return;
	}
	rowCount = adapter->loadListView(userDbId, sql);
}

void ResultListPage::paintItem(CDC & dc, CRect & paintRect)
{
	/*
	COLORREF color = RGB(255, 20, 60);
	HBRUSH brush = ::CreateSolidBrush(color);
	dc.FillRect(paintRect, brush);
	::DeleteObject(brush);
	*/
}

void ResultListPage::createOrShowListView(CListViewCtrl & win, CRect & clientRect)
{
	CRect & rect = getPageRect(clientRect);
	if (IsWindow() && !win.IsWindow()) {
		win.Create(m_hWnd, rect,NULL, 
			WS_CHILD | WS_TABSTOP | WS_VISIBLE | LVS_ALIGNLEFT | LVS_REPORT | LVS_SHOWSELALWAYS | WS_BORDER | LVS_OWNERDATA , // | LVS_OWNERDATA
			0, Config::DATABASE_QUERY_LISTVIEW_ID );
		win.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
		adapter = new ResultListPageAdapter(m_hWnd, &win);
	}
	else if (IsWindow() && win.IsWindow() && clientRect.Width() > 1) {
		win.MoveWindow(rect);
		win.ShowWindow(true);
	}
}

int ResultListPage::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	bool ret = QPage::OnCreate(lpCreateStruct);

	return ret;
}

int ResultListPage::OnDestroy()
{
	bool ret = QPage::OnDestroy();

	if (exportButton.IsWindow()) exportButton.DestroyWindow();
	if (copyButton.IsWindow()) copyButton.DestroyWindow();
	if (readWriteComboBox.IsWindow()) readWriteComboBox.DestroyWindow();
	if (formViewCheckBox.IsWindow()) formViewCheckBox.DestroyWindow();
	if (filterButton.IsWindow()) filterButton.DestroyWindow();
	if (refreshButton.IsWindow()) refreshButton.DestroyWindow();
	if (limitCheckBox.IsWindow()) limitCheckBox.DestroyWindow();
	if (offsetLabel.IsWindow()) offsetLabel.DestroyWindow();
	if (offsetEdit.IsWindow()) offsetEdit.DestroyWindow();
	if (limitLabel.IsWindow()) limitLabel.DestroyWindow();
	if (limitEdit.IsWindow()) limitEdit.DestroyWindow();

	if (listView.IsWindow()) listView.DestroyWindow();

	if (adapter) {
		delete adapter;
	}
	return ret;
}

LRESULT ResultListPage::OnNMClickListView(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
	return 0;
}

LRESULT ResultListPage::OnGetListViewData(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
	NMLVDISPINFO* plvdi = (NMLVDISPINFO*)pnmh;
	adapter->fillListViewItemData(plvdi);

	return 0;
}

LRESULT ResultListPage::OnPrepareListViewData(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
	return 0;
}

LRESULT ResultListPage::OnFindListViewData(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
	LPNMLVFINDITEM  pnmfi = (LPNMLVFINDITEM)pnmh;

	auto iItem = pnmfi->iStart;
	if (-1 == iItem)
		return -1;

	auto count = rowCount;
	if (!count || count <= iItem)
		return -1;

	return 0;
}

