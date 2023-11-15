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

 * @file   SqlLogList.h
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-11-14
 *********************************************************************/
#pragma once
#include <atlwin.h>
#include "ui/database/rightview/page/editor/list/SqlLogListItem.h"

class SqlLogListBox : public CWindowImpl<SqlLogListBox> 
{
public:
	BEGIN_MSG_MAP_EX(SqlLogListBox)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_SIZE(OnSize)
		MSG_WM_SHOWWINDOW(OnShowWindow)
		MSG_WM_PAINT(OnPaint)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MESSAGE_HANDLER(WM_VSCROLL, OnVScroll)
		MESSAGE_HANDLER(WM_MOUSEWHEEL, OnMouseWheel)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()
	SqlLogListBox();
	void addGroup(const std::wstring & group);
	void addItem(ResultInfo & info);
	void selectItem(int nItem);
	void removeItem(int nItem);
private:
	COLORREF bkgColor = RGB(255, 255, 255);
	HBRUSH bkgBrush;

	std::vector<CStatic *> groups;
	std::vector<SqlLogListItem *> items;

	// scroll bar
	TEXTMETRIC tm;
	SCROLLINFO si;
	int nHeightSum = 0;
	static int cxChar, cyChar, iVscrollPos, vScrollPages;
	void initScrollBar(CSize & clientSize);
	void calcHeightSum();

	void createOrShowUI();
	void createOrShowGroupLabel(CStatic & win, std::wstring text, CRect & rect, CRect & clientRect);
	void createOrShowItem(SqlLogListItem & win, CRect & rect, CRect & clientRect);
	
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	int OnDestroy();
	void OnSize(UINT nType, CSize size);
	void OnShowWindow(BOOL bShow, UINT nStatus);
	void OnPaint(CDCHandle dc);
	BOOL OnEraseBkgnd(CDCHandle dc);

	LRESULT OnVScroll(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};
