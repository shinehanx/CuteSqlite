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

 * @file   NewTablePage.h
 * @brief  The class provide create a new table 
 * 
 * @author Xuehan Qin
 * @date   2023-10-09
 *********************************************************************/
#pragma once
#include "core/service/db/DatabaseService.h"
#include "ui/common/page/QPage.h"
#include "ui/database/rightview/page/result/ResultTabView.h"
#include "ui/common/edit/QHelpEdit.h"
#include "ui/database/supplier/DatabaseSupplier.h"
#include "ui/database/rightview/page/table/TableTabView.h"

class NewTablePage : public QPage {
public:
	BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP_EX(NewTablePage)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_CTLCOLORSTATIC(OnCtlColorStatic)
		MSG_WM_CTLCOLOREDIT(OnCtlColorEdit)
		MSG_WM_CTLCOLORLISTBOX(OnCtlColorListBox)
		CHAIN_MSG_MAP(QPage)
		FORWARD_NOTIFICATIONS()
	END_MSG_MAP()
private:
	HFONT textFont = nullptr;	
	HFONT comboFont = nullptr;
	Gdiplus::Font * btnFont = nullptr;

	CStatic tblNameLabel;
	CEdit tblNameEdit;

	CStatic databaseLabel;
	CComboBox databaseComboBox;

	CStatic schemaLabel;
	CComboBox schemaComboBox;

	TableTabView tableTabView;

	QImageButton saveButton;
	QImageButton revertButton;

	DatabaseSupplier * supplier = DatabaseSupplier::getInstance();
	DatabaseService * databaseService = DatabaseService::getInstance();
	
	virtual void createOrShowUI();
	void createTblNameElems(CRect & clientRect);
	void createDatabaseElems(CRect & clientRect);
	void createSchemaElems(CRect & clientRect);
	void createOrShowTableTabView(TableTabView & win, CRect & clientRect);
	void createOrShowButtons(CRect & clientRect);

	virtual void loadWindow();
	void loadDatabaseComboBox();
	void loadSchemaComboBox();

	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual int OnDestroy();	
	virtual void paintItem(CDC & dc, CRect & paintRect);

	HBRUSH OnCtlColorStatic(HDC hdc, HWND hwnd);
	HBRUSH OnCtlColorEdit(HDC hdc, HWND hwnd);
	HBRUSH OnCtlColorListBox(HDC hdc, HWND hwnd);
};
