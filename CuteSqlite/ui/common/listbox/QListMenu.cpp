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

 * @file   QListMenu.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-12-28
 *********************************************************************/
#include "stdafx.h"
#include <atlstr.h>
#include <atlgdi.h>
#include "QListMenu.h"
#include "utils/GdiPlusUtil.h"
#include "utils/Log.h"
#include "utils/FontUtil.h"
#include "core/common/Lang.h"

BOOL QListMenu::PreTranslateMessage(MSG* pMsg)
{
	pMsg;
	return FALSE;
}

/**
 * 添加菜单项.
 * 
 * @param text 菜单文字
 * @param id 窗口ID
 * @return 返回菜单的nItem
 */
int QListMenu::addItem(std::wstring text, UINT id)
{
	CRect clientRect;
	GetClientRect(&clientRect);

	QListMenuItem * menu = new QListMenuItem();	
	menu->setBkgColor(bkgColor);
	menu->setRightTriangleColor(itemTriangleColor); //右边三角形的颜色
	itemVector.push_back(menu);
	itemIdVector.push_back(id);

	int nItem = static_cast<int>(itemVector.size()) - 1;
	// 创建数据
	createOrShowItem(*menu, nItem, id, text, clientRect);
	
	return nItem;
}

/**
 * 默认选中的项.
 * 
 * @param nItem
 */
void QListMenu::setSelItem(int nItem)
{
	ATLASSERT(nItem >= 0 && nItem < itemVector.size());

	auto item = itemVector.at(nItem);
	if (item == nullptr) {
		return ;
	}

	for (int i = 0; i < itemIdVector.size(); i++) {
		auto menuItem = itemVector.at(i);
		if (i == nItem) {
			menuItem->setStatus(QListMenuItem::kStatusPressed);
		}else {
			menuItem->setStatus(QListMenuItem::kStatusNormal);
		}
	}
	loadWindow();
}

/**
 * 获得选中项.
 * 
 * @return 
 */
int QListMenu::getSelItem()
{
	int nSelItem = -1;
	for (int i = 0; i < itemIdVector.size(); i++) {
		auto menuItem = itemVector.at(i);
		if (menuItem->getStatus() == QListMenuItem::kStatusPressed) {
			nSelItem = i;
			break;
		}
	}
	return nSelItem;
}

UINT QListMenu::getSelItemID()
{
	UINT nID = -1;
	for (int i = 0; i < itemIdVector.size(); i++) {
		auto menuItem = itemVector.at(i);
		if (menuItem->getStatus() == QListMenuItem::kStatusPressed) {
			nID = menuItem->getID();
			break;
		}
	}
	return nID;
}

/**
 * 获得菜单统计数.
 * 
 * @return 
 */
int QListMenu::getItemCount()
{
	return static_cast<int>(itemVector.size());
}

// 选中项任务完成，打勾
void QListMenu::completeSelItem()
{
	int nSelItem = getSelItem();
	auto menuItem = itemVector.at(nSelItem);
	CString str;
	menuItem->GetWindowText(str);
	if (str.Find(L"√") != -1) {
		return ;
	}
	std::wstring newText(L"√ ");
	newText.append(str.GetString());
	bool ret = menuItem->SetWindowTextW(newText.c_str());
	Q_DEBUG(L"QListMenu::completeSelItem(), ret:{}", ret);
	CString str2;
	menuItem->GetWindowText(str2);
	Q_DEBUG(L"QListMenu::completeSelItem(), str2:{}", str2);
}

void QListMenu::completeItem(UINT nID, bool isComplete)
{
	size_t nItem = -1;
	for (size_t i = 0; i < itemIdVector.size(); i++) {
		if (nID == itemIdVector.at(i)) {
			nItem = i;
		}
	}
	if (nItem == -1) {
		return ;
	}
	auto menuItem = itemVector.at(nItem);
	CString str;
	menuItem->GetWindowText(str);
	if (str.Find(L"√") != -1 && isComplete) { //如果完成打勾了，就不管了
		return ;
	} else if(str.Find(L"√") != -1 && !isComplete) {
		str.Replace(L"√ ", L"");
		menuItem->SetWindowText(str);
	} else if (isComplete) {
		str = L"√ " + str;
		menuItem->SetWindowText(str);
	}	
}

void QListMenu::setItemTriangleColor(COLORREF color)
{
	itemTriangleColor = color;
}

void QListMenu::setBkgColor(COLORREF color)
{
	bkgColor = color;
}

void QListMenu::loadWindow()
{
	CRect clientRect;
	GetClientRect(&clientRect);

	int nItem = 0;
	for (auto item : itemVector) {
		CString str;
		item->GetWindowText(str); 
		DWORD style = item->GetStyle();
		createOrShowItem(*item, nItem, itemIdVector.at(nItem), str.GetString(), clientRect);
		nItem++;
	}
}

/**
 * 创建或者显示.
 * 
 * @param win
 * @param nTtem
 * @param id
 * @param text
 * @param clientRect
 * @param exStyle
 */
void QListMenu::createOrShowItem(QListMenuItem & win, int nItem, UINT id,  std::wstring text, CRect &clientRect, DWORD exStyle)
{
	int x = 0, y = measurePrevItemsHeigh(nItem), w = clientRect.Width(), h = itemHeight;
	CRect rect(x, y, x + w, y + h);
	int normalFontSize = Lang::fontSize(L"list-menu-item-normal-size");
	int pressedFontSize = Lang::fontSize(L"list-menu-item-pressed-size"); 
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_VISIBLE  | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | SS_NOTIFY | SS_OWNERDRAW; // SS_NOTIFY - 表示static接受点击事件, SS_OWNERDRAW表示可以自绘
		if (exStyle) {
			dwStyle = dwStyle | exStyle;
		}
		HWND  hwnd = win.Create(m_hWnd, rect, text.c_str(), dwStyle, 0 , id);	
		if (::IsWindow(hwnd)) {
			win.SetFont(itemFont);
			win.setFontColors(RGB(100, 100, 100), RGB(119, 182, 219), RGB(100, 100, 100));
			win.setBkgColors(RGB(238, 238, 238), RGB(238, 238, 238), RGB(238, 238, 238));
			win.setTextFontSize(normalFontSize);
			win.SetWindowText(text.c_str());
		}
		
	} else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		if (win.getStatus() == QListMenuItem::kStatusPressed) { // 选中项
			win.setTextColor(RGB(119, 182, 219));
			win.setBkgColor(RGB(238, 238, 238));
			// 计算允许的大小
			int allowSize = measureAllowTextFont(text.c_str(), pressedFontSize, clientRect.Width() - 30);
			win.setTextFontSize(allowSize);
		} else { // 非选中项
			win.setTextColor(RGB(100, 100, 100));
			win.setBkgColor(RGB(238, 238, 238));
			win.setTextFontSize(normalFontSize);
		}
		win.MoveWindow(&rect);
		win.SetWindowText(text.c_str());
		win.Invalidate(false);
	}	
}

/**
 * 计算nItem之前所有item的高度.
 * 
 * @param nItem
 * @return 
 */
int QListMenu::measurePrevItemsHeigh(int nItem)
{
	int measureHeight = 0;
	for (int i = 0; i < nItem; i++) {
		auto item = itemVector.at(i);
		if (item->IsWindow()) {
			CRect itemRect;
			item->GetWindowRect(itemRect);
			measureHeight += itemRect.Height();
		}
	}

	return measureHeight;
}

/**
 * 循环计算文字的宽度.
 * 
 * @param text
 * @param initFontSize
 * @param rectWidth 
 * @return 
 */
int QListMenu::measureAllowTextFont(const wchar_t * text, int initFontSize, int allowMaxWidth)
{
	ATLASSERT(text != nullptr && wcslen(text) > 0); 
	for (int i = initFontSize; i > 1; i--) {
		std::wstring fontName = Lang::fontName().c_str();
		HFONT font = FontUtil::getFont(i, true, fontName.c_str());
		CClientDC dc(NULL);
		HFONT oldFont = dc.SelectFont(font);
		SIZE size;
		dc.GetTextExtent(text, static_cast<int>(wcslen(text)), &size);
		dc.SelectFont(oldFont);
		::DeleteObject(font);

		// 如果字体不超过允许的最大宽度
		if (size.cx <= allowMaxWidth) {
			return i;
		}
	}
	return initFontSize;
}

LRESULT QListMenu::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	itemFont = FT(L"list-menu-item-size");
	bkgBrush = ::CreateSolidBrush(bkgColor);
	itemHeight = 30;
	return TRUE;
}

LRESULT QListMenu::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	clearItems();

	DeleteObject(itemFont);
	DeleteObject(bkgBrush);
	return 0;
}

/**
 * 清理所有项.
 * 
 */
void QListMenu::clearItems()
{
	int nItem = 0;
	for (auto item : itemVector) {
		if (item->IsWindow()) {
			item->DestroyWindow();
		}
		item = nullptr;
	}
	itemVector.clear();
	itemIdVector.clear();
}

LRESULT QListMenu::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	loadWindow();
	return 0;
}

LRESULT QListMenu::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC dc(m_hWnd);

	//全局的颜色
	dc.FillRect(&dc.m_ps.rcPaint, bkgBrush);
	return 0;
}

/**
 * 接收来自QListMenuItem的自定义消息，某个菜单项被点击了.
 * 
 * @param 
 * @param wParam 窗口ID
 * @param 
 * @param 
 * @return 
 */
LRESULT QListMenu::OnPressedMenuItem(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	UINT id = static_cast<UINT>(wParam);
	int nSelItem = 0;
	
	for (int nItem = 0; nItem < itemIdVector.size(); nItem++) {
		auto itemId = itemIdVector.at(nItem);
		auto item = itemVector.at(nItem);
		if (itemId == id) {			
			item->setStatus(QListMenuItem::kStatusPressed); // 更改点击状态
			nSelItem = nItem;
		} else {
			item->setStatus(QListMenuItem::kStatusNormal); // 更改点击状态
		}
	}

	//刷新界面
	loadWindow();

	//使用路径1：QListMenu->AnalysisMenuBar->AnalysisPanel->HomeView->MainFrm ，这里取上一级的
	::PostMessage(GetParent().m_hWnd, Config::MSG_MENU_ITEM_CLICK_ID, (WPARAM)nSelItem, (LPARAM)id);
	return 0;
}

