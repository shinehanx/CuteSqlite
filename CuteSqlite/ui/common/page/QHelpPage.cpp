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

 * @file   QBarPage.cpp
 * @brief  Abstract page class, inherits its subclass and is embedded in the TabView page
 * 
 * @author Xuehan Qin
 * @date   2023-05-22
 *********************************************************************/
#include "stdafx.h"
#include "QHelpPage.h"
#include "ui/common/QWinCreater.h"

void QHelpPage::setup(std::wstring & helpText)
{
	this->helpText = helpText;
}

void QHelpPage::createOrShowUI()
{
	CRect clientRect;
	GetClientRect(clientRect);
	CRect rect = getTopRect(clientRect);
	QWinCreater::createOrShowLabel(m_hWnd, helpLabel, helpText.c_str(), rect, clientRect, SS_CENTERIMAGE, 12);
}

int QHelpPage::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	int ret = QPage::OnCreate(lpCreateStruct);
	staticBrush = ::CreateSolidBrush(staticColor);
	return ret;
}

int QHelpPage::OnDestroy()
{
	int ret = QPage::OnDestroy();
	if (staticBrush) ::DeleteObject(staticBrush);
	if (helpLabel.IsWindow()) helpLabel.DestroyWindow();
	return ret;
}

HBRUSH QHelpPage::OnCtlColorStatic(HDC hdc, HWND hwnd)
{
	if (hwnd == helpLabel.m_hWnd) {
		::SetTextColor(hdc, RGB(0x2c, 0x2c, 0x2c)); //文本区域前景色
		::SetBkColor(hdc, RGB(255, 255, 255)); // 文本区域背景色		
		return staticBrush; // 整个CStatic的Client区域背景色
	}

	return staticBrush;
}

