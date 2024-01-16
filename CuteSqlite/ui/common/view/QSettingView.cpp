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

 * @file   QSettingView.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-12-28
 *********************************************************************/
#include "stdafx.h"
#include "QSettingView.h"
#include "utils/FontUtil.h"
#include "utils/GdiPlusUtil.h"
#include "core/common/Lang.h"

QSettingView::~QSettingView()
{
	m_hWnd = nullptr;
}

LRESULT QSettingView::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bkgBrush = ::CreateSolidBrush(bkgColor);
	setionFont = FT(L"setting-section-size");
	sectionDescription = FT(L"setting-section-description-size");
	buttonFont = GDI_PLUS_FT(L"setting-button-size");
	textFont = FT(L"form-text-size");

	return 0;
}

LRESULT QSettingView::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (bkgBrush) ::DeleteObject(bkgBrush);
	if (setionFont) ::DeleteObject(setionFont);
	if (sectionDescription) ::DeleteObject(sectionDescription);
	if (buttonFont) ::DeleteObject(buttonFont);
	if (textFont) ::DeleteObject(textFont);

	return 0;
}

LRESULT QSettingView::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CPaintDC dc(m_hWnd);
	CMemoryDC mdc(dc, dc.m_ps.rcPaint);
	mdc.FillRect(&(dc.m_ps.rcPaint), bkgBrush);

	CRect clientRect;
	GetClientRect(clientRect);
	paintItem(mdc, clientRect);
	return 0;
}

LRESULT QSettingView::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	createOrShowUI();
	return 0;
}

LRESULT QSettingView::OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!wParam) {
		return 0;
	}
	loadWindow();
	return 0;
}

HBRUSH QSettingView::OnCtlColorStatic(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, bkgColor);
	::SetTextColor(hdc, textColor);
	::SelectObject(hdc, textFont);
	return bkgBrush;
}

HBRUSH QSettingView::OnCtlColorEdit(HDC hdc, HWND hwnd)
{
	// 注意这个 hWnd 句柄并不是CCombobox的.
	// 是ComboLBox也就是下拉列表控件的,它的父控件是桌面
	::SetTextColor(hdc, RGB(84, 83, 82)); //文本区域前景色
	::SetBkColor(hdc, RGB(255, 255, 255)); //文本区域背景色
	// 配置字体
	::SelectObject(hdc, textFont);
	return (HBRUSH)::GetStockObject(WHITE_BRUSH);
}

HBRUSH QSettingView::OnCtlColorListBox(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, RGB(255, 255, 255));
	// 配置字体
	::SelectObject(hdc, textFont);

	return (HBRUSH)::GetStockObject(WHITE_BRUSH);
}


BOOL QSettingView::OnEraseBkgnd(CDCHandle dc)
{
	return true;
}

void QSettingView::paintItem(CDC & dc, CRect & paintRect)
{
	// 子类实现
}

/**
 * 画子节点.
 * 
 * @param dc 上下文DC
 * @param pt 节点的开始位置
 * @param section 节点名称
 * @param description 节点的描述
 */
void QSettingView::paintSection(CDC & dc, CPoint &pt, CRect &clientRect, const wchar_t * section, const wchar_t * description)
{
	UINT uFormat = DT_LEFT | DT_VCENTER | DT_END_ELLIPSIS;
	CRect rect(pt.x +5, pt.y, clientRect.right - 40, pt.y + 26);
	
	dc.SetTextColor(textColor);
	dc.SetBkColor(bkgColor);

	// section text
	HFONT oldFont = dc.SelectFont(setionFont);	
	dc.DrawText(section, static_cast<int>(wcslen(section)), rect, uFormat);
	dc.SelectFont(oldFont);

	// line
	CPen pen;
	pen.CreatePen(PS_SOLID, 1, textColor);
	HPEN oldPen = dc.SelectPen(pen);
	dc.MoveTo(pt.x, rect.bottom + 5);
	dc.LineTo(clientRect.right - 40, rect.bottom + 5);
	dc.SelectPen(oldPen);
	pen.DeleteObject();

	// description text
	if (description != nullptr && wcslen(description) > 0) {
		rect = { pt.x + 5, rect.bottom + 5 + 5, clientRect.right - 20, rect.bottom + 5 + 5 + 20 };
		HFONT oldFont = dc.SelectFont(sectionDescription);	
		dc.DrawText(description, static_cast<int>(wcslen(description)), rect, uFormat);
		dc.SelectFont(oldFont);
	}
	
}

void QSettingView::createOrShowEdit(WTL::CEdit & win, UINT id, std::wstring text, std::wstring cueBanner, CRect rect, CRect &clientRect, DWORD exStyle)
{
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_VISIBLE |WS_TABSTOP;
		if (exStyle) {
			dwStyle |= exStyle;
		}
		win.Create(m_hWnd, rect, text.c_str(), dwStyle , 0, id);
		HFONT hfont = FT(L"form-ctls-size");
		win.SetFont(hfont);
		::DeleteObject(hfont);
		win.SetCueBannerText(cueBanner.c_str());
		win.SetMargins(5, 5);

		return;
	} else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);		
	}
}
