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

 * @file   ExportAsSqlDialog.h
 * @brief  Export the data of database
 * 
 * @author Xuehan Qin
 * @date   2023-06-07
 *********************************************************************/
#pragma once
#include "ui/common/dialog/QDialog.h"
#include "ui/database/dialog/adapter/ExportDatabaseAdapter.h"
#include "ui/database/dialog/adapter/ObjectsTreeViewAdapter.h"
#include "ui/common/image/QStaticImage.h"
#include "ui/common/process/QProcessBar.h"
#include "core/service/system/SettingService.h"
#include "core/service/export/ExportDatabaseObjectsService.h"

#define EXPORT_AS_SQL_DIALOG_WIDTH		640
#define EXPORT_AS_SQL_DIALOG_HEIGHT		610
class ExportAsSqlDialog : public QDialog<ExportAsSqlDialog>
{
public:
	BEGIN_MSG_MAP_EX(ExportAsSqlDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SHOWWINDOW, OnShowWindow)
		COMMAND_HANDLER_EX(Config::EXPORT_DB_AS_SQL_SELECT_DB_COMBOBOX_ID, CBN_SELENDOK, OnChangeSelectDbComboBox)
		COMMAND_RANGE_CODE_HANDLER_EX(Config::EXPORT_SQL_STRUCTURE_ONLY_RADIO_ID, Config::EXPORT_SQL_STRUCTURE_DATA_RADIO_ID, BN_CLICKED, OnClickStructureAndDataRadios)		
		COMMAND_RANGE_CODE_HANDLER_EX(Config::EXPORT_RETAIN_COLUMN_CHECKBOX_ID, Config::EXPORT_CONTAIN_MULTI_ROWS_CHECKBOX_ID, BN_CLICKED, OnClickInsertStatemenCheckBox)
		COMMAND_RANGE_CODE_HANDLER_EX(Config::EXPORT_RETAIN_TABLE_RADIO_ID, Config::EXPORT_OVERRIDE_TABLE_RADIO_ID, BN_CLICKED, OnClickTblStatementRadios)
		COMMAND_HANDLER_EX(Config::EXPORT_DB_AS_SQL_OPEN_FILE_BUTTON_ID, BN_CLICKED, OnClickOpenFileButton)
		COMMAND_HANDLER_EX(Config::EXPORT_DB_AS_SQL_SELECT_ALL_OBJECTS_BUTTON_ID, BN_CLICKED, OnClickSelectAllButton)
		COMMAND_HANDLER_EX(Config::EXPORT_DB_AS_SQL_DESELECT_ALL_OBJECTS_BUTTON_ID, BN_CLICKED, OnClickDeSelectAllButton)
		NOTIFY_HANDLER_EX(Config::DATABASE_OBJECTS_TREEVIEW_ID, NM_TVSTATEIMAGECHANGING, OnCheckObjectsTreeViewItem)
		MESSAGE_HANDLER(Config::MSG_EXPORT_DB_AS_SQL_PROCESS_ID, OnProcessExport); // ÏìÓ¦½ø¶È
		CHAIN_MSG_MAP(QDialog<ExportAsSqlDialog>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
	ExportAsSqlDialog(HWND parentHwnd, ExportDatabaseAdapter * adapter);
private:
	bool isNeedReload = true;
	HWND parentHwnd = nullptr;
	ExportDatabaseAdapter * adapter = nullptr;
	ObjectsTreeViewAdapter * objectsTreeViewAdapter = nullptr;
	SettingService * settingService = SettingService::getInstance();
	

	COLORREF lineColor = RGB(127, 127, 127);
	CPen linePen = nullptr;
	HFONT elemFont = nullptr;
	bool isRunning = false;

	QStaticImage exportImage;
	CStatic exportTextLabel;

	CStatic selectDbLabel;
	CComboBox selectDbComboBox;

	CStatic selectObjectsLabel;
	CTreeViewCtrlEx dbObjectsTreeView;
	CButton selectAllButton;
	CButton deselectAllButton;

	// GROUPBOX	
	CButton structAndDataGroupBox;
	CButton insertStatementGroupBox;
	CButton tblStatementGroupBox;
	

	// STRUCTURE AND DATA SETTINGS
	CButton structureOnlyRadio;
	CButton dataOnlyRadio;
	CButton structureAndDataRadio;
	std::vector<CButton *> structureAndDataRadioPtrs;

	// INSERT STATEMENT SETTINGS
	CButton retainColumnCheckbox;
	CButton containMultiRowsCheckbox;

	// TABLE STATEMENT SETTINGS
	CButton retainTableRadio;
	CButton overrideTableRadio;
	std::vector<CButton *> tblStatementRadioPtrs;

	// PROCESS BAR
	QProcessBar processBar;

	// EXPORT PATH
	CStatic exportPathLabel;
	CEdit exportPathEdit;
	CButton openFileButton;
	
	// create elements when initting dialog
	void createOrShowUI();
	void createOrShowExportImage(QStaticImage &win, CRect & clientRect);
	void createOrShowExportTextLabel(CStatic &win, CRect & clientRect);
	void createOrShowGroupBoxes(CRect & clientRect);
	void createOrShowProcessBar(QProcessBar &win, CRect & clientRect);

	void createOrShowSelectDbElems(CRect & clientRect);
	void createOrShowExportPathElems(CRect & clientRect);
	void createOrShowDbObjectsElems(CRect & clientRect);
	void createOrShowTreeView(CTreeViewCtrlEx & win, CRect & rect, CRect & clientRect);
	// settings
	void createOrShowStructureAndDataSettingsElems(CRect & clientRect);
	void createOrShowInsertStatementSettingsElems(CRect & clientRect);
	void createOrShowTblStatementSettingsElems(CRect & clientRect);
	

	// load data when showing the window visible
	void loadWindow();
	void loadSelectDbComboBox();		
	void loadExportPathEdit();
	void loadObjectsTreeView();
	void loadStructureAndDataSettingsElems();
	void loadInsertStatementSettingsElems();
	void loadTblStatementSettingsElems();

	HWND getSelStructureAndDataRadioHwnd();
	HWND getSelTblStatementRadioHwnd();
	
	bool getSelUserDbId(uint64_t & userDbId);
	bool getExportPath(std::wstring & exportPath);
	bool getExportStructureAndDataParams(StructAndDataParams & params);
	void getExportInsertStatementParams(InsertStatementParams & params);
	bool getExportTblStatementParams(TblStatementParams & params);
	
	void saveExportPath(std::wstring & exportPath);
	void saveExportStructureAndDataSettings(StructAndDataParams & params);
	void saveInsertStatementSettings(InsertStatementParams & params);
	void saveTblStatementSettings(TblStatementParams & params);

	virtual LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnCheckObjectsTreeViewItem(LPNMHDR pnmh);
	virtual void paintItem(CDC &dc, CRect &paintRect);
	
	LRESULT OnChangeSelectDbComboBox(UINT uNotifyCode, int nID, HWND hwnd);

	void OnClickStructureAndDataRadios(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickInsertStatemenCheckBox(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickTblStatementRadios(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickOpenFileButton(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickSelectAllButton(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickDeSelectAllButton(UINT uNotifyCode, int nID, HWND hwnd);
	

	virtual void OnClickYesButton(UINT uNotifyCode, int nID, HWND hwnd);
	virtual void OnClickNoButton(UINT uNotifyCode, int nID, HWND hwnd);

	// Handle the message for export process
	LRESULT OnProcessExport(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};