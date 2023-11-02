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

 * @file   ShardingStrategyExpressDialog.h
 * @brief  Create or alter a view for database 
 * 
 * @author Xuehan Qin
 * @date   2023-11-02
 *********************************************************************/
#pragma once
#include "ui/common/dialog/QDialog.h"
#include "core/service/db/TableService.h"
#include "core/service/db/DatabaseService.h"
#include "ui/common/edit/QHelpEdit.h"
#include "ui/database/leftview/dialog/adapter/CopyTableAdapter.h"
#include "ui/database/leftview/dialog/supplier/CopyTableSupplier.h"

#define EXPRESS_DIALOG_WIDTH	500
#define EXPRESS_DIALOG_HEIGHT	520
class ExpressDialog : public QDialog<ExpressDialog>
{
public:
	BEGIN_MSG_MAP_EX(ExpressDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SHOWWINDOW, OnShowWindow)
		COMMAND_HANDLER_EX(Config::EXPRESS_FUNCTIONS_LISTBOX_ID, LBN_DBLCLK, OnDbClickFunctionsListBox)
		COMMAND_HANDLER_EX(Config::EXPRESS_COLUMNS_LISTBOX_ID, LBN_DBLCLK, OnDbClickColumnsListBox)
		CHAIN_MSG_MAP(QDialog<ExpressDialog>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
	ExpressDialog(HWND parentHwnd, CopyTableAdapter * adapter);

private:
	bool isNeedReload = true;
	HWND parentHwnd = nullptr;

	CStatic functionsLabel;
	CListBox functionsListBox;

	CStatic columnsLabel;
	CListBox columnsListBox;

	QHelpEdit expressEdit;

	CopyTableAdapter * adapter = nullptr;
	CopyTableSupplier * supplier = nullptr;
	TableService * tableService = TableService::getInstance();
	DatabaseService * databaseService = DatabaseService::getInstance();
	DatabaseSupplier * databaseSupplier = DatabaseSupplier::getInstance();

	// create elements when initialize dialog
	virtual void createOrShowUI();
	void createOrShowFunctionsElems(CRect & clientRect);
	void createOrShowColumnsElems(CRect & clientRect);
	void createOrShowExpressEdit(CRect & clientRect);
	void createOrShowExpressEditor(QHelpEdit & win, UINT id, CRect & rect, CRect & clientRect, DWORD exStyle = 0);
	// load data when showing the window visible
	virtual void loadWindow();
	void loadFunctionsListBox();
	void loadColumnsListBox();
	void laodExpressEditor();

	virtual LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void OnDbClickFunctionsListBox(UINT uNotifyCode, int nID, HWND hwnd);
	void OnDbClickColumnsListBox(UINT uNotifyCode, int nID, HWND hwnd);

	virtual void OnClickYesButton(UINT uNotifyCode, int nID, HWND hwnd);
};
