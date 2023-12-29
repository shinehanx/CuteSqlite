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

 * @file   QListMenuItem.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-12-28
 *********************************************************************/
#include "stdafx.h"
#include "QListMenuItem.h"
#include <gdiplus.h>
#include <atltypes.h>
#include "utils/GdiPlusUtil.h"
#include "common/Config.h"
#include "utils/Log.h"
#include "utils/FontUtil.h"
#include "core/common/Lang.h"

LRESULT QListMenuItem::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!m_bTracking) {  
        TRACKMOUSEEVENT tme;  
        tme.cbSize = sizeof(TRACKMOUSEEVENT);  
        tme.dwFlags = TME_LEAVE | TME_HOVER ;//要触发的消息类型  
        tme.hwndTrack = m_hWnd;  
        tme.dwHoverTime = 10;// 如果不设此参数,无法触发mouseHover  
  
		 //MOUSELEAVE|MOUSEHOVER消息由此函数触发.  
        if (::_TrackMouseEvent(&tme)) {  
            m_bTracking = true;     
        }  
    }
    return 0;  
}


LRESULT QListMenuItem::OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(status == kStatusPressed){
		bHandled = FALSE;
		return 0;
	}
	status = kStatusHover;
	Invalidate(FALSE);
	bHandled = FALSE;

	//背景色/文字颜色/边框颜色
	color_backgroud  = RGB(GetRValue(pressed_bkg_color), GetGValue(pressed_bkg_color), GetBValue(pressed_bkg_color));
	color_border = RGB(GetRValue(pressed_border_color), GetGValue(pressed_border_color), GetBValue(pressed_border_color));
	color_text = RGB(GetRValue(pressed_font_color), GetGValue(pressed_font_color), GetBValue(pressed_font_color));
	
	return 0;
}

LRESULT QListMenuItem::OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(status == kStatusPressed){
		bHandled = FALSE;
		return 0;
	}

	status = kStatusNormal;
	Invalidate(FALSE);
	m_bTracking = false;
	bHandled = FALSE;

	//背景色/文字颜色/边框颜色
	color_backgroud  = RGB(GetRValue(normal_bkg_color), GetGValue(normal_bkg_color), GetBValue(normal_bkg_color));
	color_border = RGB(GetRValue(normal_border_color), GetGValue(normal_border_color), GetBValue(normal_border_color));
	color_text = RGB(GetRValue(normal_font_color), GetGValue(normal_font_color), GetBValue(normal_font_color));

	return 0; 
}

LRESULT QListMenuItem::OnMousePressed(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	status = kStatusPressed;
	Invalidate(FALSE);
	m_bTracking = false;
	bHandled = FALSE;

	//背景色/文字颜色/边框颜色
	color_backgroud  = RGB(GetRValue(pressed_border_color), GetGValue(pressed_border_color), GetBValue(pressed_border_color));
	color_border = RGB(GetRValue(pressed_border_color), GetGValue(pressed_border_color), GetBValue(pressed_border_color));
	color_text = RGB(GetRValue(pressed_font_color), GetGValue(pressed_font_color), GetBValue(pressed_font_color));

	// 向父窗口QListMenu发射消息MSG_MENU_ITEM_PRESSED_ID, 带上wParam参数:窗口Id
	// QListMenuItem->QListMenu->AnalysisMenuBar->AnalysisPanel->HomeView->MainFrm,这里取上一级的QListMenu发送
	::PostMessage(GetParent().m_hWnd, Config::MSG_MENU_ITEM_CLICK_ID, (WPARAM)::GetWindowLong(m_hWnd, GWL_ID), (LPARAM)nullptr);
	return 0;
}


LRESULT QListMenuItem::OnHotItemChange(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
	return TRUE;
}

LRESULT QListMenuItem::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	/*
	Gdiplus::FontFamily fontFamily(L"Microsoft Yahei UI");
	font = new Gdiplus::Font(&fontFamily, 32, 
			(false)?Gdiplus::FontStyleBold : Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);*/
	textFont = FontUtil::getFont(textFontSize, true, L"Microsoft Yahei UI");
	rightTriangleBrush = ::CreateSolidBrush(rightTriangleColor);

	bHandled = FALSE;
	status = kStatusNormal;
	m_bTracking = false;

	return 0;
}


LRESULT QListMenuItem::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (textFont) ::DeleteObject(textFont);
	if (rightTriangleBrush) ::DeleteObject(rightTriangleBrush);
	return 0;
}


LRESULT QListMenuItem::OnPaint(HDC hdc)
{
	CPaintDC dc(m_hWnd);
	CRect dcRect(dc.m_ps.rcPaint);
	CMemoryDC mdc(dc,dc.m_ps.rcPaint);

	//HBRUSH brush = AtlGetStockBrush(GRAY_BRUSH);
	HBRUSH brush = ::CreateSolidBrush(color_backgroud);
	mdc.FillRect(&dc.m_ps.rcPaint, brush);
	::DeleteObject(brush);

	
	CString str;
	GetWindowText(str);

	// 获得文本的宽高
	CClientDC clientDc(m_hWnd);	
	clientDc.SelectFont(textFont);
	CSize size;
	clientDc.GetTextExtent(str, str.GetLength(), &size);

	mdc.SelectFont(textFont);
	CRect rect = {dcRect.Width() - 20 - size.cx, (dcRect.Height() - size.cy)/2, dcRect.Width() - 20, (dcRect.Height() - size.cy)/2 +size.cy};
	
	mdc.SetBkColor(color_backgroud); 
	mdc.SetTextColor(color_text);
	mdc.DrawText(str, str.GetLength(), rect, DT_RIGHT | DT_VCENTER);

	// 最右边的白色小三角形
	if (status == kStatusPressed) {		
		// 右边的两点的y轴不能小于20
		int mid_y = (dcRect.Height() - size.cy) / 2 + size.cy / 2;
		CPoint points[4] = { 
			{dcRect.Width() - 10, mid_y}, // 最左边的点
			{dcRect.right, mid_y - 8}, // 右上角的点
			{dcRect.right, mid_y + 8}, // 右下角的点
			{dcRect.Width() - 10, mid_y} // 回到最左边的点，4个点封闭成三角形
		};
		HBRUSH oldBr = mdc.SelectBrush(rightTriangleBrush);
		mdc.Polygon(points, 4);
		// 画两条线取锯齿
		CPen pen;
		pen.CreatePen(PS_SOLID, 1, rightTriangleColor);
		HPEN oldPen = mdc.SelectPen(pen);
		mdc.MoveTo(points[0]);
		mdc.LineTo(points[1]);
		mdc.MoveTo(points[0]);
		mdc.LineTo(points[2]);

		mdc.SelectPen(oldPen);
		mdc.SelectBrush(oldBr);
		::DeleteObject(pen);
	}
	

	return 0;
}

BOOL QListMenuItem::OnEraseBkgnd(CDCHandle cdc)
{
	return TRUE;
}



LRESULT QListMenuItem::OnSetText(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CString str = (LPCTSTR)lParam;
	// Q_DEBUG(L" QListMenuItem::OnSetText , str:{}", str);
	return 0;
}

void QListMenuItem::setTextColor(COLORREF color)
{
	color_text = color;
}

void QListMenuItem::setBkgColor(COLORREF color)
{
	color_backgroud = color;
}

void QListMenuItem::setRightTriangleColor(COLORREF color)
{
	rightTriangleColor = color;
	if (rightTriangleBrush) ::DeleteObject(rightTriangleBrush);
	rightTriangleBrush = ::CreateSolidBrush(rightTriangleColor);
}

QListMenuItem::ItemStatus QListMenuItem::getStatus()
{
	return this->status;
}

void QListMenuItem::setStatus(QListMenuItem::ItemStatus status)
{
	this->status = status;
}


UINT QListMenuItem::getID()
{
	UINT nID = ::GetWindowLong(m_hWnd, GWL_ID);
	return nID;
}

void QListMenuItem::setBorderColor(COLORREF color)
{
	color_border = color;
}

void QListMenuItem::setFontColors(DWORD _normal_font_color, DWORD _pressed_font_color, DWORD _disable_font_color)
{
	this->normal_font_color = _normal_font_color;
	this->pressed_font_color = _pressed_font_color;
	this->disable_font_color = _disable_font_color;

	//背景色/文字颜色/边框颜色
	color_text = normal_font_color;
}

void QListMenuItem::setBkgColors(DWORD _normal_bkg_color, DWORD _pressed_bkg_color, DWORD _disable_bkg_color)
{
	this->normal_bkg_color = _normal_bkg_color;
	this->pressed_bkg_color = _pressed_bkg_color;
	this->disable_bkg_color = _disable_bkg_color;
	color_backgroud  = disable_bkg_color;

}

void QListMenuItem::setBorderColors(DWORD _normal_border_color, DWORD _pressed_border_color, DWORD _disable_border_color)
{
	this->normal_border_color = _normal_border_color;
	this->pressed_border_color = _pressed_border_color;
	this->disable_border_color = _disable_border_color;
	color_border = normal_border_color;
}

void QListMenuItem::setTextFont(HFONT font)
{
	DeleteObject(textFont);
	textFont = font;
}

void QListMenuItem::setTextFontSize(int fontSize)
{
	DeleteObject(textFont);
	this->textFontSize = fontSize;
	std::wstring fontName = Lang::fontName();
	textFont = FontUtil::getFont(textFontSize, true, fontName.c_str());
}
