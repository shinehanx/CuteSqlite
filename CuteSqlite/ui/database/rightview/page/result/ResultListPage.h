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

class ResultListPage : public QPage {
public:
	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP_EX(ResultListPage)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		NOTIFY_HANDLER(Config::DATABASE_QUERY_LISTVIEW_ID, NM_CLICK,  OnNMClickListView)
		NOTIFY_HANDLER(Config::DATABASE_QUERY_LISTVIEW_ID, LVN_GETDISPINFO, OnGetListViewData)
		NOTIFY_HANDLER(Config::DATABASE_QUERY_LISTVIEW_ID, LVN_ODCACHEHINT, OnPrepareListViewData)
		NOTIFY_HANDLER(Config::DATABASE_QUERY_LISTVIEW_ID, LVN_ODCACHEHINT, OnFindListViewData)

		CHAIN_MSG_MAP(QPage)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	void setup(std::wstring & sql);
protected:
	bool isNeedReload = true;
	std::wstring sql;
	int rowCount = 0;
	COLORREF buttonColor = RGB(238, 238, 238);
	CImageList imageList;
	HBITMAP checkNoBitmap = nullptr;
	HBITMAP checkYesBitmap = nullptr;

	// toolbar button
	QImageButton exportButton;
	QDropButton copyButton;
	CComboBox readWriteComboBox;
	QCheckBox formViewCheckBox;
	QImageButton filterButton;
	QImageButton refreshButton;
	QCheckBox limitCheckBox;
	CStatic offsetLabel;
	CEdit offsetEdit;
	CStatic limitLabel;
	CEdit limitEdit;

	CListViewCtrl listView;
	

	ResultListPageAdapter * adapter = nullptr;
	DatabaseSupplier * supplier = DatabaseSupplier::getInstance();

	virtual void createOrShowUI();
	virtual void loadWindow();

	void createImageList();
	void createOrShowToolBarElements(CRect & clientRect);
	void createOrShowListView(CListViewCtrl & win, CRect & clientRect);

	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual int OnDestroy();
	virtual void paintItem(CDC & dc, CRect & paintRect);

	LRESULT OnNMClickListView(int idCtrl, LPNMHDR pnmh, BOOL &bHandled);
	LRESULT OnGetListViewData(int idCtrl, LPNMHDR pnmh, BOOL &bHandled);
	LRESULT OnPrepareListViewData(int idCtrl, LPNMHDR pnmh, BOOL &bHandled);
	LRESULT OnFindListViewData(int idCtrl, LPNMHDR pnmh, BOOL &bHandled);
};
