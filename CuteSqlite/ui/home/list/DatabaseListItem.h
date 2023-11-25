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

 * @file   DatabaseListItem.h
 * @brief  Show database list in HomePanel 
 * 
 * @author Xuehan Qin
 * @date   2023-11-25
 *********************************************************************/
#pragma once
#include <atlwin.h>
#include <atlcrack.h>
#include <atltypes.h>
#include <atlgdi.h>
#include "core/entity/Entity.h"
#include "ui/common/image/QStaticImage.h"

#define DATABASE_LIST_ITEM_WIDTH 240
#define DATABASE_LIST_ITEM_HEIGHT 100
class DatabaseListItem : public CWindowImpl<DatabaseListItem>
{
public:
	BEGIN_MSG_MAP_EX(DatabaseListItem)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_SIZE(OnSize)
		MSG_WM_SHOWWINDOW(OnShowWindow)
		MSG_WM_PAINT(OnPaint)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_MOUSEHOVER(OnMouseHover)
		MSG_WM_MOUSELEAVE(OnMouseLeave)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_CTLCOLORSTATIC(OnCtlStaticColor)
		COMMAND_HANDLER_EX(Config::DBLIST_ITEM_PATH_EDIT_ID, EN_SETFOCUS, OnClickPathEdit)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()
	DatabaseListItem(UserDb & _userDb);
	uint64_t getUserDbId();
private:
	UserDb userDb;

	bool isHover = false;
	bool isTracking = false;
	COLORREF bkgColor = RGB(213, 213, 213);
	COLORREF bkgHoverColor = RGB(180, 180, 180);
	COLORREF textColor = RGB(80, 80, 80);
	CBrush bkgBrush;
	CBrush bkgHoverBrush;
	HFONT nameFont = nullptr;
	HFONT textFont = nullptr;

	QStaticImage storeImage;
	CEdit pathEdit;
	void createOrShowUI();
	void createOrShowStoreImage(QStaticImage & win, CRect &clientRect);
	void createOrShowPathEdit(CRect &clientRect);
	void createOrShowEdit(WTL::CEdit & win, UINT id, std::wstring text, CRect rect, CRect &clientRect, DWORD exStyle);
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	int OnDestroy();
	void OnSize(UINT nType, CSize size);
	void OnShowWindow(BOOL bShow, UINT nStatus);
	void OnPaint(CDCHandle dc);
	BOOL OnEraseBkgnd(CDCHandle dc);
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnMouseHover(WPARAM wParam, CPoint ptPos);
	void OnMouseLeave();
	void OnLButtonDown(UINT nFlags, CPoint point);
	HBRUSH OnCtlStaticColor(HDC hdc, HWND hwnd);
	void OnClickPathEdit(UINT uNotifyCode, int nID, CWindow wndCtl);
};
