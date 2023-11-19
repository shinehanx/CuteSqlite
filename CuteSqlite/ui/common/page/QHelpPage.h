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

* @file   QBarPage.h
* @brief  Abstract page class, inherits its subclass and is embedded in the TabView page
*		   Follow the view-handler design pattern,
*			handler: RightWorkView.
*			view: QBarPage and it's subclass in the page folder.
*			provider: RightWorkProvider
*
* @author Xuehan Qin
* @date   2023-05-22
*********************************************************************/
#pragma once
#include <string>
#include <atlwin.h>
#include <atlcrack.h>

#include "QPage.h"

class QHelpPage : public QPage
{
public:
	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP_EX(QHelpPage)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_CTLCOLORSTATIC(OnCtlColorStatic)
		CHAIN_MSG_MAP(QPage)		
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	void setup(std::wstring & helpText);
protected:
	COLORREF staticColor = RGB(238, 238, 238);
	CBrush staticBrush;

	CStatic helpLabel;
	std::wstring helpText;

	virtual void createOrShowUI();
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual int OnDestroy();
	HBRUSH OnCtlColorStatic(HDC hdc, HWND hwnd);
};



