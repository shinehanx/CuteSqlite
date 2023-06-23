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
#include "common/Config.h"


class QListViewCtrl : public CWindowImpl<QListViewCtrl,CListViewCtrl>
{
public:
	BOOL PreTranslateMessage(MSG* pMsg);

	typedef struct {
		std::pair<int, int> subItemPos;
		std::wstring origVal;
		std::wstring newVal;
	} SubItemValue;
	typedef std::vector<SubItemValue> SubItemValues;

	DECLARE_WND_SUPERCLASS(_T("WTL_SortListViewCtrl"), GetWndClassName())
 
	BEGIN_MSG_MAP_EX(QListViewCtrl)
		MSG_WM_NOTIFY(OnNotify)
		MESSAGE_HANDLER(WM_VSCROLL, OnVScroll)
		MESSAGE_HANDLER(WM_HSCROLL, OnHScroll)
		COMMAND_HANDLER_EX(Config::QLISTVIEWCTRL_SUBITEM_EDIT_ID, EN_KILLFOCUS, OnSubItemEditKillFocus);
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	void createOrShowEdits(std::pair<int, int> subItemPos, CRect & clientRect);
	SubItemValues getChangedVals();
	void clearChangeVals();
private:
	CEdit subItemEdit;
	std::pair<int, int> subItemPos; // pair.first-iItem, pair.second-iSubItem
	CRect subItemRect;
	bool isVisibleEdit = false;

	SubItemValues changeVals;
	SCROLLINFO si;
	
	void changeSubItemText();
	void createOrShowSubItemEdit(CEdit & win, std::wstring & text, CRect & rect);

	LRESULT OnVScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnHScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSubItemEditKillFocus(UINT uNotifyCode, int nID, HWND hwnd);

	LRESULT OnNotify(int idCtrl, LPNMHDR pnmh);
};
