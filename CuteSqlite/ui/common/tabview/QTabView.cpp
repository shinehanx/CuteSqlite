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

 * @file   QTabView.cpp
 * @brief  Common TabView class
 * 
 * @author Xuehan Qin
 * @date   2023-05-21
 *********************************************************************/
#include "stdafx.h"
#include "QTabView.h"
#include "utils/Log.h"

/**
 * Overrideables - add someone to tab control.
 * 
 * @return 
 */
bool QTabView::CreateTabControl()
{
	bool ret = CTabViewImpl<QTabView>::CreateTabControl();

	// m_tab.SetItemSize(30, 100);

	return ret;
}

/**
 * Overrideables - someone move window.
 * 
 */
void QTabView::UpdateLayout()
{
	RECT rect;
	GetClientRect(&rect);
	
	if (m_tab.IsWindow() && rect.right - rect.left > 1) {
		m_tab.SetWindowPos(NULL, 0, 0, rect.right - rect.left, m_cyTabHeight, SWP_NOZORDER);
	}
	
	if (m_nActivePage != -1 && rect.right - rect.left > 1) {
		// Q_DEBUG(L"ActivePage, x:{}, y:{},w:{},h:{}", 0, m_cyTabHeight, rect.right - rect.left, rect.bottom - rect.top - m_cyTabHeight);
		HWND hwnd = GetPageHWND(m_nActivePage);
		::SetWindowPos(hwnd, m_tab.m_hWnd, 1, m_cyTabHeight + 1 , rect.right - rect.left -2, rect.bottom - rect.top - m_cyTabHeight - 2, SWP_NOZORDER);
		::BringWindowToTop(hwnd);
	}
		
}

/**
 * Overrideables - update tooltip of tab control.
 * 
 * @param pTTDI
 */
void QTabView::UpdateTooltipText(LPNMTTDISPINFO pTTDI)
{

}
