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

 * @file   ExportResultDialog.h
 * @brief  Export the data of query result
 * 
 * @author Xuehan Qin
 * @date   2023-06-07
 *********************************************************************/
#pragma once
#include "ui/common/dialog/QDialog.h"
#include "ui/database/rightview/page/result/adapter/ResultListPageAdapter.h"
#include "ui/common/image/QStaticImage.h"
#include "core/service/system/SettingService.h"

class ExportResultDialog : public QDialog<ExportResultDialog>
{
public:
	BEGIN_MSG_MAP_EX(ExportResultDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SHOWWINDOW, OnShowWindow)
		COMMAND_RANGE_CODE_HANDLER_EX(Config::EXPORT_TO_CSV_RADIO_ID, Config::EXPORT_TO_SQL_RADIO_ID, BN_CLICKED, OnClickRadios)
		COMMAND_HANDLER_EX(Config::EXPORT_SELECT_ALL_FIELDS_BUTTON_ID, BN_CLICKED, OnClickSelectAllFieldsButton)
		COMMAND_HANDLER_EX(Config::EXPORT_DESELECT_ALL_FIELDS_BUTTON_ID, BN_CLICKED, OnClickDeselectAllFieldsButton)
		CHAIN_MSG_MAP(QDialog<ExportResultDialog>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
	ExportResultDialog(HWND parentHwnd, ResultListPageAdapter * adapter);
private:
	bool isNeedReload = true;
	HWND parentHwnd = nullptr;
	ResultListPageAdapter * adapter = nullptr;
	SettingService * settingService = SettingService::getInstance();

	COLORREF lineColor = RGB(127, 127, 127);
	CPen linePen = nullptr;
	HFONT elemFont = nullptr;

	QStaticImage exportImage;
	CStatic exportTextLabel;

	// RADIO
	CButton csvRadio;
	CButton jsonRadio;
	CButton htmlRadio;
	CButton xmlRadio;
	CButton excelXmlRadio;
	CButton sqlRadio;
	std::vector<CButton *> radioPtrs;
	
	// GROUPBOX
	CButton csvGroupBox;
	CButton csvFieldGroupBox;
	CButton csvLineGroupBox;
	CButton csvCharsetGroupBox;
	CButton excelGroupBox;
	CButton sqlGroupBox;
	CButton exportFieldsGroupBox;

	// CSV SETTINGS - Feilds
	CStatic csvFieldTerminatedByLabel;
	CEdit csvFieldTerminatedByEdit;
	CStatic csvFieldEnclosedByLabel;
	CEdit csvFieldEnclosedByEdit;
	CStatic csvFieldEscapedByLabel;
	CEdit csvFieldEscapedByEdit;

	// CSV SETTINGS - Line
	CStatic csvLineTerminatedByLabel;
	CEdit csvLineTerminatedByEdit;

	// CSV SETTINGS - Charset
	CStatic csvCharsetLabel;
	CEdit csvCharsetEdit;

	// CSV SETTINGS - add column name on top
	CButton csvColumnNameCheckBox;

	// EXCEL SETTINGS
	CStatic excelComlumnMaxSizeLabel;
	CEdit excelComlumnMaxSizeEdit;
	CStatic excelDecimalPlacesLabel;
	CEdit excelDecimalPlacesEdit;

	// SQL SETTINGS
	CButton structureOnlyRadio;
	CButton dataOnlyRadio;
	CButton structurAndDataRadio;
	std::vector<CButton *> sqlRadioPtrs;

	// SELECT FIELDS
	CListBox selectFieldsListBox;
	CButton selectAllButton;
	CButton deselectAllButton;

	// EXPORT PATH
	CStatic exportPathLabel;
	CEdit exportPathEdit;
	CButton openFileButton;
	
	// create elements when initting dialog
	void createOrShowUI();
	void createOrShowExportImage(QStaticImage &win, CRect & clientRect);
	void createOrShowExportTextLabel(CStatic &win, CRect & clientRect);
	void createOrShowRadios(CRect & clientRect);
	void createOrShowGroupBoxes(CRect & clientRect);

	void createOrShowCsvSettingsElems(CRect & clientRect);
	void createOrShowExcelSettingsElems(CRect & clientRect);
	void createOrShowSqlSettingsElems(CRect & clientRect);
	void createOrShowSelectFieldsElems(CRect & clientRect);
	void createOrShowExportPathElems(CRect & clientRect);

	// load data when showing the window visible
	void loadWindow();
	void loadExportFmtElems();
	void loadCsvSettingsElems();
	void loadExcelSettingsElems();
	void loadSqlSettingsElems();
	void loadSelectFieldsListBox();
	void loadExportPathEdit();

	//Click yes button, then get params to export
	bool getExportCsvParams(ExportCsvParams & ExportCsvParam);
	bool getExportPath(std::wstring & exportPath);

	virtual LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual void paintItem(CDC &dc, CRect &paintRect);
	void OnClickRadios(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickSelectAllFieldsButton(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickDeselectAllFieldsButton(UINT uNotifyCode, int nID, HWND hwnd);

	virtual void OnClickYesButton(UINT uNotifyCode, int nID, HWND hwnd);
	virtual void OnClickNoButton(UINT uNotifyCode, int nID, HWND hwnd);
};