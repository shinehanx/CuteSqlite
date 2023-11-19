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

 * @file   QSearchEdit.h
 * @brief  Common search edit, include text edit and search button
 * 
 * @author Xuehan Qin
 * @date   2023-11-14
 *********************************************************************/
#pragma once
#include <atlwin.h>
#include <atlcrack.h>
#include "common/Config.h"
#include "ui/common/button/QImageButton.h"

class QSearchEdit : public CWindowImpl<QSearchEdit> 
{
public:
	BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP_EX(QSearchEdit)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MSG_WM_CTLCOLOREDIT(OnCtlColorEdit)
		COMMAND_HANDLER_EX(Config::COMMON_SEARCH_BUTTON_ID, BN_CLICKED, OnClickSearchButton)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	void setFont(HFONT font);
	void setCueBanner(std::wstring &cueBanner);
	void setSel(int nStartChar, int nEndChar, BOOL bNoScroll = FALSE);
	void setFocus();

	std::wstring getText();
private:

	HFONT editFont = nullptr;
	COLORREF bkgColor = RGB(255, 255, 255);
	CBrush bkgBrush;

	CEdit edit;
	QImageButton button;

	std::wstring curBanner;
	std::wstring text;
	int nStartChar = 0;  
	int nEndChar = 0; 
	BOOL bNoScroll = false;
	bool isFocus = true;

	void createOrShowUI();
	void createOrShowEdit(CEdit &win, CRect &clientRect);
	void createOrShowEdit(CEdit & win, UINT id, std::wstring text, CRect rect, CRect &clientRect, HFONT hfont, DWORD exStyle = 0, bool readOnly = true);
	void createOrShowButton(QImageButton &win, CRect &clientRect);
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	HBRUSH OnCtlColorEdit(HDC hdc, HWND hwnd);
	BOOL OnEraseBkgnd(CDCHandle dc);

	LRESULT OnClickSearchButton(UINT uNotifyCode, int nID, HWND hwnd);
};
