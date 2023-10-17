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

 * @file   QListViewCtrl.h
 * @brief  Extended ListViewCtrl class , add edit and button for subitem
 * 
 * @author Xuehan Qin
 * @date   2023-06-22
 *********************************************************************/
#pragma once
#include <utility>
#include <string>
#include <atlwin.h>
#include <atlctrlx.h>
#include <atlcrack.h>
#include <atltypes.h>
#include <map>
#include "common/Config.h"
#include "core/entity/Entity.h"

// Compare key by pair, this class use for declare SubItemComboBoxMap/SubItemCheckBoxMap class
struct CompareKeyByPair {
	bool operator()(const std::pair<int, int> & key1, const std::pair<int, int> & key2) const 
	{  
		if (key1.first < key2.first) {
			return true;
		} else if (key1.first == key2.first && key1.second < key2.second) {
			return true;
		}
		return false;
	}  
};
// template params : std::pair<int - iItem, int - iSubItem>, CComboBox *  - combobox pointer
typedef std::map<std::pair<int, int>, CComboBox *, CompareKeyByPair> SubItemComboBoxMap;
typedef std::map<std::pair<int, int>, CButton *, CompareKeyByPair> SubItemCheckBoxMap, SubItemButtonMap;

class QListViewCtrl : public CWindowImpl<QListViewCtrl, CListViewCtrl>
{
public:
	BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_WND_SUPERCLASS(_T("WTL_CheckListViewCtrl"), GetWndClassName())
 
	BEGIN_MSG_MAP_EX(QListViewCtrl)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_NOTIFY(OnNotify)
		MSG_WM_SIZE(OnSize)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MESSAGE_HANDLER(WM_VSCROLL, OnVScroll)
		MESSAGE_HANDLER(WM_HSCROLL, OnHScroll)
		COMMAND_HANDLER_EX(Config::QLISTVIEWCTRL_SUBITEM_EDIT_ID, EN_KILLFOCUS, OnSubItemEditKillFocus)
		COMMAND_RANGE_CODE_HANDLER_EX(Config::QLISTVIEWCTRL_CHECKBOX_ID_START, Config::QLISTVIEWCTRL_CHECKBOX_ID_END, BN_CLICKED, OnClickCheckBox)
		COMMAND_RANGE_CODE_HANDLER_EX(Config::QLISTVIEWCTRL_BUTTON_ID_START, Config::QLISTVIEWCTRL_BUTTON_ID_END, BN_CLICKED, OnClickButton)
		MSG_WM_CTLCOLORSTATIC(OnCtlColorStatic)
		MSG_WM_CTLCOLOREDIT(OnCtlColorEdit)
		MSG_WM_CTLCOLORLISTBOX(OnCtlColorListBox)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
	
	// editor
	void createOrShowEditor(std::pair<int, int> subItemPos);
	void createOrShowEditor(int iItem, int iSubItem);

	// combobox
	void createOrShowComboBox(std::pair<int, int> & subItemPosition, const std::vector<std::wstring> &strList, int nSelItem = 0);
	void createOrShowComboBox(int iItem, int iSubItem, const std::vector<std::wstring> &strList, int nSelItem = 0);
	CComboBox * getComboBoxPtr(int iItem, int iSubItem);
	
	//CheckBox
	void createOrShowCheckBox(int iItem, int iSubItem);
	CButton * getCheckBoxPtr(int iItem, int iSubItem);

	//button
	void createOrShowButton(int iItem, int iSubItem, const std::wstring & text);
	CButton * getButtonPtr(int iItem, int iSubItem);
	

	SubItemValues getChangedVals();
	int getChangedCount();
	SubItemValues getRowChangedVals(int iItem);
	void setChangedVals(SubItemValues & changeVals);
	void setChangeVal(SubItemValue &subItemVal);
	void clearChangeVals();
	void removeChangedValsItems(int iItem);
	void moveUpChangeValsItem(int iItem);
	void moveDownChangeValsItem(int iItem);
	void RemoveItem(int iItem);

	// move up / down
	void moveUpComboBoxes(int iItem);
	void moveDownComboBoxes(int iItem);

	void moveUpCheckBoxes(int iItem);
	void moveDownCheckBoxes(int iItem);

	void moveUpButtons(int iItem);
	void moveDownButtons(int iItem);

	void resetChildElemsRect();
private:
	COLORREF bkgColor = RGB(255, 255, 255);
	HBRUSH bkgBrush = nullptr;
	HFONT textFont = nullptr;	
	HFONT comboFont = nullptr;

	// the row item height
	int itemHeight = 0;

	CEdit subItemEdit;
	std::pair<int, int> subItemPos; // pair.first-iItem, pair.second-iSubItem
	CRect subItemRect;
	bool isVisibleEdit = false;

	SubItemValues changeVals;
	SubItemComboBoxMap subItemComboBoxMap;
	SubItemCheckBoxMap subItemCheckBoxMap;
	SubItemButtonMap subItemButtonMap;
	
	void changeSubItemText();
	void changeComboBoxesRect();
	void changeCheckBoxesRect();
	void changeButtonsRect();

	void removeComboBoxes(int iItem);
	void removeCheckBoxes(int iItem);

	void createOrShowSubItemEdit(CEdit & win, std::wstring & text, CRect & rect);
	void createOrShowSubItemComboBox(CRect & rect, int iItem, int iSubItem, const std::vector<std::wstring> &strList, int nSelItem);
	void createOrShowSubItemCheckBox(CRect & rect, int iItem, int iSubItem);
	void createOrShowSubItemButton(CRect & rect, int iItem, int iSubItem, const std::wstring &text);
	void loadSubItemComboBox(CComboBox * comboBoxPtr, const std::vector<std::wstring> &strList, int nSelItem);
	void pressedTabToMoveEditor();

	LRESULT OnVScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnHScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSubItemEditKillFocus(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickCheckBox(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickButton(UINT uNotifyCode, int nID, HWND hwnd);
		
	void OnDestroy();
	void OnSize(UINT nType, CSize size);
	LRESULT OnNotify(int idCtrl, LPNMHDR pnmh);	
	BOOL OnEraseBkgnd(CDCHandle dc);

	HBRUSH OnCtlColorStatic(HDC hdc, HWND hwnd);
	HBRUSH OnCtlColorEdit(HDC hdc, HWND hwnd);
	HBRUSH OnCtlColorListBox(HDC hdc, HWND hwnd);
};
