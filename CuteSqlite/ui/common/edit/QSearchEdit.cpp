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

 * @file   QSearchEdit.cpp
 * @brief  Common search edit, include text edit and search button
 * 
 * @author Xuehan Qin
 * @date   2023-11-14
 *********************************************************************/
#include "stdafx.h"
#include "QSearchEdit.h"
#include <atlmisc.h>
#include "core/common/Lang.h"
#include "ui/common/QWinCreater.h"
#include "utils/ResourceUtil.h"

#define SEARCH_BTN_WIDTH 30

BOOL QSearchEdit::PreTranslateMessage(MSG* pMsg)
{
	// 捕捉回车键
	if (WM_KEYDOWN == pMsg->message && VK_RETURN == pMsg->wParam) {
		//根据不同控件焦点判断是那个在执行 
		if (::GetFocus() == GetDlgItem(Config::SEARCH_EDIT_ID).m_hWnd) {
			HWND pHwnd = GetParent().m_hWnd;
			::PostMessage(pHwnd, Config::MSG_SEARCH_BUTTON_ID, NULL, NULL);
		}
	}

	return FALSE;
}

void QSearchEdit::setFont(HFONT font)
{
	if (font == nullptr) return ;

	if (editFont)::DeleteObject(editFont);
	editFont = font;
	if (edit.IsWindow()) {
		edit.SetFont(editFont);
	}
}


void QSearchEdit::setCueBanner(std::wstring &cueBanner)
{
	this->curBanner = curBanner;
	if (edit.IsWindow()) {
		edit.SetCueBannerText(cueBanner.c_str());
	}
}


void QSearchEdit::setSel(int nStartChar, int nEndChar, BOOL bNoScroll)
{
	this->nStartChar = nStartChar;
	this->nEndChar = nEndChar;
	this->bNoScroll = bNoScroll;

	if (edit.IsWindow()) {
		edit.SetSel(nStartChar, nEndChar, bNoScroll);
	}
}


void QSearchEdit::setFocus()
{
	this->isFocus = true;
	if (edit.IsWindow()) {
		edit.SetFocus();
	}
}

std::wstring QSearchEdit::getText()
{
	CString str;
	edit.GetWindowText(str);
	text = str.GetString();
	return text;
}

void QSearchEdit::createOrShowUI()
{
	CRect clientRect;
	GetClientRect(clientRect);

	createOrShowEdit(edit, clientRect);
	createOrShowButton(button, clientRect);
}


void QSearchEdit::createOrShowEdit(CEdit &win, CRect &clientRect)
{
	CRect rect(clientRect.left+5, clientRect.top+3, clientRect.right - SEARCH_BTN_WIDTH, clientRect.bottom-3);
	createOrShowEdit(win, Config::COMMON_SEARCH_BUTTON_ID, L"", rect, clientRect, editFont, WS_VISIBLE, false);
	if (nStartChar) {
		win.SetSel(nStartChar, nEndChar, bNoScroll);
	}
	if (isFocus) {
		win.SetFocus();
	}
	if (!curBanner.empty()) {
		win.SetCueBannerText(curBanner.c_str());
	}
}

void QSearchEdit::createOrShowEdit(CEdit & win, UINT id, std::wstring text, CRect rect, CRect &clientRect, HFONT hfont, DWORD exStyle,  bool readOnly)
{
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN  | WS_CLIPSIBLINGS | WS_TABSTOP; // WS_TABSTOP 
		if (exStyle) {
			dwStyle |= exStyle;
		}

		win.Create(m_hWnd, rect, text.c_str(), dwStyle , 0, id);
		win.SetFont(hfont);
		win.SetReadOnly(readOnly);
		win.SetSel(0, 0, true);

		return;
	} else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
	}
}

void QSearchEdit::createOrShowButton(QImageButton &win, CRect &clientRect)
{
	CRect rect(clientRect.right - SEARCH_BTN_WIDTH, clientRect.top, clientRect.right, clientRect.bottom);

	std::wstring imageDir = ResourceUtil::getProductImagesDir();
	std::wstring normalImagePath = imageDir + L"common\\search-button-normal.png";
	std::wstring pressedImagePath = imageDir + L"common\\search-button-pressed.png";
	win.SetIconPath(normalImagePath, pressedImagePath);
	QWinCreater::createOrShowButton(m_hWnd, win, Config::COMMON_SEARCH_BUTTON_ID, L"", rect, clientRect);
}

LRESULT QSearchEdit::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	editFont = FTB(L"search-edit-size", true);
	bkgBrush = ::CreateSolidBrush(bkgColor);
	return 0;
}

LRESULT QSearchEdit::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (editFont) ::DeleteObject(editFont);	
	if (bkgBrush) ::DeleteObject(bkgBrush);
	editFont = nullptr;

	if (edit.IsWindow()) edit.DestroyWindow();
	if (button.IsWindow()) button.DestroyWindow();

	return 0;
}

LRESULT QSearchEdit::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	createOrShowUI();
	return 0;
}

LRESULT QSearchEdit::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC pdc(m_hWnd);
	pdc.FillRect(&(pdc.m_ps.rcPaint), bkgBrush);
	return 0;
}


HBRUSH QSearchEdit::OnCtlColorEdit(HDC hdc, HWND hwnd)
{
	::SetTextColor(hdc, RGB(0,0,0)); //文本区域前景色 
	::SetBkColor(hdc, bkgColor); // 文本区域背景色
	::SelectObject(hdc, editFont);
	return AtlGetStockBrush(WHITE_BRUSH);
}

BOOL QSearchEdit::OnEraseBkgnd(CDCHandle dc)
{
	return true;
}

LRESULT QSearchEdit::OnClickSearchButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	HWND pHwnd = GetParent().m_hWnd;
	::PostMessage(pHwnd, Config::MSG_SEARCH_BUTTON_ID, NULL, NULL);
	return 0;
}
