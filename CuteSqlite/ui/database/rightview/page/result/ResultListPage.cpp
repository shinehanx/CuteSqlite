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
#include "ui/common/message/QMessageBox.h"
#include "ui/common/QWinCreater.h"
#include "ui/database/rightview/page/result/dialog/ExportResultDialog.h"

void ResultListPage::setup(std::wstring & sql)
{
	this->sql = sql;
}

void ResultListPage::createOrShowUI()
{
	QPage::createOrShowUI();

	CRect clientRect;
	GetClientRect(clientRect);
	createOrShowToolBarElems(clientRect);
	createOrShowListView(listView, clientRect);
}

/**
 * create or show page toolbar buttons and other elments.
 * 
 * @param clientRect The page's client rect
 */
void ResultListPage::createOrShowToolBarElems(CRect & clientRect)
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
	QWinCreater::createOrShowCheckBox(m_hWnd, formViewCheckBox, Config::LISTVIEW_FORMVIEW_CHECKBOX_ID, S(L"show-form-view"), rect, clientRect);
		
	rect.MoveToX(topbarRect.right - 350);
	rect.InflateRect(0, -2, -100, 0);
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

	rect.OffsetRect(16 + 20, 0);
	rect.InflateRect(0, 0, 36, 0);
	QWinCreater::createOrShowCheckBox(m_hWnd, limitCheckBox, Config::LISTVIEW_LIMIT_CHECKBOX_ID, S(L"limit-rows"), rect, clientRect);

	rect.OffsetRect(60 + 5, 2);
	rect.InflateRect(0, 0, -10, 0);
	QWinCreater::createOrShowLabel(m_hWnd, offsetLabel, S(L"offset").append(L":"), rect, clientRect, SS_RIGHT);

	rect.OffsetRect(50 + 2, -2);
	QWinCreater::createOrShowEdit(m_hWnd, offsetEdit, 0, L"0", rect, clientRect, ES_NUMBER, false);

	rect.OffsetRect(50 + 5, 2);
	QWinCreater::createOrShowLabel(m_hWnd, limitLabel, S(L"rows").append(L":"), rect, clientRect, SS_RIGHT);

	rect.OffsetRect(50 + 2, -2);
	QWinCreater::createOrShowEdit(m_hWnd, limitEdit, 0, L"1000", rect, clientRect, ES_NUMBER, false);
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
	
}

void ResultListPage::createOrShowListView(CListViewCtrl & win, CRect & clientRect)
{
	CRect & rect = getPageRect(clientRect);
	if (IsWindow() && !win.IsWindow()) {
		win.Create(m_hWnd, rect,NULL, 
			WS_CHILD | WS_TABSTOP | WS_VISIBLE | LVS_ALIGNLEFT | LVS_REPORT | LVS_SHOWSELALWAYS | WS_BORDER | LVS_OWNERDATA , // | LVS_OWNERDATA
			0, Config::DATABASE_QUERY_LISTVIEW_ID );
		win.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER );
		win.SetImageList(imageList, LVSIL_SMALL);
		CHeaderCtrl header = win.GetHeader();
		header.SetImageList(imageList);
		adapter = new ResultListPageAdapter(m_hWnd, &win);
	}
	else if (IsWindow() && win.IsWindow() && clientRect.Width() > 1) {
		win.MoveWindow(rect);
		win.ShowWindow(true);
	}
}

void ResultListPage::createCopyMenu()
{
	copyMenu.CreatePopupMenu();
	copyMenu.AppendMenu(MF_STRING, Config::COPY_ALL_ROWS_TO_CLIPBOARD_MEMU_ID, S(L"copy-all-rows-to-clipboard").c_str());
	copyMenu.AppendMenu(MF_STRING, Config::COPY_SEL_ROWS_TO_CLIPBOARD_MEMU_ID, S(L"copy-sel-rows-to-clipboard").c_str());
	copyMenu.AppendMenu(MF_STRING, Config::COPY_ALL_ROWS_AS_SQL_MEMU_ID, S(L"copy-all-rows-as-sql").c_str());
	copyMenu.AppendMenu(MF_STRING, Config::COPY_SEL_ROWS_AS_SQL_MEMU_ID, S(L"copy-sel-rows-as-sql").c_str());

}

void ResultListPage::popupCopyMenu(CPoint & pt)
{
	if (listView.GetItemCount() == 0) {
		copyMenu.EnableMenuItem(Config::COPY_ALL_ROWS_TO_CLIPBOARD_MEMU_ID, MF_DISABLED);
		copyMenu.EnableMenuItem(Config::COPY_SEL_ROWS_TO_CLIPBOARD_MEMU_ID, MF_DISABLED);
		copyMenu.EnableMenuItem(Config::COPY_ALL_ROWS_AS_SQL_MEMU_ID, MF_DISABLED);
		copyMenu.EnableMenuItem(Config::COPY_SEL_ROWS_AS_SQL_MEMU_ID, MF_DISABLED);
	} else {
		copyMenu.EnableMenuItem(Config::COPY_ALL_ROWS_TO_CLIPBOARD_MEMU_ID, MF_ENABLED);
		copyMenu.EnableMenuItem(Config::COPY_ALL_ROWS_AS_SQL_MEMU_ID, MF_ENABLED);
	}

	if (listView.GetSelectedCount() == 0) {
		copyMenu.EnableMenuItem(Config::COPY_SEL_ROWS_TO_CLIPBOARD_MEMU_ID, MF_DISABLED);
		copyMenu.EnableMenuItem(Config::COPY_SEL_ROWS_AS_SQL_MEMU_ID, MF_DISABLED);
	}else {
		copyMenu.EnableMenuItem(Config::COPY_SEL_ROWS_TO_CLIPBOARD_MEMU_ID, MF_ENABLED);
		copyMenu.EnableMenuItem(Config::COPY_SEL_ROWS_AS_SQL_MEMU_ID, MF_ENABLED);
	}
	copyMenu.TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, m_hWnd);
}

void ResultListPage::createImageList()
{
	if (!imageList.IsNull()) {
		return ;
	}
	std::wstring imgDir = ResourceUtil::getProductImagesDir();
	std::wstring checkNoPath = imgDir + L"database\\list\\check-no.png";
	std::wstring checkYesPath = imgDir + L"database\\list\\check-yes.png";
	Gdiplus::Bitmap  checkNoImage(checkNoPath.c_str());
	Gdiplus::Bitmap  checkYesImage(checkYesPath.c_str());
	Gdiplus::Color color(RGB(0xff, 0xff, 0xff));

	checkNoImage.GetHBITMAP(color, &checkNoBitmap);	
	checkYesImage.GetHBITMAP(color, &checkYesBitmap);

	imageList.Create(checkNoImage.GetWidth(),checkNoImage.GetHeight(), ILC_COLOR32, 0, 2);
	imageList.Add(checkNoBitmap); //0- No Checked image 
	imageList.Add(checkYesBitmap); //1- Yes Checked image
}

int ResultListPage::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	bool ret = QPage::OnCreate(lpCreateStruct);

	textFont = FT(L"form-text-size");
	createImageList();
	createCopyMenu();
	return ret;
}

int ResultListPage::OnDestroy()
{
	bool ret = QPage::OnDestroy();
	if (textFont) ::DeleteObject(textFont);

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

	if (imageList.IsNull()) imageList.Destroy();
	if (checkNoBitmap) ::DeleteObject(checkNoBitmap);
	if (checkYesBitmap) ::DeleteObject(checkYesBitmap);
	return ret;
}

LRESULT ResultListPage::OnClickListView(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
	auto ptr = (LPNMITEMACTIVATE)pnmh;
	
	return 0;
}

LRESULT ResultListPage::OnRightClickListView(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
	LPNMITEMACTIVATE lpmnItemActive = (LPNMITEMACTIVATE)pnmh;
	//CPoint pt = lpmnItemActive->ptAction;
	CPoint pt; 
	GetCursorPos(&pt);
	popupCopyMenu(pt);

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
	auto pCachehint = (NMLVCACHEHINT *)pnmh;
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

/**
 * Click the header.
 * Refrence:https://learn.microsoft.com/zh-cn/windows/win32/controls/hdn-itemclick
 * 
 * @param idCtrl
 * @param pnmh
 * @param bHandled
 * @return 
 */
LRESULT ResultListPage::OnClickListViewHeader(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
	LPNMLISTVIEW headerPtr = (LPNMLISTVIEW)pnmh; 

	// Refrence url:https://learn.microsoft.com/zh-cn/windows/win32/controls/lvn-columnclick
	if (headerPtr->iSubItem  != 0) {
		return 0;
	}
	adapter->changeSelectAllItems();
	return 0;
}

/**
 * select the grid subitem .
 * 
 * @param idCtrl
 * @param pnmh
 * @param bHandled
 * @return 
 */
LRESULT ResultListPage::OnListViewItemChange(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
	LPNMLISTVIEW pnmListView = (LPNMLISTVIEW)pnmh;

	if(pnmListView->uChanged == LVIF_STATE) {
		if(pnmListView->uNewState) {
			int nIndex = pnmListView->iItem;
			int nSubIndex = pnmListView->iSubItem;
			LVITEM item;
			item.iItem = nIndex;
			item.iSubItem = nSubIndex;
			listView.GetItem(&item);
			// todo...
		}

    }
	return 0;
}

void ResultListPage::OnClickExportButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	ExportResultDialog exportResultDialog(m_hWnd, adapter);

	int rows = static_cast<int>(exportResultDialog.DoModal(m_hWnd));
	if (rows > 0) {
		std::wstring msg = StringUtil::replace(S(L"export-success-text"), std::wstring(L"{rows}"), std::to_wstring(rows));
		//QPopAnimate::success(m_hWnd, msg);
		if (QMessageBox::confirm(m_hWnd, msg, S(L"open-the-file")) == Config::CUSTOMER_FORM_YES_BUTTON_ID) {
			std::wstring exportPath = SettingService::getInstance()->getSysInit(L"export_path");
			std::wstring selelctFile = L"/select,"; // ×¢Òâselect,ÒªÓÐ¶ººÅ
			selelctFile.append(exportPath.c_str());
			::ShellExecuteW(NULL, L"open", L"Explorer.exe",selelctFile.c_str() , NULL, SW_SHOWDEFAULT);
		}
	}
}

void ResultListPage::OnClickCopyButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	CRect rect;
	copyButton.GetWindowRect(rect);
	CPoint pt(rect.left, rect.bottom);
	popupCopyMenu(pt);
}

void ResultListPage::OnClickCopyAllRowsToClipboardMenu(UINT uNotifyCode, int nID, HWND hwnd)
{
	adapter->copyAllRowsToClipboard();
}

void ResultListPage::OnClickCopySelRowsToClipboardMenu(UINT uNotifyCode, int nID, HWND hwnd)
{
	adapter->copySelRowsToClipboard();
}

HBRUSH ResultListPage::OnCtlColorStatic(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, topbarColor);
	::SelectObject(hdc, textFont);
	return topbarBrush;
}

HBRUSH ResultListPage::OnCtlColorListBox(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, bkgColor);
	::SelectObject(hdc, textFont);
	return AtlGetStockBrush(WHITE_BRUSH);
}

HBRUSH ResultListPage::OnCtlColorEdit(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, bkgColor);
	::SelectObject(hdc, textFont);
	return AtlGetStockBrush(WHITE_BRUSH);
}

