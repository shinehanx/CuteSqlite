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

 * @file   QPage.cpp
 * @brief  Abstract page class, inherits its subclass and is embedded in the TabView page
 * 
 * @author Xuehan Qin
 * @date   2023-05-22
 *********************************************************************/
#include "stdafx.h"
#include "QPage.h"



CRect QPage::getTopRect(CRect & clientRect)
{
	return { 0, 0, clientRect.right, PAGE_TOPBAR_HEIGHT };
}


CRect QPage::getPageRect(CRect & clientRect)
{
	return { 0, PAGE_TOPBAR_HEIGHT, clientRect.right, clientRect.bottom };
}

void QPage::createOrShowUI()
{

}

void QPage::loadWindow()
{
	
}

int QPage::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	topbarBrush = ::CreateSolidBrush(topbarColor);
	bkgBrush = ::CreateSolidBrush(bkgColor);
	return 0;
}

int QPage::OnDestroy()
{
	if (topbarBrush) ::DeleteObject(topbarBrush);
	if (bkgBrush) ::DeleteObject(bkgBrush);
	return 0;
}

void QPage::OnSize(UINT nType, CSize size)
{
	createOrShowUI();

}

void QPage::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if (!bShow) {
		return ;
	}
	loadWindow();
}

void QPage::OnPaint(CDCHandle dc)
{
	CPaintDC pdc(m_hWnd);
	CMemoryDC mdc(pdc, pdc.m_ps.rcPaint);
	CRect clientRect(pdc.m_ps.rcPaint);
	// GetClientRect(clientRect);


	CRect topRect = getTopRect(clientRect);
	mdc.FillRect(topRect, topbarBrush);

	CRect pageRect = getPageRect(clientRect);
	mdc.FillRect(pageRect, bkgBrush);

	// The subclass implements this method to extend the screen content
	paintItem(mdc, clientRect);
}

BOOL QPage::OnEraseBkgnd(CDCHandle dc)
{
	return TRUE;
}
