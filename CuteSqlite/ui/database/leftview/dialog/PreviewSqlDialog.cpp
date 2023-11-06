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

 * @file   PreviewSqlDialog.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-10-24
 *********************************************************************/
#include "stdafx.h"
#include "PreviewSqlDialog.h"
#include "ui/common/message/QPopAnimate.h"
#include "utils/SavePointUtil.h"

PreviewSqlDialog::PreviewSqlDialog(HWND parentHwnd, const std::wstring &content /*= std::wstring()*/, const std::wstring &sqlTitle)
{
	setFormSize(PREVIEW_SQL_DIALOG_WIDTH, PREVIEW_SQL_DIALOG_HEIGHT);
	caption = S(L"preview-sql");
	this->parentHwnd = parentHwnd;
	this->content = content;
}

void PreviewSqlDialog::createOrShowUI()
{
	QDialog::createOrShowUI();

	CRect clientRect;
	GetClientRect(clientRect);

	createOrShowSqlPreviewEdit(clientRect);
}


void PreviewSqlDialog::createOrShowSqlPreviewEdit(CRect & clientRect)
{
	CRect rect = clientRect;
	rect.DeflateRect(2, 2, 2, 2);
	rect.bottom = rect.top + (rect.Height() - 30);
	createOrShowSqlEditor(sqlPreviewEdit, Config::PREVIEW_SQL_EDITOR_ID, L"", rect, clientRect);
}


void PreviewSqlDialog::createOrShowSqlEditor(QHelpEdit & win, UINT id, const std::wstring & text, CRect & rect, CRect & clientRect, DWORD exStyle /*= 0*/)
{
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		std::wstring editorTitle = L"These sql statement(s) will be executed.";
		win.setup(editorTitle, std::wstring(L""));
		win.Create(m_hWnd, rect, text.c_str(), WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, id);
		return;
	} else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
	}
}

void PreviewSqlDialog::loadWindow()
{
	if (!isNeedReload) {
		return ;
	}

	isNeedReload = false;
	loadSqlPreviewEdit();
}


void PreviewSqlDialog::loadSqlPreviewEdit()
{
	sqlPreviewEdit.setText(content);
	// parent window may be handle the message to append this specify sql statements to preview editor
	::PostMessage(parentHwnd, Config::MSG_LOADING_SQL_PREVIEW_EDIT_ID, (WPARAM)&sqlPreviewEdit, NULL);
}

LRESULT PreviewSqlDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	QDialog::OnInitDialog(uMsg, wParam, lParam, bHandled); 
	return 0;
}

LRESULT PreviewSqlDialog::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	QDialog::OnDestroy(uMsg, wParam, lParam, bHandled);
	
	if (sqlPreviewEdit.IsWindow()) sqlPreviewEdit.DestroyWindow();
	return 0;
}

LRESULT PreviewSqlDialog::OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// invisible then return 
	if (!wParam) { 
		return 0;
	}
	loadWindow();
	return 0;
}

void PreviewSqlDialog::OnClickYesButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	EndDialog(Config::QDIALOG_YES_BUTTON_ID);
}
