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

 * @file   QParamElem.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2024-01-19
 *********************************************************************/
#include "stdafx.h"
#include <string>
#include <WinUser.h>
#include "QParamElem.h"
#include "core/common/Lang.h"
#include "utils/EntityUtil.h"
#include "ui/common/QWinCreater.h"
#include "utils/StringUtil.h"

QParamElem::QParamElem(const ParamElemData & data)
{
	setData(data);
}

QParamElem::~QParamElem()
{
	m_hWnd = nullptr;
}

void QParamElem::setData(const ParamElemData & data)
{
	this->data = EntityUtil::copy(data);
	this->newData = EntityUtil::copy(data);
}

void QParamElem::createOrShowUI()
{
	CRect clientRect;
	GetClientRect(clientRect);

	createOrShowElems(clientRect);
}


void QParamElem::createOrShowElems(CRect & clientRect)
{
	int x = 5, y = 0, w = 220, h = 25;
	CRect rect(x, y, x + w, y + h);
	QWinCreater::createOrShowLabel(m_hWnd, label, data.labelText, rect, clientRect, SS_LEFT | SS_CENTERIMAGE);

	rect.OffsetRect(w + 5, 0);
	rect.right = rect.left + 150;
	if (data.type == EDIT_ELEM) {
		QWinCreater::createOrShowEdit(m_hWnd, valEdit, Config::QPARAMELEM_EDIT_ELEM_ID, data.val, rect, clientRect, 0, false);
	} else if (data.type == READ_ELEM) {
		QWinCreater::createOrShowEdit(m_hWnd, valEdit, Config::QPARAMELEM_READ_ELEM_ID, data.val, rect, clientRect, 0, true);
	} else if (data.type == COMBO_EDIT_ELEM) {
		QWinCreater::createOrShowComboBox(m_hWnd, valComboBox, Config::QPARAMELEM_COMBO_EDIT_ID,  rect, clientRect, true);
	} else if (data.type == COMBO_READ_ELEM) {
		QWinCreater::createOrShowComboBox(m_hWnd, valComboBox, Config::QPARAMELEM_COMBO_READ_ID,  rect, clientRect, false);
	}

	rect.OffsetRect(rect.Width() + 5, 0);
	rect.right = clientRect.right - 5;
	createOrShowLabel(desLabel, data.description, rect, clientRect, SS_LEFT | SS_CENTERIMAGE);
}

void QParamElem::createOrShowLabel(CStatic & win, std::wstring text, CRect rect, CRect &clientRect, DWORD exStyle, int fontSize)
{
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_VISIBLE  | WS_CLIPCHILDREN  | WS_CLIPSIBLINGS ; 
		if (exStyle) dwStyle = dwStyle | exStyle;
		win.Create(m_hWnd, rect, text.c_str(), dwStyle , 0);		
		HFONT font = GdiPlusUtil::GetHFONT(fontSize, DEFAULT_CHARSET, false);
		win.SetFont(font);
		DeleteObject(font);
		win.SetWindowText(text.c_str());
		createAndBindToolTip();	
		return;
	} else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
	}	
}

void QParamElem::createOrShowComboBox(HWND hwnd, CComboBox &win, UINT id, CRect & rect, CRect &clientRect, bool allowEdit) 
{
	if (::IsWindow(hwnd) && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP  | CBS_HASSTRINGS | CBS_AUTOHSCROLL | WS_VSCROLL | WS_HSCROLL; 
		if (allowEdit) {
			dwStyle = dwStyle | CBS_DROPDOWN;
		} else {
			dwStyle = dwStyle | CBS_DROPDOWNLIST;
		}
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

void QParamElem::loadWindow()
{
	if (!isNeedReload) {
		return ;
	}

	isNeedReload = false;
	loadValElem();
}

void QParamElem::loadValElem()
{
	if (data.type == COMBO_READ_ELEM || data.type == COMBO_EDIT_ELEM) {
		valComboBox.ResetContent();
		for (auto & opt : data.options) {			
			std::wstring optVal, optText;
			bool hasVal = false;
			if (opt.find_first_of(L'|') != std::wstring::npos) {
				auto & optVec = StringUtil::split(opt, L"|");
				optText = optVec.at(0);
				optVal = optVec.at(1);	
				hasVal = true;
			} else {
				optVal = opt;
				optText = opt;
			}
			int nItem = valComboBox.AddString(optText.c_str());
			
			if (optVal != data.val) {
				continue;
			}

			valComboBox.SetCurSel(nItem);
			if (data.type == COMBO_EDIT_ELEM) {
				valComboBox.SetWindowText(optText.c_str());
			}
		}
	}
}

LRESULT QParamElem::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bkgBrush.CreateSolidBrush(bkgColor);
	readBrush.CreateSolidBrush(readColor);
	textFont = FT(L"form-text-size"); 
	textPen.CreatePen(PS_SOLID, 1, textColor);
	if (!comboFont) comboFont = FT(L"list-combobox-size");

	return 0;
}

LRESULT QParamElem::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!bkgBrush.IsNull()) bkgBrush.DeleteObject();
	if (!readBrush.IsNull()) readBrush.DeleteObject();
	if (textFont) ::DeleteObject(textFont);
	if (comboFont) ::DeleteObject(comboFont);
	if (!textPen.IsNull()) textPen.DeleteObject();

	if (label.IsWindow()) label.DestroyWindow();
	if (valEdit.IsWindow()) valEdit.DestroyWindow();
	if (valComboBox.IsWindow()) valComboBox.DestroyWindow();
	if (desLabel.IsWindow()) desLabel.DestroyWindow();

	if(tooltipCtrl.IsWindow()) {
		tooltipCtrl.DestroyWindow();
		tooltipCtrl.m_hWnd = NULL;
	}
	return 0;
}

LRESULT QParamElem::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CPaintDC pdc(m_hWnd);
	CMemoryDC mdc(pdc, pdc.m_ps.rcPaint);

	// background
	CRect clientRect(pdc.m_ps.rcPaint);
	mdc.FillRect(clientRect, bkgBrush.m_hBrush);
		
	return 0;
}

LRESULT QParamElem::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	createOrShowUI();
	return 0;
}

LRESULT QParamElem::OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// invisible then return 
	if (!wParam) { 
		return 0;
	}
	loadWindow();
	return 0;
}

BOOL QParamElem::OnEraseBkgnd(CDCHandle dc)
{
	return true;
}

HBRUSH QParamElem::OnCtlStaticColor(HDC hdc, HWND hwnd)
{
	
	::SelectObject(hdc, textFont);
	if (hwnd == label && data.val != getNewVal()) {
		::SetTextColor(hdc, changColor); 
	} else {
		::SetTextColor(hdc, textColor); 
	}
	

	if (data.type == READ_ELEM && valEdit.IsWindow() && valEdit.m_hWnd == hwnd) {
		::SetBkColor(hdc, readColor);
		return readBrush.m_hBrush;
	} else {
		::SetBkColor(hdc, bkgColor);
		return bkgBrush.m_hBrush;
	}
}

HBRUSH QParamElem::OnCtlEditColor(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, bkgColor);
	::SetTextColor(hdc, textColor); 
	::SelectObject(hdc, textFont);
	return bkgBrush.m_hBrush;
}


HBRUSH QParamElem::OnCtlListBoxColor(HDC hdc, HWND hwnd)
{
	::SetTextColor(hdc, textColor); // Text area foreground color
	::SetBkColor(hdc, bkgColor); // Text area background color
	::SelectObject(hdc, comboFont); 
	return AtlGetStockBrush(WHITE_BRUSH);
}

void QParamElem::OnValEditElemChange(UINT uNotifyCode, int nID, HWND hwnd)
{
	if (!valEdit.IsWindow()) {
		return;
	}
	CString text;
	valEdit.GetWindowText(text);
	std::wstring newVal(text.GetString());
	if (newVal != data.val) {
		newData.val = newVal;
		::PostMessage(GetParent().m_hWnd, Config::MSG_QPARAMELEM_VAL_CHANGE_ID, WPARAM(m_hWnd), NULL);
	}
	label.Invalidate(true);
}

void QParamElem::OnValComboBoxChange(UINT uNotifyCode, int nID, HWND hwnd)
{
	if (!valComboBox.IsWindow()) {
		return;
	}
	CString text;
	valComboBox.GetWindowText(text);
	std::wstring newVal(text.GetString());
	if (newVal != data.val) {
		newData.val = newVal;
		::PostMessage(GetParent().m_hWnd, Config::MSG_QPARAMELEM_VAL_CHANGE_ID, WPARAM(m_hWnd), NULL);
	}
	label.Invalidate(true);
}


void QParamElem::createAndBindToolTip()
{
	if (tooltipCtrl.IsWindow() || !desLabel.IsWindow() ) {
		return;
	}
	tooltipCtrl.Create(desLabel.m_hWnd, NULL, NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP);
	tooltipCtrl.AddTool(desLabel.m_hWnd, data.description.c_str());
	tooltipCtrl.Activate(TRUE);		
	
	// 拦截鼠标消息.
	m_pWndProc = (WNDPROC)::GetWindowLongPtr(desLabel.m_hWnd, GWLP_WNDPROC); // 获取原窗口处理函数
	procWndPair.first = m_pWndProc; // 这个是用户定义的类型1,不重要
	procWndPair.second = tooltipCtrl.m_hWnd; // 这个是用户定义的类型2,不重要
	::SetWindowLongPtr(desLabel.m_hWnd, GWLP_USERDATA, (LONG_PTR)&procWndPair); // 设置窗口的自定义数据,用于存储原处理函数和ToolTip句柄
	::SetWindowLongPtr(desLabel.m_hWnd, GWLP_WNDPROC, (LONG_PTR)QParamElem::funcLabelProcWnd); // 自定义一个窗口处理函数，对鼠标消息预先过滤.
}


std::wstring QParamElem::getNewVal()
{
	std::wstring newVal;
	if (data.type == EDIT_ELEM || data.type == READ_ELEM) {
		CString str;
		valEdit.GetWindowText(str);
		newVal = str;		
	} else {
		int i = valComboBox.GetCurSel();
		if (i == -1) {
			return L"";
		}
		wchar_t cch[256];
		valComboBox.GetLBText(i, cch);
		std::wstring newTxt(cch);
		std::wstring text;
		std::wstring val;
		for(auto & item : newData.options) {			
			if (item.find_first_of(L'|') != std::wstring::npos) {
				auto vec = StringUtil::split(item, L"|");
				if (vec.size() > 1) {
					text = vec[0];
					val = vec[1];
				} else {
					text = vec[0];
					val = vec[0];
				}
			} else {
				text = item;
				val = item;
			}
			if (text == newTxt) {
				return val;
			}
		}
	}
	return newVal;
}

LRESULT QParamElem::funcLabelProcWnd(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	auto pp = (std::pair<WNDPROC, HWND> *)::GetWindowLongPtr(hWnd, GWLP_USERDATA);
	WNDPROC funcOld = pp->first;
	auto tooltip_hwnd = pp->second;
	if(nMsg == WM_NCHITTEST){
		// 1.static 控件没有相应 WM_MOUSEMOVE 消息,需要返回一个HTCLIENT来让窗口处理函数执行 WM_MOUSEMOVE 消息.
		// 2.就是把 WM_NCHITTEST 消息转换为 WM_MOUSEMOVE消息.
		return HTCLIENT;
	} else if(nMsg == WM_MOUSEMOVE){
		// WM_MOUSEMOVE
		// WM_NCHITTEST
		// 1.发送一格WM_MOUSEMOVE消息给tooltip控件处理.这样tooltip才会在指定位置显示.
		MSG msg = { hWnd, nMsg, wParam, lParam };
		CToolTipCtrl tip;
		tip.Attach(tooltip_hwnd);
		tip.RelayEvent(&msg);
	}
	return CallWindowProc(funcOld, hWnd, nMsg, wParam, lParam);
}

/**
 * Refresh the data after saved.
 * 
 */
void QParamElem::refreshAfterSaved()
{
	// copy the newData to the data
	this->data.val = newData.val;
}
