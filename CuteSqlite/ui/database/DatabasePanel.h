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

 * @file   DatabasePanel.h
 * @brief  Database Panel for HomeView
 * 
 * @author Xuehan Qin
 * @date   2023-05-19
 *********************************************************************/
#pragma once
#include <string>
#include <atltypes.h>
#include <atlctrlx.h>
#include <atlcrack.h>
#include <atlsplit.h>
#include <GdiPlus.h>
#include "common/Config.h"
#include "core/service/system/SettingService.h"
#include "ui/database/leftview/LeftTreeView.h"
#include "ui/database/rightview/RightWorkView.h"

class DatabasePanel :public CWindowImpl<DatabasePanel>
{
public:
	BOOL PreTranslateMessage(MSG* pMsg);
	~DatabasePanel();
	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP_EX(DatabasePanel)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SIZE, OnSize)		
		MESSAGE_HANDLER(WM_SHOWWINDOW, OnShowWindow)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
	END_MSG_MAP()

	const Config::PanelId panelId = Config::DATABASE_PANEL;

private:	
	bool isNeededReload = true;
	CSplitterWindow splitter;
	LeftTreeView leftTreeView;
	RightWorkView rightWorkView;

	SettingService * settingService = SettingService::getInstance();
	
	void createOrShowUI();
	void createOrShowSplitter(CSplitterWindow & win , CRect & clientRect);
	void createOrShowLeftTreeView(LeftTreeView & win, CRect & clientRect);
	void createOrShowRightWorkView(RightWorkView & win, CRect & clientRect);

	virtual LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	virtual LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	virtual LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	virtual LRESULT OnShowWindow(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	virtual LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	BOOL OnEraseBkgnd(CDCHandle dc);
};