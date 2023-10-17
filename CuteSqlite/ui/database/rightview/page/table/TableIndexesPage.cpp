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

 * @file   TableIndexesPage.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-10-10
 *********************************************************************/
#include "stdafx.h"
#include "TableIndexesPage.h"
#include "utils/ResourceUtil.h"
#include "ui/common/QWinCreater.h"
#include "core/common/Lang.h"
#include "ui/database/rightview/page/table/dialog/TableIndexColumnsDialog.h"

BOOL TableIndexesPage::PreTranslateMessage(MSG* pMsg)
{
	if (listView.IsWindow() && listView.PreTranslateMessage(pMsg)) {
		return TRUE;
	}
	return FALSE;
}

void TableIndexesPage::setup(uint64_t userDbId, const std::wstring & schema /*= L""*/, 
	TableColumnsPageAdapter * tblColumnsPageAdapter /*= nullpter */)
{
	this->userDbId = userDbId;
	this->schema = schema;
	this->tblColumnsPageAdapter = tblColumnsPageAdapter;
}

void TableIndexesPage::createImageList()
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

void TableIndexesPage::createOrShowUI()
{
	QPage::createOrShowUI();

	CRect clientRect;
	GetClientRect(clientRect);
	createOrShowToolBarElems(clientRect);
	createOrShowListView(listView, clientRect);
}

void TableIndexesPage::createOrShowToolBarElems(CRect & clientRect)
{
	CRect topbarRect = getTopRect(clientRect);
	int x = 5, y = 5, w = PAGE_BUTTON_WIDTH, h = PAGE_BUTTON_HEIGHT;
	CRect rect(x, y, x + w, y + h);
	
	// create or show button
	std::wstring imgDir = ResourceUtil::getProductImagesDir();
	std::wstring normalImagePath, pressedImagePath;
	if (!newIndexButton.IsWindow()) {
		normalImagePath = imgDir + L"database\\list\\button\\column\\new-button-normal.png";
		pressedImagePath = imgDir + L"database\\list\\button\\column\\new-button-pressed.png";
		newIndexButton.SetIconPath(normalImagePath, pressedImagePath);
		newIndexButton.SetBkgColors(buttonColor, buttonColor, buttonColor);
	}
	QWinCreater::createOrShowButton(m_hWnd, newIndexButton, Config::TABLE_NEW_INDEX_BUTTON_ID, L"", rect, clientRect);
	newIndexButton.SetToolTip(S(L"insert-new-column"));

	rect.OffsetRect(16 + 10, 0);
	if (!delIndexButton.IsWindow()) {
		normalImagePath = imgDir + L"database\\list\\button\\column\\del-button-normal.png";
		pressedImagePath = imgDir + L"database\\list\\button\\column\\del-button-pressed.png";
		delIndexButton.SetIconPath(normalImagePath, pressedImagePath);
		delIndexButton.SetBkgColors(buttonColor, buttonColor, buttonColor);
	}
	QWinCreater::createOrShowButton(m_hWnd, delIndexButton, Config::TABLE_DEL_INDEX_BUTTON_ID, L"", rect, clientRect);
	delIndexButton.SetToolTip(S(L"delete-sel-column"));
}

void TableIndexesPage::createOrShowListView(QListViewCtrl & win, CRect & clientRect)
{
	CRect & rect = getPageRect(clientRect);
	if (IsWindow() && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_BORDER | LVS_ALIGNLEFT | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_OWNERDATA ;
		win.Create(m_hWnd, rect,NULL,dwStyle , // | LVS_OWNERDATA
			0, Config::DATABASE_TABLE_INDEXES_LISTVIEW_ID );
		win.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER );
		win.SetImageList(imageList, LVSIL_SMALL);
		CHeaderCtrl header = win.GetHeader();
		header.SetImageList(imageList);
		adapter = new TableIndexesPageAdapter(m_hWnd, &win, NEW_TABLE);
	} else if (IsWindow() && win.IsWindow() && clientRect.Width() > 1) {
		win.MoveWindow(rect);
		win.ShowWindow(true);
	}
}

void TableIndexesPage::loadWindow()
{
	QPage::loadWindow();

	if (!isNeedReload) {
		return;
	}
	isNeedReload = false;	
	loadListView();
}

void TableIndexesPage::loadListView()
{
	rowCount = adapter->loadTblIndexesListView(userDbId, schema);
}

int TableIndexesPage::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	bool ret = QPage::OnCreate(lpCreateStruct);

	textFont = FT(L"form-text-size");
	createImageList();

	return ret;
}

int TableIndexesPage::OnDestroy()
{
	bool ret = QPage::OnDestroy();
	if (textFont) ::DeleteObject(textFont);

	if (newIndexButton.IsWindow()) newIndexButton.DestroyWindow();
	if (delIndexButton.IsWindow()) delIndexButton.DestroyWindow();
	if (upIndexButton.IsWindow()) upIndexButton.DestroyWindow();
	if (downIndexButton.IsWindow()) downIndexButton.DestroyWindow();

	if (listView.IsWindow()) listView.DestroyWindow();

	if (imageList.IsNull()) imageList.Destroy();
	if (checkNoBitmap) ::DeleteObject(checkNoBitmap);
	if (checkYesBitmap) ::DeleteObject(checkYesBitmap);

	if (adapter) delete adapter;
	return ret;
}

void TableIndexesPage::paintItem(CDC & dc, CRect & paintRect)
{

}


LRESULT TableIndexesPage::OnListViewItemChanged(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
	LPNMLISTVIEW nvListViewPtr = reinterpret_cast<LPNMLISTVIEW>(pnmh);
	if (nvListViewPtr->uOldState == 0 && nvListViewPtr->uNewState == 0) 
		return 0;         // No change 
	//   Old   check   box   state 
	BOOL bPrevState = (BOOL)(((nvListViewPtr->uOldState & LVIS_STATEIMAGEMASK) >> 12) - 1);     
	if (bPrevState < 0)         //   On   startup   there 's   no   previous   state   
		bPrevState = 0;   //   so   assign   as   false   (unchecked) 
	//   New   check   box   state 
	BOOL bChecked = (BOOL)(((nvListViewPtr->uNewState & LVIS_STATEIMAGEMASK) >> 12) - 1);       
	if (bChecked < 0)   //   On   non-checkbox   notifications   assume   false 
		bChecked = 0;   
	if (bPrevState == bChecked)   //   No   change   in   check   box 
		return 0; 
	
	return 0;
}

LRESULT TableIndexesPage::OnDbClickListView(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
	NMITEMACTIVATE * aItem = (NMITEMACTIVATE *)pnmh; 
	
	subItemPos.first = aItem->iItem, subItemPos.second = aItem->iSubItem;

	if (aItem->iSubItem == 2 || aItem->iSubItem == 3) {
		return 0;
	}

	// show the editor
	listView.createOrShowEditor(subItemPos);
	return 0;
}

HBRUSH TableIndexesPage::OnCtlColorStatic(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, topbarColor);
	::SelectObject(hdc, textFont);
	return topbarBrush;
}

HBRUSH TableIndexesPage::OnCtlColorListBox(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, bkgColor);
	::SelectObject(hdc, textFont);
	return AtlGetStockBrush(WHITE_BRUSH);
}

HBRUSH TableIndexesPage::OnCtlColorEdit(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, bkgColor);
	::SelectObject(hdc, textFont);
	return AtlGetStockBrush(WHITE_BRUSH);
}

LRESULT TableIndexesPage::OnGetListViewData(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
	NMLVDISPINFO* plvdi = (NMLVDISPINFO*)pnmh;
	adapter->fillDataInListViewSubItem(plvdi);

	return 0;
}

LRESULT TableIndexesPage::OnPrepareListViewData(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
	auto pCachehint = (NMLVCACHEHINT *)pnmh;
	return 0;
}

LRESULT TableIndexesPage::OnFindListViewData(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
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

LRESULT TableIndexesPage::OnListViewSubItemTextChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	SubItemValues changedVals = listView.getChangedVals();
	for (auto val : changedVals) {
		adapter->changeRuntimeDatasItem(val.iItem, val.iSubItem, val.origVal, val.newVal);
		adapter->invalidateSubItem(val.iItem, val.iSubItem);
	}
	return 0;
}

/**
 * Will send Config::MSG_QLISTVIEW_SUBITEM_BUTTON_CLICK_ID message to 
 * parent window when clicking the button in the QListView, wParam=iItem, lParam=iSubItem
 * 
 * @param uMsg
 * @param wParam - QListView iItem
 * @param lParam - QListView iSubItem
 * @param bHandled
 * @return 
 */
LRESULT TableIndexesPage::OnClickListViewSelColumnsButton(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int iItem = static_cast<int>(wParam);
	int iSubItem = static_cast<int>(lParam);
	CButton  * ptr = listView.getButtonPtr(iItem, iSubItem);
	CRect btnRect;
	ptr->GetWindowRect(btnRect);

	TableIndexColumnsDialog columnsDialog(m_hWnd, tblColumnsPageAdapter, adapter, btnRect, iItem, iSubItem);

	columnsDialog.DoModal(m_hWnd);
	return 0;
}

LRESULT TableIndexesPage::OnClickNewIndexButton(UINT uNotifyCode, int nID, HWND wndCtl)
{
	adapter->createNewIndex();
	return 0;
}

LRESULT TableIndexesPage::OnClickDelIndexButton(UINT uNotifyCode, int nID, HWND wndCtl)
{
	adapter->deleteSelIndexes();
	return 0;
}