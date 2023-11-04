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

 * @file   ImportFromCsvDialog.h
 * @brief  The dialog provide importing the structure and data of the selected database from sql file
 * 
 * @author Xuehan Qin
 * @date   2023-10-08
 *********************************************************************/
#pragma once
#include "ui/common/dialog/QDialog.h"
#include "ui/database/dialog/adapter/ImportFromCsvAdapter.h"
#include "ui/common/image/QStaticImage.h"
#include "ui/common/process/QProcessBar.h"
#include "core/service/system/SettingService.h"
#include "ui/database/dialog/supplier/ImportFromCsvSupplier.h"
#include "ui/database/supplier/DatabaseSupplier.h"
#include "ui/common/listview/QListViewCtrl.h"

#define IMPORT_FROM_CSV_DIALOG_WIDTH		820
#define IMPORT_FROM_CSV_DIALOG_HEIGHT		750
class ImportFromCsvDialog : public QDialog<ImportFromCsvDialog>
{
public:
	BEGIN_MSG_MAP_EX(ImportFromCsvDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SHOWWINDOW, OnShowWindow)
		
		NOTIFY_HANDLER(Config::IMPORT_COLUMN_LISTVIEW_ID, LVN_GETDISPINFO, OnGetListViewData)
		NOTIFY_HANDLER(Config::IMPORT_COLUMN_LISTVIEW_ID, LVN_ODCACHEHINT, OnPrepareListViewData)
		NOTIFY_HANDLER(Config::IMPORT_COLUMN_LISTVIEW_ID, LVN_ODFINDITEM, OnFindListViewData)
		NOTIFY_HANDLER(Config::IMPORT_COLUMN_LISTVIEW_ID, NM_CLICK, OnClickColumnListView)
		MESSAGE_HANDLER(Config::MSG_QLISTVIEW_SUBITEM_TEXT_CHANGE_ID, OnListViewSubItemTextChange)

		NOTIFY_HANDLER(Config::IMPORT_DATA_LISTVIEW_ID, LVN_GETDISPINFO, OnGetListViewData)
		NOTIFY_HANDLER(Config::IMPORT_DATA_LISTVIEW_ID, LVN_ODCACHEHINT, OnPrepareListViewData)
		NOTIFY_HANDLER(Config::IMPORT_DATA_LISTVIEW_ID, LVN_ODFINDITEM, OnFindListViewData)

		COMMAND_HANDLER_EX(Config::IMPORT_TARGET_DB_COMBOBOX_ID, CBN_SELENDOK, OnChangeTargetDbComboBox)
		COMMAND_HANDLER_EX(Config::IMPORT_TARGET_TBL_COMBOBOX_ID, CBN_SELENDOK, OnChangeTargetTblComboBox)
		COMMAND_HANDLER_EX(Config::IMPORT_TARGET_TBL_COMBOBOX_ID, CBN_SELENDOK, OnChangeTargetTblComboBox)
		COMMAND_HANDLER_EX(Config::IMPORT_OPEN_FILE_BUTTON_ID, BN_CLICKED, OnClickOpenFileButton)
		COMMAND_HANDLER_EX(Config::CSV_COLUMN_NAME_CHECKBOX_ID, BN_CLICKED, OnClickCsvColumnNameCheckBox)
		COMMAND_HANDLER_EX(Config::CSV_FIELD_TERMINAATED_BY_COMBOBOX_ID, CBN_SELENDOK, OnChangeCsvFieldTerminatedByComboBox)
		COMMAND_HANDLER_EX(Config::CSV_FIELD_ENCLOSED_BY_EDIT_ID, EN_CHANGE, OnChangeCsvFieldEnclosedByEdit)
		COMMAND_HANDLER_EX(Config::CSV_FIELD_ESCAPED_BY_EDIT_ID, EN_CHANGE, OnChangeCsvFieldEscapedByEdit)
		COMMAND_HANDLER_EX(Config::CSV_LINE_TERMINAATED_BY_COMBOBOX_ID, CBN_SELENDOK, OnChangeCsvLineTerminatedByComboBox)
		COMMAND_HANDLER_EX(Config::CSV_NULL_AS_KEYWORD_COMBOBOX_ID, CBN_SELENDOK, OnChangeCsvNullAsKeyWordComboBox)
		COMMAND_HANDLER_EX(Config::CSV_CHARSET_COMBOBOX_ID, CBN_SELENDOK, OnChangeCsvCharsetComboBox)
		COMMAND_HANDLER_EX(Config::PREVIEW_SQL_BUTTON_ID, BN_CLICKED, OnClickPreviewSqlButton)
		MESSAGE_HANDLER(Config::MSG_IMPORT_PROCESS_ID, OnProcessImport); // ÏìÓ¦½ø¶È
		
		CHAIN_MSG_MAP(QDialog<ImportFromCsvDialog>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
	ImportFromCsvDialog(HWND parentHwnd);

private:
	bool isNeedReload = true;
	HWND parentHwnd = nullptr;
	ImportFromCsvAdapter * adapter = nullptr;
	ImportFromCsvSupplier * supplier = nullptr;
	DatabaseSupplier * databaseSupplier = DatabaseSupplier::getInstance();
	SettingService * settingService = SettingService::getInstance();
	TableService * tableService = TableService::getInstance();


	COLORREF lineColor = RGB(127, 127, 127);
	CPen linePen = nullptr;
	HFONT elemFont = nullptr;
	bool isRunning = false;

	QStaticImage importImage;
	CStatic importTextLabel;

	// EXPORT PATH
	CStatic importPathLabel;
	CEdit importPathEdit;
	CButton openFileButton;

	CStatic targetDbLabel;
	CComboBox targetDbComboBox;
	CStatic targetTblLabel;
	CComboBox targetTblComboBox;

	// GROUPBOX	
	CButton topGroupBox;
	CButton csvSettingsGroupBox;
	CButton columnsSettingsGroupBox;
	CButton csvDatasGroupBox;

	QListViewCtrl columnListView;
	QListViewCtrl dataListView;

	// CSV SETTINGS - Fields
	CStatic csvFieldTerminatedByLabel;
	CComboBox csvFieldTerminatedByComboBox;
	CStatic csvFieldEnclosedByLabel;
	CEdit csvFieldEnclosedByEdit;
	CStatic csvLineTerminatedByLabel;
	CComboBox csvLineTerminatedByComboBox;
	CStatic csvFieldEscapedByLabel;
	CEdit csvFieldEscapedByEdit;
	CStatic csvNullAsKeywordLabel;
	CComboBox csvNullAsKeywordComboBox;
	CStatic csvCharsetLabel;
	CComboBox csvCharsetComboBox;
	// CSV SETTINGS - add column name on top
	CButton csvColumnNameCheckBox;

	// PROCESS BAR
	QProcessBar processBar;
	CButton previewSqlButton;
	
	int columnRowCount = 0;
	int dataRowCount = 0;

	// create elements when initting dialog
	void createOrShowUI();
	void createOrShowImportImage(QStaticImage &win, CRect & clientRect);
	void createOrShowImportTextLabel(CStatic &win, CRect & clientRect);
	void createOrShowTargetElems(CRect & clientRect);
	void createOrShowGroupBoxes(CRect & clientRect);
	void createOrShowImportPathElems(CRect & clientRect);
	void createOrShowListViews(CRect & clientRect);
	void createOrShowListView(QListViewCtrl & win, UINT id, CRect & rect, CRect & clientRect);
	void createOrShowCsvSettingsElems(CRect & clientRect);
	void createOrShowProcessBar(QProcessBar &win, CRect & clientRect);
	void createOrShowPreviewSqlButton(CRect & clientRect);
	
	// load data when showing the window visible
	void loadWindow();
	void loadImportPathEdit();
	void loadTargetDbComboBox();
	void loadTargetTblComboBox();
	void loadColumnsListView();
	void loadCsvSettingsElems();

	bool getSelUserDbId(uint64_t & userDbId);
	bool getImportPath(std::wstring & importPath);

	void saveImportSettings();

	virtual LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual void paintItem(CDC &dc, CRect &paintRect);

	LRESULT OnChangeTargetDbComboBox(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnChangeTargetTblComboBox(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickOpenFileButton(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickCsvColumnNameCheckBox(UINT uNotifyCode, int nID, HWND hwnd);
	void OnChangeCsvFieldTerminatedByComboBox(UINT uNotifyCode, int nID, HWND hwnd);
	void OnChangeCsvFieldEnclosedByEdit(UINT uNotifyCode, int nID, HWND hwnd);
	void OnChangeCsvFieldEscapedByEdit(UINT uNotifyCode, int nID, HWND hwnd);
	void OnChangeCsvLineTerminatedByComboBox(UINT uNotifyCode, int nID, HWND hwnd);
	void OnChangeCsvNullAsKeyWordComboBox(UINT uNotifyCode, int nID, HWND hwnd);
	void OnChangeCsvCharsetComboBox(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickPreviewSqlButton(UINT uNotifyCode, int nID, HWND hwnd);
	virtual void OnClickYesButton(UINT uNotifyCode, int nID, HWND hwnd);

	// Handle the message for import process
	LRESULT OnProcessImport(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnGetListViewData(int idCtrl, LPNMHDR pnmh, BOOL &bHandled);
	LRESULT OnPrepareListViewData(int idCtrl, LPNMHDR pnmh, BOOL &bHandled);
	LRESULT OnFindListViewData(int idCtrl, LPNMHDR pnmh, BOOL &bHandled);
	LRESULT OnClickColumnListView(int idCtrl, LPNMHDR pnmh, BOOL &bHandled);
	LRESULT OnListViewSubItemTextChange(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};
