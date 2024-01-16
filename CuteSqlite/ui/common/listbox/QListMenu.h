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
		// ��������QListMenuItem���Զ�����Ϣ��ĳ���˵�������.
		MESSAGE_HANDLER(Config::MSG_MENU_ITEM_CLICK_ID, OnPressedMenuItem)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	~QListMenu();
	// �������е�item
	void clearItems();

	// ��Ӳ˵���.
	int addItem(std::wstring text, UINT id);
	// ָ��ѡ����
	void setSelItem(int nItem);
	

	// ���ѡ����
	int getSelItem();

	// ���ѡ�����ID
	UINT getSelItemID();

	// ���������
	int getItemCount();

	// ���ѡ����,��
	void completeSelItem();

	// ���ָ��ID�Ĳ˵���
	void completeItem(UINT nID, bool isComplete);

	// item�ĸ߶�
	void setItemHeight(int h) { itemHeight = h; }

	// item����
	void setItemFont(HFONT font) { itemFont = font; }

	// item �ұ������ε���ɫ
	void setItemTriangleColor(COLORREF color);

	// ����ͼƬ
	void setBkgColor(COLORREF color);

	// ��������
	void loadWindow();

private:
	// �˵�������
	std::vector<QListMenuItem *> itemVector;
	// �˵�ID������
	std::vector<UINT> itemIdVector;

	// �˵���ĸ߶�
	int itemHeight = 50; //Ĭ�ϸ߶���50

	//�ı�������
	HFONT itemFont ;
	COLORREF bkgColor = RGB(238, 238, 238);
	HBRUSH bkgBrush;
	COLORREF itemTriangleColor = RGB(255, 255, 255);

	// �����˵���
	void createOrShowItem(QListMenuItem & win, int nTtem, UINT id, std::wstring text, CRect &clientRect, DWORD exStyle = SS_RIGHT);
	// ����nItem֮ǰ����item�ĸ߶�
	int measurePrevItemsHeigh(int nItem);
	int measureAllowTextFont(const wchar_t * text, int initFontSize, int allowWidth);
protected:
	

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	

	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	// ��������QListMenuItem���Զ�����Ϣ��ĳ���˵�������.
	LRESULT OnPressedMenuItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
};

