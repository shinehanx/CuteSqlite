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

 * @file   QHorizontalBar.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2024-01-19
 *********************************************************************/
#include "stdafx.h"
#include <string>
#include "QHorizontalBar.h"
#include "core/common/Lang.h"

QHorizontalBar::QHorizontalBar(const std::wstring & val, double percent /*= 0*/, COLORREF processColor/* = RGB(49, 139, 202)*/)
{
	this->val = val;
	this->percent = percent;
	this->processColor = processColor;
}

QHorizontalBar::~QHorizontalBar()
{
	m_hWnd = nullptr;
}

void QHorizontalBar::draw(const std::wstring & val, double percent)
{
	this->val = val;
	this->percent = percent;	
	Invalidate(true);
}

void QHorizontalBar::error(const std::wstring & err)
{
	this->err = err;
	Invalidate(true);
}

void QHorizontalBar::reset()
{
	this->percent = 0;
	this->err.clear();
	Invalidate(true);
}

/**
 * 颜色：背景颜色和进度颜色.
 * 
 * @param bkgColor 背景颜色
 * @param processColor 进度颜色.
 */
void QHorizontalBar::setColors(COLORREF bkgColor, COLORREF processColor)
{
	this->bkgColor = bkgColor;
	this->processColor = processColor;

	if (!bkgBrush.IsNull()) bkgBrush.DeleteObject();
	if (!processBrush.IsNull()) processBrush.DeleteObject();

	bkgBrush.CreateSolidBrush(bkgColor);
	processBrush.CreateSolidBrush(processColor);
}

LRESULT QHorizontalBar::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bkgBrush.CreateSolidBrush(bkgColor);
	processBrush.CreateSolidBrush(processColor);
	errorBrush.CreateSolidBrush(errorColor);
	textFont = FT(L"process-text-size"); 
	textPen.CreatePen(PS_SOLID, 1, textColor);

	CRect clientRect;
	GetClientRect(clientRect);
	// 画圆角
	 hRgn = ::CreateRoundRectRgn(0, 0, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, 10, 10);
	 ::SetWindowRgn(m_hWnd, hRgn, TRUE);

	return 0;
}

LRESULT QHorizontalBar::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!bkgBrush.IsNull()) bkgBrush.DeleteObject();
	if (!processBrush.IsNull()) processBrush.DeleteObject();
	if (!errorBrush.IsNull()) errorBrush.DeleteObject();
	if (textFont) ::DeleteObject(textFont);
	if (!textPen.IsNull()) textPen.DeleteObject();

	if (hRgn)  DeleteObject(hRgn);
	return 0;
}

LRESULT QHorizontalBar::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CPaintDC pdc(m_hWnd);
	CMemoryDC mdc(pdc, pdc.m_ps.rcPaint);

	// background
	CRect clientRect(pdc.m_ps.rcPaint);
	mdc.FillRect(clientRect, bkgBrush.m_hBrush);

	// Draw percent
	double perPixel = clientRect.Width() * 1.0 / 100.0;
	int pixel = int(round(perPixel * percent));
	int x = clientRect.left, y = clientRect.top, w = pixel, h = clientRect.Height();
	CRect rect(x, y, x + w, y + h);
	if (err.empty()) {
		if (val != L"0") {
			mdc.FillRect(rect, processBrush.m_hBrush);
		}		
	} else {
		mdc.FillRect(rect, errorBrush.m_hBrush); 
	}
	

	//  Draw text
	std::wstring text = val;
	wchar_t buff[16];
	swprintf_s(buff, 16, L"%.2f", percent); 
	std::wstring perText = percent > 0 && percent < 100 ? std::wstring(buff) + L"%" : L"";
	UINT uFormat = DT_LEFT | DT_VCENTER | DT_END_ELLIPSIS;
	HFONT oldFont = mdc.SelectFont(textFont);
	HPEN oldPen = mdc.SelectPen(textPen);
	x = 5, y = rect.top + 2, w = 80, h = 20;
	CRect textRect(x, y, x + w, y + h);
	COLORREF oldTextColor = mdc.SetTextColor(textColor);
	COLORREF textBkColor =  processColor;
	//COLORREF oldTextBkgColor = mdc.SetBkColor(textBkColor);
	int oldMode = mdc.SetBkMode(TRANSPARENT);
	mdc.DrawText(text.c_str(), static_cast<int>(text.size()), textRect, uFormat);
	if (!perText.empty()) {
		x = rect.Width() - 80 - 5;
		if (x < textRect.right) {
			x = textRect.right + 5;
		}
		CRect perRect(x, y, x + w, y + h);
		mdc.DrawText(perText.c_str(), static_cast<int>(perText.size()), perRect, DT_RIGHT | DT_VCENTER | DT_END_ELLIPSIS);
	}
	mdc.SetTextColor(oldTextColor);
	//mdc.SetBkColor(oldTextBkgColor);
	mdc.SelectPen(oldPen);
	mdc.SelectFont(oldFont);
	mdc.SetBkMode(oldMode);
	return 0;
}

BOOL QHorizontalBar::OnEraseBkgnd(CDCHandle dc)
{
	return true;
}

