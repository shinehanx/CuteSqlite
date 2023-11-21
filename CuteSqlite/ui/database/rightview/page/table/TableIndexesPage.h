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

 * @file   TableIndexesPage.h
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-10-10
 *********************************************************************/
#pragma once
#include <string>
#include "common/Config.h"
#include "ui/common/page/QPage.h"
#include "ui/common/button/QImageButton.h"
#include "ui/database/supplier/DatabaseSupplier.h"
#include "ui/common/listview/QListViewCtrl.h"
#include "ui/database/rightview/page/table/adapter/TableColumnsPageAdapter.h"
#include "ui/database/rightview/page/table/adapter/TableIndexesPageAdapter.h"
#include "ui/database/rightview/page/supplier/TableStructureSupplier.h"

class TableIndexesPage : public QPage 
{
public:
	BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP_EX(TableIndexesPage)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)

		MSG_WM_CTLCOLORSTATIC(OnCtlColorStatic)
		MSG_WM_CTLCOLORLISTBOX(OnCtlColorListBox)
		MSG_WM_CTLCOLOREDIT(OnCtlColorEdit)
		NOTIFY_HANDLER(Config::DATABASE_TABLE_INDEXES_LISTVIEW_ID, NM_DBLCLK, OnDbClickListView)
		NOTIFY_HANDLER(Config::DATABASE_TABLE_INDEXES_LISTVIEW_ID, NM_CLICK, OnClickListView)
		NOTIFY_HANDLER(Config::DATABASE_TABLE_INDEXES_LISTVIEW_ID, LVN_ITEMCHANGED, OnListViewItemChanged)
		NOTIFY_HANDLER(Config::DATABASE_TABLE_INDEXES_LISTVIEW_ID, LVN_GETDISPINFO, OnGetListViewData)
		NOTIFY_HANDLER(Config::DATABASE_TABLE_INDEXES_LISTVIEW_ID, LVN_ODCACHEHINT, OnPrepareListViewData)
		NOTIFY_HANDLER(Config::DATABASE_TABLE_INDEXES_LISTVIEW_ID, LVN_ODFINDITEM, OnFindListViewData)
		MESSAGE_HANDLER(Config::MSG_QLISTVIEW_SUBITEM_TEXT_CHANGE_ID, OnListViewSubItemTextChange)
		MESSAGE_HANDLER(Config::MSG_TABLE_COLUMNS_CHANGE_PRIMARY_KEY_ID, OnTableColumsChangePrimaryKey)
		MESSAGE_HANDLER(Config::MSG_TABLE_COLUMNS_DELETE_COLUMN_NAME_ID, OnTableColumsDeleteColumnName)
		MESSAGE_HANDLER(Config::MSG_DATA_HAS_CHANGED_ID, OnHandleDataHasChanged)
		COMMAND_HANDLER_EX(Config::TABLE_NEW_INDEX_BUTTON_ID, BN_CLICKED, OnClickNewIndexButton)
		COMMAND_HANDLER_EX(Config::TABLE_DEL_INDEX_BUTTON_ID, BN_CLICKED, OnClickDelIndexButton)
		CHAIN_MSG_MAP(QPage)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
	void setup(TableColumnsPageAdapter * tblColumnsPageAdapter, TableStructureSupplier * supplier);
	TableColumnsPageAdapter * getTblColumnsPageAdapter() const { return tblColumnsPageAdapter; }
	void setTblColumnsPageAdapter(TableColumnsPageAdapter * val) { tblColumnsPageAdapter = val; }
	TableIndexesPageAdapter * getAdapter();

	TableStructureSupplier * getSupplier() const { return supplier; }
	void setSupplier(TableStructureSupplier * val) { supplier = val; }

	void refreshDirtyAfterSave();
private:
	bool isNeedReload = true;
	uint64_t runtimeUserDbId = 0;
	std::wstring runtimeTblName;
	std::wstring runtimeSchema;
	int rowCount = 0;

	COLORREF buttonColor = RGB(238, 238, 238);
	HFONT textFont = nullptr;

	// toolbar button
	QImageButton newIndexButton;
	QImageButton delIndexButton;

	QListViewCtrl listView;
	std::pair<int, int> subItemPos; // pair.first-iItem, pair.second-iSubItem

	TableStructureSupplier * supplier = nullptr;
	DatabaseSupplier * databaseSupplier = DatabaseSupplier::getInstance();
	TableColumnsPageAdapter * tblColumnsPageAdapter = nullptr;
	TableIndexesPageAdapter * adapter = nullptr;
	
	virtual void createOrShowUI();
	virtual void createOrShowToolBarElems(CRect & clientRect);
	virtual void createOrShowListView(QListViewCtrl & win, CRect & clientRect);

	virtual void loadWindow();
	void loadListView();

	void enableDataDirty();

	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual int OnDestroy();
	virtual void paintItem(CDC & dc, CRect & paintRect);

	LRESULT OnListViewItemChanged(int idCtrl, LPNMHDR pnmh, BOOL &bHandled);
	LRESULT OnDbClickListView(int idCtrl, LPNMHDR pnmh, BOOL &bHandled);
	LRESULT OnClickListView(int idCtrl, LPNMHDR pnmh, BOOL &bHandled);

	HBRUSH OnCtlColorStatic(HDC hdc, HWND hwnd);
	HBRUSH OnCtlColorListBox(HDC hdc, HWND hwnd);
	HBRUSH OnCtlColorEdit(HDC hdc, HWND hwnd);

	LRESULT OnGetListViewData(int idCtrl, LPNMHDR pnmh, BOOL &bHandled);
	LRESULT OnPrepareListViewData(int idCtrl, LPNMHDR pnmh, BOOL &bHandled);
	LRESULT OnFindListViewData(int idCtrl, LPNMHDR pnmh, BOOL &bHandled);

	LRESULT OnListViewSubItemTextChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTableColumsChangePrimaryKey(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTableColumsDeleteColumnName(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnHandleDataHasChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClickNewIndexButton(UINT uNotifyCode, int nID, HWND wndCtl);
	LRESULT OnClickDelIndexButton(UINT uNotifyCode, int nID, HWND wndCtl);
};
