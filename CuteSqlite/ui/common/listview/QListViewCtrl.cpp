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
#include <atlmisc.h>
#include <algorithm>
#include "core/common/Lang.h"
#include "utils/GdiPlusUtil.h"
#include "utils/Log.h"
#include "utils/ResourceUtil.h"

BOOL QListViewCtrl::PreTranslateMessage(MSG* pMsg)
{
	// press enter key or the mouse wheel
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN && pMsg->hwnd == subItemEdit.m_hWnd) {
		changeSubItemText();
	} else if (pMsg->hwnd == subItemEdit.m_hWnd && pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_TAB) {
		pressedTabToMoveEditor();
	}
	else if (pMsg->hwnd == m_hWnd && pMsg->message == WM_MOUSEWHEEL) {
		changeSubItemText();
		resetChildElemsRect();
	}

	return FALSE;
}

void QListViewCtrl::createOrShowEditor(std::pair<int, int> subItemPos)
{
	if (subItemPos.first < 0 || subItemPos.second < 0) {
		return;
	}
	this->subItemPos = subItemPos;
	GetSubItemRect(subItemPos.first, subItemPos.second, LVIR_BOUNDS, subItemRect);
	

	wchar_t cch[1024];
	memset(cch, 0, sizeof(cch));
	BSTR itemText = cch;
	GetItemText(subItemPos.first, subItemPos.second, cch, 1024);
	std::wstring text = itemText;
	createOrShowSubItemEdit(subItemEdit, text, subItemRect);
}


void QListViewCtrl::createOrShowEditor(int iItem, int iSubItem)
{
	createOrShowEditor(std::pair<int, int>(iItem, iSubItem));
}

/**
 * Create or show the combo button in listview subitem.
 * 
 * @param subItemPos - The subitem position in the treeview , subItemPos.fist=iItem, subItemPos.second=iSubItem
 * @param strList - string list will show in the combobox of the subItem
 * @param nSelItem - selected item in the the combobox
 */
void QListViewCtrl::createOrShowComboBoxBtn(std::pair<int, int> & subItemPosition, const std::vector<std::wstring> & strList, int nSelItem)
{
	if (subItemPosition.first < 0 || subItemPosition.second < 0) {
		return;
	}
	GetSubItemRect(subItemPosition.first, subItemPosition.second, LVIR_BOUNDS, subItemRect);
	CRect rect(subItemRect.right - 21, 
			subItemRect.top + 1, 
			subItemRect.right - 1, 
			subItemRect.bottom -2);	
	createOrShowSubItemComboBoxBtn(rect, subItemPosition.first, subItemPosition.second, strList, nSelItem);
}


void QListViewCtrl::createOrShowComboBoxBtn(int iItem, int iSubItem, const std::vector<std::wstring> &strList, int nSelItem /*= 0*/)
{
	createOrShowComboBoxBtn(std::pair<int, int>(iItem, iSubItem), strList, nSelItem);
}


CButton * QListViewCtrl::getComboBoxBtnPtr(int iItem, int iSubItem)
{
	if (iItem < 0 || iSubItem < 0) {
		return nullptr;
	}

	CButton * ptr = nullptr;
	if (subItemComboBoxBtnMap.empty()) {
		return ptr;
	}
	std::pair<int, int> pair(iItem, iSubItem);
	auto iter = subItemComboBoxBtnMap.find(pair);
	if (iter == subItemComboBoxBtnMap.end()) {
		return ptr;
	}
	ptr = iter->second;
	return ptr;
}

void QListViewCtrl::createOrShowCheckBox(int iItem, int iSubItem)
{
	if (iItem < 0 || iSubItem < 0) {
		return;
	}

	GetSubItemRect(iItem, iSubItem, LVIR_BOUNDS, subItemRect);
	createOrShowSubItemCheckBox(subItemRect, iItem, iSubItem);
}


CButton * QListViewCtrl::getCheckBoxPtr(int iItem, int iSubItem)
{
	if (iItem < 0 || iSubItem < 0) {
		return nullptr;
	}
	CButton * ptr = nullptr;
	if (subItemCheckBoxMap.empty()) {
		return ptr;
	}
	std::pair<int, int> pair(iItem, iSubItem);
	auto iter = subItemCheckBoxMap.find(pair);
	if (iter == subItemCheckBoxMap.end()) {
		return ptr;
	}
	ptr = iter->second;
	return ptr;
}


void QListViewCtrl::createOrShowButton(int iItem, int iSubItem, const std::wstring & text)
{
	if (iItem < 0 || iSubItem < 0) {
		return;
	}

	GetSubItemRect(iItem, iSubItem, LVIR_BOUNDS, subItemRect);
	createOrShowSubItemButton(subItemRect, iItem, iSubItem, text);
}


CButton * QListViewCtrl::getButtonPtr(int iItem, int iSubItem)
{
	if (iItem < 0 || iSubItem < 0) {
		return nullptr;
	}
	CButton * ptr = nullptr;
	if (subItemButtonMap.empty()) {
		return ptr;
	}
	std::pair<int, int> pair(iItem, iSubItem);
	auto iter = subItemButtonMap.find(pair);
	if (iter == subItemButtonMap.end()) {
		return ptr;
	}
	ptr = iter->second;
	return ptr;
}

SubItemValues QListViewCtrl::getChangedVals()
{
	return changeVals;
}


int QListViewCtrl::getChangedCount()
{
	return static_cast<int>(changeVals.size());
}

/**
 * Get changed values only the specified row index.
 * 
 * @param iItem The row index
 * @return 
 */
SubItemValues QListViewCtrl::getRowChangedVals(int iItem)
{
	if (changeVals.empty() || iItem < 0 || iItem >= GetItemCount()) {
		return SubItemValues();
	}

	SubItemValues result;
	for (auto val : changeVals) {
		if (val.iItem == iItem) {
			result.push_back(val);
		}
	}

	return result;
}

void QListViewCtrl::setChangedVals(SubItemValues & changeVals)
{
	if (changeVals.empty()) {
		this->changeVals.clear();
		return ;
	}
	this->changeVals.assign(changeVals.begin(), changeVals.end());
}

void QListViewCtrl::clearChangeVals()
{
	changeVals.clear();
}

/**
 * find the item(s) from changeVals and then remove them.
 * and then subtract the (*iter).iItem index if (*iter).iItem is bigger than the specified iItem
 * 
 * @param iItem
 */
void QListViewCtrl::removeChangedValsItems(int iItem)
{	
	auto iter = changeVals.begin();
	while (iter != std::end(changeVals)) {
		if ((*iter).iItem == iItem) {
			if (iter != changeVals.begin()) {
				changeVals.erase(iter--);
			} else {
				changeVals.erase(iter);
				iter = changeVals.begin();
				continue;
			}
			if (changeVals.empty()) {
				break;
			}
		}
		iter++;
	}

	// subtract the iItem index 
	for (auto iter = changeVals.begin(); iter != changeVals.end(); iter++) {
		if ((*iter).iItem > iItem) {
			(*iter).iItem--;
		}
	}
}


void QListViewCtrl::moveUpChangeValsItem(int iItem)
{
	if (iItem < 1) {
		return ;
	}
	int iSwapItem = iItem - 1;
	auto iter = changeVals.begin();
	while (iter != std::end(changeVals)) {
		if ((*iter).iItem == iItem) {
			(*iter).iItem--;
		}

		if ((*iter).iItem == iSwapItem) {
			(*iter).iItem++;
		}
		iter++;
	}
}


void QListViewCtrl::moveDownChangeValsItem(int iItem)
{
	if (iItem >= GetItemCount() - 1) {
		return ;
	}
	int iSwapItem = iItem + 1;
	auto iter = changeVals.begin();
	while (iter != std::end(changeVals)) {
		if ((*iter).iItem == iItem) {
			(*iter).iItem++;
		}

		if ((*iter).iItem == iSwapItem) {
			(*iter).iItem--;
		}
		iter++;
	}
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
		wchar_t cch[1025];
		memset(cch, 0, sizeof(cch));
		GetItemText(subItemPos.first, subItemPos.second, cch, 1024);
		

		subItemVal.iItem = subItemPos.first;
		subItemVal.iSubItem = subItemPos.second;
		subItemVal.origVal.assign(cch);
		subItemVal.newVal = text;

		// it will be return when original text equals new text
		if (subItemVal.origVal == subItemVal.newVal) {
			subItemEdit.DestroyWindow();
			isVisibleEdit = false;
			return;
		}

		setChangeVal(subItemVal);
		
		subItemEdit.DestroyWindow();
		isVisibleEdit = false;

		::PostMessage(GetParent().m_hWnd, Config::MSG_QLISTVIEW_SUBITEM_TEXT_CHANGE_ID, WPARAM(subItemPos.first), LPARAM(subItemPos.second));
	}
}

void QListViewCtrl::resetChildElemsRect()
{
	changeComboBoxBtnsRect();
	changeCheckBoxesRect();
	changeButtonsRect();
}


void QListViewCtrl::setItemHeight(int height)
{
	itemHeight = height;
    CRect rcWin;
    GetWindowRect(&rcWin);
    WINDOWPOS wp;
    wp.hwnd = m_hWnd;
    wp.cx = rcWin.Width();
    wp.cy = rcWin.Height();
    wp.flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;
    SendMessage(WM_WINDOWPOSCHANGED, 0, (LPARAM)&wp);
}

void QListViewCtrl::changeComboBoxBtnsRect()
{
	CHeaderCtrl header = GetHeader();
	CRect headRect;
	header.GetClientRect(headRect);
	for (auto iter = subItemComboBoxBtnMap.begin(); iter != subItemComboBoxBtnMap.end(); iter++) {
		auto pair = iter->first;
		auto ptr = iter->second;
		CRect subItemRect;
		GetSubItemRect(pair.first, pair.second, LVIR_BOUNDS, subItemRect);	
		InvalidateRect(subItemRect, false);
		if (ptr && ptr->IsWindow()) {
			CRect rect(subItemRect.right - 21, 
				subItemRect.top + 1, 
				subItemRect.right - 1, 
				subItemRect.bottom -2);
			ptr->MoveWindow(rect);
			if (subItemRect.top >= headRect.Height()) {
				ptr->ShowWindow(true);
				ptr->BringWindowToTop();
			} else {
				ptr->ShowWindow(false);
			}			
		}
	}
}

void QListViewCtrl::changeCheckBoxesRect()
{
	CHeaderCtrl header = GetHeader();
	CRect headRect;
	header.GetClientRect(headRect);
	for (auto iter = subItemCheckBoxMap.begin(); iter != subItemCheckBoxMap.end(); iter++) {
		auto pair = iter->first;
		auto ptr = iter->second;
		CRect subItemRect;
		if (GetSubItemRect(pair.first, pair.second, LVIR_BOUNDS, subItemRect) == false) {
			Q_ERROR(L"GetSubItemRect error, iItem={}, iSubItem={}", pair.first, pair.second);
			return;
		}
		InvalidateRect(subItemRect, false);
		if (ptr && ptr->IsWindow()) {
			int x = subItemRect.left + (subItemRect.Width() - 20) / 2,
				y = subItemRect.top + (subItemRect.Height() - 20) / 2,
				w = 20, h = 20;
			CRect rect(x, y, x + w, y + h);
			ptr->MoveWindow(rect);
			if (subItemRect.top >= headRect.Height()) {
				ptr->ShowWindow(true);
				ptr->BringWindowToTop();
			}
			else {
				ptr->ShowWindow(false);
			}
		}
	}
}


void QListViewCtrl::changeButtonsRect()
{
	CHeaderCtrl header = GetHeader();
	CRect headRect;
	header.GetClientRect(headRect);
	for (auto iter = subItemButtonMap.begin(); iter != subItemButtonMap.end(); iter++) {
		auto pair = iter->first;
		auto ptr = iter->second;
		CRect subItemRect;
		if (GetSubItemRect(pair.first, pair.second, LVIR_BOUNDS, subItemRect) == false) {
			Q_ERROR(L"GetSubItemRect error, iItem={}, iSubItem={}", pair.first, pair.second);
			return;
		}
		InvalidateRect(subItemRect, false);
		if (ptr && ptr->IsWindow()) {
			int x = subItemRect.right - 31,
				y = subItemRect.top + (subItemRect.Height() - 20) / 2,
				w = 30, h = 20;
			CRect rect(x, y, x + w, y + h);
			ptr->MoveWindow(rect);

			if (subItemRect.top >= headRect.Height()) {
				ptr->ShowWindow(true);
				ptr->BringWindowToTop();
			} else {
				ptr->ShowWindow(false);
			}
		}
	}
}

void QListViewCtrl::setChangeVal(SubItemValue &subItemVal)
{
	// change the newVal in exists item in changeVals vector
	auto iter = std::find_if(changeVals.begin(), changeVals.end(), [&subItemVal](SubItemValue &val) {
		return val.iItem == subItemVal.iItem && val.iSubItem == subItemVal.iSubItem;
	});

	// Only change newVal, origVal not need change for the exist item.
	if (iter != changeVals.end()) {
		(*iter).newVal = subItemVal.newVal;
	}
	else {
		changeVals.push_back(subItemVal);
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

/**
 * create .
 * 
 * @param subItemRect
 * @param iItem
 * @param iSubItem
 * @param strList
 * @param nSelItem
 */
void QListViewCtrl::createOrShowSubItemComboBoxBtn(CRect & rect, int iItem, int iSubItem, const std::vector<std::wstring> &strList, int nSelItem)
{
	CButton * comboBoxBtnPtr = nullptr;
	std::pair<int, int> pair(iItem, iSubItem);
	auto iter = subItemComboBoxBtnMap.find(pair);
	if (iter != subItemComboBoxBtnMap.end()) {
		comboBoxBtnPtr = iter->second;
		return;
	}
	
	comboBoxBtnPtr = new CButton();
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN  | WS_CLIPSIBLINGS | WS_TABSTOP ;
	UINT nID = Config::QLISTVIEWCTRL_COMBOBOX_BTN_ID_START + static_cast<int>(subItemComboBoxBtnMap.size());
	comboBoxBtnPtr->Create(m_hWnd, rect, S(L"down").c_str(),  dwStyle , 0, nID);
	comboBoxBtnPtr->SetFont(comboFont);
	// loadSubItemComboBoxBtn(comboBoxBtnPtr, strList, nSelItem);
	subItemComboBoxBtnMap[pair] = comboBoxBtnPtr;
	changeComboBoxBtnsRect();
}


void QListViewCtrl::createOrShowSubItemCheckBox(CRect &subItemRect, int iItem, int iSubItem)
{
	CButton * checkboxPtr = nullptr;
	std::pair<int, int> pair(iItem, iSubItem);
	auto iter = subItemCheckBoxMap.find(pair);
	int x = subItemRect.left + (subItemRect.Width() - 20) / 2,
		y = subItemRect.top + (subItemRect.Height() - 20) / 2,
		w = 20, h = 20;
	CRect rect(x, y, x + w, y + h);
	if (iter != subItemCheckBoxMap.end()) {
		checkboxPtr = iter->second;
		return;
	}

	checkboxPtr = new CButton();
	UINT nID = Config::QLISTVIEWCTRL_CHECKBOX_ID_START + static_cast<int>(subItemCheckBoxMap.size());	
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN  |WS_TABSTOP | WS_CLIPSIBLINGS | BS_CHECKBOX ;
	checkboxPtr->Create(m_hWnd, rect, L"", dwStyle , 0, nID);
	//checkboxPtr->Create(m_hWnd, rect, std::to_wstring(iItem).c_str(), dwStyle , 0, nID);
	subItemCheckBoxMap[pair] = checkboxPtr;
	changeCheckBoxesRect();
}


void QListViewCtrl::createOrShowSubItemButton(CRect & subItemRect, int iItem, int iSubItem, const std::wstring &text)
{
	CButton * buttonPtr = nullptr;
	std::pair<int, int> pair(iItem, iSubItem);
	auto iter = subItemButtonMap.find(pair);
	int x = subItemRect.right - 31,
		y = subItemRect.top + (subItemRect.Height() - 20) / 2,
		w = 30, h = 20;
	CRect rect(x, y, x + w, y + h);
	if (iter != subItemButtonMap.end()) {
		buttonPtr = iter->second;
		return;
	}
	
	buttonPtr = new CButton();
	UINT nID = Config::QLISTVIEWCTRL_BUTTON_ID_START + static_cast<int>(subItemButtonMap.size());	
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN  |WS_TABSTOP | WS_CLIPSIBLINGS ;
	buttonPtr->Create(m_hWnd, rect, text.c_str(), dwStyle , 0, nID);
	subItemButtonMap[pair] = buttonPtr;
	changeButtonsRect();
}

void QListViewCtrl::loadSubItemComboBoxBtn(CComboBox * comboBoxPtr, const std::vector<std::wstring> &strList, int nSelItem)
{
	ATLASSERT(comboBoxPtr && comboBoxPtr->IsWindow());
	comboBoxPtr->ResetContent();
	int n = static_cast<int>(strList.size());
	for (int i = 0; i < n; i++) {
		int nItem = comboBoxPtr->AddString(strList.at(i).c_str());
		comboBoxPtr->SetItemData(nItem, i);
	}

	comboBoxPtr->SetCurSel(nSelItem);
}

void QListViewCtrl::pressedTabToMoveEditor()
{
	changeSubItemText();
	int columnCount = GetHeader().GetItemCount();
	int rowCount = GetItemCount();
	if (subItemPos.first >= rowCount - 1 && subItemPos.second >= columnCount - 1) {
		return;
	}

	int iItem = subItemPos.first, iSubItem = subItemPos.second;
	if (subItemPos.first < rowCount - 1 && subItemPos.second == columnCount - 1) {
		iItem = subItemPos.first + 1;
		iSubItem = 1;
		createOrShowEditor(iItem, iSubItem);
		return;
	}

	if (subItemPos.first <= rowCount - 1 && subItemPos.second < columnCount - 1) {
		iSubItem = subItemPos.second + 1;
	}

	createOrShowEditor(iItem, iSubItem);
}

LRESULT QListViewCtrl::OnVScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{	 
	changeSubItemText();
	
	resetChildElemsRect();
	// The message return to parent class 
	bHandled = false;
	return 1;
}

LRESULT QListViewCtrl::OnHScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	changeSubItemText();
	resetChildElemsRect();

	bHandled = false; // The message return to parent class 	
	return 1;
}

LRESULT QListViewCtrl::OnSubItemEditKillFocus(UINT uNotifyCode, int nID, HWND hwnd)
{
	changeSubItemText();

	return 0;
}


void QListViewCtrl::OnClickCheckBox(UINT uNotifyCode, int nID, HWND hwnd)
{
	ATLASSERT(hwnd != NULL);
	CButton * checkBoxPtr;
	for (auto iter = subItemCheckBoxMap.begin(); iter != subItemCheckBoxMap.end(); iter++) {
		checkBoxPtr = iter->second;
		if (hwnd == checkBoxPtr->m_hWnd) {
			checkBoxPtr->SetCheck(!checkBoxPtr->GetCheck());
			break;
		}
	}
	
}

/**
 * Will send Config::MSG_QLISTVIEW_SUBITEM_BUTTON_CLICK_ID message to 
 * parent window when clicking the button in the QListView, wParam=iItem, lParam=iSubItem
 * 
 * @param uNotifyCode
 * @param nID
 * @param hwnd
 */
void QListViewCtrl::OnClickButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	for (auto iter = subItemButtonMap.begin(); iter != subItemButtonMap.end(); iter++) {
		if (iter->second && iter->second->IsWindow() && iter->second->m_hWnd == hwnd) {
			auto pair = iter->first;
			::PostMessage(GetParent().m_hWnd, Config::MSG_QLISTVIEW_SUBITEM_BUTTON_CLICK_ID, 
				WPARAM(pair.first), LPARAM(pair.second));
			break;
		}
	}	
}

/**
 * Remove all the subItems by specified iItem.
 * 
 * @param iItem - subItem(iItem, iSubItem) iItem
 */
void QListViewCtrl::RemoveItem(int iItem)
{
	// delete ComboBoxes
	removeComboBoxes(iItem);
	removeCheckBoxes(iItem);

	CListViewCtrl::DeleteItem(iItem);
}

/**
 * Remove the ComboBoxes by specified nTh(iItem) row.
 * 
 * @param nItem - Specified nTh(iItem) row
 */
void QListViewCtrl::removeComboBoxes(int iItem)
{
	// swap the std::map content begin from iItem to last row, then remove the last row's iterators in the std::map
	auto lastIt = subItemComboBoxBtnMap.begin(); // the last iterator will be deleted
	int maxRows = GetItemCount();
	int lastRow = maxRows - 1;
	for (auto it = subItemComboBoxBtnMap.begin(), ite = subItemComboBoxBtnMap.end(); it != ite;) {
		if (it->first.first != iItem) {
			it++;
			continue;
		}
		int iSubItem = it->first.second;
		int nextRow = it->first.first + 1;
		if (nextRow == maxRows) {
			break;
		}

		// find the next row subitem, will be swap the value in the nTh row subitem and the nTh+1 row subitem 
		auto nextRowSubItemIt = subItemComboBoxBtnMap.end();		
		for (auto nIter = subItemComboBoxBtnMap.begin(); nIter != subItemComboBoxBtnMap.end(); nIter++) {
			if (nIter->first.first == nextRow && nIter->first.second == iSubItem) {
				nextRowSubItemIt = nIter;
				break;
			}
		}
			
		// swap the std::map value begin from nItem to end
		if (it->first.first >= iItem && nextRowSubItemIt != ite) {
			std::swap(it->second, nextRowSubItemIt->second);
		}
		it++;

	}
	//2.then remove the last row's iterators in the std::map
	for (auto it = subItemComboBoxBtnMap.begin(), ite = subItemComboBoxBtnMap.end(); it != ite;) {
		if (it->first.first != lastRow) {
			it++;
			continue;
		}
		// release the memory of the ComboBox pointer
		auto ptr = it->second;
		if (ptr && ptr->IsWindow()) {
			ptr->ShowWindow(false);
			ptr->DestroyWindow();
			delete ptr;
			ptr = nullptr;
		}
		it = subItemComboBoxBtnMap.erase(it);
	}
	
}

/**
 * Remove the CheckBoxes by the specified nTh(iItem) row.
 * 
 * @param nItem - Specified nTh(iItem) row
 */
void QListViewCtrl::removeCheckBoxes(int iItem)
{
	// swap the std::map content begin from nTh(iItem) row to last row, then remove the last row's iterators in the std::map
	auto lastIt = subItemCheckBoxMap.begin(); // the last iterator will be deleted
	int maxRows = GetItemCount();
	int lastRow = maxRows - 1;
	for (auto it = subItemCheckBoxMap.begin(), ite = subItemCheckBoxMap.end(); it != ite;) {
		if (it->first.first != iItem) {
			it++;
			continue;
		}
		int iSubItem = it->first.second;
		int nextRow = it->first.first + 1;
		if (nextRow == maxRows) {
			break;
		}

		// find the next row subitem, will be swap the value in the nTh row subitem and the nTh+1 row subitem 
		auto nextRowSubItemIt = subItemCheckBoxMap.end();		
		for (auto nIter = subItemCheckBoxMap.begin(); nIter != subItemCheckBoxMap.end(); nIter++) {
			if (nIter->first.first == nextRow && nIter->first.second == iSubItem) {
				nextRowSubItemIt = nIter;
				break;
			}
		}
			
		// swap the std::map value begin from nItem to end
		if (nextRowSubItemIt != ite) {
			std::swap(it->second, nextRowSubItemIt->second);
		}
		it++;
	}
	//2.then remove the last row's iterators in the std::map
	for (auto it = subItemCheckBoxMap.begin(), ite = subItemCheckBoxMap.end(); it != ite;) {
		if (it->first.first != lastRow) {
			it++;
			continue;
		}
		// release the memory of the ComboBox pointer
		auto ptr = it->second;
		if (ptr && ptr->IsWindow()) {
			ptr->ShowWindow(false);
			ptr->DestroyWindow();
			delete ptr;
			ptr = nullptr;
		}
		it = subItemCheckBoxMap.erase(it);
	}
}

/**
 * Move up the ComboBoxes by the specified nTh(iItem) row.
 * 
 * @param nItem - Specified nTh(iItem) row
 */
void QListViewCtrl::moveUpComboBoxes(int iItem)
{
	// swap the std::map content
	int maxRows = GetItemCount();
	for (auto it = subItemComboBoxBtnMap.begin(), ite = subItemComboBoxBtnMap.end(); it != ite;) {
		if (it->first.first != iItem) {
			it++;
			continue;
		}
		int iSubItem = it->first.second;
		int prevRow = iItem - 1;
		if (prevRow < 0) {
			break;
		}

		// find the next row subitem, will be swap the value in the nTh row subitem and the nTh-1 row subitem 
		auto prevRowSubItemIt = subItemComboBoxBtnMap.end();		
		for (auto nIter = subItemComboBoxBtnMap.begin(); nIter != subItemComboBoxBtnMap.end(); nIter++) {
			if (nIter->first.first == prevRow && nIter->first.second == iSubItem) {
				prevRowSubItemIt = nIter;
				break;
			}
		}
			
		// swap the std::map value begin from nItem to end
		if (prevRowSubItemIt != ite) {
			std::swap(it->second, prevRowSubItemIt->second);
		}
		it++;
	}
}

/**
 * Move down the ComboBoxes by the specified nTh(iItem) row.
 * 
 * @param nItem - Specified nTh(iItem) row
 */
void QListViewCtrl::moveDownComboBoxes(int iItem)
{
	// swap the std::map content
	int maxRows = GetItemCount();
	for (auto it = subItemComboBoxBtnMap.begin(), ite = subItemComboBoxBtnMap.end(); it != ite;) {
		if (it->first.first != iItem) {
			it++;
			continue;
		}
		int iSubItem = it->first.second;
		int nextRow = it->first.first + 1;
		if (nextRow == maxRows) {
			break;
		}

		// find the next row subitem, will be swap the value in the nTh row subitem and the nTh-1 row subitem 
		auto nextRowSubItemIt = subItemComboBoxBtnMap.end();		
		for (auto nIter = subItemComboBoxBtnMap.begin(); nIter != subItemComboBoxBtnMap.end(); nIter++) {
			if (nIter->first.first == nextRow && nIter->first.second == iSubItem) {
				nextRowSubItemIt = nIter;
				break;
			}
		}
			
		// swap the std::map value begin from nItem to end
		if (nextRowSubItemIt != ite) {
			std::swap(it->second, nextRowSubItemIt->second);
		}
		it++;
	}
}

/**
 * Move up the CheckBoxes by the specified nTh(iItem) row.
 * 
 * @param nItem - Specified nTh(iItem) row
 */
void QListViewCtrl::moveUpCheckBoxes(int iItem)
{
	// swap the std::map content
	int maxRows = GetItemCount();
	for (auto it = subItemCheckBoxMap.begin(), ite = subItemCheckBoxMap.end(); it != ite;) {
		if (it->first.first != iItem) {
			it++;
			continue;
		}
		int iSubItem = it->first.second;
		int prevRow = it->first.first - 1;
		if (prevRow < 0) {
			break;
		}

		// find the next row subitem, will be swap the value in the nTh row subitem and the nTh-1 row subitem 
		auto prevRowSubItemIt = subItemCheckBoxMap.end();		
		for (auto nIter = subItemCheckBoxMap.begin(); nIter != subItemCheckBoxMap.end(); nIter++) {
			if (nIter->first.first == prevRow && nIter->first.second == iSubItem) {
				prevRowSubItemIt = nIter;
				break;
			}
		}
			
		// swap the std::map value begin from nItem to end
		if (prevRowSubItemIt != ite) {
			std::swap(it->second, prevRowSubItemIt->second);
		}
		it++;
	}
}

/**
 * Move down the CheckBoxes by the specified nTh(iItem) row.
 * 
 * @param nItem - Specified nTh(iItem) row
 */
void QListViewCtrl::moveDownCheckBoxes(int iItem)
{
	// swap the std::map content
	int maxRows = GetItemCount();
	for (auto it = subItemCheckBoxMap.begin(), ite = subItemCheckBoxMap.end(); it != ite;) {
		if (it->first.first != iItem) {
			it++;
			continue;
		}

		int iSubItem = it->first.second;
		int nextRow = it->first.first + 1;
		if (nextRow == maxRows) {
			break;
		}

		// find the next row subitem, will be swap the value in the nTh row subitem and the nTh-1 row subitem 
		auto nextRowSubItemIt = subItemCheckBoxMap.end();		
		for (auto nIter = subItemCheckBoxMap.begin(); nIter != subItemCheckBoxMap.end(); nIter++) {
			if (nIter->first.first == nextRow && nIter->first.second == iSubItem) {
				nextRowSubItemIt = nIter;
				break;
			}
		}
			
		// swap the std::map value begin from nItem to end
		if (nextRowSubItemIt != ite) {
			std::swap(it->second, nextRowSubItemIt->second);
		}
		it++;
	}
}


void QListViewCtrl::moveUpButtons(int iItem)
{
	// swap the std::map content
	int maxRows = GetItemCount();
	for (auto it = subItemButtonMap.begin(), ite = subItemButtonMap.end(); it != ite;) {
		if (it->first.first != iItem) {
			it++;
			continue;
		}
		int iSubItem = it->first.second;
		int prevRow = it->first.first - 1;
		if (prevRow < 0) {
			break;
		}

		// find the next row subitem, will be swap the value in the nTh row subitem and the nTh-1 row subitem 
		auto prevRowSubItemIt = subItemButtonMap.end();		
		for (auto nIter = subItemButtonMap.begin(); nIter != subItemButtonMap.end(); nIter++) {
			if (nIter->first.first == prevRow && nIter->first.second == iSubItem) {
				prevRowSubItemIt = nIter;
				break;
			}
		}
			
		// swap the std::map value begin from nItem to end
		if (prevRowSubItemIt != ite) {
			std::swap(it->second, prevRowSubItemIt->second);
		}
		it++;
	}
}


void QListViewCtrl::moveDownButtons(int iItem)
{
	// swap the std::map content
	int maxRows = GetItemCount();
	for (auto it = subItemButtonMap.begin(), ite = subItemButtonMap.end(); it != ite;) {
		if (it->first.first != iItem) {
			it++;
			continue;
		}

		int iSubItem = it->first.second;
		int nextRow = it->first.first + 1;
		if (nextRow == maxRows) {
			break;
		}

		// find the next row subitem, will be swap the value in the nTh row subitem and the nTh-1 row subitem 
		auto nextRowSubItemIt = subItemButtonMap.end();		
		for (auto nIter = subItemButtonMap.begin(); nIter != subItemButtonMap.end(); nIter++) {
			if (nIter->first.first == nextRow && nIter->first.second == iSubItem) {
				nextRowSubItemIt = nIter;
				break;
			}
		}
			
		// swap the std::map value begin from nItem to end
		if (nextRowSubItemIt != ite) {
			std::swap(it->second, nextRowSubItemIt->second);
		}
		it++;
	}
}

void QListViewCtrl::OnDestroy()
{
	if (bkgBrush) ::DeleteObject(bkgBrush);
	if (textFont) ::DeleteObject(textFont);
	if (comboFont) ::DeleteObject(comboFont);
	if (normalFont) ::DeleteObject(normalFont);

	if (subItemEdit.IsWindow()) subItemEdit.DestroyWindow();

	// release subItemComboBoxMap
	auto iter = subItemComboBoxBtnMap.begin();
	while (iter != subItemComboBoxBtnMap.end()) {
		auto ptr = iter->second;
		if (!ptr || !ptr->IsWindow()) {
			continue;
		}
		ptr->DestroyWindow();
		delete ptr;
		ptr = nullptr;
		iter++;
	}
	subItemComboBoxBtnMap.clear();

	// release subItemCheckBoxMap
	auto iter2 = subItemCheckBoxMap.begin();
	while (iter2 != subItemCheckBoxMap.end()) {
		auto ptr = iter2->second;
		if (!ptr || !ptr->IsWindow()) {
			continue;
		}
		ptr->DestroyWindow();
		delete ptr;
		ptr = nullptr;
		iter2++;
	}
	subItemCheckBoxMap.clear();

	// release subItemButtonMap
	auto iter3 = subItemButtonMap.begin();
	while (iter3 != subItemButtonMap.end()) {
		auto ptr = iter3->second;
		if (!ptr || !ptr->IsWindow()) {
			continue;
		}
		ptr->DestroyWindow();
		delete ptr;
		ptr = nullptr;
		iter3++;
	}
	subItemButtonMap.clear();

	if (!imageList.IsNull()) {
		imageList.Destroy();
	}

	if (!checkYesBitmap.IsNull()) {
		::DeleteObject(checkYesBitmap);
		checkYesBitmap.Detach();
	}
	if (!checkNoBitmap.IsNull()) {
		::DeleteObject(checkNoBitmap);
		checkNoBitmap.Detach();
	}
}

void QListViewCtrl::OnSize(UINT nType, CSize size)
{
	if (!bkgBrush) bkgBrush = ::CreateSolidBrush(bkgColor);
	if (!textFont) textFont = FT(L"list-text-size");
	if (!comboFont) comboFont = FT(L"list-combobox-size");
	if (!normalFont) normalFont = FT(L"list-text-size");

	createImageList();
	changeSubItemText();
	resetChildElemsRect();
}


void QListViewCtrl::createImageList()
{
	if (imageList.IsNull()) {
		std::wstring imgDir = ResourceUtil::getProductImagesDir();
		std::wstring checkNoPath = imgDir + L"database\\list\\check-no.bmp";
		std::wstring checkYesPath = imgDir + L"database\\list\\check-yes.bmp";
		checkYesBitmap = (HBITMAP)::LoadImageW(ModuleHelper::GetModuleInstance(), checkYesPath.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		checkNoBitmap = (HBITMAP)::LoadImageW(ModuleHelper::GetModuleInstance(), checkNoPath.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

		imageList.Create(16, 16, ILC_COLOR32, 0, 2);
		imageList.Add(checkNoBitmap);
		imageList.Add(checkYesBitmap);

		SetImageList(imageList, LVSIL_SMALL);
		CHeaderCtrl headerCtrl = GetHeader();
		headerCtrl.SetImageList(imageList);
	}
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
			resetChildElemsRect();
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


BOOL QListViewCtrl::OnEraseBkgnd(CDCHandle dc)
{
	// Fill the header bgColor
	CBrush  br;
	CRect   rcCli;
	CRect   rcItemsRect(0, 0, 0, 0);
	int     nHeadHeight = 0;
	int     nItems      = GetItemCount();

	GetClientRect(&rcCli);

	CHeaderCtrl pHeadCtrl = GetHeader();
	CRect  rcHead;
	pHeadCtrl.GetWindowRect(&rcHead);
	nHeadHeight = rcHead.Height();
	rcCli.top += nHeadHeight;

	CRect   rcItem;
	if (nItems > 0)
	{
		CPoint  ptItem;

		GetItemRect(nItems - 1, &rcItem, LVIR_BOUNDS);
		GetItemPosition(nItems - 1, &ptItem);

		rcItemsRect.top    = rcCli.top;
		rcItemsRect.left   = ptItem.x;
		rcItemsRect.right  = rcItem.right;
		rcItemsRect.bottom = rcItem.bottom;

		if (GetExStyle() & LVS_EX_CHECKBOXES)
			rcItemsRect.left -= GetSystemMetrics(SM_CXEDGE) + 16;
	}

	br.CreateSolidBrush(GetBkColor());

	if (rcItemsRect.IsRectEmpty())
		dc.FillRect(rcCli, br);
	else
	{
		if (rcItemsRect.left > rcCli.left)     // fill left rectangle
			dc.FillRect(
			CRect(0, rcItemsRect.bottom, rcItemsRect.left, rcCli.bottom), br);
		if (rcItemsRect.bottom < rcCli.bottom) // fill bottom rectangle
			dc.FillRect(
			CRect(0, rcItemsRect.bottom, rcCli.right, rcCli.bottom), br);
		if (rcItemsRect.right < rcCli.right) // fill right rectangle
			dc.FillRect(
			CRect(rcItemsRect.right, rcCli.top, rcCli.right, rcCli.bottom), br);
	}
	DeleteObject(br);
	return TRUE;
}

HBRUSH QListViewCtrl::OnCtlColorStatic(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, bkgColor);
	::SelectObject(hdc, textFont);
	return bkgBrush;
}

HBRUSH QListViewCtrl::OnCtlColorEdit(HDC hdc, HWND hwnd)
{	
	::SetBkColor(hdc, bkgColor);	
	::SelectObject(hdc, textFont);
	return bkgBrush;
}

HBRUSH QListViewCtrl::OnCtlColorListBox(HDC hdc, HWND hwnd)
{
	::SetTextColor(hdc, RGB(0, 0, 0)); // Text area foreground color
	::SetBkColor(hdc, RGB(153, 153, 153)); // Text area background color
	::SelectObject(hdc, comboFont);
	return AtlGetStockBrush(WHITE_BRUSH);
}


void QListViewCtrl::drawSubItems(CDC & memDc, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	LVCOLUMN lvc;
	lvc.mask = LVCF_WIDTH | LVCF_FMT;

	TCHAR szBuf[MAX_PATH] = {0};
	LVITEM lvi;
	lvi.mask = LVIF_TEXT;
	lvi.iItem = lpDrawItemStruct->itemID; // 这里指定第几行 lvi.iItem：指定行数，lvi.iSubItem指定列数
	lvi.pszText = szBuf;
	lvi.cchTextMax = MAX_PATH;

	static const int kCheckBoxColumn = 0; //定死多选框显示第0列
	static const int kPhotoColumn = 1;//定死图片显示第1列
	int const leftMargin = 2;

	for (int nCol=0; GetColumn(nCol, &lvc); ++nCol) {
		CRect rcSubItem;
		if(!GetSubItemRect(lpDrawItemStruct->itemID, nCol, LVIR_LABEL, rcSubItem)) {	
			continue;
		}
		lvi.iSubItem = nCol; // lvi.iItem：指定行数，lvi.iSubItem指定列数
		GetItem(&lvi); // 把原始的格子的结构体数据提取出来，参数指定第几行

		CRect rctext(rcSubItem.left- hOffset, 0, rcSubItem.right - hOffset, rcSubItem.Height());  //相对于内存dc的item rect

		int checkbox_width = 20;
		int checkbox_height = 20;
		
		// draw the CheckBox of first column
		if(nCol == 0) {
			rctext.left += 5 + checkbox_width;

			int nleft = 5;
			int ntop = (rcSubItem.Height() - checkbox_height)/2;

			BOOL bChecked = ListView_GetCheckState(m_hWnd, lpDrawItemStruct->itemID );
			
			CDC bitmapDc;
			bitmapDc.CreateCompatibleDC(NULL);
			if (bChecked && bChecked != -1) {
				HBITMAP hbmoldsource = (HBITMAP)::SelectObject(bitmapDc.m_hDC, (HBITMAP)checkYesBitmap);
				memDc.BitBlt(nleft, ntop, checkbox_width, checkbox_height, bitmapDc, 0, 0, SRCCOPY);
				::SelectObject(bitmapDc.m_hDC, hbmoldsource);				
			} else {
				HBITMAP hbmoldsource = (HBITMAP)::SelectObject(bitmapDc.m_hDC, (HBITMAP)checkNoBitmap);
				memDc.BitBlt(nleft, ntop, checkbox_width, checkbox_height, bitmapDc, 0, 0, SRCCOPY);
				::SelectObject(bitmapDc.m_hDC, hbmoldsource);
			}
			::DeleteObject(bitmapDc.m_hDC );
			
			// 第0列，文本显示的序号（正序还是倒叙, 正序序号从1开始，所以要加1）
			int id = lpDrawItemStruct->itemID + 1;
			std::wstring str = std::to_wstring(id);
			if(str.size())
			{
				CRect photo_rect = rctext;
				photo_rect.left = nleft + checkbox_width + leftMargin;
				photo_rect.top = rctext.top;
				UINT uFormat = DT_LEFT | DT_VCENTER | DT_END_ELLIPSIS;
				CSize lpSize; // GetTextExtentPoint32 函数计算指定文本字符串的宽度和高度。
				::GetTextExtentPoint32(memDc, str.c_str(), static_cast<int>(str.size()), &lpSize);
				photo_rect.top += (photo_rect.Height()-lpSize.cy)/2;
				DrawText(memDc, str.c_str(), static_cast<int>(str.size()), photo_rect, uFormat);
			}
		}else {
			wchar_t* str = NULL;
			GetItemText(lpDrawItemStruct->itemID,nCol, str);
			if(str && wcslen(str)) {
				CSize lpSize;
				int strLen = static_cast<int>(wcslen(str));
				::GetTextExtentPoint32(memDc, str, strLen, &lpSize);
				CRect other_text_rect = rctext;
				other_text_rect.top += (other_text_rect.Height() - lpSize.cy) / 2;
				UINT uFormat = DT_LEFT | DT_VCENTER | DT_END_ELLIPSIS;
				other_text_rect.left += leftMargin;
				DrawText(memDc,str, strLen, other_text_rect, uFormat);
			}
		}
	}
}

/**
 * Draw the row's all item(s).
 * 
 * @param lpDrawItemStruct
 */
void QListViewCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	HDC hdc = lpDrawItemStruct->hDC;
	CRect rcItem = lpDrawItemStruct->rcItem;  // listview每项的rect，受滚动条位置影响
	int nItem = lpDrawItemStruct->itemID;
	CRect rcCient;
	GetClientRect(rcCient);  // 固定listview的区域，宽度可小于rcitem

	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_POS | SIF_RANGE;
	// Horizontal offset and size 
	if (GetScrollInfo(SB_HORZ, &si)) {
		hOffset =- si.nPos;   // 计算偏移 因为内存dc的rect和rcItem的原点不同
		hSize = max(si.nMax+1, rcCient. Width());
	} else {
		hOffset = rcCient.left;
		hSize = rcCient.Width();
	}
	CDC mdc;
	/// CreateCompatibleDC 函数 (DC) 创建与指定设备兼容的内存设备上下文。
	mdc.CreateCompatibleDC(hdc);
	CBitmap hMemBitmap;
	hMemBitmap.CreateCompatibleBitmap(hdc, rcItem.Width(), rcItem.Height());
	mdc.SelectBitmap(hMemBitmap);
	DeleteObject(hMemBitmap);
	
	CBrush hBrush;
	CPen hPen;
	hPen.CreatePen(PS_SOLID, 1, RGB(220,220,220));
	mdc.SelectPen(hPen);
	mdc.SelectFont(normalFont);

	if (lpDrawItemStruct->itemState & ODS_SELECTED  )  {
		hBrush.CreateSolidBrush(RGB(161,219,238));
	} else {
		hBrush.CreateSolidBrush(RGB(255,255,255));
	}
	mdc.FillRect(CRect(0, 1, rcItem.Width(), rcItem.Height()), hBrush);
	DeleteObject(hBrush);

	//画item的左边
	mdc.MoveTo(0, 0, NULL);
	mdc.LineTo(0, rcItem.Height());
	
	//画item的右边
	mdc.MoveTo(rcItem.Width()-1, 0, NULL);
	mdc.LineTo(rcItem.Width()-1, rcItem.Height()-1);

	//画item的上边
	mdc.MoveTo(0, 0, NULL);
	mdc.LineTo(rcItem.Width()-1, 0);

	//画item的底边
	if(nItem == GetItemCount()-1){
		mdc.MoveTo(0,rcItem.Height()-1, NULL);
		mdc.LineTo(rcItem.Width()-1, rcItem.Height()-1);
	}
	
	mdc.SetBkMode(TRANSPARENT);
	// 上面代码是对格子的准备工作，具体画格子是这个DrawEveryColumnItem方法，最后的效果集中到上下文hdcmem中
	drawSubItems(mdc, lpDrawItemStruct);
	// BitBlt 函数执行与从指定源设备上下文到目标设备上下文中的像素矩形对应的颜色数据的位块传输。即使打到显卡上
	BitBlt(hdc, rcItem.left, rcItem.top, rcItem.Width(), rcItem.Height(), mdc, 0, 0, SRCCOPY);
	
	DeleteObject(hPen);
	DeleteObject(mdc);
}

void QListViewCtrl::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	lpMeasureItemStruct->itemHeight = itemHeight;
}

int QListViewCtrl::CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct)
{
	return 1;
}

void QListViewCtrl::DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct)
{

}


