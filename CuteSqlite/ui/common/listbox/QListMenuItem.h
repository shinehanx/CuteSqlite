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

 * @file   QListMenuItem.h
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-12-28
 *********************************************************************/
#pragma once
#include "stdafx.h"
#include <atlbase.h>
#include <atlapp.h>

#include <atlcrack.h>
#include <atlwin.h>
#include <atlframe.h>
#include <atlctrls.h>
#include <atlctrlx.h>
#include <atlmisc.h>
#include <GdiPlus.h>
#include <string>

class QListMenuItem :public CWindowImpl<QListMenuItem, CStatic>
{
public:
	BEGIN_MSG_MAP_EX(QListMenuItem)	
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MSG_WM_PAINT(OnPaint)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSEHOVER, OnMouseHover) 
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnMousePressed)
		MESSAGE_HANDLER(WM_SETTEXT, OnSetText)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	// 按钮的鼠标点击状态
	typedef enum {
		kStatusNormal,
		kStatusPressed,
		kStatusHover,
		kStatusDisable
	} ItemStatus;	

	// 控制字体/文字颜色/背景颜色/边框颜色
	void setTextColor(COLORREF color);
	void setFontColors(DWORD normal_font_color, DWORD pressed_font_color, DWORD disable_font_color);
	void setBkgColors(DWORD normal_bkg_color, DWORD pressed_bkg_color, DWORD disable_bkg_color);
	void setBorderColors(DWORD normal_border_color, DWORD pressed_border_color, DWORD disable_border_color);
	void setTextFont(HFONT theFont);
	void setTextFontSize(int fontSize);
	void setBorderColor(COLORREF color);
	void setBkgColor(COLORREF color);
	void setRightTriangleColor(COLORREF color);

	//控制鼠标状态
	ItemStatus getStatus();
	void setStatus(ItemStatus status);
	UINT getID();
private:
	bool	m_bTracking;
	ItemStatus status;	//鼠标的状态
	int		itemHeight; //非选中状态的高度

	HFONT  textFont = nullptr; // 用于GDI的文本字体
	int textFontSize = 30; // 用于GDI的文本字体
	COLORREF rightTriangleColor = RGB(255, 255, 255); //菜单项最右边的三角形颜色
	HBRUSH rightTriangleBrush = nullptr; //菜单项最右边的三角形画刷

	COLORREF color_text = RGB(153,153,153);
	COLORREF color_backgroud = RGB(255,255,255);
	COLORREF color_border = RGB(255,255,255);

	DWORD normal_bkg_color = RGB(255, 255, 255);
	DWORD normal_font_color = RGB(153, 153, 153);
	DWORD normal_border_color = RGB(255, 255, 255);

	DWORD pressed_bkg_color = RGB(255, 255, 255);
	DWORD pressed_border_color = RGB(255, 255, 255);
	DWORD pressed_font_color = RGB(0x12, 0x96, 0xdb);

	DWORD disable_bkg_color = RGB(255, 255, 255);
	DWORD disable_border_color = RGB(255, 255, 255);
	DWORD disable_font_color = RGB(153, 153, 153);

	
	virtual LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnMousePressed(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT OnHotItemChange(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnPaint(HDC wParam);
	virtual BOOL OnEraseBkgnd(CDCHandle dc);

	
	virtual LRESULT OnSetText(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};
