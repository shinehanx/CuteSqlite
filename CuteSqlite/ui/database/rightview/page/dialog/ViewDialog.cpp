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

 * @file   ViewDialog.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-10-24
 *********************************************************************/
#include "stdafx.h"
#include "ViewDialog.h"
#include <ui\common\message\QPopAnimate.h>


ViewDialog::ViewDialog(HWND parentHwnd)
{
	setFormSize(TRIGGER_DIALOG_WIDTH, TRIGGER_DIALOG_HEIGHT);
	caption = S(L"new-view");
}

void ViewDialog::createOrShowUI()
{
	QDialog::createOrShowUI();

	CRect clientRect;
	GetClientRect(clientRect);

	createViewNameElems(clientRect);
}


void ViewDialog::createViewNameElems(CRect & clientRect)
{
	int x = 20, y = 10, w = clientRect.Width() - 40, h = 20;
	CRect rect(x, y, x + w, y + h);
	createOrShowFormLabel(viewNameLabel, S(L"create-a-view-name"), rect, clientRect, SS_LEFT); 

	rect.OffsetRect(0, h + 10);
	createOrShowFormEdit(viewNameEdit, Config::VIEWDIALOG_VIEW_NAME_EDIT_ID, L"", L"", rect, clientRect, ES_LEFT, false);
}

void ViewDialog::loadWindow()
{
	if (!isNeedReload) {
		return ;
	}

	isNeedReload = false;
	viewNameEdit.SetFocus();
}

LRESULT ViewDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	QDialog::OnInitDialog(uMsg, wParam, lParam, bHandled); 
	return 0;
}

LRESULT ViewDialog::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	QDialog::OnDestroy(uMsg, wParam, lParam, bHandled);
	if (viewNameLabel.IsWindow()) viewNameLabel.DestroyWindow();
	if (viewNameEdit.IsWindow()) viewNameEdit.DestroyWindow();
	return 0;
}

LRESULT ViewDialog::OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// invisible then return 
	if (!wParam) { 
		return 0;
	}
	loadWindow();
	return 0;
}

void ViewDialog::OnClickYesButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	CString str;
	viewNameEdit.GetWindowText(str);
	if (str.IsEmpty()) {
		QPopAnimate::error(m_hWnd, S(L"view-name-not-empty"));
		viewNameEdit.SetFocus();
		return;
	}
	std::wstring viewName = str.GetString();
	auto viewInfo = databaseService->getUserView(supplier->getSelectedUserDbId(), viewName);
	if (!viewInfo.name.empty()) {
		QPopAnimate::error(m_hWnd, S(L"view-name-dupplicated"));
		viewNameEdit.SetFocus();
		viewNameEdit.SetSel(0, -1, FALSE);
		return;
	}
	supplier->newViewName = viewName;

	EndDialog(Config::QDIALOG_YES_BUTTON_ID);
}
