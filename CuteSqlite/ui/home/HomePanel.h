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

 * @file   HomePanel.h
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-05-19
 *********************************************************************/
#pragma once
#include <string>
#include <atltypes.h>
#include <atlcrack.h>
#include <GdiPlus.h>
#include "common/Config.h"
#include "core/service/system/SettingService.h"

class HomePanel :public CWindowImpl<HomePanel>
{
public:
	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP_EX(HomePanel)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SIZE, OnSize)		
		MESSAGE_HANDLER(WM_SHOWWINDOW, OnShowWindow)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
	END_MSG_MAP()

public:
	const Config::PanelId panelId = Config::HOME_PANEL;
	COLORREF bkgColor = RGB(0x2c, 0x2c, 0x2c);
	CBrush bkgBrush;
	COLORREF titleColor = RGB(255, 255, 255);
	COLORREF homeColor = RGB(230, 230, 230);
	HFONT titleFont = nullptr;
	HFONT homeFont = nullptr;
	std::wstring bkgImagePath;
	CBitmap bkgBitmap = nullptr;

	SettingService * settingService = SettingService::getInstance();

	void createOrShowUI();

	virtual LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	virtual LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	virtual LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	virtual LRESULT OnShowWindow(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	virtual LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	BOOL OnEraseBkgnd(CDCHandle dc);
};