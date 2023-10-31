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

 * @file   RenameTableDialog.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-10-24
 *********************************************************************/
#include "stdafx.h"
#include "RenameTableDialog.h"
#include "ui/common/message/QPopAnimate.h"


RenameTableDialog::RenameTableDialog(HWND parentHwnd)
{
	setFormSize(RENAME_DIALOG_WIDTH, RENAME_DIALOG_HEIGHT);
	caption = S(L"table-rename");
}

void RenameTableDialog::createOrShowUI()
{
	QDialog::createOrShowUI();

	CRect clientRect;
	GetClientRect(clientRect);

	createViewNameElems(clientRect);
}


void RenameTableDialog::createViewNameElems(CRect & clientRect)
{
	int x = 20, y = 10, w = clientRect.Width() - 40, h = 20;
	CRect rect(x, y, x + w, y + h);
	createOrShowFormLabel(tblNameLabel, S(L"new-table-name"), rect, clientRect, SS_LEFT); 

	rect.OffsetRect(0, h + 10);
	createOrShowFormEdit(tblNameEdit, Config::TABLE_TBL_NAME_EDIT_ID, L"", L"", rect, clientRect, ES_LEFT, false);
}

void RenameTableDialog::loadWindow()
{
	if (!isNeedReload) {
		return ;
	}

	isNeedReload = false;
	tblNameEdit.SetWindowText(supplier->selectedTable.c_str());
	tblNameEdit.SetSel(0, -1);
	tblNameEdit.SetFocus();
}

LRESULT RenameTableDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	QDialog::OnInitDialog(uMsg, wParam, lParam, bHandled); 
	return 0;
}

LRESULT RenameTableDialog::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	QDialog::OnDestroy(uMsg, wParam, lParam, bHandled);
	if (tblNameLabel.IsWindow()) tblNameLabel.DestroyWindow();
	if (tblNameEdit.IsWindow()) tblNameEdit.DestroyWindow();
	return 0;
}

LRESULT RenameTableDialog::OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// invisible then return 
	if (!wParam) { 
		return 0;
	}
	loadWindow();
	return 0;
}

void RenameTableDialog::OnClickYesButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	CString str;
	tblNameEdit.GetWindowText(str);
	if (str.IsEmpty()) {
		QPopAnimate::error(S(L"table-name-not-empty"));
		tblNameEdit.SetFocus();
		return;
	}
	std::wstring & oldTblName = supplier->selectedTable;
	std::wstring newTblName = str.GetString();
	if (oldTblName == newTblName) {
		tblNameEdit.SetFocus();
		tblNameEdit.SetSel(0, -1, FALSE);
		return;
	}
	auto tableInfo = tableService->getUserTable(supplier->getSelectedUserDbId(), newTblName);
	if (!tableInfo.name.empty()) {
		QPopAnimate::error(S(L"table-name-duplicated"));
		tblNameEdit.SetFocus();
		tblNameEdit.SetSel(0, -1, FALSE);
		return;
	}

	supplier->oldTableName = oldTblName;
	supplier->newTableName = newTblName;

	EndDialog(Config::QDIALOG_YES_BUTTON_ID);
}
