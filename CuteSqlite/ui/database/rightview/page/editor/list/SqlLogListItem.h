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

 * @file   SqlLogListItem.h
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-11-15
 *********************************************************************/
#pragma once
#include <atlwin.h>
#include <atlctrls.h>
#include <atlcrack.h>
#include "core/entity/Entity.h"
#include "ui/common/image/QStaticImage.h"
#include "ui/common/button/QImageButton.h"
#include "ui/database/rightview/page/supplier/QueryPageSupplier.h"

class  SqlLogListItem : public CWindowImpl<SqlLogListItem>
{
public:
	BEGIN_MSG_MAP_EX(SqlLogListItem)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_SIZE(OnSize)
		MSG_WM_SHOWWINDOW(OnShowWindow)
		MSG_WM_PAINT(OnPaint)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_MOUSEHOVER(OnMouseHover)
		MSG_WM_MOUSELEAVE(OnMouseLeave)
		MSG_WM_CTLCOLORSTATIC(OnCtlStaticColor)
		MSG_WM_CTLCOLORBTN(OnCtlBtnColor)
		COMMAND_HANDLER_EX(Config::SQL_LOG_ITEM_USE_BUTTON_ID, BN_CLICKED, OnClickUseButton)
		COMMAND_HANDLER_EX(Config::SQL_LOG_ITEM_EXPLAIN_BUTTON_ID, BN_CLICKED, OnClickExplainButton)
		COMMAND_HANDLER_EX(Config::SQL_LOG_ITEM_COPY_BUTTON_ID, BN_CLICKED, OnClickCopyButton)
		COMMAND_HANDLER_EX(Config::SQL_LOG_ITEM_TOP_BUTTON_ID, BN_CLICKED, OnClickTopButton)
		COMMAND_HANDLER_EX(Config::SQL_LOG_ITEM_DELELE_BUTTON_ID, BN_CLICKED, OnClickDeleteButton)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	SqlLogListItem(ResultInfo & info, QueryPageSupplier * supplier);
	void select(bool state);	
	const ResultInfo & getInfo() { return info; }
private:
	ResultInfo info;
	bool selectState = false;
	bool bTracking = false;

	COLORREF bkgColor = RGB(238, 238, 238);
	COLORREF bkgMouseOverColor = RGB(238, 238, 238);
	COLORREF bkgSelectColor = RGB(145, 201, 247);
	COLORREF textColor = RGB(81, 81, 81);
	COLORREF sqlColor = RGB(64, 64, 64);
	COLORREF btnBkgColor = RGB(210, 210, 210);

	CBrush bkgBrush;
	HFONT textFont = nullptr;
	HFONT sqlFont = nullptr;

	// top elems
	CStatic createdAtLabel;
	QStaticImage topImage;

	// middle elems
	CEdit sqlEdit;
	QStaticImage stateImage;
	CStatic resultMsgLabel;
	CStatic execTimeLabel;
	
	//bottom elem
	CButton useButton;
	CButton topButton;
	CButton explainButton;
	CButton copyButton;
	CButton deleteButton;

	QueryPageSupplier * supplier = nullptr;

	void createOrShowUI();
	void createTopElems(CRect & clientRect);
	void createMiddleElems(CRect & clientRect);
	void createBottomElems(CRect & clientRect);

	void createOrShowButton(HWND hwnd, CButton & win, UINT id, std::wstring text, CRect rect, CRect &clientRect, DWORD exStyle = 0);
	void loadWindow();

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	int OnDestroy();
	void OnSize(UINT nType, CSize size);
	void OnShowWindow(BOOL bShow, UINT nStatus);
	void OnPaint(CDCHandle dc);
	BOOL OnEraseBkgnd(CDCHandle dc);
	HBRUSH OnCtlStaticColor(HDC hdc, HWND hwnd);
	HBRUSH OnCtlBtnColor(HDC hdc, HWND hwnd);
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnMouseHover(WPARAM wParam, CPoint ptPos);
	void OnMouseLeave();
	void OnClickUseButton(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickCopyButton(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickExplainButton(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickTopButton(UINT uNotifyCode, int nID, HWND hwnd);
	void OnClickDeleteButton(UINT uNotifyCode, int nID, HWND hwnd);
};
