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

 * @file   SqlLogListItem.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-11-15
 *********************************************************************/
#pragma once
#include "stdafx.h"
#include "SqlLogListItem.h"
#include "utils/ResourceUtil.h"
#include "utils/EntityUtil.h"
#include "core/common/Lang.h"
#include "ui/common/QWinCreater.h"
#include "utils/FontUtil.h"

SqlLogListItem::SqlLogListItem(ResultInfo & info)
{
	this->info = EntityUtil::copy(info);
}

void SqlLogListItem::select(bool state)
{
	selectState = state;
	if (selectState) {
		bkgColor = bkgSelectColor;
	} else {
		bkgColor = RGB(255, 255, 255);
	}
	if (bkgBrush) {
		::DeleteObject(bkgBrush);
		bkgBrush = ::CreateSolidBrush(bkgColor);
	}
	Invalidate(true);
}

int SqlLogListItem::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	this->bkgBrush = ::CreateSolidBrush(bkgColor);
	this->textFont = FT(L"log-list-item-text-size");
	this->sqlFont = FT(L"log-list-item-sql-size");
	return 0;
}

int SqlLogListItem::OnDestroy()
{
	if (bkgBrush) ::DeleteObject(bkgBrush);
	if (textFont) ::DeleteObject(textFont);
	if (sqlFont) ::DeleteObject(sqlFont);

	if (createdAtLabel.IsWindow()) createdAtLabel.DestroyWindow();
	if (topImage.IsWindow()) topImage.DestroyWindow();
	if (sqlEdit.IsWindow()) sqlEdit.DestroyWindow();
	if (stateImage.IsWindow()) stateImage.DestroyWindow();
	if (resultMsgLabel.IsWindow()) resultMsgLabel.DestroyWindow();
	if (execTimeLabel.IsWindow()) execTimeLabel.DestroyWindow();
	if (useButton.IsWindow()) useButton.DestroyWindow();
	if (topButton.IsWindow()) topButton.DestroyWindow();
	if (explainButton.IsWindow()) explainButton.DestroyWindow();
	if (deleteButton.IsWindow()) deleteButton.DestroyWindow();
	return 0;
}

void SqlLogListItem::OnSize(UINT nType, CSize size)
{
	createOrShowUI();
}

void SqlLogListItem::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if (!bShow) {
		return ;
	}
	loadWindow();
}

void SqlLogListItem::OnPaint(CDCHandle dc)
{
	CPaintDC pdc(m_hWnd);
	CMemoryDC mdc(pdc, pdc.m_ps.rcPaint);

	mdc.FillRect(&pdc.m_ps.rcPaint, bkgBrush);
}

BOOL SqlLogListItem::OnEraseBkgnd(CDCHandle dc)
{
	return TRUE;
}

HBRUSH SqlLogListItem::OnCtlStaticColor(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, bkgColor);
	if (hwnd == sqlEdit.m_hWnd) {
		::SetTextColor(hdc, sqlColor); 
		::SelectObject(hdc, sqlFont);
	} else {
		::SetTextColor(hdc, textColor); 
		::SelectObject(hdc, textFont);
	}
	
	return bkgBrush;
}

HBRUSH SqlLogListItem::OnCtlBtnColor(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, btnBkgColor);
	::SetTextColor(hdc, textColor); 
	::SelectObject(hdc, textFont);
	return bkgBrush;
}

void SqlLogListItem::createOrShowUI()
{
	CRect clientRect;
	GetClientRect(clientRect);

	createTopElems(clientRect);
	createMiddleElems(clientRect);
	createBottomElems(clientRect);
}

void SqlLogListItem::createTopElems(CRect & clientRect)
{
	int x = 10, y = 5, w = 150, h = 12;
	CRect rect(x, y, x + w, y + h);
	QWinCreater::createOrShowLabel(m_hWnd, createdAtLabel, info.createdAt, rect, clientRect, SS_LEFT, 10);

	x = clientRect.right - 10, w = 12, h = 12;
	if (info.top) {
		rect = { x, y, x + w, y + h };
		std::wstring imgDir = ResourceUtil::getProductImagesDir();
		std::wstring imagePath = imgDir + L"database\\list\\top.png";
		topImage.load(imagePath.c_str(), BI_PNG, true); 
		topImage.setBkgColor(bkgColor); 
		QWinCreater::createOrShowImage(m_hWnd, topImage, 0, rect, clientRect);
	}
}

void SqlLogListItem::createMiddleElems(CRect & clientRect)
{
	CRect rcTop = GdiPlusUtil::GetWindowRelativeRect(createdAtLabel); 

	int x = 20, y = rcTop.bottom + 5, w = clientRect.Width() - 20 * 2, h = 60;
	CRect rect(x, y, x + w, y + h);
	DWORD dwStyle = ES_MULTILINE;
	std::wstring fontName = Lang::fontName();
	CSize  size = FontUtil::measureTextSize(info.sql.c_str(), Lang::fontSize(L"log-list-item-sql-size"), false, fontName.c_str());
	if (size.cx / (w * 1.0) > 3.0) {
		dwStyle = dwStyle | WS_VSCROLL;
	}
	QWinCreater::createOrShowEdit(m_hWnd, sqlEdit, 0, info.sql, rect, clientRect, textFont, dwStyle);

	rect.OffsetRect(0, h + 5);
	rect.right = rect.left + 14;
	rect.bottom = rect.top + 14;
	std::wstring imgDir = ResourceUtil::getProductImagesDir();
	std::wstring imagePath;
	if (info.code == 0) {
		imagePath = imgDir + L"database\\list\\success.png";
	}else {
		imagePath = imgDir + L"database\\list\\error.png";
	}
	stateImage.load(imagePath.c_str(), BI_PNG, true); 
	stateImage.setBkgColor(bkgColor); 
	QWinCreater::createOrShowImage(m_hWnd, stateImage, 0, rect, clientRect);

	rect.OffsetRect(rect.Width() + 10, 3);
	rect.right = rect.left + 200;
	rect.bottom = rect.top + 20;
	std::wstring resultMsg = L"Effect Rows:";
	resultMsg.append(std::to_wstring(info.effectRows)).append(L", Code:")
		.append(std::to_wstring(info.code));
	QWinCreater::createOrShowLabel(m_hWnd, resultMsgLabel, resultMsg, rect, clientRect, SS_LEFT, 10);

	rect.OffsetRect(rect.Width() + 10, 0);
	std::wstring execTime = L"Total Times:";
	execTime.append(info.totalTime);
	QWinCreater::createOrShowLabel(m_hWnd, execTimeLabel, execTime, rect, clientRect, SS_RIGHT, 10);
}

void SqlLogListItem::createBottomElems(CRect & clientRect)
{
	CRect rcMid = GdiPlusUtil::GetWindowRelativeRect(execTimeLabel);
	int x = clientRect.right - 4 * (50 + 10) - 20, y = rcMid.bottom + 5, w = 50, h = 20;
	CRect rect(x, y, x + w, y + h);
	QWinCreater::createOrShowButton(m_hWnd, useButton, Config::SQL_LOG_ITEM_USE_BUTTON_ID, L"Use", rect, clientRect);

	rect.OffsetRect(w + 10, 0);
	QWinCreater::createOrShowButton(m_hWnd, explainButton, Config::SQL_LOG_ITEM_EXPLAIN_BUTTON_ID, L"Explain", rect, clientRect);

	rect.OffsetRect(w + 10, 0);
	QWinCreater::createOrShowButton(m_hWnd, topButton, Config::SQL_LOG_ITEM_TOP_BUTTON_ID, L"Top", rect, clientRect);

	rect.OffsetRect(w + 10, 0);
	QWinCreater::createOrShowButton(m_hWnd, deleteButton, Config::SQL_LOG_ITEM_DELELE_BUTTON_ID, L"Delete", rect, clientRect);
}

void SqlLogListItem::loadWindow()
{
	
}

