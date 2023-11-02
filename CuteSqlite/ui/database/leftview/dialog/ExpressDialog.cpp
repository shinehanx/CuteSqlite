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

 * @file   ShardingStrategyExpressDialog.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-11-02
 *********************************************************************/
#include "stdafx.h"
#include "ExpressDialog.h"
#include "ui/common/message/QPopAnimate.h"


ExpressDialog::ExpressDialog(HWND parentHwnd, CopyTableAdapter * adapter)
{
	setFormSize(EXPRESS_DIALOG_WIDTH, EXPRESS_DIALOG_HEIGHT);
	caption = S(L"express-title");
	this->adapter = adapter;
	this->supplier = adapter->getSupplier();
	this->parentHwnd = parentHwnd;
}

void ExpressDialog::createOrShowUI()
{
	QDialog::createOrShowUI();

	CRect clientRect;
	GetClientRect(clientRect);

	createOrShowFunctionsElems(clientRect);
	createOrShowColumnsElems(clientRect);
	createOrShowExpressEdit(clientRect);
}


void ExpressDialog::createOrShowFunctionsElems(CRect & clientRect)
{
	int x = 10, y = 10, w = 150, h = 20;
	CRect rect(x, y, x + w, y + h);
	createOrShowFormLabel(functionsLabel, S(L"functions"), rect, clientRect, SS_LEFT);

	rect.OffsetRect(0, h + 5);
	rect.bottom = rect.top + 180;
	createOrShowFormListBox(functionsListBox, Config::EXPRESS_FUNCTIONS_LISTBOX_ID, rect, clientRect,  LBS_NOTIFY);
}


void ExpressDialog::createOrShowColumnsElems(CRect & clientRect)
{
	int x = 10, y = 220, w = 150, h = 20;
	CRect rect(x, y, x + w, y + h);
	createOrShowFormLabel(columnsLabel, S(L"columns"), rect, clientRect, SS_LEFT); 

	rect.OffsetRect(0, h + 5);
	rect.bottom = rect.top + 180;
	createOrShowFormListBox(columnsListBox, Config::EXPRESS_COLUMNS_LISTBOX_ID, rect, clientRect,  LBS_NOTIFY);
}


void ExpressDialog::createOrShowExpressEdit(CRect & clientRect)
{
	int x = 10 + 150 + 20, y = 10, w = clientRect.Width() - x - 20, h = 425;
	CRect rect(x, y, x + w, y + h);
	createOrShowExpressEditor(expressEdit, Config::EXPRESS_EDITOR_ID, rect, clientRect);
}

void ExpressDialog::createOrShowExpressEditor(QHelpEdit & win, UINT id, CRect & rect, CRect & clientRect, DWORD exStyle /*= 0*/)
{
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		std::wstring editorTitle = S(L"express-editor-help");
		win.setup(editorTitle, supplier->getShardingStrategyExpress());
		win.Create(m_hWnd, rect, L"", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_BORDER, 0, id);
		return;
	} else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
	}
}

void ExpressDialog::loadWindow()
{
	if (!isNeedReload) {
		return ;
	}

	isNeedReload = false;
	loadFunctionsListBox();
	loadColumnsListBox();
	laodExpressEditor();
}


void ExpressDialog::loadFunctionsListBox()
{
	Functions functions = databaseService->getFunctionsStrings(supplier->getRuntimeUserDbId());
	for (auto & fun : functions) {
		functionsListBox.AddString(fun.c_str());
	}
}


void ExpressDialog::loadColumnsListBox()
{
	Columns columns = tableService->getUserColumnStrings(supplier->getRuntimeUserDbId(), supplier->getRuntimeTblName());
	for (auto & column : columns) {
		columnsListBox.AddString(column.c_str());
	}
}

void ExpressDialog::laodExpressEditor()
{
	expressEdit.setText(supplier->getShardingStrategyExpress());
}

LRESULT ExpressDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	QDialog::OnInitDialog(uMsg, wParam, lParam, bHandled); 
	return 0;
}

LRESULT ExpressDialog::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	QDialog::OnDestroy(uMsg, wParam, lParam, bHandled);
	if (functionsLabel.IsWindow()) functionsLabel.DestroyWindow();
	if (functionsListBox.IsWindow()) functionsListBox.DestroyWindow();

	if (columnsLabel.IsWindow()) columnsLabel.DestroyWindow();
	if (columnsListBox.IsWindow()) columnsListBox.DestroyWindow();

	if (expressEdit.IsWindow()) expressEdit.DestroyWindow();
	return 0;
}

LRESULT ExpressDialog::OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// invisible then return 
	if (!wParam) { 
		return 0;
	}
	loadWindow();
	return 0;
}


void ExpressDialog::OnDbClickFunctionsListBox(UINT uNotifyCode, int nID, HWND hwnd)
{
	int nSelItem = functionsListBox.GetCurSel();
	wchar_t cch[256] = {0};
	functionsListBox.GetText(nSelItem, cch);
	std::wstring func = cch;
	func.append(L"(");
	expressEdit.replaceSelText(func);
}


void ExpressDialog::OnDbClickColumnsListBox(UINT uNotifyCode, int nID, HWND hwnd)
{
	int nSelItem = columnsListBox.GetCurSel();
	wchar_t cch[256] = {0};
	columnsListBox.GetText(nSelItem, cch);
	std::wstring column(cch);
	expressEdit.replaceSelText(column);
}

void ExpressDialog::OnClickYesButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	adapter->getSupplier()->setShardingStrategyExpress(expressEdit.getText());
	EndDialog(Config::QDIALOG_YES_BUTTON_ID);
}
