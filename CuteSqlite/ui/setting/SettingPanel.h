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

 * @file   SettingPanel.h
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-12-28
 *********************************************************************/
#pragma once
#include <atlwin.h>
#include <unordered_map>
#include "SettingMenuBar.h"
#include "ui/common/view/QSettingView.h"
#include "ui/setting/view/GeneralSettingsView.h"
#include "ui/setting/view/AboutView.h"

class SettingPanel :public CWindowImpl<SettingPanel>
{
public:
	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP(SettingPanel)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_SHOWWINDOW, OnShowWindow)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		// 响应子窗口QListMenu发来的消息：菜单下的项被点击了
		MESSAGE_HANDLER(Config::MSG_MENU_ITEM_CLICK_ID, OnClickMenuItem) 
		REFLECT_NOTIFICATIONS() //这一句必须包含进来，不然子控件的事件不生效
	END_MSG_MAP()
	~SettingPanel();
	void setup();
	void createOrShowUI();
	void loadWindow();

private:
	COLORREF leftColor = RGB(240, 240, 240);
	HBRUSH leftBrush = nullptr;
	COLORREF rightColor = RGB(118, 118, 118);
	HBRUSH rightBrush = nullptr;

	SettingMenuBar menuBar;
	std::unordered_map<int, QSettingView *> settingViews; // <nItem, settingView point>
	std::unordered_map<int, UINT> settingViewIds; // <nItem, settingView's id>

	GeneralSettingsView generalSettingsView;
	AboutView aboutView;

	// 左边侧栏
	void createOrShowMenuBar(SettingMenuBar &win, CRect &clientRect);
	// 右边的视图
	void createOrShowViews(CRect &clientRect);
	void createOrShowView(QSettingView * win, UINT id, CRect rect, CRect &clientRect, DWORD exStyle=0);

	CRect getLeftRect(CRect & clientRect);
	CRect getRightRect(CRect & clientRect);
	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClickMenuItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	BOOL OnEraseBkgnd(CDCHandle dc);

};