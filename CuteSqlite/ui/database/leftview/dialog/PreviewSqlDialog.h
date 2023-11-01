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

 * @file   PreviewSqlDialog.h
 * @brief  Create or alter a view for database 
 * 
 * @author Xuehan Qin
 * @date   2023-10-24
 *********************************************************************/
#pragma once
#include "ui/common/dialog/QDialog.h"
#include "core/service/db/TableService.h"
#include "ui/database/leftview/dialog/adapter/CopyTableAdapter.h"
#include "ui/database/supplier/DatabaseSupplier.h"
#include "ui/common/edit/QHelpEdit.h"

#define PREVIEW_SQL_DIALOG_WIDTH		800
#define PREVIEW_SQL_DIALOG_HEIGHT	600
class PreviewSqlDialog : public QDialog<PreviewSqlDialog>
{
public:
	BEGIN_MSG_MAP_EX(PreviewSqlDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SHOWWINDOW, OnShowWindow)
		CHAIN_MSG_MAP(QDialog<PreviewSqlDialog>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
	PreviewSqlDialog(HWND parentHwnd, CopyTableAdapter * adapter);

private:
	bool isNeedReload = true;
	HWND parentHwnd = nullptr;
	std::wstring sql;

	QHelpEdit sqlPreviewEdit;

	CopyTableAdapter * adapter = nullptr;
	CopyTableSupplier * supplier = nullptr;
	TableService * tableService = TableService::getInstance();
	DatabaseSupplier * databaseSupplier = DatabaseSupplier::getInstance();

	// create elements when initialize dialog
	virtual void createOrShowUI();
	void createOrShowSqlPreviewEdit(CRect & clientRect);
	void createOrShowSqlEditor(QHelpEdit & win, UINT id, const std::wstring & text, CRect & rect, CRect & clientRect, DWORD exStyle = 0);

	// load data when showing the window visible
	virtual void loadWindow();
	void loadSqlPreviewEdit();
	void loadTargetTableSqlForOtherDb();
	bool doAppendCopyDataSql(uint16_t suffix, const std::wstring & targetTblName);

	virtual LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual void OnClickYesButton(UINT uNotifyCode, int nID, HWND hwnd);
};
