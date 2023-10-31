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
		COMMAND_RANGE_CODE_HANDLER_EX(Config::COPYTABLE_SHARDING_BEGIN_EDIT_ID, Config::COPYTABLE_SHARDING_END_EDIT_ID, EN_CHANGE, OnShardingEditChange)
		COMMAND_HANDLER_EX(Config::COPYTABLE_TO_TBL_EDIT_ID, EN_CHANGE, OnShardingEditChange)
		COMMAND_HANDLER_EX(Config::COPYTABLE_SHARDING_ENABLE_CHECKBOX_ID, BN_CLICKED, OnClickShardingEnableCheckBox)
		COMMAND_HANDLER_EX(Config::COPYTABLE_DATA_STRATEGY_ENABLE_CHECKBOX_ID, BN_CLICKED, OnClickDataStrategyEnableCheckBox)
		MESSAGE_HANDLER(Config::MSG_COPY_TABLE_PROCESS_ID, OnProcessCopy); // ÏìÓ¦½ø¶È
		CHAIN_MSG_MAP(QDialog<CopyTableDialog>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
	CopyTableDialog(HWND parentHwnd, bool isSharding = false);

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

	CStatic fromDbLabel;
	CEdit fromDbEdit;
	CStatic fromTblLabel;
	CEdit fromTblEdit;

	CStatic toDbLabel;
	CComboBox toDbComboBox;
	CStatic toTblLabel;
	CEdit toTblEdit;

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
	CButton shardingEnableCheckBox;
	CStatic shardingBeginLabel;
	CEdit shardingBeginEdit;
	CStatic shardingEndLabel;
	CEdit shardingEndEdit;
	CEdit shardingRangeEdit;

	// advanced options elems
	CButton dataStrategyEnableCheckBox;
	CStatic suffixNumberExpressLabel; 
	CEdit suffixNumberExpressEdit; 
	CButton suffixNumberExpressButton;

	// PROCESS BAR
	QProcessBar processBar;

	CButton previewSqlButton;

	DatabaseService * databaseService = DatabaseService::getInstance();
	TableService * tableService = TableService::getInstance();
	DatabaseSupplier * databaseSupplier = DatabaseSupplier::getInstance();
	CopyTableSupplier * supplier = nullptr;
	CopyTableAdapter * adapter = nullptr;

	// create elements when initialize dialog
	virtual void createOrShowUI();
	void createOrShowCopyImage(QStaticImage &win, CRect & clientRect);
	void createOrShowCopyTextLabel(CStatic &win, CRect & clientRect);
	void createFromElems(CRect & clientRect);
	void createToElems(CRect & clientRect);
	void createOrShowGroupBoxes(CRect & clientRect);
	void createOrShowPreviewSqlButton(CRect & clientRect);

	void createOrShowStructureAndDataSettingsElems(CRect & clientRect);
	void createOrShowShardingTableElems(CRect & clientRect);
	void createOrShowAdvancedOptionsElems(CRect & clientRect);	
	void createOrShowProcessBar(QProcessBar &win, CRect & clientRect);

	// load data when showing the window visible
	virtual void loadWindow();
	void loadToDbComboBox();
	void loadOnlyForIsSharding();
	void loadShardingTableElems();
	void enableShardingTableElems();
	void enableAdvancedSettingsElems();

	virtual LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual void paintItem(CDC &dc, CRect &paintRect);

	virtual HBRUSH OnCtlColorStatic(HDC hdc, HWND hwnd);

	void OnClickStructureAndDataRadios(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickShardingEnableCheckBox(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickDataStrategyEnableCheckBox(UINT uNotifyCode, int nID, HWND hwnd);

	void OnShardingEditChange(UINT uNotifyCode, int nID, HWND hwnd);
	// Handle the message for export process
	LRESULT OnProcessCopy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual void OnClickYesButton(UINT uNotifyCode, int nID, HWND hwnd);
	
	
};
