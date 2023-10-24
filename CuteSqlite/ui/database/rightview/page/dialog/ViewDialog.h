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

 * @file   ViewDialog.h
 * @brief  Create or alter a view for database 
 * 
 * @author Xuehan Qin
 * @date   2023-10-24
 *********************************************************************/
#pragma once
#include "ui/common/dialog/QDialog.h"
#include "core/service/db/DatabaseService.h"
#include "../../../supplier/DatabaseSupplier.h"

#define TRIGGER_DIALOG_WIDTH	300
#define TRIGGER_DIALOG_HEIGHT	160
class ViewDialog : public QDialog<ViewDialog>
{
public:
	BEGIN_MSG_MAP_EX(ViewDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SHOWWINDOW, OnShowWindow)
		CHAIN_MSG_MAP(QDialog<ViewDialog>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
	ViewDialog(HWND parentHwnd);

private:
	bool isNeedReload = true;
	HWND parentHwnd = nullptr;

	CStatic viewNameLabel;
	CEdit viewNameEdit;

	DatabaseService * databaseService = DatabaseService::getInstance();
	DatabaseSupplier * supplier = DatabaseSupplier::getInstance();

	// create elements when initialize dialog
	virtual void createOrShowUI();
	void createViewNameElems(CRect & clientRect);

	// load data when showing the window visible
	virtual void loadWindow();

	virtual LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual void OnClickYesButton(UINT uNotifyCode, int nID, HWND hwnd);
};
