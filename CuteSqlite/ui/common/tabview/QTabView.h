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

 * @file   QTabView.h
 * @brief  Common TabView class
 * 
 * @author Xuehan Qin
 * @date   2023-05-21
 *********************************************************************/
#pragma once
#include <atlwin.h>
#include <atlcrack.h>
#include <atlctrlx.h>
#include "common/Config.h"

class QTabView : public CTabViewImpl<QTabView>
{
public:
	DECLARE_WND_CLASS_EX(_T("QTabView"), 0, COLOR_APPWORKSPACE)
	void enableCloseBtn(bool enabled);

	// Override ables - add someone to tab control
	bool CreateTabControl();

	// Override ables - someone move window
	void UpdateLayout();

	// Override ables - update tooltip to tab control
	void UpdateTooltipText(LPNMTTDISPINFO pTTDI);

	virtual void OnTabCloseBtn(int nPage);

	// Override ables - Specify the rect of close button in the TabItem
	void CalcCloseButtonRect(int nItem, RECT& rcClose);

	// Override ables - 
	virtual LRESULT OnTabMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};
