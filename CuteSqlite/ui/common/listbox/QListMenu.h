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

 * @file   QListMenu.h
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-12-28
 *********************************************************************/
#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <atlwin.h>
#include <atlcrack.h>
#include <atlctrls.h>
#include <atltypes.h>
#include "utils/GdiPlusUtil.h"
#include "QListMenuItem.h"
#include "common/Config.h"

class QListMenu : public CWindowImpl<QListMenu>
{
public:
	DECLARE_WND_CLASS(NULL)

	BOOL PreTranslateMessage(MSG* pMsg);
	
	BEGIN_MSG_MAP_EX(QListMenu)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		// 接收来自QListMenuItem的自定义消息，某个菜单项被点击了.
		MESSAGE_HANDLER(Config::MSG_MENU_ITEM_CLICK_ID, OnPressedMenuItem)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	~QListMenu();
	// 清理所有的item
	void clearItems();

	// 添加菜单项.
	int addItem(std::wstring text, UINT id);
	// 指定选中项
	void setSelItem(int nItem);
	

	// 获得选中项
	int getSelItem();

	// 获得选中项的ID
	UINT getSelItemID();

	// 获得所有项
	int getItemCount();

	// 完成选中项,打钩
	void completeSelItem();

	// 完成指定ID的菜单项
	void completeItem(UINT nID, bool isComplete);

	// item的高度
	void setItemHeight(int h) { itemHeight = h; }

	// item字体
	void setItemFont(HFONT font) { itemFont = font; }

	// item 右边三角形的颜色
	void setItemTriangleColor(COLORREF color);

	// 背景图片
	void setBkgColor(COLORREF color);

	// 加载数据
	void loadWindow();

private:
	// 菜单的数组
	std::vector<QListMenuItem *> itemVector;
	// 菜单ID的数组
	std::vector<UINT> itemIdVector;

	// 菜单项的高度
	int itemHeight = 50; //默认高度是50

	//文本的字体
	HFONT itemFont ;
	COLORREF bkgColor = RGB(238, 238, 238);
	HBRUSH bkgBrush;
	COLORREF itemTriangleColor = RGB(255, 255, 255);

	// 创建菜单项
	void createOrShowItem(QListMenuItem & win, int nTtem, UINT id, std::wstring text, CRect &clientRect, DWORD exStyle = SS_RIGHT);
	// 计算nItem之前所有item的高度
	int measurePrevItemsHeigh(int nItem);
	int measureAllowTextFont(const wchar_t * text, int initFontSize, int allowWidth);
protected:
	

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	

	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	// 接收来自QListMenuItem的自定义消息，某个菜单项被点击了.
	LRESULT OnPressedMenuItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};

