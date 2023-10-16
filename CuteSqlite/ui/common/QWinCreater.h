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

 * @file   QWinCreater.h
 * @brief  The common creator util for some ui component window
 * 
 * @author Xuehan Qin
 * @date   2023-05-19
 *********************************************************************/
#pragma once

#include <atlwin.h>
#include <atlctrls.h>
#include <atltypes.h>
#include "utils/GdiPlusUtil.h"
#include "utils/FontUtil.h"
#include "ui/common/button/QDropButton.h"
#include "ui/common/image/QStaticImage.h"
#include "ui/common/image/QImageList.h"
#include "ui/common/checkbox/QCheckBox.h"

class QWinCreater
{
public:
	static void createOrShowButton(HWND hwnd, QImageButton & win, UINT id, std::wstring text, CRect rect, CRect &clientRect, DWORD exStyle = BS_OWNERDRAW)
	{
		if (::IsWindow(hwnd) && !win.IsWindow()) {
			DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN  | WS_CLIPSIBLINGS ; // 
			if (exStyle) dwStyle = dwStyle | exStyle;
			win.Create(hwnd, rect, text.c_str(), dwStyle , 0, id);
			return ;
		} else if (::IsWindow(hwnd) && (clientRect.bottom - clientRect.top) > 0) {
			win.MoveWindow(&rect);
			win.ShowWindow(SW_SHOW);		
		}
	}

	static void createOrShowDropButton(HWND hwnd, QDropButton & win, UINT id, std::wstring text, CRect rect, CRect &clientRect, DWORD exStyle = BS_OWNERDRAW)
	{
		if (::IsWindow(hwnd) && !win.IsWindow()) {
			DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN  | WS_CLIPSIBLINGS ; // 
			if (exStyle) dwStyle = dwStyle | exStyle;
			win.Create(hwnd, rect, text.c_str(), dwStyle , 0, id);
			return ;
		} else if (::IsWindow(hwnd) && (clientRect.bottom - clientRect.top) > 0) {
			win.MoveWindow(&rect);
			win.ShowWindow(SW_SHOW);
		}
	}

	static void createOrShowCheckBox(HWND hwnd, QCheckBox & win, UINT id, std::wstring text, CRect rect, CRect &clientRect, DWORD exStyle = BS_OWNERDRAW)
	{
		if (::IsWindow(hwnd) && !win.IsWindow()) {
			DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN  | WS_CLIPSIBLINGS | BS_CHECKBOX ; // 注意 BS_CHECKBOX | BS_OWNERDRAW 才能出发
			if (exStyle) dwStyle = dwStyle | exStyle;
			win.Create(hwnd, rect, text.c_str(), dwStyle , 0, id);
			return ;
		} else if (::IsWindow(hwnd) && (clientRect.bottom - clientRect.top) > 0) {
			win.MoveWindow(&rect);
			win.ShowWindow(SW_SHOW);		
		}
	}

	static void createOrShowCheckBox(HWND hwnd, CButton & win, UINT id, std::wstring text, CRect rect, CRect &clientRect, DWORD exStyle = 0)
	{
		if (::IsWindow(hwnd) && !win.IsWindow()) {
			DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN  | WS_CLIPSIBLINGS | BS_CHECKBOX ; // 注意 BS_CHECKBOX
			if (exStyle) dwStyle = dwStyle | exStyle;
			win.Create(hwnd, rect, text.c_str(), dwStyle , 0, id);
			win.BringWindowToTop();
			return ;
		} else if (::IsWindow(hwnd) && (clientRect.bottom - clientRect.top) > 0) {
			win.MoveWindow(&rect);
			win.ShowWindow(SW_SHOW);		
		}
	}

	static void createOrShowLabel(HWND hwnd, CStatic & win, std::wstring text, CRect rect, CRect &clientRect, DWORD exStyle = SS_LEFT, int fontSize = 14)
	{
		if (::IsWindow(hwnd) && !win.IsWindow()) {
			DWORD dwStyle = WS_CHILD | WS_VISIBLE  | WS_CLIPCHILDREN  | WS_CLIPSIBLINGS | SS_NOTIFY; // SS_NOTIFY - 表示static接受点击事件
			if (exStyle) dwStyle = dwStyle | exStyle;
			win.Create(hwnd, rect, text.c_str(), dwStyle , 0);		
			HFONT font = GdiPlusUtil::GetHFONT(fontSize, DEFAULT_CHARSET, false);
			win.SetFont(font);
			DeleteObject(font);
			return;
		} else if (::IsWindow(hwnd) && (clientRect.bottom - clientRect.top) > 0) {
			win.MoveWindow(&rect);
			win.ShowWindow(SW_SHOW);
		}
		win.SetWindowText(text.c_str());
	}

	
	static void createOrShowEdit(HWND hwnd, WTL::CEdit & win, UINT id, std::wstring text, CRect rect, CRect &clientRect, DWORD exStyle = WS_CLIPCHILDREN  | WS_CLIPSIBLINGS, bool readOnly=true)
	{
		if (::IsWindow(hwnd) && !win.IsWindow()) {
			DWORD dwStyle = WS_CHILD | WS_VISIBLE |WS_TABSTOP ;
			if (exStyle) {
				dwStyle |= exStyle;
			}
			win.Create(hwnd, rect, text.c_str(), dwStyle , WS_EX_CLIENTEDGE, id);
			HFONT hfont = GdiPlusUtil::GetHFONT(14, DEFAULT_CHARSET, true);
			win.SetFont(hfont);
			win.SetReadOnly(readOnly);
			win.SetWindowText(text.c_str());
			DeleteObject(hfont);

			return;
		} else if (::IsWindow(hwnd) && (clientRect.bottom - clientRect.top) > 0) {
			win.MoveWindow(&rect);
			win.ShowWindow(SW_SHOW);
		}
	}

	static void createOrShowEdit(HWND hwnd, WTL::CEdit & win, UINT id, std::wstring text, CRect rect, CRect &clientRect, HFONT hfont, DWORD exStyle = 0,  bool readOnly=true)
	{
		if (::IsWindow(hwnd) && !win.IsWindow()) {
			DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN  | WS_CLIPSIBLINGS | WS_TABSTOP; // WS_TABSTOP 
			if (exStyle) {
				dwStyle |= exStyle;
			}

			win.Create(hwnd, rect, text.c_str(), dwStyle , WS_EX_CLIENTEDGE, id);
			win.SetFont(hfont);
			win.SetReadOnly(readOnly);			
			//win.SetWindowText(text.c_str());
			win.SetSel(0, 0, true);

			return;
		} else if (::IsWindow(hwnd) && (clientRect.bottom - clientRect.top) > 0) {
			win.MoveWindow(&rect);
			win.ShowWindow(SW_SHOW);
		}
	}
	
	static void createOrShowComboBox(HWND hwnd, CComboBox &win, UINT id, CRect & rect, CRect &clientRect, DWORD exStyle = 0) 
	{
		if (::IsWindow(hwnd) && !win.IsWindow()) {
			DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_HASSTRINGS;
			if (exStyle) dwStyle = dwStyle | exStyle;
			win.Create(hwnd, rect, L"",  dwStyle , 0, id);
			HFONT hfont = GdiPlusUtil::GetHFONT(18, DEFAULT_CHARSET, false);
			win.SetFont(hfont);
			DeleteObject(hfont);
			return;
		} else if (::IsWindow(hwnd) && (clientRect.bottom - clientRect.top) > 0) {
			win.MoveWindow(&rect);
			win.ShowWindow(SW_SHOW);
		}
	}

	static void createOrShowListBox(HWND hwnd, CListBox &win, UINT id, CRect & rect, CRect &clientRect, DWORD exStyle = 0) 
	{
		if (::IsWindow(hwnd) && !win.IsWindow()) {
			DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_CLIPCHILDREN  | WS_CLIPSIBLINGS | LBS_MULTIPLESEL;
			if (exStyle) dwStyle = dwStyle | exStyle;
			win.Create(hwnd, rect, L"",  dwStyle , 0, id);
			return;
		} else if (::IsWindow(hwnd) && (clientRect.bottom - clientRect.top) > 0) {
			win.MoveWindow(&rect);
			win.ShowWindow(SW_SHOW);
		}
	}

	static void createOrShowImage(HWND hwnd, QImage &win, UINT id, CRect & rect, CRect & clientRect)
	{
		if (::IsWindow(hwnd) && !win.IsWindow()) {
			DWORD dwStyle = WS_CHILD | WS_VISIBLE |WS_CLIPCHILDREN  | WS_CLIPSIBLINGS | SS_NOTIFY; // SS_NOTIFY - 表示static接受点击事件
		
			win.Create(hwnd, rect, L"", dwStyle , 0);
			return;
		} else if (::IsWindow(hwnd) && (clientRect.bottom - clientRect.top) > 0) {
			win.MoveWindow(&rect);
			win.ShowWindow(SW_SHOW);
		}
	}

	static void createOrShowImage(HWND hwnd, QStaticImage &win, UINT id, CRect & rect, CRect & clientRect)
	{
		if (::IsWindow(hwnd) && !win.IsWindow()) {
			DWORD dwStyle = WS_CHILD | WS_VISIBLE |WS_CLIPCHILDREN  | WS_CLIPSIBLINGS | SS_NOTIFY; // SS_NOTIFY - 表示static接受点击事件
		
			win.Create(hwnd, rect, L"", dwStyle , 0);
			return;
		} else if (::IsWindow(hwnd) && (clientRect.bottom - clientRect.top) > 0) {
			win.MoveWindow(&rect);
			win.ShowWindow(SW_SHOW);
		}
	}

	static void createOrShowImage(HWND hwnd, QStaticImage * win, UINT id, CRect & rect, CRect & clientRect)
	{
		if (::IsWindow(hwnd) && !win->IsWindow()) {
			DWORD dwStyle = WS_CHILD | WS_VISIBLE |WS_CLIPCHILDREN  | WS_CLIPSIBLINGS | SS_NOTIFY; // SS_NOTIFY - 表示static接受点击事件
		
			win->Create(hwnd, rect, L"", dwStyle , 0);
			return;
		} else if (::IsWindow(hwnd) && (clientRect.bottom - clientRect.top) > 0) {
			win->MoveWindow(&rect);
			win->ShowWindow(SW_SHOW);
		}
	}


	static void createOrShowImageList(HWND hwnd, QImageList &win, UINT id, CRect & rect, CRect & clientRect)
	{
		if (::IsWindow(hwnd) && !win.IsWindow()) {
			DWORD dwStyle = WS_CHILD | WS_VISIBLE |WS_CLIPCHILDREN  | WS_CLIPSIBLINGS  ; // SS_NOTIFY - 表示static接受点击事件
		
			win.Create(hwnd, rect, L"", dwStyle , 0);
			return;
		} else if (::IsWindow(hwnd) && (clientRect.bottom - clientRect.top) > 0) {
			win.MoveWindow(&rect);
			win.ShowWindow(SW_SHOW);
		}
	}
};
