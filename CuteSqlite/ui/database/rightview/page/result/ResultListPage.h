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
#include "common/Config.h"
#include "ui/common/page/QPage.h"
#include "ui/common/button/QImageButton.h"
#include "ui/common/button/QDropButton.h"
#include "ui/common/checkbox/QCheckBox.h"
#include "ui/database/supplier/DatabaseSupplier.h"
#include "ui/database/rightview/page/result/adapter/ResultListPageAdapter.h"
#include "ui/database/rightview/page/result/form/RowDataFormView.h"

class ResultListPage : public QPage {
public:
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
		COMMAND_HANDLER_EX(Config::LISTVIEW_FORMVIEW_CHECKBOX_ID, BN_CLICKED, OnClickFormViewCheckBox)
		COMMAND_ID_HANDLER_EX(Config::COPY_ALL_ROWS_TO_CLIPBOARD_MEMU_ID, OnClickCopyAllRowsToClipboardMenu)
		COMMAND_ID_HANDLER_EX(Config::COPY_SEL_ROWS_TO_CLIPBOARD_MEMU_ID, OnClickCopySelRowsToClipboardMenu)
		COMMAND_ID_HANDLER_EX(Config::COPY_ALL_ROWS_AS_SQL_MEMU_ID, OnClickCopyAllRowsAsSqlMenu)
		COMMAND_ID_HANDLER_EX(Config::COPY_SEL_ROWS_AS_SQL_MEMU_ID, OnClickCopySelRowsAsSqlMenu)
		
		MSG_WM_CTLCOLORSTATIC(OnCtlColorStatic)
		MSG_WM_CTLCOLORLISTBOX(OnCtlColorListBox)
		MSG_WM_CTLCOLOREDIT(OnCtlColorEdit)
		CHAIN_MSG_MAP(QPage)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	void setup(std::wstring & sql);
protected:
	bool isNeedReload = true;
	std::wstring sql;
	int rowCount = 0;

	COLORREF buttonColor = RGB(238, 238, 238);
	HFONT textFont = nullptr;

	CImageList imageList;
	HBITMAP checkNoBitmap = nullptr;
	HBITMAP checkYesBitmap = nullptr;

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
	CStatic limitLabel;
	CEdit limitEdit;

	CListViewCtrl listView;
	RowDataFormView formView;

	ResultListPageAdapter * adapter = nullptr;
	DatabaseSupplier * supplier = DatabaseSupplier::getInstance();

	virtual void createOrShowUI();
	virtual void loadWindow();

	CRect getLeftListRect(CRect & clientRect);
	CRect getRightFormRect(CRect & clientRect);

	void createImageList();
	void createOrShowToolBarElems(CRect & clientRect);
	void createOrShowListView(CListViewCtrl & win, CRect & clientRect);
	void createOrShowFormView(RowDataFormView & win, CRect & clientRect);

	void createCopyMenu();
	void popupCopyMenu(CPoint & pt);

	void loadReadWriteComboBox();
	void loadFormViewCheckBox();

	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual int OnDestroy();
	virtual void paintItem(CDC & dc, CRect & paintRect);

	LRESULT OnClickListView(int idCtrl, LPNMHDR pnmh, BOOL &bHandled);
	LRESULT OnRightClickListView(int idCtrl, LPNMHDR pnmh, BOOL &bHandled);
	LRESULT OnGetListViewData(int idCtrl, LPNMHDR pnmh, BOOL &bHandled);
	LRESULT OnPrepareListViewData(int idCtrl, LPNMHDR pnmh, BOOL &bHandled);
	LRESULT OnFindListViewData(int idCtrl, LPNMHDR pnmh, BOOL &bHandled);
	LRESULT OnClickListViewHeader(int idCtrl, LPNMHDR pnmh, BOOL &bHandled);
	LRESULT OnListViewItemChange(int idCtrl, LPNMHDR pnmh, BOOL &bHandled);

	void OnClickExportButton(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickCopyButton(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickFormViewCheckBox(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickCopyAllRowsToClipboardMenu(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickCopySelRowsToClipboardMenu(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickCopyAllRowsAsSqlMenu(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickCopySelRowsAsSqlMenu(UINT uNotifyCode, int nID, HWND hwnd);

	HBRUSH OnCtlColorStatic(HDC hdc, HWND hwnd);
	HBRUSH OnCtlColorListBox(HDC hdc, HWND hwnd);
	HBRUSH OnCtlColorEdit(HDC hdc, HWND hwnd);

	bool isShowFormView();
};
