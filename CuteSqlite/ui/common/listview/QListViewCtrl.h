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
#include <map>
#include <atlwin.h>
#include <atlcrack.h> 
#include <atltypes.h>
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
// template params : std::pair<int - iItem, int - iSubItem>,std::wstring  - combobox selected string
typedef std::map<std::pair<int, int>, std::wstring, CompareKeyByPair> SubItemComboBoxMap, SubItemButtonMap;

// template params : std::pair<int - iItem, int - iSubItem>, int  - Checkbox checked
typedef std::map<std::pair<int, int>, int, CompareKeyByPair> SubItemCheckBoxMap;

class QListViewCtrl : public CWindowImpl<QListViewCtrl, CListViewCtrl>,
	public COwnerDraw<QListViewCtrl>
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
		COMMAND_HANDLER_EX(Config::QLISTVIEWCTRL_SUBITEM_EDIT_ID, EN_CHANGE, OnSubItemEditChange)
		COMMAND_HANDLER_EX(Config::QLISTVIEWCTRL_SUBITEM_COMBOBOX_ID, CBN_KILLFOCUS, OnSubItemComboBoxKillFocus)
		COMMAND_HANDLER_EX(Config::QLISTVIEWCTRL_SUBITEM_COMBOBOX_ID, CBN_EDITCHANGE, OnSubItemComboBoxEditChange)
		COMMAND_HANDLER_EX(Config::QLISTVIEWCTRL_SUBITEM_COMBOBOX_ID, CBN_SELCHANGE, OnSubItemComboBoxSelChange)
		MSG_WM_CTLCOLORSTATIC(OnCtlColorStatic)
		MSG_WM_CTLCOLOREDIT(OnCtlColorEdit)
		MSG_WM_CTLCOLORLISTBOX(OnCtlColorListBox)
		CHAIN_MSG_MAP_ALT(COwnerDraw<QListViewCtrl>, 1)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()
	void enabledCheckBox(bool val) { hasCheckBox = val; };

	// editor
	void createOrShowEditor(std::pair<int, int> subItemPos);
	void createOrShowEditor(int iItem, int iSubItem);

	// combobox
	void createComboBox(int iItem, int iSubItem, const std::wstring & val = std::wstring(L""));
	void showComboBox(int iItem, int iSubItem, const std::vector<std::wstring> & strList, bool allowEdit = false);
	std::wstring getComboBoxString(int iItem, int iSubItem);
	
	//CheckBox
	void createCheckBox(int iItem, int iSubItem, int isChecked);
	void setCheckBoxIsChecked(int iItem, int iSubItem, int isChecked);
	int getCheckBoxIsChecked(int iItem, int iSubItem);

	//button
	void createButton(int iItem, int iSubItem, const std::wstring & text);
	

	const SubItemValues & getChangedVals() ;
	int getChangedCount();
	SubItemValues getRowChangedVals(int iItem);
	void setChangedVals(SubItemValues & changeVals);
	void setChangeVal(SubItemValue &subItemVal);
	void clearChangeVals();
	void cancelChangedVal(const SubItemValue &subItemVal);

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

	void setItemHeight(int height);
	void activeSubItem(int iItem, int iSubItem);

	void changeAllItemsCheckState();

	void destroySubItemElems();

	// owner draw, must set the ListView style with LVS_OWNERDRAWFIXED
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	int CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);
	void DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct);
	BOOL SelectItem(int nIndex);
private:
	COLORREF bkgColor = RGB(255, 255, 255);
	COLORREF borderColor = RGB(220, 220, 220);
	COLORREF btnBorderColor = RGB(160, 160, 160);
	COLORREF selSubItemBorderColor = RGB(59, 125, 187);
	COLORREF chkBorderColor = RGB(0, 0, 0);
	COLORREF chkColor = RGB(0, 0, 0);
	COLORREF btnDownColor = RGB(0, 0, 0);
	COLORREF btnColor = RGB(238, 238, 238);
	COLORREF nullTextColor = RGB(128, 128, 128);
	COLORREF doubleLineColor = RGB(245, 245, 245);
	CBrush bkgBrush;
	CBrush btnBrush;	
	CBrush btnDownBrush;
	CBrush chkBrush;
	CBrush selSubItemBorderBrush;
	CBrush doubleLineBrush;
	HFONT textFont = nullptr;
	HFONT comboFont = nullptr;
	HFONT normalFont = nullptr;
	CPen borderPen;
	CPen btnBorderPen;
	CPen chkBorderPen;
	CPen selSubItemBorderPen;

	bool hasCheckBox = true;

	// the row item height
	int itemHeight = 21;

	// horizontal offset and size
	int hOffset = 0;
	int hSize = 0;
	int leftMargin = 2;

	CEdit subItemEdit;
	CComboBox subItemComboBox;
	std::pair<int, int> oldSubItemPos, activeSubItemPos; // active subitem position, pair.first - iItem, pair.second - iSubItem
	CRect oldSubItemRect, activeSubItemRect; 
	bool isVisibleEdit = false;
	bool isVisibleComboBox = false;

	SubItemValues changeVals;
	SubItemComboBoxMap subItemComboBoxMap;
	SubItemCheckBoxMap subItemCheckBoxMap;
	SubItemButtonMap subItemButtonMap;

	CBitmap checkYesBitmap;
	CBitmap checkNoBitmap;
	CImageList imageList;
	
	void changeSubItemText();

	void removeChangedValsItems(int iItem);
	void removeComboBoxes(int iItem);
	void removeCheckBoxes(int iItem);

	void createOrShowSubItemEdit(CEdit & win, std::wstring & text, CRect & rect);
	void pressedTabToMoveEditor();
	void pressedShiftTabToMoveEditor();
	void pressedSpaceToCheckbox();
	bool pressedUpToMoveEditor();
	bool pressedDownToMoveEditor();

	LRESULT OnVScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnHScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSubItemEditKillFocus(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnSubItemEditChange(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnSubItemComboBoxKillFocus(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnSubItemComboBoxEditChange(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnSubItemComboBoxSelChange(UINT uNotifyCode, int nID, HWND hwnd);
		
	void OnDestroy();
	void OnSize(UINT nType, CSize size);

	void createImageList();
	void destroyComboBox();
	void destroyEditor();
	void checkedAllItems();

	LRESULT OnNotify(int idCtrl, LPNMHDR pnmh);
	BOOL OnEraseBkgnd(CDCHandle dc);

	HBRUSH OnCtlColorStatic(HDC hdc, HWND hwnd);
	HBRUSH OnCtlColorEdit(HDC hdc, HWND hwnd);
	HBRUSH OnCtlColorListBox(HDC hdc, HWND hwnd);

	// draw every subItems, must set the ListView style with LVS_OWNERDRAWFIXED
	void drawSubItems(CDC & memDc, LPDRAWITEMSTRUCT lpDrawItemStruct);
	void drawFirstSubItem(CDC &mdc, int iItem, bool checked, CRect &rcSubItem, CRect &rctext);
	void drawTextInSubItem(CDC & mdc, int iItem, int iSubItem, CRect rcText, std::wstring & text = std::wstring());
	void drawComboBoxInSubItem(CDC & mdc, LPDRAWITEMSTRUCT lpDrawItemStruct, SubItemComboBoxMap::iterator & comboBoxIter, CRect &rcText);
	void drawCheckBoxInSubItem(CDC & mdc, LPDRAWITEMSTRUCT lpDrawItemStruct, SubItemCheckBoxMap::iterator & checkBoxIter, CRect & rcText);
	void drawButtonInSubItem(CDC & mdc, LPDRAWITEMSTRUCT lpDrawItemStruct, SubItemButtonMap::iterator & buttonIter, CRect & rcText);
	
	void createOrShowComboBox(int iItem, int iSubItem, const std::vector<std::wstring> & strList, bool allowEdit = false);	
	void createOrShowSubItemComboBox(CComboBox & win, CRect & rect, bool allowEdit = false);
	
};
