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

 * @file   QHelpEdit.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-05-24
 *********************************************************************/
#include "stdafx.h"
#include "QHelpEdit.h"
#include "utils/Log.h"
#include "ui/common/QWinCreater.h"
#include <Scintilla/Scintilla.h>
#include <Scintilla/SciLexer.h>

BOOL QHelpEdit::PreTranslateMessage(MSG* pMsg)
{
	if (editor.IsWindow() && editor.PreTranslateMessage(pMsg)) {
		return TRUE;
	}
	return FALSE;
}

void QHelpEdit::setup(std::wstring & helpText, std::wstring & content)
{
	QHelpPage::setup(helpText);
	this->content = content;
}

std::wstring QHelpEdit::getSelText()
{
	return editor.getSelText();
}

std::wstring QHelpEdit::getText()
{
	return editor.getText();
}

void QHelpEdit::focus()
{
	editor.SetFocus();
}

void QHelpEdit::createOrShowUI()
{
	QHelpPage::createOrShowUI();

	CRect clientRect;
	GetClientRect(clientRect);
	crateOrShowEditor(editor, clientRect);
}


void QHelpEdit::crateOrShowEditor(QSqlEdit &win, CRect &clientRect)
{
	CRect rect = getPageRect(clientRect);
	if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
		Q_INFO(L"QHelpEdit(editor),rect.w{}:{},rect.h:{}", rect.Width(), rect.Height());
	}
}

int QHelpEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	int ret = QHelpPage::OnCreate(lpCreateStruct);

	// ע��������ES_MUTIPLELINE,��ON_SIZE����ʱ�����ػ�CEdit������,CEdit������ı������ػ棬CEdit���������
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

	CRect clientRect;
	GetClientRect(clientRect);
	CRect rect = getPageRect(clientRect);
	editor.Create(m_hWnd, rect, 0); // cancel WS_EX_CLIENTEDGE ( 3D )
	editor.initEdit(10, _T("Courier New"));

	return ret;
}

int QHelpEdit::OnDestroy()
{
	int ret = QHelpPage::OnDestroy();
	if (editor.IsWindow()) editor.DestroyWindow();
	editor.m_hWnd = nullptr; // �����������Ϊnull ����Ȼ�ᱨ��
	return ret;
}

HBRUSH QHelpEdit::OnCtlColorEdit(HDC hdc, HWND hwnd)
{	
	if (hwnd == editor.m_hWnd) {
		::SetBkColor(hdc, bkgColor); // �ı����򱳾�ɫ	
	}
	
	return bkgBrush;
}

LRESULT QHelpEdit::OnNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	struct SCNotification* scn = (struct SCNotification*)lParam;
	LPNMHDR pnmh = (LPNMHDR)lParam;
	switch (pnmh->code)
	{
	case SCN_MODIFIED://�޸����ļ�
	case SCN_ZOOM://�Ŵ���С
		if (editor.getMarginWidthN(0) != 0)
			editor.updateLineNumberWidth();
		break;
	case SCN_MARGINCLICK://ȷ����Foldҳ�ߵ���¼�
	{
		long n = editor.lineFromPosition(scn->position);
		editor.toggleFold(n);
	}
	break;
	case SCN_UPDATEUI://�������(������꣬���¼�ͷ��)
		break;
	}

	return 0;
}
