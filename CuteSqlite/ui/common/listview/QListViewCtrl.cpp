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

 * @file   QListViewCtrl.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-06-22
 *********************************************************************/
#include "stdafx.h"
#include "QListViewCtrl.h"
#include <atlstr.h>


BOOL QListViewCtrl::PreTranslateMessage(MSG* pMsg)
{
	// press enter key or the mouse wheel
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN && pMsg->hwnd == subItemEdit.m_hWnd) {
		changeSubItemText();
		return TRUE;
	} else if (pMsg->hwnd == m_hWnd && pMsg->message == WM_MOUSEWHEEL) {
		changeSubItemText();
		//return FALSE;
	}

	return FALSE;
}

void QListViewCtrl::createOrShowEdits(std::pair<int, int> subItemPos, CRect & clientRect)
{
	if (subItemPos.first < 0 || subItemPos.second < 0) {
		return;
	}
	CRect listRect;
	this->subItemPos = subItemPos;
	GetSubItemRect(subItemPos.first, subItemPos.second, LVIR_BOUNDS, subItemRect);
	

	wchar_t cch[1024];
	memset(cch, 0, sizeof(cch));
	BSTR itemText = cch;
	GetItemText(subItemPos.first, subItemPos.second, cch, 1024);
	std::wstring text = itemText;
	createOrShowSubItemEdit(subItemEdit, text, subItemRect);
}


QListViewCtrl::SubItemValues QListViewCtrl::getChangedVals()
{
	return changeVals;
}


void QListViewCtrl::clearChangeVals()
{
	changeVals.clear();
}

/**
 * change the sub item text from edit.
 * 
 */
void QListViewCtrl::changeSubItemText()
{	
	if (subItemPos.first < 0 || subItemPos.second < 0) {
		return;
	}

	if (isVisibleEdit && subItemEdit.IsWindow()) {
		CString text;
		subItemEdit.GetWindowText(text);
		//SetItemText(subItemPos.first, subItemPos.second, text);
		SubItemValue subItemVal;
		wchar_t cch[1024];
		memset(cch, 0, sizeof(cch));
		BSTR itemText = cch;
		GetItemText(subItemPos.first, subItemPos.second, cch, 1024);
		

		subItemVal.subItemPos = { subItemPos.first, subItemPos.second };
		subItemVal.origVal.assign(cch);
		subItemVal.newVal = text;

		// it will be return when original text equals new text
		if (subItemVal.origVal == subItemVal.newVal) {
			subItemEdit.DestroyWindow();
			isVisibleEdit = false;
			return;
		}
		changeVals.push_back(subItemVal);
		subItemEdit.DestroyWindow();
		isVisibleEdit = false;

		::PostMessage(GetParent().m_hWnd, Config::MSG_QLISTVIEW_SUBITEM_TEXT_CHANGE_ID, WPARAM(subItemPos.first), LPARAM(subItemPos.second));
	}
}

void QListViewCtrl::createOrShowSubItemEdit(CEdit & win, std::wstring & text, CRect & rect)
{
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN  | WS_CLIPSIBLINGS | ES_AUTOHSCROLL;
		win.Create(m_hWnd, rect, L"", dwStyle , WS_EX_CLIENTEDGE, Config::QLISTVIEWCTRL_SUBITEM_EDIT_ID);
		win.SetLimitText(1024);
	} else if (::IsWindow(m_hWnd) && win.IsWindow()) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
	}
	isVisibleEdit = true;
	win.SetWindowText(text.c_str());
	win.BringWindowToTop();
	win.SetSel(0, -1, false);
	win.SetFocus();
}


LRESULT QListViewCtrl::OnVScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{	 
	changeSubItemText();
	// The message return to parent class 
	bHandled = false;
	return 1;
}


LRESULT QListViewCtrl::OnHScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	changeSubItemText();

	bHandled = false; // The message return to parent class 	
	return 1;
}

LRESULT QListViewCtrl::OnSubItemEditKillFocus(UINT uNotifyCode, int nID, HWND hwnd)
{
	changeSubItemText();

	return 0;
}

LRESULT QListViewCtrl::OnNotify(int idCtrl, LPNMHDR pnmh)
{
	if (pnmh->code == HDN_DIVIDERDBLCLICKA 
		|| pnmh->code == HDN_DIVIDERDBLCLICKW
		|| pnmh->code == HDN_ENDTRACKA
		|| pnmh->code == HDN_ENDTRACKW
		) {
		NMHEADER * pNMHeader = (NMHEADER *)pnmh;
		if (pNMHeader->pitem && pNMHeader->pitem->mask == HDI_WIDTH) {
			SetColumnWidth(pNMHeader->iItem, pNMHeader->pitem->cxy);
			Invalidate(false);
			return 1;
		}		
	} else if (pnmh->code == HDN_ITEMCLICKA 
		|| pnmh->code == HDN_ITEMCLICKW) {
		NMHEADER * pNMHeader = (NMHEADER *)pnmh;
		if (pNMHeader->iItem >= 0) {
			::PostMessage(GetParent().m_hWnd, Config::MSG_QLISTVIEW_COLUMN_CLICK_ID, WPARAM(pNMHeader->iItem), LPARAM(pNMHeader));
			return 0;
		}
	}
	this->SetMsgHandled(false); 
	return 0;
}

