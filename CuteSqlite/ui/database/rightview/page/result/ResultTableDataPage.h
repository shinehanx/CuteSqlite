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

 * @file   ResultTableDataPage.h
 * @brief  Select the table and then show the list of table data
 * 
 * @author Xuehan Qin
 * @date   2023-05-22
 *********************************************************************/
#pragma once
#include <atltypes.h>
#include "ResultListPage.h"
#include "ui/common/image/QStaticImage.h"

class ResultTableDataPage : public ResultListPage 
{
public:
	
	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP_EX(ResultTableDataPage)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MESSAGE_HANDLER(Config::MSG_QLISTVIEW_SUBITEM_TEXT_CHANGE_ID, OnListViewSubItemTextChange)
		MESSAGE_HANDLER(Config::MSG_DATA_HAS_CHANGED_ID, OnHandleDataHasChanged)
		NOTIFY_HANDLER(Config::DATABASE_QUERY_LISTVIEW_ID, NM_DBLCLK, OnDbClickListView)
		COMMAND_HANDLER_EX(Config::LISTVIEW_NEW_ROW_BUTTON_ID, BN_CLICKED, OnClickNewRowButton)
		COMMAND_HANDLER_EX(Config::LISTVIEW_COPY_ROW_BUTTON_ID, BN_CLICKED, OnClickCopyRowButton)
		COMMAND_HANDLER_EX(Config::LISTVIEW_SAVE_BUTTON_ID, BN_CLICKED, OnClickSaveButton)
		COMMAND_HANDLER_EX(Config::LISTVIEW_DELETE_BUTTON_ID, BN_CLICKED, OnClickDeleteButton)
		COMMAND_HANDLER_EX(Config::LISTVIEW_CANCEL_BUTTON_ID, BN_CLICKED, OnClickCancelButton)
		CHAIN_MSG_MAP(ResultListPage)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
	ResultTableDataPage();

	virtual void setup(std::wstring & table);
	virtual void setup(QueryPageSupplier *supplier, std::wstring sql);
	void loadTableDatas();
protected:
	std::wstring table; // table name

	std::pair<int, int> subItemPos; // pair.first-iItem, pair.second-iSubItem
	CRect subItemRect;

	QStaticImage splitImage,splitImage2;
	QImageButton newRowButton;
	QImageButton copyRowButton;
	QImageButton saveButton;
	QImageButton deleteButton;
	QImageButton cancelButton;

	virtual void createOrShowUI();
	virtual void loadWindow();

	virtual void createOrShowListView(QListViewCtrl & win, CRect & clientRect);

	virtual void createOrShowToolBarElems(CRect & clientRect); // override
	virtual void doCreateOrShowToolBarSecondPaneElems(CRect &rect, CRect & clientRect);	 // override
	void doCreateOrShowToolBarThirdPaneElems(CRect &rect, CRect & clientRect);	 // extend 

	void enableToolButtonsAndDirty();
	void enableDataDirty();
	void enableSaveButton();
	void enableCancelButton();
	void enableDeleteButton();
	void enableDeleteButtonState(bool enabled);

	void doCancelFormView();

	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct); 
	virtual int OnDestroy();
	virtual void OnClickFilterButton(UINT uNotifyCode, int nID, HWND hwnd);
	virtual void OnClickRefreshButton(UINT uNotifyCode, int nID, HWND hwnd);

	virtual LRESULT OnClickListView(int idCtrl, LPNMHDR pnmh, BOOL &bHandled);
	virtual LRESULT OnClickListViewHeader(int idCtrl, LPNMHDR pnmh, BOOL &bHandled);


	LRESULT OnDbClickListView(int idCtrl, LPNMHDR pnmh, BOOL &bHandled);
	LRESULT OnListViewSubItemTextChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnHandleDataHasChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	
	LRESULT OnClickNewRowButton(UINT uNotifyCode, int nID, HWND wndCtl);

	void enableSelectAll();

	LRESULT OnClickCopyRowButton(UINT uNotifyCode, int nID, HWND wndCtl);
	LRESULT OnClickSaveButton(UINT uNotifyCode, int nID, HWND wndCtl);
	LRESULT OnClickDeleteButton(UINT uNotifyCode, int nID, HWND wndCtl);
	LRESULT OnClickCancelButton(UINT uNotifyCode, int nID, HWND wndCtl);

	
};
