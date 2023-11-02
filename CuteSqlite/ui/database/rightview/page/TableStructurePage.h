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
#include "core/service/db/TableService.h"
#include "ui/database/rightview/common/QTabPage.h"
#include "ui/database/rightview/page/result/ResultTabView.h"
#include "ui/common/edit/QHelpEdit.h"
#include "ui/database/rightview/page/table/TableTabView.h"
#include "ui/database/rightview/page/supply/TableStructureSupplier.h"

class TableStructurePage : public QTabPage<TableStructureSupplier> {
public:
	BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP_EX(TableStructurePage)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		COMMAND_HANDLER_EX(Config::TABLE_TBL_NAME_EDIT_ID, EN_CHANGE, OnChangeTblNameEdit)
		COMMAND_HANDLER_EX(Config::TABLE_DATABASE_COMBOBOX_ID, CBN_SELENDOK, OnChangeDatabaseComboBox)
		COMMAND_HANDLER_EX(Config::TABLE_SAVE_BUTTON_ID, BN_CLICKED, OnClickSaveButton)
		MESSAGE_HANDLER(Config::MSG_TABLE_PREVIEW_SQL_ID, OnPreviewSql);
		MSG_WM_CTLCOLORSTATIC(OnCtlColorStatic)
		MSG_WM_CTLCOLOREDIT(OnCtlColorEdit)
		MSG_WM_CTLCOLORLISTBOX(OnCtlColorListBox)
		CHAIN_MSG_MAP(QPage)
		FORWARD_NOTIFICATIONS()
	END_MSG_MAP()

	void setup(PageOperateType tblOperateType, const std::wstring &tblName = std::wstring(), 
		TableStructurePageType activePageType = TABLE_COLUMNS_PAGE, const std::wstring & schema = std::wstring());
	TableStructureSupplier * getSupplier() { return supplier; };
	void renameTable(const std::wstring &oldTableName, const std::wstring &newTableName);

	void activePage(TableStructurePageType pageType);
private:
	TableStructurePageType activePageType = TABLE_COLUMNS_PAGE;
	COLORREF editorBkgColor = RGB(238, 238, 238);
	HFONT textFont = nullptr;	
	HFONT comboFont = nullptr;
	Gdiplus::Font * btnFont = nullptr;
	HBRUSH editorBkgBrush = nullptr;

	CStatic tblNameLabel;
	CEdit tblNameEdit;

	CStatic databaseLabel;
	CComboBox databaseComboBox;

	CStatic schemaLabel;
	CComboBox schemaComboBox;

	TableTabView tableTabView;

	QImageButton saveButton;
	QImageButton revertButton;

	QHelpEdit sqlPreviewEdit;

	DatabaseService * databaseService = DatabaseService::getInstance();
	TableService * tableService = TableService::getInstance();
	
	CRect getEditorRect(CRect & clientRect);

	virtual void createOrShowUI();
	void createOrShowTblNameElems(CRect & clientRect);
	void createOrShowDatabaseElems(CRect & clientRect);
	void createOrShowSchemaElems(CRect & clientRect);
	void createOrShowTableTabView(TableTabView & win, CRect & clientRect);
	void createOrShowSqlPreviewElems(CRect & clientRect);
	void createOrShowButtons(CRect & clientRect);
	void createOrShowSqlEditor(QHelpEdit & win, UINT id, const std::wstring & text, CRect & rect, CRect & clientRect, DWORD exStyle = 0);

	virtual void loadWindow();
	void loadDatabaseComboBox();
	void loadSchemaComboBox();
	void loadSqlPreviewEdit();

	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual int OnDestroy();
	virtual void paintItem(CDC & dc, CRect & paintRect);
	void OnChangeTblNameEdit(UINT uNotifyCode, int nID, HWND hwnd);
	void OnChangeDatabaseComboBox(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickSaveButton(UINT uNotifyCode, int nID, HWND hwnd);
	void afterCreatedTable(const std::wstring & tblName);

	LRESULT OnPreviewSql(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	HBRUSH OnCtlColorStatic(HDC hdc, HWND hwnd);
	HBRUSH OnCtlColorEdit(HDC hdc, HWND hwnd);
	HBRUSH OnCtlColorListBox(HDC hdc, HWND hwnd);
	
	void previewRuntimeSql();
	std::wstring getCreateRuntimeSql();
	std::wstring generateCreateTableDDL(std::wstring &schema, std::wstring tblName);

	std::wstring execAlterTable();

	std::wstring generateCreateColumnsClause();
	std::wstring generateCreateIndexesClause();	
	std::wstring generateCreateForeignKeyClause();

	std::wstring generateInsertIntoTmpTableSql(std::wstring& schema, std::wstring& tmpTblName, std::wstring& oldTblName);
	
};
