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

 * @file   SettingMenuBar.h
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-12-28
 *********************************************************************/
#pragma once
#include "ui/common/listbox/QListMenu.h"

class SettingMenuBar : public CWindowImpl<SettingMenuBar>
{
public:
	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP_EX(SettingMenuBar)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(Config::MSG_MENU_ITEM_CLICK_ID, OnClickMenuItem) // 响应子窗口QListMenu analysisMenu发来的消息：菜单下的项被点击了
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	void setup();
	void createOrShowUI();
	void loadWindow();

	 // 选中项
	int getSelItem();
	void setSelItem(int nItem);
private:
	COLORREF bkgColor = RGB(240, 240, 240);
	HBRUSH bkgBrush = nullptr;
	QListMenu listMenu;
	

	// 显示分析菜单
	void createOrShowListMenu(QListMenu & win, CRect &clientRect);
	void createOrShowListMenuItem(QListMenu &win);

	void loadListMenu(); 

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClickMenuItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};
