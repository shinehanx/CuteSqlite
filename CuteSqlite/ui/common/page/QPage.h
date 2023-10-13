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

* @file   QPage.h
* @brief  Abstract page class, inherits its subclass and is embedded in the TabView page
*		   Follow the view-handler design pattern,
*			handler: RightWorkView.
*			view: QPage and it's subclass in the page folder.
*			provider: RightWorkProvider
*
* @author Xuehan Qin
* @date   2023-05-22
*********************************************************************/
#pragma once
#include <atlwin.h>
#include <atlcrack.h>
#include <atltypes.h>

#define PAGE_TOPBAR_HEIGHT 28
#define PAGE_BUTTON_WIDTH 16
#define PAGE_BUTTON_HEIGHT 16

class QPage : public CWindowImpl<QPage>
{
public:
	BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP_EX(QPage)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_SIZE(OnSize)
		MSG_WM_SHOWWINDOW(OnShowWindow)
		MSG_WM_PAINT(OnPaint)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

protected:
	bool isNeedReload = true;
	COLORREF bkgColor = RGB(255, 255, 255);
	HBRUSH bkgBrush = nullptr;

	COLORREF topbarColor = RGB(238, 238, 238);
	HBRUSH topbarBrush = nullptr;

	CRect getTopRect(CRect & clientRect);
	CRect getPageRect(CRect & clientRect);

	virtual void createOrShowUI();
	virtual void loadWindow();
	// The subclass implements this method to extend the screen content
	virtual void paintItem(CDC & dc, CRect & paintRect) {};

	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual int OnDestroy();
	virtual void OnSize(UINT nType, CSize size);
	virtual void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual void OnPaint(CDCHandle dc);
	BOOL OnEraseBkgnd(CDCHandle dc);
};



