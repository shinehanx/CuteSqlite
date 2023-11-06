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

 * @file   CopyTableDialog.h
 * @brief  Copy a table for database 
 * 
 * @author Xuehan Qin
 * @date   2023-10-24
 *********************************************************************/
#pragma once
#include "ui/common/dialog/QDialog.h"
#include "core/service/db/TableService.h"
#include "core/service/db/DatabaseService.h"
#include "core/service/system/SettingService.h"
#include "ui/database/supplier/DatabaseSupplier.h"
#include "ui/database/leftview/dialog/supplier/CopyTableSupplier.h"
#include "ui/common/process/QProcessBar.h"
#include "ui/database/leftview/dialog/adapter/CopyTableAdapter.h"

#define COPY_DIALOG_WIDTH	500
#define COPY_DIALOG_HEIGHT	520
class CopyTableDialog : public QDialog<CopyTableDialog>
{
public:
	BEGIN_MSG_MAP_EX(CopyTableDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SHOWWINDOW, OnShowWindow)
		COMMAND_RANGE_CODE_HANDLER_EX(Config::STRUCTURE_ONLY_RADIO_ID, Config::STRUCTURE_DATA_RADIO_ID, BN_CLICKED, OnClickStructureAndDataRadios)
		COMMAND_RANGE_CODE_HANDLER_EX(Config::COPYTABLE_TBL_SUFFER_BEGIN_EDIT_ID, Config::COPYTABLE_TBL_SUFFER_END_EDIT_ID, EN_CHANGE, OnShardingEditChange)
		COMMAND_HANDLER_EX(Config::COPYTABLE_TARGET_TABLE_EDIT_ID, EN_CHANGE, OnShardingEditChange)
		COMMAND_HANDLER_EX(Config::COPYTABLE_TARGET_DB_COMBOBOX_ID, CBN_SELCHANGE, OnTargetDbComboBoxChange)
		COMMAND_HANDLER_EX(Config::COPYTABLE_ENABLE_TABLE_SHARDING_CHECKBOX_ID, BN_CLICKED, OnClickEnableTableShardingCheckBox)
		COMMAND_HANDLER_EX(Config::COPYTABLE_ENABLE_SHARDING_STRATEGY_CHECKBOX_ID, BN_CLICKED, OnClickEnableShardingStrategyCheckBox)
		COMMAND_HANDLER_EX(Config::PREVIEW_SQL_BUTTON_ID, BN_CLICKED, OnClickPreviewSqlButton)
		COMMAND_HANDLER_EX(Config::COPYTABLE_SHARDING_STRATEGY_EXPRESS_BUTTON_ID, BN_CLICKED, OnClickShardingStrategyExpressButton)
		MESSAGE_HANDLER(Config::MSG_LOADING_SQL_PREVIEW_EDIT_ID, OnLoadingSqlPreviewEdit)

		MESSAGE_HANDLER(Config::MSG_COPY_TABLE_PROCESS_ID, OnProcessCopy) // ÏìÓ¦½ø¶È
		
		CHAIN_MSG_MAP(QDialog<CopyTableDialog>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
	CopyTableDialog(HWND parentHwnd, bool isSharding = false);
	~CopyTableDialog() {};
private:
	bool isNeedReload = true;
	HWND parentHwnd = nullptr;
	bool isSharding = false;

	COLORREF lineColor = RGB(127, 127, 127);
	CPen linePen = nullptr;
	HFONT elemFont = nullptr;
	bool isRunning = false;

	QStaticImage copyImage;
	CStatic copyTextLabel;

	CStatic sourceDbLabel;
	CEdit sourceDbEdit;
	CStatic sourceTblLabel;
	CEdit sourceTblEdit;

	CStatic targetDbLabel;
	CComboBox targetDbComboBox;
	CStatic targetTblLabel;
	CEdit targetTblEdit;

	// GROUPBOX	
	CButton structAndDataGroupBox;
	CButton tableShardingGroupBox;
	CButton advancedOptionsGroupBox;

	// STRUCTURE AND DATA SETTINGS
	CButton structureOnlyRadio;
	CButton dataOnlyRadio;
	CButton structureAndDataRadio;
	std::vector<CButton *> structureAndDataRadioPtrs;

	// sharding table elems
	CButton enableTableShardingCheckBox;
	CStatic tblSuffixBeginLabel;
	CEdit tblSuffixBeginEdit;
	CStatic tblSuffixEndLabel;
	CEdit tblSuffixEndEdit;
	CEdit shardingRangeEdit;

	// advanced options elems
	CButton enableShardingStrategyCheckBox;
	CStatic shardingStrategyExpressLabel; 
	CEdit shardingStrategyExpressEdit; 
	CButton shardingStrategyExpressButton;

	// PROCESS BAR
	QProcessBar processBar;

	CButton previewSqlButton;

	DatabaseService * databaseService = DatabaseService::getInstance();
	SettingService * settingService = SettingService::getInstance();
	TableService * tableService = TableService::getInstance();
	DatabaseSupplier * databaseSupplier = DatabaseSupplier::getInstance();
	CopyTableSupplier * supplier = nullptr;
	CopyTableAdapter * adapter = nullptr;

	// create elements when initialize dialog
	virtual void createOrShowUI();
	void createOrShowCopyImage(QStaticImage &win, CRect & clientRect);
	void createOrShowCopyTextLabel(CStatic &win, CRect & clientRect);
	void createOrShowSourceElems(CRect & clientRect);
	void createOrShowTargetElems(CRect & clientRect);
	void createOrShowGroupBoxes(CRect & clientRect);
	void createOrShowPreviewSqlButton(CRect & clientRect);

	void createOrShowStructureAndDataSettingsElems(CRect & clientRect);
	void createOrShowTableShardingElems(CRect & clientRect);
	void createOrShowAdvancedOptionsElems(CRect & clientRect);	
	void createOrShowProcessBar(QProcessBar &win, CRect & clientRect);

	// load data when showing the window visible
	virtual void loadWindow();
	void loadToDbComboBox();
	void loadOnlyForIsSharding();
	void loadShardingTableElems();
	void loadAdvancedOptionsElems();
	void enableShardingTableElems();
	void enableAdvancedSettingsElems();

	virtual LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual void paintItem(CDC &dc, CRect &paintRect);

	virtual HBRUSH OnCtlColorStatic(HDC hdc, HWND hwnd);

	void OnClickStructureAndDataRadios(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickEnableTableShardingCheckBox(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickEnableShardingStrategyCheckBox(UINT uNotifyCode, int nID, HWND hwnd);
	
	void OnShardingEditChange(UINT uNotifyCode, int nID, HWND hwnd);
	void OnTargetDbComboBoxChange(UINT uNotifyCode, int nID, HWND hwnd);

	// When PreviewSqlDialog loading the sql editor,will be send this msg to his parent window, parent window handle this message, wParam=sqlEditorPtr, lParam=null  
	LRESULT OnLoadingSqlPreviewEdit(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	// Handle the message for export process
	LRESULT OnProcessCopy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	

	void OnClickPreviewSqlButton(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickShardingStrategyExpressButton(UINT uNotifyCode, int nID, HWND hwnd);
	virtual void OnClickYesButton(UINT uNotifyCode, int nID, HWND hwnd);

	bool verifyParams();


};
