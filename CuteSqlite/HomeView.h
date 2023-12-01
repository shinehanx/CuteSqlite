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

 * @file   HomeView.h
 * @brief  HomeView is a handler that manage panels such as HomePanel/DatabasePanel...
 * 
 * @author Xuehan Qin
 * @date   2023-05-19
 *********************************************************************/
#pragma once
#include "ui/home/LeftPanel.h"
#include "ui/home/HomePanel.h"
#include "ui/database/DatabasePanel.h"
#include "ui/analysis/AnalysisPanel.h"

class HomeView : public CWindowImpl<HomeView>
{
public:
	DECLARE_WND_CLASS(NULL)

	BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP(HomeView)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_RANGE_HANDLER(Config::HOME_BUTTON_ID, Config::SETTING_BUTTON_ID, OnClickLeftPanelButtons)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER_EX(Config::MSG_ACTIVE_PANEL_ID, OnActivePanel)
	END_MSG_MAP()

	void createOrShowUI();
private:
	// left panel
	LeftPanel leftPanel;

	// Home panel
	HomePanel homePanel;

	// Home panel
	DatabasePanel databasePanel;
	AnalysisPanel analysisPanel;

	// all panel sets, use to foreach, hide or show
	std::map<Config::PanelId, CWindowImpl *> panels;

	// left button id and panelId relactions
	std::map<Config::FrmButtonId, Config::PanelId> buttonPanelRelations;

	// selected buttonId and selected panelId
	Config::FrmButtonId selectedButtonId = Config::UNSED_BUTTON_ID;
	Config::PanelId selectedPannelId = static_cast<Config::PanelId>(0);

	// create or show panels
	void createOrShowLeftPanel(CRect &clientRect);
	void createOrShowPanel(Config::PanelId panelId, CWindowImpl & panel, CRect &clientRect);

	// change Panel(by buttonId)
	CWindow * changePanelByButtonId(UINT selButtonId);

	// change Panel(by panelId)
	CWindow * changePanelByPanelId(UINT panelId);
	

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	// click the leftpanel's button event
	LRESULT OnClickLeftPanelButtons(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	// MSG
	LRESULT OnActivePanel(UINT uMsg, WPARAM wParam, LPARAM lParam);
};
