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
	} else if ( (pMsg->hwnd == subItemEdit.m_hWnd && pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_TAB) // press tab on subItemEdit  
		|| (pMsg->hwnd == subItemComboBox.m_hWnd && pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_TAB) // press tab on subItemComboBox  
		|| (pMsg->hwnd == m_hWnd && pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_TAB) ) { // press tab on ListView  
		// if pressed shift + tab
		if ( (GetKeyState(VK_SHIFT) & (1 << (sizeof(SHORT)*8-1))) != 0) { 
			pressedShiftTabToMoveEditor(); //has press shift 
		} else { 
			pressedTabToMoveEditor(); // no press shift
		}		
	} else if ( (pMsg->hwnd == subItemEdit.m_hWnd && pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_UP) // press up on subItemEdit  
		|| (pMsg->hwnd == subItemComboBox.m_hWnd && pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_UP) // press up on subItemComboBox  
		|| (pMsg->hwnd == m_hWnd && pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_UP) ) { // press up on ListView  		
		return pressedUpToMoveEditor();
		
	} else if ( (pMsg->hwnd == subItemEdit.m_hWnd && pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_DOWN) // press down on subItemEdit  
		|| (pMsg->hwnd == subItemComboBox.m_hWnd && pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_DOWN) // press down on subItemComboBox  
		|| (pMsg->hwnd == m_hWnd && pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_DOWN) ) { // press down on ListView  		
		return pressedDownToMoveEditor();
	} else if ( (pMsg->hwnd == subItemEdit.m_hWnd && pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_LEFT) // press left on subItemEdit  
		|| (pMsg->hwnd == subItemComboBox.m_hWnd && pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_LEFT) // press left on subItemComboBox  
		|| (pMsg->hwnd == m_hWnd && pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_LEFT) ) { // press left on ListView  		
		pressedShiftTabToMoveEditor();	// left == shift + tab	
	}else if ( (pMsg->hwnd == subItemEdit.m_hWnd && pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RIGHT) // press right on subItemEdit  
		|| (pMsg->hwnd == subItemComboBox.m_hWnd && pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RIGHT) // press right on subItemComboBox  
		|| (pMsg->hwnd == m_hWnd && pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RIGHT) ) { // press right on ListView  		
		pressedTabToMoveEditor();	// left == shift + tab	
	}  else if (pMsg->hwnd == m_hWnd && pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_SPACE) { // press space on ListView  
		pressedSpaceToCheckbox();
	} else if (pMsg->hwnd == m_hWnd && pMsg->message == WM_MOUSEWHEEL) { // mouse wheel
		changeSubItemText();
	}

	return FALSE;
}

void QListViewCtrl::createOrShowEditor(std::pair<int, int> subItemPos)
{
	if (subItemPos.first < 0 || subItemPos.second < 0) {
		return;
	}

	// destroy the Combobox
	if (isVisibleComboBox && subItemComboBox.IsWindow()) {
		subItemComboBox.DestroyWindow();
	}
	activeSubItem(subItemPos.first, subItemPos.second);
	

	wchar_t * cch = nullptr;
	GetItemText(subItemPos.first, subItemPos.second, cch);
	std::wstring text;
	if (cch) {
		text.assign(cch);
		::SysFreeString(cch);
	}
	
	createOrShowSubItemEdit(subItemEdit, text, activeSubItemRect);
}


void QListViewCtrl::createOrShowEditor(int iItem, int iSubItem)
{
	createOrShowEditor(std::pair<int, int>(iItem, iSubItem));
}

/**
 * Create or show the combo button in ListView subitem.
 * 
 * @param iItem - The row item position in the ListView
 * @param iSubItem - The sub item position in the ListView
 * @param val - The selected text in Combobox
 */
void QListViewCtrl::createComboBox(int iItem, int iSubItem, const std::wstring & val)
{
	if (iItem < 0 || iSubItem < 0) {
		return;
	}
	std::pair<int, int> pair(iItem, iSubItem);
	auto iter = subItemComboBoxMap.find(pair);
	if (iter != subItemComboBoxMap.end()) {
		iter->second = val;
		return;
	}
	
	subItemComboBoxMap[pair] = val;
}


void QListViewCtrl::showComboBox(int iItem, int iSubItem, const std::vector<std::wstring> & strList, bool allowEdit)
{
	if (iItem < 0 || iSubItem < 0) {
		return;
	}
	createOrShowComboBox(iItem, iSubItem, strList, allowEdit);
	
	activeSubItem(iItem, iSubItem);
	this->isVisibleComboBox = true;
}

void QListViewCtrl::createCheckBox(int iItem, int iSubItem, int isChecked)
{
	if (iItem < 0 || iSubItem < 0) {
		return;
	}
	std::pair<int, int> pair(iItem, iSubItem);
	auto iter = subItemCheckBoxMap.find(pair);
	if (iter != subItemCheckBoxMap.end()) {
		iter->second = isChecked;
		return;
	}
	subItemCheckBoxMap[pair] = isChecked;
}


void QListViewCtrl::setCheckBoxIsChecked(int iItem, int iSubItem, int isChecked)
{
	if (iItem < 0 || iSubItem < 0) {
		return;
	}
	// destroy the Combobox
	destroyComboBox();

	std::pair<int, int> pair(iItem, iSubItem);
	auto iter = subItemCheckBoxMap.find(pair);
	if (iter != subItemCheckBoxMap.end()) {
		iter->second = isChecked;
		return;
	}
	subItemCheckBoxMap[pair] = isChecked;
}

int QListViewCtrl::getCheckBoxIsChecked(int iItem, int iSubItem)
{
	if (iItem < 0 || iSubItem < 0) {
		return 0;
	}
	int result = 0;
	if (subItemCheckBoxMap.empty()) {
		return result;
	}
	std::pair<int, int> pair(iItem, iSubItem);
	auto iter = subItemCheckBoxMap.find(pair);
	if (iter == subItemCheckBoxMap.end()) {
		return 0;
	}
	result = iter->second;
	return result;
}


void QListViewCtrl::createButton(int iItem, int iSubItem, const std::wstring & text)
{
	if (iItem < 0 || iSubItem < 0) {
		return;
	}
	std::pair<int, int> pair(iItem, iSubItem);
	auto iter = subItemButtonMap.find(pair);
	if (iter != subItemButtonMap.end()) {
		iter->second = text;
		return;
	}
	subItemButtonMap[pair] = text;
}

const SubItemValues & QListViewCtrl::getChangedVals()
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
	if (activeSubItemPos.first < 0 || activeSubItemPos.second < 0) {
		return;
	}
	
	//SetItemText(subItemPos.first, subItemPos.second, text);
	SubItemValue subItemVal;
	
	if (isVisibleEdit && subItemEdit.IsWindow()) {	
		CString text;
		subItemEdit.GetWindowText(text);		
		wchar_t * cch = nullptr;
		GetItemText(activeSubItemPos.first, activeSubItemPos.second, cch);
		subItemVal.iItem = activeSubItemPos.first;
		subItemVal.iSubItem = activeSubItemPos.second;
		if (cch) {
			subItemVal.origVal.assign(cch);
			::SysFreeString(cch);
		}
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
		::PostMessage(GetParent().m_hWnd, Config::MSG_QLISTVIEW_SUBITEM_TEXT_CHANGE_ID, WPARAM(activeSubItemPos.first), LPARAM(activeSubItemPos.second));
	} else if (isVisibleComboBox && subItemComboBox.IsWindow()) {
		CString text;
		subItemComboBox.GetWindowText(text);
		wchar_t * cch = nullptr;
		GetItemText(activeSubItemPos.first, activeSubItemPos.second, cch);
		
		subItemVal.iItem = activeSubItemPos.first;
		subItemVal.iSubItem = activeSubItemPos.second;
		if (cch) {
			subItemVal.origVal.assign(cch);
			::SysFreeString(cch);
		}
		subItemVal.newVal = text;
		// it will be return when original text equals new text
		if (subItemVal.origVal == subItemVal.newVal) {
			subItemComboBox.DestroyWindow();
			isVisibleComboBox = false;
			return;
		}

		setChangeVal(subItemVal);
		subItemComboBox.DestroyWindow();
		isVisibleComboBox = false;
		::PostMessage(GetParent().m_hWnd, Config::MSG_QLISTVIEW_SUBITEM_TEXT_CHANGE_ID, WPARAM(activeSubItemPos.first), LPARAM(activeSubItemPos.second));
	}
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

void QListViewCtrl::activeSubItem(int iItem, int iSubItem)
{
	ATLASSERT(iItem >= 0 && iSubItem > 0);
	oldSubItemPos = activeSubItemPos;
	activeSubItemPos.first = iItem;
	activeSubItemPos.second = iSubItem;
	GetSubItemRect(oldSubItemPos.first, oldSubItemPos.second, LVIR_BOUNDS, oldSubItemRect);
	GetSubItemRect(activeSubItemPos.first, activeSubItemPos.second, LVIR_BOUNDS, activeSubItemRect);
	InvalidateRect(oldSubItemRect);
	InvalidateRect(activeSubItemRect);
}

/**
 * Set all item checked state in the header first column.
 * if selected all then set image = 1(checked), otherwise set image=0(unchecked).
 * 
 */
void QListViewCtrl::changeAllItemsCheckState()
{
	CHeaderCtrl headerCtrl = GetHeader();
	HDITEM headerItem;
	headerItem.mask = HDI_IMAGE;
	headerCtrl.GetItem(0, &headerItem);

	// rows count equal selected rows count
	bool checkAll = GetItemCount() == GetSelectedCount();

	if (checkAll) {		
		if (headerItem.iImage == 1) {
			return ;
		}
		headerItem.iImage = 1;
	} else {
		if (headerItem.iImage == 0) {
			return ;
		}
		headerItem.iImage = 0;
	}

	headerItem.fmt = HDF_LEFT | HDF_IMAGE;
	bool ret = headerCtrl.SetItem(0, &headerItem);
	CRect rect;
	headerCtrl.GetItemRect(0, rect);
	headerCtrl.InvalidateRect(rect, true);
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
 * Press Tab key will be active next subitem.
 * 
 */
void QListViewCtrl::pressedTabToMoveEditor()
{
	changeSubItemText();
	int columnCount = GetHeader().GetItemCount();
	int rowCount = GetItemCount();
	if (activeSubItemPos.first >= rowCount - 1 && activeSubItemPos.second >= columnCount - 1) {
		return;
	}
	
	if (activeSubItemPos.first > rowCount - 1 || activeSubItemPos.second > columnCount - 1) {
		activeSubItemPos.first = 0;
		activeSubItemPos.second = 1;
	}

	int iItem = activeSubItemPos.first, iSubItem = activeSubItemPos.second;
	if (activeSubItemPos.first < rowCount - 1 && activeSubItemPos.second == columnCount - 1) {
		iItem = activeSubItemPos.first + 1;
		iSubItem = 1;
		SelectItem(iItem);
		changeAllItemsCheckState();
		
	}else if (activeSubItemPos.first <= rowCount - 1 && activeSubItemPos.second < columnCount - 1) {
		iSubItem = activeSubItemPos.second + 1;
	}
	activeSubItem(iItem, iSubItem);
	
	std::pair<int, int> findPair({ iItem, iSubItem });
	SubItemComboBoxMap::iterator comboBoxIter;
	SubItemCheckBoxMap::iterator checkBoxIter;
	SubItemButtonMap::iterator buttonIter;
	
	if ((comboBoxIter = subItemComboBoxMap.find(findPair)) != subItemComboBoxMap.end() 
		|| (checkBoxIter = subItemCheckBoxMap.find(findPair)) != subItemCheckBoxMap.end()
		|| (buttonIter = subItemButtonMap.find(findPair)) != subItemButtonMap.end()) {
		oldSubItemPos = activeSubItemPos;
		return ;
	} 
	createOrShowEditor(iItem, iSubItem);
}

/**
 * Press Shift + Tab will be active prev subitem.
 * 
 */
void QListViewCtrl::pressedShiftTabToMoveEditor()
{
	changeSubItemText();
	int columnCount = GetHeader().GetItemCount();
	int rowCount = GetItemCount();
	if (activeSubItemPos.first <= 0 && activeSubItemPos.second <= 0) {
		return;
	}
	if (activeSubItemPos.first > rowCount - 1 || activeSubItemPos.second > columnCount - 1) {
		activeSubItemPos.first = 0;
		activeSubItemPos.second = 1;
		return ;
	}

	// iSubItem == 0 is the first checkbox column, so valid iSubItem is begin from 1th
	int iItem = activeSubItemPos.first, iSubItem = activeSubItemPos.second;
	if (activeSubItemPos.first > 0 && activeSubItemPos.second == 1) {
		iItem = activeSubItemPos.first - 1;
		iSubItem = columnCount - 1;
		SelectItem(iItem);
		changeAllItemsCheckState();
	} else  if (activeSubItemPos.first >= 0 && activeSubItemPos.second > 1) { // common
		iSubItem = activeSubItemPos.second - 1;
	}
	activeSubItem(iItem, iSubItem);
	
	std::pair<int, int> findPair({ iItem, iSubItem });
	SubItemComboBoxMap::iterator comboBoxIter;
	SubItemCheckBoxMap::iterator checkBoxIter;
	SubItemButtonMap::iterator buttonIter;
	if ((comboBoxIter = subItemComboBoxMap.find(findPair)) != subItemComboBoxMap.end() 
		|| (checkBoxIter = subItemCheckBoxMap.find(findPair)) != subItemCheckBoxMap.end()
		|| (buttonIter = subItemButtonMap.find(findPair)) != subItemButtonMap.end()) {		
		return ;
	} 
	createOrShowEditor(iItem, iSubItem);
}

void QListViewCtrl::pressedSpaceToCheckbox()
{
	::PostMessage(GetParent().m_hWnd, Config::MSG_QLISTVIEW_SUBITEM_CHECKBOX_CHANGE_ID, 
		WPARAM(activeSubItemPos.first), LPARAM(activeSubItemPos.second));
}

bool QListViewCtrl::pressedUpToMoveEditor()
{
	changeSubItemText();
	int columnCount = GetHeader().GetItemCount();
	int rowCount = GetItemCount();
	if (activeSubItemPos.first <= 0 && activeSubItemPos.second <= 0) {
		return false;
	}

	if (activeSubItemPos.first > rowCount - 1 || activeSubItemPos.second > columnCount - 1) {
		activeSubItemPos.first = 0;
		activeSubItemPos.second = 1;
		return false;
	}

	int iItem = activeSubItemPos.first, iSubItem = activeSubItemPos.second;
	// iSubItem == 0 is the first checkbox column, so valid iSubItem is begin from 1th
	if (iItem > 0 && iSubItem > 0) {
		iItem = activeSubItemPos.first - 1;	
	}
	SelectItem(iItem);
	changeAllItemsCheckState();
	activeSubItem(iItem, iSubItem);
	
	std::pair<int, int> findPair({ iItem, iSubItem });
	SubItemComboBoxMap::iterator comboBoxIter;
	SubItemCheckBoxMap::iterator checkBoxIter;
	SubItemButtonMap::iterator buttonIter;
	if ((comboBoxIter = subItemComboBoxMap.find(findPair)) != subItemComboBoxMap.end() 
		|| (checkBoxIter = subItemCheckBoxMap.find(findPair)) != subItemCheckBoxMap.end()
		|| (buttonIter = subItemButtonMap.find(findPair)) != subItemButtonMap.end()) {
		return true;
	} 

		
	createOrShowEditor(iItem, iSubItem);
	return true;
}

bool QListViewCtrl::pressedDownToMoveEditor()
{
	changeSubItemText();
	int columnCount = GetHeader().GetItemCount();
	int rowCount = GetItemCount();
	if (activeSubItemPos.first >= rowCount - 1 || activeSubItemPos.second <= 0) {
		return false;
	}

	if (activeSubItemPos.first > rowCount - 1 || activeSubItemPos.second > columnCount - 1) {
		activeSubItemPos.first = 0;
		activeSubItemPos.second = 1;
		return false;
	}

	int iItem = activeSubItemPos.first, iSubItem = activeSubItemPos.second;
	// iSubItem == 0 is the first checkbox column, so valid iSubItem is begin from 1th
	if (iItem >= 0 && iItem < rowCount - 1 && iSubItem > 0) {
		iItem = activeSubItemPos.first + 1;	
	}
	SelectItem(iItem);
	changeAllItemsCheckState();	
	activeSubItem(iItem, iSubItem);
	
	std::pair<int, int> findPair({ iItem, iSubItem });
	SubItemComboBoxMap::iterator comboBoxIter;
	SubItemCheckBoxMap::iterator checkBoxIter;
	SubItemButtonMap::iterator buttonIter;
	if ((comboBoxIter = subItemComboBoxMap.find(findPair)) != subItemComboBoxMap.end() 
		|| (checkBoxIter = subItemCheckBoxMap.find(findPair)) != subItemCheckBoxMap.end()
		|| (buttonIter = subItemButtonMap.find(findPair)) != subItemButtonMap.end()) {	
		return true;
	} 
	createOrShowEditor(iItem, iSubItem);
	return true;
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


LRESULT QListViewCtrl::OnSubItemEditChange(UINT uNotifyCode, int nID, HWND hwnd)
{
	if (isVisibleEdit && subItemEdit.IsWindow()) {	
		SubItemValue subItemVal;
		CString text;
		subItemEdit.GetWindowText(text);		
		wchar_t * cch = nullptr;
		GetItemText(activeSubItemPos.first, activeSubItemPos.second, cch);
		subItemVal.iItem = activeSubItemPos.first;
		subItemVal.iSubItem = activeSubItemPos.second;
		if (cch) {
			subItemVal.origVal.assign(cch);
			::SysFreeString(cch);
		}
		subItemVal.newVal = text;
		// it will be return when original text equals new text
		if (subItemVal.origVal == subItemVal.newVal) {
			return 0;
		}

		setChangeVal(subItemVal);
		::PostMessage(GetParent().m_hWnd, Config::MSG_QLISTVIEW_SUBITEM_TEXT_CHANGE_ID, WPARAM(activeSubItemPos.first), LPARAM(activeSubItemPos.second));
	}
	return 0; 
}

LRESULT QListViewCtrl::OnSubItemComboBoxKillFocus(UINT uNotifyCode, int nID, HWND hwnd)
{
	changeSubItemText();
	return 0;
}

LRESULT QListViewCtrl::OnSubItemComboBoxEditChange(UINT uNotifyCode, int nID, HWND hwnd)
{
	SubItemValue subItemVal;
	if (isVisibleComboBox && subItemComboBox.IsWindow()) {
		CString text;
		subItemComboBox.GetWindowText(text);
		wchar_t * cch = nullptr;
		GetItemText(activeSubItemPos.first, activeSubItemPos.second, cch);
		
		subItemVal.iItem = activeSubItemPos.first;
		subItemVal.iSubItem = activeSubItemPos.second;
		if (cch) {
			subItemVal.origVal.assign(cch);
			::SysFreeString(cch);
		}
		subItemVal.newVal = text;
		// it will be return when original text equals new text
		if (subItemVal.origVal == subItemVal.newVal) {
			return 0;
		}

		setChangeVal(subItemVal);
		::PostMessage(GetParent().m_hWnd, Config::MSG_QLISTVIEW_SUBITEM_TEXT_CHANGE_ID, WPARAM(activeSubItemPos.first), LPARAM(activeSubItemPos.second));
	}
	return 0;
}

LRESULT QListViewCtrl::OnSubItemComboBoxSelChange(UINT uNotifyCode, int nID, HWND hwnd)
{
	SubItemValue subItemVal;
	if (isVisibleComboBox && subItemComboBox.IsWindow()) {
		wchar_t * cch = nullptr;
		GetItemText(activeSubItemPos.first, activeSubItemPos.second, cch);
		
		subItemVal.iItem = activeSubItemPos.first;
		subItemVal.iSubItem = activeSubItemPos.second;
		if (cch) {
			subItemVal.origVal.assign(cch);
			::SysFreeString(cch);
		}
		subItemVal.iItem = activeSubItemPos.first;
		subItemVal.iSubItem = activeSubItemPos.second;
		int nSelItem = subItemComboBox.GetCurSel();
		cch = nullptr;
		bool ret = subItemComboBox.GetLBTextBSTR(nSelItem,cch);
		if (ret && cch) {
			subItemVal.newVal.assign(cch);
			::SysFreeString(cch);
		}
		// it will be return when original text equals new text
		if (subItemVal.origVal == subItemVal.newVal) {
			return 0;
		}

		setChangeVal(subItemVal);
		::PostMessage(GetParent().m_hWnd, Config::MSG_QLISTVIEW_SUBITEM_TEXT_CHANGE_ID, WPARAM(activeSubItemPos.first), LPARAM(activeSubItemPos.second));
	}
	return 0;
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
	removeChangedValsItems(iItem);

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
	if (btnBrush) ::DeleteObject(btnBrush);
	if (btnDownBrush) ::DeleteObject(btnDownBrush);
	if (chkBrush) ::DeleteObject(chkBrush);
	if (selSubItemBorderBrush) ::DeleteObject(selSubItemBorderBrush);
	if (!chkBorderPen.IsNull()) chkBorderPen.DeleteObject();
	if (textFont) ::DeleteObject(textFont);
	if (comboFont) ::DeleteObject(comboFont);
	if (normalFont) ::DeleteObject(normalFont);
	if (!borderPen.IsNull()) borderPen.DeleteObject();
	if (!btnBorderPen.IsNull()) btnBorderPen.DeleteObject();
	if (!selSubItemBorderPen.IsNull()) selSubItemBorderPen.DeleteObject();

	if (subItemEdit.IsWindow()) subItemEdit.DestroyWindow();
	if (subItemComboBox.IsWindow()) subItemComboBox.DestroyWindow();

	// release subItemComboBoxMap
	subItemComboBoxMap.clear();

	// release subItemCheckBoxMap
	subItemCheckBoxMap.clear();

	// release subItemButtonMap
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
	if (!btnBrush) btnBrush = ::CreateSolidBrush(btnColor);
	if (!btnDownBrush) btnDownBrush = ::CreateSolidBrush(btnDownColor);
	if (!chkBrush) chkBrush = ::CreateSolidBrush(chkColor);
	if (!selSubItemBorderBrush) chkBrush = ::CreateSolidBrush(selSubItemBorderColor);
	if (!textFont) textFont = FT(L"list-text-size");
	if (!comboFont) comboFont = FT(L"list-combobox-size");
	if (!normalFont) normalFont = FT(L"list-text-size");
	if (borderPen.IsNull()) borderPen.CreatePen(PS_SOLID, 1, borderColor);
	if (btnBorderPen.IsNull()) btnBorderPen.CreatePen(PS_SOLID, 1, btnBorderColor);
	if (chkBorderPen.IsNull()) chkBorderPen.CreatePen(PS_SOLID, 2, chkBorderColor); 
	if (selSubItemBorderPen.IsNull()) selSubItemBorderPen.CreatePen(PS_SOLID, 1, selSubItemBorderColor); 

	createImageList();
	changeSubItemText();
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
			Invalidate(false);

			destroySubItemElems();
			return 1;
		}
	} else if (pnmh->code == HDN_ITEMCLICKA 
		|| pnmh->code == HDN_ITEMCLICKW) {
		destroySubItemElems();

		NMHEADER * pNMHeader = (NMHEADER *)pnmh;
		if (pNMHeader->iItem >= 0) {
			if (pNMHeader->iItem == 0) { // SELECT ALL
				checkedAllItems();
			}
			::PostMessage(GetParent().m_hWnd, Config::MSG_QLISTVIEW_COLUMN_CLICK_ID, WPARAM(pNMHeader->iItem), LPARAM(pNMHeader));
			return 0;
		}
	}
	this->SetMsgHandled(false); 
	return 0;
}


void QListViewCtrl::checkedAllItems()
{
	CHeaderCtrl headerCtrl = GetHeader();

	HDITEM headerItem;
	headerItem.mask = HDI_IMAGE;
	headerCtrl.GetItem(0, &headerItem);
	if (headerItem.iImage == 1) {
		headerItem.iImage = 0;
		SelectAllItems(false);
	}
	else {
		headerItem.iImage = 1;
		SelectAllItems(true);
	}
	headerItem.fmt = HDF_LEFT;
	headerCtrl.SetItem(0, &headerItem);
}

void QListViewCtrl::destroyComboBox()
{
	if (isVisibleComboBox && subItemComboBox.IsWindow()) {
		subItemComboBox.DestroyWindow();
	}
}


void QListViewCtrl::destroyEditor()
{
	if (isVisibleEdit && subItemEdit.IsWindow()) {
		subItemEdit.DestroyWindow();
	}
}


void QListViewCtrl::destroySubItemElems()
{
	destroyComboBox();
	destroyEditor();
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

/**
 * Draw the row's all item(s).
 * 
 * @param lpDrawItemStruct
 */
void QListViewCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	HDC hdc = lpDrawItemStruct->hDC;
	CRect rcItem = lpDrawItemStruct->rcItem;  // listview每项的rect，受滚动条位置影响
	int iItem = lpDrawItemStruct->itemID;
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
	mdc.SelectPen(borderPen);
	mdc.SelectFont(normalFont);

	if (lpDrawItemStruct->itemState & ODS_SELECTED)  {
		hBrush.CreateSolidBrush(RGB(145, 201, 247));
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
	if(iItem == GetItemCount()-1){
		mdc.MoveTo(0,rcItem.Height()-1, NULL);
		mdc.LineTo(rcItem.Width()-1, rcItem.Height()-1);
	}
	
	mdc.SetBkMode(TRANSPARENT);
	// 上面代码是对格子的准备工作，具体画格子是这个drawSubItems方法，最后的效果集中到上下文hdcmem中
	drawSubItems(mdc, lpDrawItemStruct);
	// BitBlt 函数执行与从指定源设备上下文到目标设备上下文中的像素矩形对应的颜色数据的位块传输。即使打到显卡上
	BitBlt(hdc, rcItem.left, rcItem.top, rcItem.Width(), rcItem.Height(), mdc, 0, 0, SRCCOPY);
	
	DeleteObject(mdc);
}

void QListViewCtrl::drawSubItems(CDC & mdc,  LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	int iItem = lpDrawItemStruct->itemID;
	LVCOLUMN lvc;
	lvc.mask = LVCF_WIDTH | LVCF_FMT;

	TCHAR szBuf[MAX_PATH] = {0};
	LVITEM lvi;
	lvi.mask = LVIF_TEXT;
	lvi.iItem = lpDrawItemStruct->itemID; // 这里指定第几行 lvi.iItem：指定行数，lvi.iSubItem指定列数
	lvi.pszText = szBuf;
	lvi.cchTextMax = MAX_PATH;

	for (int iSubItem = 0; GetColumn(iSubItem, &lvc); ++iSubItem) {
		CRect rcSubItem;
		if(!GetSubItemRect(lpDrawItemStruct->itemID, iSubItem, LVIR_LABEL, rcSubItem)) {	
			continue;
		}
		lvi.iSubItem = iSubItem; // lvi.iItem：指定行数，lvi.iSubItem指定列数
		GetItem(&lvi); // 把原始的格子的结构体数据提取出来，参数指定第几行

		CRect rcText(rcSubItem.left - hOffset + 1, 0, rcSubItem.right - hOffset, rcSubItem.Height() - 1);  //相对于内存dc的item rect
		

		// draw the CheckBox of first column
		auto comboBoxIter = subItemComboBoxMap.end();
		auto checkBoxIter = subItemCheckBoxMap.end();
		auto buttonIter = subItemButtonMap.end();
		if(iSubItem == 0) {
			bool checked = lpDrawItemStruct->itemState & ODS_SELECTED;
			drawFirstSubItem(mdc, iItem, checked, rcSubItem, rcText);
		} else if ((comboBoxIter = subItemComboBoxMap.find(std::pair<int, int>(iItem, iSubItem))) != subItemComboBoxMap.end()){
			drawComboBoxInSubItem(mdc, lpDrawItemStruct, comboBoxIter, rcText);
		} else if ((checkBoxIter = subItemCheckBoxMap.find(std::pair<int, int>(iItem, iSubItem))) != subItemCheckBoxMap.end()){
			drawCheckBoxInSubItem(mdc, lpDrawItemStruct, checkBoxIter, rcText);
		} else if ((buttonIter = subItemButtonMap.find(std::pair<int, int>(iItem, iSubItem))) != subItemButtonMap.end()){
			drawButtonInSubItem(mdc, lpDrawItemStruct, buttonIter, rcText);
		} else {
			drawTextInSubItem(mdc, iItem, iSubItem, rcText);

		}
	}
}

void QListViewCtrl::drawFirstSubItem(CDC &mdc, int iItem, bool checked, CRect &rcSubItem, CRect &rcText)
{
	int chkWidth = 16;
	int chkHeight = 16;

	rcText.left += 5 + chkWidth;

	int nleft = 6;
	int ntop = (rcSubItem.Height() - chkHeight) / 2;

	CDC bitmapDc;
	bitmapDc.CreateCompatibleDC(NULL);
	if (checked) {
		HBITMAP hbmoldsource = (HBITMAP)::SelectObject(bitmapDc.m_hDC, (HBITMAP)checkYesBitmap);
		mdc.BitBlt(nleft, ntop, chkWidth, chkHeight, bitmapDc, 0, 0, SRCCOPY);
		::SelectObject(bitmapDc.m_hDC, hbmoldsource);
	} else {
		HBITMAP hbmoldsource = (HBITMAP)::SelectObject(bitmapDc.m_hDC, (HBITMAP)checkNoBitmap);
		mdc.BitBlt(nleft, ntop, chkWidth, chkHeight, bitmapDc, 0, 0, SRCCOPY);
		::SelectObject(bitmapDc.m_hDC, hbmoldsource);
	}
	::DeleteObject(bitmapDc.m_hDC);

	// 第0列，文本显示的序号（正序还是倒叙, 正序序号从1开始，所以要加1）
	int id = iItem + 1;
	std::wstring lineNum = std::to_wstring(id);
	if (lineNum.size()) {
		CRect rcLineNum = rcText;
		rcLineNum.left = nleft + chkWidth + leftMargin + 2;
		rcLineNum.top = rcText.top;
		UINT uFormat = DT_LEFT | DT_VCENTER | DT_END_ELLIPSIS;
		CSize lpSize; // GetTextExtentPoint32 函数计算指定文本字符串的宽度和高度。
		::GetTextExtentPoint32(mdc, lineNum.c_str(), static_cast<int>(lineNum.size()), &lpSize);
		rcLineNum.top += (rcLineNum.Height() - lpSize.cy) / 2;
		DrawText(mdc, lineNum.c_str(), static_cast<int>(lineNum.size()), rcLineNum, uFormat);
	}
}

void QListViewCtrl::drawTextInSubItem(CDC & mdc, int iItem, int iSubItem,  CRect rcText, std::wstring & text)
{
	wchar_t* str = NULL;
	GetItemText(iItem, iSubItem, str);
	if (str) {
		text.assign(str);
		::SysFreeString(str);
	}	
	
	if (iSubItem > 0 && activeSubItemPos.first == iItem && activeSubItemPos.second == iSubItem) {
		HPEN oldPen = mdc.SelectPen(selSubItemBorderPen);
		HBRUSH oldBrush = mdc.SelectStockBrush(WHITE_BRUSH);
		mdc.Rectangle(rcText);
		mdc.SelectBrush(oldBrush);
		mdc.SelectPen(oldPen);
	}

	if (!text.empty()) {
		CSize lpSize;
		int strLen = static_cast<int>(text.size());
		::GetTextExtentPoint32(mdc, text.c_str(), strLen, &lpSize);
		CRect rect = rcText;
		rect.top += (rect.Height() - lpSize.cy) / 2;
		UINT uFormat = DT_LEFT | DT_VCENTER | DT_END_ELLIPSIS;
		rect.left += leftMargin;
		DrawText(mdc, text.c_str(), strLen, rect, uFormat);
	}
}

void QListViewCtrl::drawComboBoxInSubItem(CDC & mdc, LPDRAWITEMSTRUCT lpDrawItemStruct, SubItemComboBoxMap::iterator & comboBoxIter, CRect & rcText)
{
	if (rcText.Width() < 20) {
		return;
	}
	drawTextInSubItem(mdc, lpDrawItemStruct->itemID, comboBoxIter->first.second, rcText);

	int x = rcText.right - 16 - 1, y = rcText.top + 1,
		w = 16, h = rcText.Height() - 2;
	CRect rect(x, y, x + w, y + h);
	HPEN oldPen = mdc.SelectPen(btnBorderPen);
	HBRUSH oldBrush = mdc.SelectBrush(btnBrush);
	mdc.Rectangle(rect);

	// draw the triangle for drop down in the rectangle 
	oldBrush = mdc.SelectBrush(btnDownBrush);
	
	CPoint pts[4];
	pts[0] = { rect.left + (rect.Width() - 10) / 2, rect.top + (rect.Height() - 6) / 2 };
	pts[1] = { rect.left + (rect.Width() - 10) / 2 + 10, rect.top + (rect.Height() - 6) / 2 };
	pts[2] = { rect.left + (rect.Width()) / 2, rect.top + (rect.Height() - 6) / 2 + 6 };
	pts[3] = { rect.left + (rect.Width() - 10) / 2, rect.top + (rect.Height() - 6) / 2 };
	mdc.Polygon(pts, 4);	
	mdc.SelectBrush(oldBrush);
	mdc.SelectPen(oldPen);
	
}


void QListViewCtrl::drawCheckBoxInSubItem(CDC & mdc, LPDRAWITEMSTRUCT lpDrawItemStruct, SubItemCheckBoxMap::iterator & checkBoxIter, CRect & rcText)
{
	if (rcText.Width() < 14) {
		return;
	}

	CRect borderRect = { rcText.left, rcText.top , rcText.right , rcText.bottom  };
	if (activeSubItemPos.first == checkBoxIter->first.first && activeSubItemPos.second == checkBoxIter->first.second) {
		HPEN oldPen = mdc.SelectPen(selSubItemBorderPen);
		HBRUSH oldBrush  = mdc.SelectStockBrush(WHITE_BRUSH);		
		mdc.Rectangle(borderRect);
		mdc.SelectBrush(oldBrush);
		mdc.SelectPen(oldPen);
	}

	int x = rcText.left + (rcText.Width() - 14) / 2, y = rcText.top + (rcText.Height() - 14) / 2,
		w = 14, h = 14;
	CRect rect(x, y, x + w, y + h);	
	
	// 画方框
	rect.DeflateRect(1, 1, 1, 1); //减一像素
	mdc.FillRect(rect, bkgBrush); // 用背景色画刷先涂抹掉原来的画面
	rect.InflateRect(1, 1, 1, 1);//增一像素
	
	HPEN oldPen = mdc.SelectPen(chkBorderPen);
	mdc.FrameRect(rect, chkBrush); // 外框

	// draw the triangle for drop down in the rectangle 

	int margin = 3;
	if (rect.Width() < 14) {
		margin = 2;
	}

	// 画打勾
	if (checkBoxIter->second == 1) {
		int x = rect.left + margin , y = rect.top + rect.Height() / 2;
		mdc.MoveTo(rect.left + margin, rect.top + rect.Height() / 2);
		mdc.LineTo(rect.left + margin + 4, rect.bottom - margin);
		mdc.LineTo(rect.right - margin, rect.top + margin);
	}
	mdc.SelectPen(oldPen);
}


void QListViewCtrl::drawButtonInSubItem(CDC & mdc, LPDRAWITEMSTRUCT lpDrawItemStruct, SubItemButtonMap::iterator & buttonIter, CRect & rcText)
{
	if (rcText.Width() < 20) {
		return;
	}
	drawTextInSubItem(mdc, lpDrawItemStruct->itemID, buttonIter->first.second, rcText); // Draw the text in subitem itself

	int x = rcText.right - 20 - 2, y = rcText.top + (rcText.Height() - 20) / 2 + 2,
		w = 20, h = 20 - 2;
	CRect rect(x, y, x + w, y + h);
	HPEN oldPen = mdc.SelectPen(btnBorderPen);
	HBRUSH oldBrush = mdc.SelectBrush(btnBrush);
	mdc.Rectangle(rect);

	UINT uFormat = DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS;
	int strLen = static_cast<int>(buttonIter->second.size());
	// Draw button text
	mdc.DrawTextW(buttonIter->second.c_str(), strLen, rect, uFormat);

	mdc.SelectBrush(oldBrush);
	mdc.SelectPen(oldPen);
}


void QListViewCtrl::createOrShowComboBox(int iItem, int iSubItem, const std::vector<std::wstring> & strList, bool allowEdit)
{
	CRect subItemRect;
	GetSubItemRect(iItem, iSubItem, LVIR_BOUNDS, subItemRect);
	createOrShowSubItemComboBox(subItemComboBox, subItemRect, allowEdit);
	subItemComboBox.ResetContent();

	wchar_t * cch = nullptr;
	GetItemText(iItem, iSubItem, cch);

	int nSelItem = -1;
	for (auto str : strList) {
		int nItem = subItemComboBox.AddString(str.c_str());
		if (cch && str == cch) {
			nSelItem = nItem;
		}
	}
	::SysFreeString(cch);
	if (nSelItem != -1) {
		subItemComboBox.SetCurSel(nSelItem);
		return;
	}
	
	cch = nullptr;
	GetItemText(iItem, iSubItem, cch);
	if (!cch) {
		return ;
	}
	subItemComboBox.SetWindowText(cch);
	::SysFreeString(cch);
}

void QListViewCtrl::createOrShowSubItemComboBox(CComboBox & win, CRect & rect, bool allowEdit)
{
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP  | CBS_HASSTRINGS | CBS_AUTOHSCROLL; 
		if (allowEdit) {
			dwStyle = dwStyle | CBS_DROPDOWN;
		} else {
			dwStyle = dwStyle | CBS_DROPDOWNLIST;
		}
		win.Create(m_hWnd, rect, L"", dwStyle , WS_EX_CLIENTEDGE, Config::QLISTVIEWCTRL_SUBITEM_COMBOBOX_ID);
		
		win.BringWindowToTop();
	} else if (::IsWindow(m_hWnd) && win.IsWindow()) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
		win.BringWindowToTop();
	}

	isVisibleComboBox = true;
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


