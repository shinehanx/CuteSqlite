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
#include "QParamElem.h"
#include "core/common/Lang.h"
#include "utils/EntityUtil.h"
#include "ui/common/QWinCreater.h"
#include "utils/StringUtil.h"

QParamElem::QParamElem(const ParamElemData & data)
{
	this->data = EntityUtil::copy(data);
}

QParamElem::~QParamElem()
{
	m_hWnd = nullptr;
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
		QWinCreater::createOrShowEdit(m_hWnd, valEdit, 0, data.val, rect, clientRect, 0, false);
	} else if (data.type == READ_ELEM) {
		QWinCreater::createOrShowEdit(m_hWnd, valEdit, 0, data.val, rect, clientRect, 0, true);
	} else if (data.type == COMBO_EDIT_ELEM) {
		QWinCreater::createOrShowComboBox(m_hWnd, valComboBox, 0,  rect, clientRect, true);
	} else if (data.type == COMBO_READ_ELEM) {
		QWinCreater::createOrShowComboBox(m_hWnd, valComboBox, 0,  rect, clientRect, false);
	}

	rect.OffsetRect(rect.Width() + 5, 0);
	rect.right = clientRect.right - 5;
	QWinCreater::createOrShowLabel(m_hWnd, desLabel, data.description, rect, clientRect, SS_LEFT | SS_CENTERIMAGE);
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
	::SetTextColor(hdc, textColor); 
	::SelectObject(hdc, textFont);
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
