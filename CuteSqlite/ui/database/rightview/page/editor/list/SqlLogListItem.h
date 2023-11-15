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
		MSG_WM_CTLCOLORSTATIC(OnCtlStaticColor)
		MSG_WM_CTLCOLORBTN(OnCtlBtnColor)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	SqlLogListItem(ResultInfo & info);
	void select(bool state);	
private:
	ResultInfo info;
	bool selectState = false;

	COLORREF bkgColor = RGB(238, 238, 238);
	COLORREF bkgMouseOverColor = RGB(238, 238, 238);
	COLORREF bkgSelectColor = RGB(145, 201, 247);
	COLORREF textColor = RGB(81, 81, 81);
	COLORREF sqlColor = RGB(64, 64, 64);	
	COLORREF btnBkgColor = RGB(210, 210, 210);

	HBRUSH bkgBrush;
	HFONT textFont;
	HFONT sqlFont;

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
	CButton deleteButton;

	void createOrShowUI();
	void createTopElems(CRect & clientRect);
	void createMiddleElems(CRect & clientRect);
	void createBottomElems(CRect & clientRect);

	void loadWindow();

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	int OnDestroy();
	void OnSize(UINT nType, CSize size);
	void OnShowWindow(BOOL bShow, UINT nStatus);
	void OnPaint(CDCHandle dc);
	BOOL OnEraseBkgnd(CDCHandle dc);
	HBRUSH OnCtlStaticColor(HDC hdc, HWND hwnd);
	HBRUSH OnCtlBtnColor(HDC hdc, HWND hwnd);
};
