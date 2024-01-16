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

 * @file   QSettingView.h
 * @brief  Base class for Setting views
 * 
 * @author Xuehan Qin
 * @date   2023-12-28
 *********************************************************************/
#pragma once
#include <string>
#include <atlwin.h>
#include <atlcrack.h>
#include <atltypes.h>
#include <gdiplus.h>
class QSettingView : public CWindowImpl<QSettingView>
{
public:
	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP_EX(QSettingView)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_SHOWWINDOW, OnShowWindow)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MSG_WM_CTLCOLORSTATIC(OnCtlColorStatic)
		MSG_WM_CTLCOLOREDIT(OnCtlColorEdit)
		MSG_WM_CTLCOLORLISTBOX(OnCtlColorListBox)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	~QSettingView();
	void setIsNeedReload(bool isNeed) {
		isNeedReload = isNeed;
	}
protected:
	bool isNeedReload = true;
	COLORREF bkgColor = RGB(118, 118, 118);
	HBRUSH bkgBrush = nullptr;
	COLORREF textColor = RGB(255, 255, 255);
	HFONT setionFont = nullptr;
	HFONT sectionDescription = nullptr;
	Gdiplus::Font * buttonFont = nullptr;
	HFONT textFont = nullptr;

	virtual void createOrShowUI() {};
	virtual void loadWindow() {};

	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual HBRUSH  OnCtlColorStatic(HDC hdc, HWND hwnd);
	virtual HBRUSH  OnCtlColorEdit(HDC hdc, HWND hwnd);
	virtual HBRUSH  OnCtlColorListBox(HDC hdc, HWND hwnd);
	BOOL OnEraseBkgnd(CDCHandle dc);

	// 子类实现这个方法，扩展画面内容
	virtual void paintItem(CDC & dc, CRect & paintRect);

	// 画子节点
	void paintSection(CDC & dc, CPoint &pt, CRect &clientRect, const wchar_t * section, const wchar_t * description);
	void createOrShowEdit(WTL::CEdit & win, UINT id, std::wstring text, std::wstring cueBanner, CRect rect, CRect &clientRect, DWORD exStyle = 0);
};
