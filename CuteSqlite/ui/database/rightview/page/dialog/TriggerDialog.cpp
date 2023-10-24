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

 * @file   TriggerDialog.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-10-24
 *********************************************************************/
#include "stdafx.h"
#include "TriggerDialog.h"
#include <ui\common\message\QPopAnimate.h>


TriggerDialog::TriggerDialog(HWND parentHwnd)
{
	setFormSize(TRIGGER_DIALOG_WIDTH, TRIGGER_DIALOG_HEIGHT);
	caption = S(L"new-trigger");
}

void TriggerDialog::createOrShowUI()
{
	QDialog::createOrShowUI();

	CRect clientRect;
	GetClientRect(clientRect);

	createTriggerNameElems(clientRect);
}


void TriggerDialog::createTriggerNameElems(CRect & clientRect)
{
	int x = 20, y = 20, w = clientRect.Width() - 40, h = 20;
	CRect rect(x, y, x + w, y + h);
	createOrShowFormLabel(triggerNameLabel, S(L"create-a-trigger-name"), rect, clientRect, SS_LEFT); 

	rect.OffsetRect(0, h + 10);
	createOrShowFormEdit(triggerNameEdit, Config::VIEWDIALOG_VIEW_NAME_EDIT_ID, L"", L"", rect, clientRect, ES_LEFT, false);
}

void TriggerDialog::loadWindow()
{
	if (!isNeedReload) {
		return ;
	}

	isNeedReload = false;
	triggerNameEdit.SetFocus();
}

LRESULT TriggerDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	QDialog::OnInitDialog(uMsg, wParam, lParam, bHandled); 
	return 0;
}

LRESULT TriggerDialog::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	QDialog::OnDestroy(uMsg, wParam, lParam, bHandled);
	if (triggerNameLabel.IsWindow()) triggerNameLabel.DestroyWindow();
	if (triggerNameEdit.IsWindow()) triggerNameEdit.DestroyWindow();
	return 0;
}

LRESULT TriggerDialog::OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// invisible then return 
	if (!wParam) { 
		return 0;
	}
	loadWindow();
	return 0;
}

void TriggerDialog::OnClickYesButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	CString str;
	triggerNameEdit.GetWindowText(str);
	if (str.IsEmpty()) {
		QPopAnimate::error(m_hWnd, S(L"trigger-name-not-empty"));
		triggerNameEdit.SetFocus();
		return;
	}
	std::wstring triggerName = str.GetString();
	auto triggerInfo = databaseService->getUserView(supplier->getSelectedUserDbId(), triggerName);
	if (!triggerInfo.name.empty()) {
		QPopAnimate::error(m_hWnd, S(L"trigger-name-dupplicated"));
		triggerNameEdit.SetFocus();
		triggerNameEdit.SetSel(0, -1, FALSE);
		return;
	}
	supplier->newViewName = triggerName;

	EndDialog(Config::QDIALOG_YES_BUTTON_ID);
}
