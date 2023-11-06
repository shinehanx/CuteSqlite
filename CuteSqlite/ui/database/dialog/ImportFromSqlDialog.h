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

 * @file   ImportFromSqlDialog.h
 * @brief  The dialog provide importing the structure and data of the selected database from sql file
 * 
 * @author Xuehan Qin
 * @date   2023-10-08
 *********************************************************************/
#pragma once
#include "ui/common/dialog/QDialog.h"
#include "ui/database/dialog/adapter/ImportDatabaseAdapter.h"
#include "ui/common/image/QStaticImage.h"
#include "ui/common/process/QProcessBar.h"
#include "core/service/system/SettingService.h"

#define IMPORT_FROM_SQL_DIALOG_WIDTH		500
#define IMPORT_FROM_SQL_DIALOG_HEIGHT		300
class ImportFromSqlDialog : public QDialog<ImportFromSqlDialog>
{
public:
	BEGIN_MSG_MAP_EX(ImportFromSqlDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SHOWWINDOW, OnShowWindow)
		COMMAND_HANDLER_EX(Config::IMPORT_TARGET_DB_COMBOBOX_ID, CBN_SELENDOK, OnChangeSelectDbComboBox)
		COMMAND_HANDLER_EX(Config::IMPORT_OPEN_FILE_BUTTON_ID, BN_CLICKED, OnClickOpenFileButton)
		COMMAND_HANDLER_EX(Config::IMPORT_ABORT_ON_ERROR_CHECKBOX_ID, BN_CLICKED, OnClickAbortOnErrorCheckBox)
		MESSAGE_HANDLER(Config::MSG_IMPORT_PROCESS_ID, OnProcessImport); // ÏìÓ¦½ø¶È
		CHAIN_MSG_MAP(QDialog<ImportFromSqlDialog>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
	ImportFromSqlDialog(HWND parentHwnd, ImportDatabaseAdapter * adapter);

private:
	bool isNeedReload = true;
	HWND parentHwnd = nullptr;
	ImportDatabaseAdapter * adapter = nullptr;
	SettingService * settingService = SettingService::getInstance();

	COLORREF lineColor = RGB(127, 127, 127);
	CPen linePen = nullptr;
	HFONT elemFont = nullptr;
	bool isRunning = false;

	QStaticImage importImage;
	CStatic importTextLabel;

	CStatic selectDbLabel;
	CComboBox selectDbComboBox;

	CButton abortOnErrorCheckbox;

	// PROCESS BAR
	QProcessBar processBar;

	// EXPORT PATH
	CStatic importPathLabel;
	CEdit importPathEdit;
	CButton openFileButton;

	// create elements when initting dialog
	void createOrShowUI();
	void createOrShowImportImage(QStaticImage &win, CRect & clientRect);
	void createOrShowImportTextLabel(CStatic &win, CRect & clientRect);
	void createOrShowSelectDbElems(CRect & clientRect);
	void createOrShowImportPathElems(CRect & clientRect);
	void createOrShowAbortOnErrorElems(CRect & clientRect);
	void createOrShowProcessBar(QProcessBar &win, CRect & clientRect);
	

	// load data when showing the window visible
	void loadWindow();
	void loadSelectDbComboBox();
	void loadImportPathEdit();
	void loadAbortOnErrorCheckBox();

	bool getSelUserDbId(uint64_t & userDbId);
	bool getImportPath(std::wstring & importPath);

	void saveImportPath(std::wstring & importPath);

	virtual LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual void paintItem(CDC &dc, CRect &paintRect);

	LRESULT OnChangeSelectDbComboBox(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickOpenFileButton(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickAbortOnErrorCheckBox(UINT uNotifyCode, int nID, HWND hwnd);

	virtual void OnClickYesButton(UINT uNotifyCode, int nID, HWND hwnd);

	// Handle the message for import process
	LRESULT OnProcessImport(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};
