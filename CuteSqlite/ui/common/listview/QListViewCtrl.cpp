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
 * Create or show the combo in listview subitem.
 * 
 * @param subItemPos - The subitem position in the treeview , subItemPos.fist=iItem, subItemPos.second=iSubItem
 * @param strList - string list will show in the combobox of the subItem
 * @param nSelItem - selected item in the the combobox
 */
void QListViewCtrl::createOrShowComboBox(std::pair<int, int> & subItemPosition, const std::vector<std::wstring> & strList, int nSelItem)
{
	if (subItemPosition.first < 0 || subItemPosition.second < 0) {
		return;
	}
	GetSubItemRect(subItemPosition.first, subItemPosition.second, LVIR_BOUNDS, subItemRect);
	CRect rect(subItemRect.left + 1, 
			subItemRect.top + 1, 
			subItemRect.right - 2, 
			subItemRect.bottom -2);	
	createOrShowSubItemComboBox(rect, subItemPosition.first, subItemPosition.second, strList, nSelItem);
}


void QListViewCtrl::createOrShowComboBox(int iItem, int iSubItem, const std::vector<std::wstring> &strList, int nSelItem /*= 0*/)
{
	createOrShowComboBox(std::pair<int, int>(iItem, iSubItem), strList, nSelItem);
}


CComboBox * QListViewCtrl::getComboBoxPtr(int iItem, int iSubItem)
{
	if (iItem < 0 || iSubItem < 0) {
		return nullptr;
	}

	CComboBox * ptr = nullptr;
	if (subItemComboBoxMap.empty()) {
		return ptr;
	}
	std::pair<int, int> pair(iItem, iSubItem);
	auto iter = subItemComboBoxMap.find(pair);
	if (iter == subItemComboBoxMap.end()) {
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
	changeComboBoxesRect();
	changeCheckBoxesRect();
	changeButtonsRect();
}

void QListViewCtrl::changeComboBoxesRect()
{
	CHeaderCtrl header = GetHeader();
	CRect headRect;
	header.GetClientRect(headRect);
	for (auto iter = subItemComboBoxMap.begin(); iter != subItemComboBoxMap.end(); iter++) {
		auto pair = iter->first;
		auto ptr = iter->second;
		CRect subItemRect;
		GetSubItemRect(pair.first, pair.second, LVIR_BOUNDS, subItemRect);	
		
		if (ptr && ptr->IsWindow()) {
			CRect rect(subItemRect.left + 1, 
				subItemRect.top + 1, 
				subItemRect.right - 2, 
				subItemRect.bottom -2);
			ptr->MoveWindow(rect);
			if (subItemRect.top >= headRect.Height()) {
				ptr->ShowWindow(true);
				ptr->BringWindowToTop();

				// tmp code, will be deleted
				CRect winRect;
				ptr->GetWindowRect(winRect);
				ScreenToClient(winRect);
				continue;
			} else {
				ptr->ShowWindow(false);
			}
			
		}
	}
}

void QListViewCtrl::changeCheckBoxesRect()
{
	for (auto iter = subItemCheckBoxMap.begin(); iter != subItemCheckBoxMap.end(); iter++) {
		auto pair = iter->first;
		auto ptr = iter->second;
		CRect subItemRect;
		if (GetSubItemRect(pair.first, pair.second, LVIR_BOUNDS, subItemRect) == false) {
			Q_ERROR(L"GetSubItemRect error, iItem={}, iSubItem={}", pair.first, pair.second);
		}
		
		if (ptr && ptr->IsWindow()) {
			int x = subItemRect.left + (subItemRect.Width() - 100) / 2,
				y = subItemRect.top + (subItemRect.Height() - 20) / 2,
				w = 100, h = 20;
			CRect rect(x, y, x + w, y + h);
			ptr->MoveWindow(rect);
		}
	}
}


void QListViewCtrl::changeButtonsRect()
{
	for (auto iter = subItemButtonMap.begin(); iter != subItemButtonMap.end(); iter++) {
		auto pair = iter->first;
		auto ptr = iter->second;
		CRect subItemRect;
		if (GetSubItemRect(pair.first, pair.second, LVIR_BOUNDS, subItemRect) == false) {
			Q_ERROR(L"GetSubItemRect error, iItem={}, iSubItem={}", pair.first, pair.second);
		}
		if (ptr && ptr->IsWindow()) {
			int x = subItemRect.right - 31,
				y = subItemRect.top + (subItemRect.Height() - 20) / 2,
				w = 30, h = 20;
			CRect rect(x, y, x + w, y + h);
			ptr->MoveWindow(rect);
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
void QListViewCtrl::createOrShowSubItemComboBox(CRect & rect, int iItem, int iSubItem, const std::vector<std::wstring> &strList, int nSelItem)
{
	CComboBox * comboBoxPtr = nullptr;
	std::pair<int, int> pair(iItem, iSubItem);
	auto iter = subItemComboBoxMap.find(pair);
	if (iter != subItemComboBoxMap.end()) {
		comboBoxPtr = iter->second;
		return;
	}
	
	comboBoxPtr = new CComboBox();
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN  | WS_CLIPSIBLINGS | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_HASSTRINGS;
	UINT nID = Config::QLISTVIEWCTRL_COMBOBOX_ID_START + static_cast<int>(subItemComboBoxMap.size());
	comboBoxPtr->Create(m_hWnd, rect, L"",  dwStyle , 0, nID);
	comboBoxPtr->SetFont(comboFont);
	comboBoxPtr->SetItemHeight(-1, rect.Height());
	loadSubItemComboBox(comboBoxPtr, strList, nSelItem);
	subItemComboBoxMap[pair] = comboBoxPtr;
	changeComboBoxesRect();
}


void QListViewCtrl::createOrShowSubItemCheckBox(CRect &subItemRect, int iItem, int iSubItem)
{
	CButton * checkboxPtr = nullptr;
	std::pair<int, int> pair(iItem, iSubItem);
	auto iter = subItemCheckBoxMap.find(pair);
	int x = subItemRect.left + (subItemRect.Width() - 100) / 2,
		y = subItemRect.top + (subItemRect.Height() - 20) / 2,
		w = 100, h = 20;
	CRect rect(x, y, x + w, y + h);
	if (iter != subItemCheckBoxMap.end()) {
		checkboxPtr = iter->second;
		return;
	}

	checkboxPtr = new CButton();
	UINT nID = Config::QLISTVIEWCTRL_CHECKBOX_ID_START + static_cast<int>(subItemCheckBoxMap.size());	
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN  |WS_TABSTOP | WS_CLIPSIBLINGS | BS_CHECKBOX ;
	//checkboxPtr->Create(m_hWnd, rect, L"", dwStyle , 0, nID);
	checkboxPtr->Create(m_hWnd, rect, std::to_wstring(iItem).c_str(), dwStyle , 0, nID);
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

void QListViewCtrl::loadSubItemComboBox(CComboBox * comboBoxPtr, const std::vector<std::wstring> &strList, int nSelItem)
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
	auto lastIt = subItemComboBoxMap.begin(); // the last iterator will be deleted
	int maxRows = GetItemCount();
	int lastRow = maxRows - 1;
	for (auto it = subItemComboBoxMap.begin(), ite = subItemComboBoxMap.end(); it != ite;) {
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
		auto nextRowSubItemIt = subItemComboBoxMap.end();		
		for (auto nIter = subItemComboBoxMap.begin(); nIter != subItemComboBoxMap.end(); nIter++) {
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
	for (auto it = subItemComboBoxMap.begin(), ite = subItemComboBoxMap.end(); it != ite;) {
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
		it = subItemComboBoxMap.erase(it);
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
	for (auto it = subItemComboBoxMap.begin(), ite = subItemComboBoxMap.end(); it != ite;) {
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
		auto prevRowSubItemIt = subItemComboBoxMap.end();		
		for (auto nIter = subItemComboBoxMap.begin(); nIter != subItemComboBoxMap.end(); nIter++) {
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
	for (auto it = subItemComboBoxMap.begin(), ite = subItemComboBoxMap.end(); it != ite;) {
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
		auto nextRowSubItemIt = subItemComboBoxMap.end();		
		for (auto nIter = subItemComboBoxMap.begin(); nIter != subItemComboBoxMap.end(); nIter++) {
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

	if (subItemEdit.IsWindow()) subItemEdit.DestroyWindow();

	// release subItemComboBoxMap
	auto iter = subItemComboBoxMap.begin();
	while (iter != subItemComboBoxMap.end()) {
		auto ptr = iter->second;
		if (!ptr || !ptr->IsWindow()) {
			continue;
		}
		ptr->DestroyWindow();
		delete ptr;
		ptr = nullptr;
		iter++;
	}
	subItemComboBoxMap.clear();

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
}

void QListViewCtrl::OnSize(UINT nType, CSize size)
{
	if (!bkgBrush) bkgBrush = ::CreateSolidBrush(bkgColor);
	if (!textFont) textFont = FT(L"list-text-size");
	if (!comboFont) comboFont = FTB(L"list-combobox-size", true);

	changeSubItemText();
	//changeComboBoxesRect();
	//changeCheckBoxesRect();
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


BOOL QListViewCtrl::OnEraseBkgnd(CDCHandle dc)
{
	return true;
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

