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

 * @file   ResultListPage.h
 * @brief  Execute sql statement and show the list of query result
 * 
 * @author Xuehan Qin
 * @date   2023-05-22
 *********************************************************************/
#pragma once
#include <string>
#include <atlctrlx.h>
#include "common/Config.h"

#include "ui/common/button/QImageButton.h"
#include "ui/common/button/QDropButton.h"
#include "ui/common/checkbox/QCheckBox.h"
#include "ui/database/rightview/common/QTabPage.h"
#include "ui/database/rightview/page/supply/QueryPageSupplier.h"
#include "ui/database/rightview/page/result/adapter/ResultListPageAdapter.h"
#include "ui/database/rightview/page/result/form/RowDataFormView.h"
#include "ui/common/listview/QListViewCtrl.h"

class ResultListPage : public QTabPage<QueryPageSupplier> {
public:
	BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP_EX(ResultListPage)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		NOTIFY_HANDLER(Config::DATABASE_QUERY_LISTVIEW_ID, NM_CLICK, OnClickListView)
		NOTIFY_HANDLER(Config::DATABASE_QUERY_LISTVIEW_ID, NM_RCLICK, OnRightClickListView)
		NOTIFY_HANDLER(Config::DATABASE_QUERY_LISTVIEW_ID, LVN_ITEMCHANGED, OnListViewItemChange)
		NOTIFY_HANDLER(Config::DATABASE_QUERY_LISTVIEW_ID, LVN_GETDISPINFO, OnGetListViewData)
		NOTIFY_HANDLER(Config::DATABASE_QUERY_LISTVIEW_ID, LVN_ODCACHEHINT, OnPrepareListViewData)
		NOTIFY_HANDLER(Config::DATABASE_QUERY_LISTVIEW_ID, LVN_ODFINDITEM, OnFindListViewData)
		NOTIFY_HANDLER(Config::DATABASE_QUERY_LISTVIEW_ID, LVN_COLUMNCLICK, OnClickListViewHeader)

		COMMAND_HANDLER_EX(Config::LISTVIEW_EXPORT_BUTTON_ID, BN_CLICKED, OnClickExportButton)
		COMMAND_HANDLER_EX(Config::LISTVIEW_COPY_BUTTON_ID, BN_CLICKED, OnClickCopyButton)
		COMMAND_HANDLER_EX(Config::LISTVIEW_FILTER_BUTTON_ID, BN_CLICKED, OnClickFilterButton)
		COMMAND_HANDLER_EX(Config::LISTVIEW_REFRESH_BUTTON_ID, BN_CLICKED, OnClickRefreshButton)
		COMMAND_HANDLER_EX(Config::LISTVIEW_FORMVIEW_CHECKBOX_ID, BN_CLICKED, OnClickFormViewCheckBox)
		COMMAND_HANDLER_EX(Config::LISTVIEW_LIMIT_CHECKBOX_ID, BN_CLICKED, OnClickLimitCheckBox)
		COMMAND_ID_HANDLER_EX(Config::COPY_ALL_ROWS_TO_CLIPBOARD_MEMU_ID, OnClickCopyAllRowsToClipboardMenu)
		COMMAND_ID_HANDLER_EX(Config::COPY_SEL_ROWS_TO_CLIPBOARD_MEMU_ID, OnClickCopySelRowsToClipboardMenu)
		COMMAND_ID_HANDLER_EX(Config::COPY_ALL_ROWS_AS_SQL_MEMU_ID, OnClickCopyAllRowsAsSqlMenu)
		COMMAND_ID_HANDLER_EX(Config::COPY_SEL_ROWS_AS_SQL_MEMU_ID, OnClickCopySelRowsAsSqlMenu)

		MESSAGE_HANDLER(Config::MSG_QLISTVIEW_COLUMN_CLICK_ID, OnClickListViewColumn)
		
		MSG_WM_CTLCOLORSTATIC(OnCtlColorStatic)
		MSG_WM_CTLCOLORLISTBOX(OnCtlColorListBox)
		MSG_WM_CTLCOLOREDIT(OnCtlColorEdit)
		CHAIN_MSG_MAP(QPage)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	virtual void setup(QueryPageSupplier * supplier, std::wstring & sql);
	void loadListView();
protected:
	bool isNeedReload = true;
	std::wstring sql;
	int rowCount = 0;
	std::wstring settingPrefix;

	COLORREF buttonColor = RGB(238, 238, 238);
	HFONT textFont = nullptr;


	// toolbar button
	QImageButton exportButton;
	QDropButton copyButton;
	CMenu copyMenu;
	CComboBox readWriteComboBox;
	CButton formViewCheckBox;
	QImageButton filterButton;
	QImageButton refreshButton;
	CButton limitCheckBox;
	CStatic offsetLabel;
	CEdit offsetEdit;
	CStatic rowsLabel;
	CEdit rowsEdit;

	QListViewCtrl listView;
	RowDataFormView formView;
	bool formViewReadOnly = true;
	CMultiPaneStatusBarCtrl statusBar;

	ResultListPageAdapter * adapter = nullptr;
	DatabaseService * databaseService = DatabaseService::getInstance();

	virtual void createOrShowUI();
	virtual void loadWindow();
	
	CRect getLeftListRect(CRect & clientRect);
	CRect getRightFormRect(CRect & clientRect);
	CRect getBottomStatusRect(CRect & clientRect);

	virtual void createOrShowToolBarElems(CRect & clientRect);
	virtual void doCreateOrShowToolBarFirstPaneElems(CRect &rect, CRect & clientRect);
	virtual void doCreateOrShowToolBarSecondPaneElems(CRect &rect, CRect & clientRect);
	virtual void doCreateOrShowToolBarRightPaneElems(CRect &rect, CRect & clientRect);

	virtual void createOrShowListView(QListViewCtrl & win, CRect & clientRect);
	void createOrShowFormView(RowDataFormView & win, CRect & clientRect);
	void createOrShowStatusBar(CMultiPaneStatusBarCtrl & win, CRect & clientRect);

	void createCopyMenu();
	void popupCopyMenu(CPoint & pt);

	void loadReadWriteComboBox();
	void loadFormViewCheckBox();
	LimitParams loadLimitElems();
	void saveLimitParams();

	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual int OnDestroy();
	virtual void paintItem(CDC & dc, CRect & paintRect);

	virtual LRESULT OnClickListView(int idCtrl, LPNMHDR pnmh, BOOL &bHandled);
	LRESULT OnRightClickListView(int idCtrl, LPNMHDR pnmh, BOOL &bHandled);
	LRESULT OnGetListViewData(int idCtrl, LPNMHDR pnmh, BOOL &bHandled);
	LRESULT OnPrepareListViewData(int idCtrl, LPNMHDR pnmh, BOOL &bHandled);
	LRESULT OnFindListViewData(int idCtrl, LPNMHDR pnmh, BOOL &bHandled);
	LRESULT OnClickListViewHeader(int idCtrl, LPNMHDR pnmh, BOOL &bHandled);
	LRESULT OnListViewItemChange(int idCtrl, LPNMHDR pnmh, BOOL &bHandled);

	void OnClickExportButton(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickCopyButton(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickFormViewCheckBox(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickLimitCheckBox(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickCopyAllRowsToClipboardMenu(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickCopySelRowsToClipboardMenu(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickCopyAllRowsAsSqlMenu(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickCopySelRowsAsSqlMenu(UINT uNotifyCode, int nID, HWND hwnd);
	virtual void OnClickFilterButton(UINT uNotifyCode, int nID, HWND hwnd);
	virtual void OnClickRefreshButton(UINT uNotifyCode, int nID, HWND hwnd);

	virtual LRESULT OnClickListViewColumn(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	HBRUSH OnCtlColorStatic(HDC hdc, HWND hwnd);
	HBRUSH OnCtlColorListBox(HDC hdc, HWND hwnd);
	HBRUSH OnCtlColorEdit(HDC hdc, HWND hwnd);

	bool isShowFormView();

	void changeFilterButtonStatus(bool hasRedIcon);

	// display the result rows and exec time in the status bar
	void displayStatusBarPanels(ResultInfo & resultInfo);
	void displayRuntimeSql();
	void displayDatabase();
	void displayResultRows();
	void displayExecTime(ResultInfo & resultInfo);
};
