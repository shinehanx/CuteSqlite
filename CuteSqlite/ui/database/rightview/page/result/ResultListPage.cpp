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
#include "utils/SqlUtil.h"
#include "utils/PerformUtil.h"
#include "core/common/Lang.h"
#include "ui/common/message/QPopAnimate.h"
#include "ui/common/message/QMessageBox.h"
#include "ui/common/QWinCreater.h" 
#include "ui/database/rightview/page/result/dialog/ExportResultDialog.h"
#include "ui/database/rightview/page/result/dialog/ResultFilterDialog.h"

#define ROW_FORM_VIEW_WIDTH 320
#define RESULT_STATUS_BAR_HEIGHT 22

BOOL ResultListPage::PreTranslateMessage(MSG* pMsg)
{
	UINT	msg = pMsg->message;
	WPARAM	key = pMsg->wParam;
	HWND	hwnd = pMsg->hwnd;
	if ((hwnd == listView.m_hWnd || hwnd == listView.GetHeader().m_hWnd) && msg == WM_KEYDOWN
		&& ((GetKeyState(VK_CONTROL) & 0xFF00) == 0xFF00)) { //CTRL + C 
		if (key == _T('C')) {
			adapter->copyAllRowsToClipboard();
			QPopAnimate::success(S(L"copy-to-clipboard"));
		}
	}

	if (listView.IsWindow() && listView.PreTranslateMessage(pMsg)) {
		return TRUE;
	}

	if (WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST) {
		HWND queryPageHwnd = nullptr;
		if (IsWindow()) {
			// class chain : ResultListPage(this)->QTabView->ResultTabView->CHorSplitterWindow->QueryPage
			queryPageHwnd = GetParent().GetParent().GetParent().GetParent().m_hWnd; // QueryPage
		}
		if (supplier->getActiveResultTabPageHwnd() == m_hWnd 
			&& databaseSupplier->isActivePage(queryPageHwnd)
			&& m_hAccel && ::TranslateAccelerator(m_hWnd, m_hAccel, pMsg)) {
			return TRUE;
		}
	}
	return FALSE;
}

void ResultListPage::setup(QueryPageSupplier * supplier, std::wstring & sql)
{
	this->supplier = supplier;
	this->sql = sql;
}

void ResultListPage::createOrShowUI()
{
	QPage::createOrShowUI();

	CRect clientRect;
	GetClientRect(clientRect);
	createOrShowToolBarElems(clientRect);
	createOrShowListView(listView, clientRect);
	createOrShowFormView(formView, clientRect);
	createOrShowStatusBar(statusBar, clientRect);
}

/**
 * create or show buttons and other elments on page toolbar.
 * 
 * @param clientRect The page's client rect
 */
void ResultListPage::createOrShowToolBarElems(CRect & clientRect)
{
	CRect topbarRect = getTopRect(clientRect);
	int x = 5, y = 5, w = PAGE_BUTTON_WIDTH, h = PAGE_BUTTON_HEIGHT;
	CRect rect(x, y, x + w, y + h);
	doCreateOrShowToolBarFirstPaneElems(rect, clientRect);


	doCreateOrShowToolBarSecondPaneElems(rect, clientRect);

		
	// create or show the topbar elements
	doCreateOrShowToolBarRightPaneElems(rect, clientRect);

}


/**
 * Create or show first pane elements on page toolbar.
 * 
 * @param rect [in,out]
 * @param clientRect
 */
void ResultListPage::doCreateOrShowToolBarFirstPaneElems(CRect &rect, CRect & clientRect)
{
	// create or show button
	std::wstring imgDir = ResourceUtil::getProductImagesDir();
	std::wstring normalImagePath, pressedImagePath;
	if (!exportButton.IsWindow()) {
		normalImagePath = imgDir + L"database\\list\\button\\export-button-normal.png";
		pressedImagePath = imgDir + L"database\\list\\button\\export-button-pressed.png";
		exportButton.SetIconPath(normalImagePath, pressedImagePath);
		exportButton.SetBkgColors(buttonColor, buttonColor, buttonColor);
	}
	QWinCreater::createOrShowButton(m_hWnd, exportButton, Config::LISTVIEW_EXPORT_BUTTON_ID, L"", rect, clientRect);
	exportButton.SetToolTip(S(L"export-result-as"));

	rect.OffsetRect(16 + 10, 0);
	rect.InflateRect(0, 0, 8, 0);
	if (!copyButton.IsWindow()) {
		normalImagePath = imgDir + L"database\\list\\button\\copy-button-normal.png";
		pressedImagePath = imgDir + L"database\\list\\button\\copy-button-pressed.png";
		copyButton.SetIconPath(normalImagePath, pressedImagePath);
		copyButton.SetBkgColors(buttonColor, buttonColor, buttonColor);
	}
	QWinCreater::createOrShowButton(m_hWnd, copyButton, Config::LISTVIEW_COPY_BUTTON_ID, L"", rect, clientRect);
	copyButton.SetToolTip(S(L"copy-result-data"));
}

/**
 *  Create or show second pane elements on page toolbar.
 * 
 * @param rect  [in,out]
 * @param clientRect
 */
void ResultListPage::doCreateOrShowToolBarSecondPaneElems(CRect &rect, CRect & clientRect)
{
	rect.OffsetRect(24 + 10, 0);
	rect.InflateRect(0, 2, 80, -2);
	QWinCreater::createOrShowComboBox(m_hWnd, readWriteComboBox, Config::LISTVIEW_READ_WRITE_COMBOBOX_ID, rect, clientRect);
	readWriteComboBox.SetFont(textFont);

	rect.OffsetRect(120 + 20, 0);
	rect.InflateRect(0, 0, 80, 4);
	QWinCreater::createOrShowCheckBox(m_hWnd, formViewCheckBox, Config::LISTVIEW_FORMVIEW_CHECKBOX_ID, S(L"show-form-view"), rect, clientRect);
}

/**
 *  Create or show right pane elements on page toolbar.
 * 
 * @param rect  [in,out]
 * @param clientRect
 */
void ResultListPage::doCreateOrShowToolBarRightPaneElems(CRect & rect, CRect & clientRect)
{
	std::wstring imgDir = ResourceUtil::getProductImagesDir();
	CRect topbarRect = getTopRect(clientRect);
	std::wstring normalImagePath, pressedImagePath;
	int x = topbarRect.right - 350, y = 5, w = PAGE_BUTTON_WIDTH, h = PAGE_BUTTON_HEIGHT;
	rect = { x , y , x + w, y + h };
	if (!filterButton.IsWindow()) {
		normalImagePath = imgDir + L"database\\list\\button\\filter-button-normal.png";
		pressedImagePath = imgDir + L"database\\list\\button\\filter-button-pressed.png";
		filterButton.SetIconPath(normalImagePath, pressedImagePath);
		filterButton.SetBkgColors(buttonColor, buttonColor, buttonColor);
	}
	QWinCreater::createOrShowButton(m_hWnd, filterButton, Config::LISTVIEW_FILTER_BUTTON_ID, L"", rect, clientRect);
	filterButton.SetToolTip(S(L"filter"));

	rect.OffsetRect(16 + 10, 0);
	if (!refreshButton.IsWindow()) {
		normalImagePath = imgDir + L"database\\list\\button\\refresh-button-normal.png";
		pressedImagePath = imgDir + L"database\\list\\button\\refresh-button-pressed.png";
		refreshButton.SetIconPath(normalImagePath, pressedImagePath);
		refreshButton.SetBkgColors(buttonColor, buttonColor, buttonColor);
	}
	QWinCreater::createOrShowButton(m_hWnd, refreshButton, Config::LISTVIEW_REFRESH_BUTTON_ID, L"", rect, clientRect);
	refreshButton.SetToolTip(S(L"refresh"));

	rect.OffsetRect(16 + 20, 2);
	rect.InflateRect(0, 0, 50, 0);
	QWinCreater::createOrShowCheckBox(m_hWnd, limitCheckBox, Config::LISTVIEW_LIMIT_CHECKBOX_ID, S(L"limit-rows"), rect, clientRect);

	rect.OffsetRect(60 + 5, 0);
	rect.InflateRect(0, 0, -16, 4);
	QWinCreater::createOrShowLabel(m_hWnd, offsetLabel, S(L"offset").append(L":"), rect, clientRect, SS_RIGHT); 

	rect.OffsetRect(50 + 2, -2);
	QWinCreater::createOrShowEdit(m_hWnd, offsetEdit, 0, L"0", rect, clientRect, ES_NUMBER, false);

	rect.OffsetRect(50 + 5, 2);
	QWinCreater::createOrShowLabel(m_hWnd, rowsLabel, S(L"rows").append(L":"), rect, clientRect, SS_RIGHT);

	rect.OffsetRect(50 + 2, -2);
	QWinCreater::createOrShowEdit(m_hWnd, rowsEdit, 0, L"1000", rect, clientRect, ES_NUMBER, false);
}

void ResultListPage::loadWindow()
{
	QPage::loadWindow();

	loadReadWriteComboBox();
	loadFormViewCheckBox();
	LimitParams limitParams = loadLimitElems();

	if (!isNeedReload || sql.empty()) {
		return;
	}
	isNeedReload = false;	

	// load the datas from dbfile to show in list view
	loadListView();

}

void ResultListPage::loadListView()
{
	uint64_t userDbId = databaseSupplier->getSelectedUserDbId();
	if (!userDbId) {
		QPopAnimate::warn(m_hWnd, S(L"no-select-userdb"));
		return;
	}
	
	auto _begin = PerformUtil::begin();
	try {
		rowCount = adapter->loadListView(userDbId, sql);
	} catch (QSqlExecuteException &ex) {
		QPopAnimate::report(ex);
		rowCount = 0;
	}
	
	ResultInfo & resultInfo = adapter->getRuntimeResultInfo();
	resultInfo.totalTime = PerformUtil::end(_begin);

	// display status bar panels 
	displayStatusBarPanels(resultInfo);

	int checkedFormView = formViewCheckBox.GetCheck();
	if (checkedFormView) {
		formView.clearLabelsAndEdits();
	}
}

CRect ResultListPage::getLeftListRect(CRect & clientRect)
{
	CRect & pageRect = getPageRect(clientRect);
	bool checked = isShowFormView();
	if (checked) {
		return { pageRect.left, pageRect.top, pageRect.right - ROW_FORM_VIEW_WIDTH - 1, pageRect.bottom - RESULT_STATUS_BAR_HEIGHT};
	} else {
		return { pageRect.left, pageRect.top, pageRect.right, pageRect.bottom - RESULT_STATUS_BAR_HEIGHT};
	}
}

CRect ResultListPage::getRightFormRect(CRect & clientRect)
{
	CRect & pageRect = getPageRect(clientRect);
	bool checked = isShowFormView();
	if (checked) {
		return { pageRect.right - ROW_FORM_VIEW_WIDTH, pageRect.top, pageRect.right, pageRect.bottom - RESULT_STATUS_BAR_HEIGHT };
	} else {
		return { 0, 0, 0, 0 };
	}
}

CRect ResultListPage::getBottomStatusRect(CRect & clientRect)
{
	int x = 0, y = clientRect.bottom - RESULT_STATUS_BAR_HEIGHT, w = clientRect.Width(), h = RESULT_STATUS_BAR_HEIGHT;
	return { x, y, x + w, y + h };
}

void ResultListPage::paintItem(CDC & dc, CRect & paintRect)
{
	
}

void ResultListPage::createOrShowListView(QListViewCtrl & win, CRect & clientRect)
{
	CRect & rect = getLeftListRect(clientRect);
	if (IsWindow() && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_TABSTOP | WS_VISIBLE | LVS_ALIGNLEFT | LVS_REPORT | LVS_SHOWSELALWAYS | WS_BORDER | LVS_OWNERDATA | LVS_OWNERDRAWFIXED;
		win.Create(m_hWnd, rect,NULL,dwStyle , // | LVS_OWNERDATA
			0, Config::DATABASE_QUERY_LISTVIEW_ID );
		win.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER | LVS_EX_CHECKBOXES);
		win.setItemHeight(22);
		adapter = new ResultListPageAdapter(m_hWnd, &win, QUERY_SQL_RESULT);
	} else if (IsWindow() && win.IsWindow() && clientRect.Width() > 1) {
		win.MoveWindow(rect);
		win.ShowWindow(true);
	}
}

void ResultListPage::createOrShowFormView(RowDataFormView & win, CRect & clientRect)
{
	CRect & rect = getRightFormRect(clientRect);
	
	if (!rect.Width()) {
		if (win.IsWindow()) {
			win.DestroyWindow();
		}
		return;
	}
	if (IsWindow() && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_BORDER;
		win.setup(adapter);
		win.Create(m_hWnd, rect, NULL, dwStyle);
		win.loadFormData(formViewReadOnly);
	} else if (IsWindow() && win.IsWindow() && clientRect.Width() > 1) {
		win.MoveWindow(rect); 
		win.ShowWindow(true);
	}
}

void ResultListPage::createOrShowStatusBar(CMultiPaneStatusBarCtrl & win, CRect & clientRect)
{
	CRect rect = getBottomStatusRect(clientRect);

	if (IsWindow() && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | SBARS_SIZEGRIP | SBT_TOOLTIPS;
		win.Create(m_hWnd, sql.c_str(), dwStyle);
		int anPanes[] = {
			Config::RESULT_STATUSBAR_SQL_PANE_ID, 
			Config::RESULT_STATUSBAR_DATABASE_PANE_ID,
			Config::RESULT_STATUSBAR_ROWS_PANE_ID,
			Config::RESULT_STATUSBAR_EXEC_TIME_PANE_ID,
		};
		 win.SetPanes (anPanes, 4, false);
		 win.SetPaneWidth(Config::RESULT_STATUSBAR_EXEC_TIME_PANE_ID, 350);
		 win.SetPaneWidth(Config::RESULT_STATUSBAR_DATABASE_PANE_ID, 150);
		 win.SetPaneWidth(Config::RESULT_STATUSBAR_ROWS_PANE_ID, 100);
		 win.SetPaneWidth(Config::RESULT_STATUSBAR_SQL_PANE_ID, rect.Width() - 600);
	} else if (IsWindow() && win.IsWindow() && clientRect.Width() > 1) {
		win.MoveWindow(rect);
		win.ShowWindow(true);
		win.SetPaneWidth(Config::RESULT_STATUSBAR_EXEC_TIME_PANE_ID, 350);
		win.SetPaneWidth(Config::RESULT_STATUSBAR_ROWS_PANE_ID, 100);
		win.SetPaneWidth(Config::RESULT_STATUSBAR_DATABASE_PANE_ID, 150);
		win.SetPaneWidth(Config::RESULT_STATUSBAR_SQL_PANE_ID, rect.Width() - 600);
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

void ResultListPage::loadReadWriteComboBox()
{
	if (readWriteComboBox.IsWindow()) {
		readWriteComboBox.ResetContent();
		readWriteComboBox.AddString(S(L"read-only").c_str());
		readWriteComboBox.SetCurSel(0);
	}	
}

void ResultListPage::loadFormViewCheckBox()
{
	bool checked = isShowFormView();
	formViewCheckBox.SetCheck(checked);
}

LimitParams ResultListPage::loadLimitElems()
{
	bool hasLimitClause = false;
	if (!sql.empty() && SqlUtil::hasLimitClause(sql)) {
		hasLimitClause = true;
	}	
	limitCheckBox.EnableWindow(!hasLimitClause);

	std::wstring limitChecked = SettingService::getInstance()->getSysInit(settingPrefix + L"limit-checked");
	std::wstring offset = SettingService::getInstance()->getSysInit(settingPrefix + L"limit-offset");
	std::wstring rows = SettingService::getInstance()->getSysInit(settingPrefix + L"limit-rows");
	offset = StringUtil::isDigit(offset) ? offset : L"0";
	rows = StringUtil::isDigit(rows) ? rows : L"1000";

	bool isLimitChecked = limitChecked == L"true";
	limitCheckBox.SetCheck(isLimitChecked);
	offsetEdit.SetWindowText(offset.c_str());
	rowsEdit.SetWindowText(rows.c_str());

	int checked = limitCheckBox.GetCheck();
	if (checked && !hasLimitClause) {
		offsetLabel.EnableWindow(true);
		offsetEdit.EnableWindow(true);
		rowsLabel.EnableWindow(true);
		rowsEdit.EnableWindow(true);

		return { true, std::stoi(offset), std::stoi(rows) };
	} else {
		offsetLabel.EnableWindow(false);
		offsetEdit.EnableWindow(false);
		rowsLabel.EnableWindow(false);
		rowsEdit.EnableWindow(false);

		return { false, std::stoi(offset), std::stoi(rows) };
	}
}

void ResultListPage::saveLimitParams()
{
	int checked = limitCheckBox.GetCheck();
	std::wstring val = checked ? L"true" : L"false";
	SettingService::getInstance()->setSysInit(settingPrefix + L"limit-checked", val);

	if (checked) {
		CString offset, rows;
		offsetEdit.GetWindowText(offset);
		rowsEdit.GetWindowText(rows);
		SettingService::getInstance()->setSysInit(settingPrefix + L"limit-offset", offset.GetString());
		SettingService::getInstance()->setSysInit(settingPrefix + L"limit-rows", rows.GetString());
	}
}

bool ResultListPage::isShowFormView()
{
	std::wstring showFormView = SettingService::getInstance()->getSysInit(settingPrefix + L"show-form-view");
	return showFormView == L"true" ? true : false;
}

void ResultListPage::clearFormView()
{
	bool checkedFormView = isShowFormView();
 	if (checkedFormView && listView.GetSelectedCount() == 0) {
 		formView.clearLabelsAndEdits();
 	}
}

/**
 * change the filter button image to red icon.
 * 
 * @param hasRedIcon
 */
void ResultListPage::changeFilterButtonStatus(bool hasRedIcon)
{
	std::wstring imgDir = ResourceUtil::getProductImagesDir();
	if (hasRedIcon) {
		std::wstring normalImagePath = imgDir + L"database\\list\\button\\filter-red-button-normal.png"; 
		std::wstring pressedImagePath = imgDir + L"database\\list\\button\\filter-red-button-pressed.png";
		filterButton.SetIconPath(normalImagePath, pressedImagePath);
		filterButton.SetToolTip(S(L"filter-in-use"));
		filterButton.Invalidate(FALSE);
	}else {
		std::wstring normalImagePath = imgDir + L"database\\list\\button\\filter-button-normal.png"; 
		std::wstring pressedImagePath = imgDir + L"database\\list\\button\\filter-button-pressed.png";
		filterButton.SetIconPath(normalImagePath, pressedImagePath);
		filterButton.SetToolTip(S(L"filter"));
		filterButton.Invalidate(FALSE); 
	}
}

int ResultListPage::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	bool ret = QPage::OnCreate(lpCreateStruct);

	HINSTANCE ins = ModuleHelper::GetModuleInstance();
	m_hAccel = ::LoadAccelerators(ins, MAKEINTRESOURCE(RESULT_LIST_PAGE_ACCEL));

	textFont = FT(L"form-text-size");
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
	if (rowsLabel.IsWindow()) rowsLabel.DestroyWindow();
	if (rowsEdit.IsWindow()) rowsEdit.DestroyWindow();
	if (copyMenu.IsMenu()) copyMenu.DestroyMenu();

	if (listView.IsWindow()) listView.DestroyWindow();
	if (formView.IsWindow()) formView.DestroyWindow();
	if (statusBar.IsWindow()) {
		int anPanes[] = {
			Config::RESULT_STATUSBAR_SQL_PANE_ID
		};
		statusBar.SetPanes(anPanes, 1);
		statusBar.DestroyWindow();
	}

	if (adapter) {
		delete adapter;
		adapter = nullptr;
	}

	return ret;
}

LRESULT ResultListPage::OnClickListView(int idCtrl, LPNMHDR pnmh, BOOL &bHandled)
{
	auto ptr = (LPNMITEMACTIVATE)pnmh;
// 	bool checkedFormView = isShowFormView();
// 	if (checkedFormView) {
// 		formView.loadFormData(formViewReadOnly);
// 	}
	
	listView.changeAllItemsCheckState();
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
	adapter->fillDataInListViewSubItem(plvdi);

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
	bHandled = false;

	LPNMLISTVIEW headerPtr = (LPNMLISTVIEW)pnmh; 

	// Reference url:https://learn.microsoft.com/zh-cn/windows/win32/controls/lvn-columnclick
	if (headerPtr->iSubItem  == 0) {
		return 0;
	}
	adapter->sortListView(headerPtr->iSubItem);
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
			bool checkedFormView = isShowFormView();
 			if (checkedFormView) {
 				formView.loadFormData(formViewReadOnly);
 			}
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
			std::wstring exportPath = SettingService::getInstance()->getSysInit(L"export-path");
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
	CPoint pt(rect.left, rect.bottom + 5);
	popupCopyMenu(pt);
}

void ResultListPage::OnClickFormViewCheckBox(UINT uNotifyCode, int nID, HWND hwnd)
{
	int checked = formViewCheckBox.GetCheck();
	bool isShowFormView = !checked;
	formViewCheckBox.SetCheck(isShowFormView);
	std::wstring val = isShowFormView ? L"true" : L"false";
	SettingService::getInstance()->setSysInit(settingPrefix + L"show-form-view", val);

	CRect clientRect;
	GetClientRect(clientRect);
	if (isShowFormView) {
		createOrShowFormView(formView, clientRect);
	} else {
		if (formView.IsWindow()) formView.DestroyWindow();
	}
	createOrShowListView(listView, clientRect);
}

void ResultListPage::OnClickLimitCheckBox(UINT uNotifyCode, int nID, HWND hwnd)
{
	int newChecked = !limitCheckBox.GetCheck();
	limitCheckBox.SetCheck(newChecked);
	if (newChecked) {
		offsetLabel.EnableWindow(true);
		offsetEdit.EnableWindow(true);
		rowsLabel.EnableWindow(true);
		rowsEdit.EnableWindow(true);
	} else {
		offsetLabel.EnableWindow(false);
		offsetEdit.EnableWindow(false);
		rowsLabel.EnableWindow(false);
		rowsEdit.EnableWindow(false);
	}
	saveLimitParams();
}

void ResultListPage::OnClickCopyAllRowsToClipboardMenu(UINT uNotifyCode, int nID, HWND hwnd)
{
	adapter->copyAllRowsToClipboard();
	QPopAnimate::success(S(L"copy-to-clipboard"));
}

void ResultListPage::OnClickCopySelRowsToClipboardMenu(UINT uNotifyCode, int nID, HWND hwnd)
{
	adapter->copySelRowsToClipboard();
	QPopAnimate::success(S(L"copy-to-clipboard"));
}

void ResultListPage::OnClickCopyAllRowsAsSqlMenu(UINT uNotifyCode, int nID, HWND hwnd)
{
	adapter->copyAllRowsAsSql();
	QPopAnimate::success(S(L"copy-to-clipboard"));
}

void ResultListPage::OnClickCopySelRowsAsSqlMenu(UINT uNotifyCode, int nID, HWND hwnd)
{
	adapter->copySelRowsAsSql();
	QPopAnimate::success(S(L"copy-to-clipboard"));
}

void ResultListPage::OnClickFilterButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	saveLimitParams();

	CRect btnWinRect;
	filterButton.GetWindowRect(btnWinRect);
	ResultFilterDialog resultFilterDialog(m_hWnd, adapter, btnWinRect);
	
	if (resultFilterDialog.DoModal(m_hWnd) == Config::QDIALOG_YES_BUTTON_ID) {
		auto _bt = PerformUtil::begin();
		try {
			rowCount = adapter->loadFilterListView();
		} catch (QSqlExecuteException &ex) {
			rowCount = 0;
			QPopAnimate::report(ex);
		}
		
		ResultInfo & resultInfo = adapter->getRuntimeResultInfo();
		resultInfo.totalTime = PerformUtil::end(_bt);
		// display status bar panels 
		displayStatusBarPanels(resultInfo);
	}
	
	bool hasRedIcon = !adapter->isRuntimeFiltersEmpty();
	// change the filterButton to red
	changeFilterButtonStatus(hasRedIcon);
	clearFormView();
}

void ResultListPage::OnClickRefreshButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	saveLimitParams();
	auto _bt = PerformUtil::begin();
	try {
		rowCount = adapter->loadFilterListView();
	} catch (QSqlExecuteException &ex) {
		QPopAnimate::report(ex);
		rowCount = 0;
	}
	ResultInfo & resultInfo = adapter->getRuntimeResultInfo();
	resultInfo.totalTime = PerformUtil::end(_bt);

	// display status bar panels 
	displayStatusBarPanels(resultInfo);
	clearFormView();
}

HBRUSH ResultListPage::OnCtlColorStatic(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, topbarColor);
	::SelectObject(hdc, textFont);
	return topbarBrush.m_hBrush;
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


void ResultListPage::displayStatusBarPanels(ResultInfo & resultInfo)
{
	adapter->sendExecSqlMessage(resultInfo);
	displayRuntimeSql();
	displayDatabase();
	displayResultRows();
	displayExecTime(resultInfo);
}

void ResultListPage::displayRuntimeSql()
{
	statusBar.SetPaneText(Config::RESULT_STATUSBAR_SQL_PANE_ID, adapter->getRuntimeSql().c_str());
}

void ResultListPage::displayDatabase()
{
	if (!adapter->getRuntimeUserDbId()) {
		return;
	}
	try {
		UserDb userDb = databaseService->getUserDb(adapter->getRuntimeUserDbId());
		statusBar.SetPaneText(Config::RESULT_STATUSBAR_DATABASE_PANE_ID, userDb.name.c_str());
	} catch (QSqlExecuteException &ex) {
		Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::report(ex);
	}
}

void ResultListPage::displayResultRows()
{
	CString resultRows;
	resultRows.Format(L"%d rows", rowCount);
	statusBar.SetPaneText(Config::RESULT_STATUSBAR_ROWS_PANE_ID, resultRows);
}

void ResultListPage::displayExecTime(ResultInfo & resultInfo)
{
	CString execTime;
	execTime.Format(L"Exec:%s Transfer:%s Total:%s", 
		resultInfo.execTime.c_str(), resultInfo.transferTime.c_str(), resultInfo.totalTime.c_str());
	statusBar.SetPaneText(Config::RESULT_STATUSBAR_EXEC_TIME_PANE_ID, execTime);
}