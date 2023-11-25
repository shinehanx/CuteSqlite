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
#include "ui/common/button/QImageButton.h"
#include "ui/home/list/DatabaseListItem.h"
#include "core/service/db/DatabaseService.h"

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
		COMMAND_HANDLER_EX(Config::HOME_CREATE_DB_BUTTON_ID, BN_CLICKED, OnClickCreateDatabaseButton)
		COMMAND_HANDLER_EX(Config::HOME_MOD_DB_BUTTON_ID, BN_CLICKED, OnClickModDatabaseButton)
		MESSAGE_HANDLER_EX(Config::MSG_DBLIST_ITEM_CLICK_ID, OnClickDbListItem)
		REFLECT_NOTIFICATIONS() // must declare this, otherwise the QImageButton buttons is not calling the DrawItem
	END_MSG_MAP()

public:
	const Config::PanelId panelId = Config::HOME_PANEL;
	bool isNeedReload = true;

	COLORREF bkgColor = RGB(42, 42, 42);
	CBrush bkgBrush;
	COLORREF titleColor = RGB(255, 255, 255);
	COLORREF homeColor = RGB(200, 200, 200);
	HFONT titleFont = nullptr;
	HFONT homeFont = nullptr;
	HFONT sectionFont = nullptr;
	
	std::wstring bkgImagePath;
	CBitmap bkgBitmap = nullptr;

	CRect dbSectionRect;

	UserDbList userDbList;
	std::vector<DatabaseListItem *> dbListItemPtrs;

	QImageButton createDababaseButton;
	QImageButton modDatabaseButton;

	SettingService * settingService = SettingService::getInstance();
	DatabaseService * databaseService = DatabaseService::getInstance();

	void createOrShowUI();
	void createOrShowDbButtons(CRect & clientRect);
	void createOrShowUserDbList(CRect & clientRect);
	void createOrShowListItem(DatabaseListItem & win, UINT id, CRect & rect, CRect & clientRect);

	void loadWindow();
	void loadBkgImage();
	void clearDbListItemPtrs();

	virtual LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	virtual LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	virtual LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	virtual LRESULT OnShowWindow(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	virtual LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	BOOL OnEraseBkgnd(CDCHandle dc);

	//buttons
	LRESULT OnClickCreateDatabaseButton(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnClickModDatabaseButton(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnClickDbListItem(UINT uMsg, WPARAM wParam, LPARAM lParam);
		
	void drawHomeText(CMemoryDC &mdc, CRect &clientRect);
	void drawDbSection(CMemoryDC &mdc, CRect &clientRect);
	void getDbSectionRect(CRect &clientRect, std::wstring &sectionText);
};