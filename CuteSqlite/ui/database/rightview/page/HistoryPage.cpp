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

 * @file   HistoryPage.cpp
 * @brief  Execute sql statement and show the info of query result
 * 
 * @author Xuehan Qin
 * @date   2023-05-22
 *********************************************************************/

#include "stdafx.h"
#include "HistoryPage.h"
#include <WinUser.h>
#include "ui/common/QWinCreater.h"
#include "core/common/Lang.h"
#include "common/AppContext.h"

BOOL HistoryPage::PreTranslateMessage(MSG* pMsg)
{
	UINT msg = pMsg->message;
	WPARAM key = pMsg->wParam;

	if (pMsg->hwnd == infoEdit.m_hWnd && msg == WM_KEYDOWN
		&& (key == _T('C') || key == _T('V')|| key == _T('X') || key == _T('A')) 
		&& ((GetKeyState(VK_CONTROL) & 0xFF00) == 0xFF00)) {
		::TranslateMessage(pMsg);
		::DispatchMessage(pMsg);
		if (key == _T('A')) {
			infoEdit.SetSel(0, -1);
		}
	}
	return FALSE;
}

void HistoryPage::clear()
{
	infoEdit.Clear();
}

void HistoryPage::createOrShowUI()
{
	QPage::createOrShowUI();

	CRect clientRect;
	GetClientRect(clientRect);
	createOrShowInfoEditor(clientRect);
}

void HistoryPage::loadWindow()
{
	QPage::loadWindow();
}

void HistoryPage::createOrShowInfoEditor(CRect & clientRect)
{
	CRect rect = clientRect;
	QWinCreater::createOrShowEdit(m_hWnd, infoEdit, Config::INFOPAGE_INFO_EDIT_ID, L"", rect, clientRect, textFont,  WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL);
	infoEdit.BringWindowToTop();
}

void HistoryPage::paintItem(CDC & dc, CRect & paintRect)
{
	dc.FillRect(paintRect, bkgColor);
}

int HistoryPage::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	bool ret = QPage::OnCreate(lpCreateStruct);
	textFont = FT(L"form-text-size");

	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_EXEC_SQL_RESULT_MESSAGE_ID);
	return ret;
}

int HistoryPage::OnDestroy()
{
	bool ret = QPage::OnDestroy();
	if (textFont) ::DeleteObject(textFont);
	if (infoEdit.IsWindow()) infoEdit.DestroyWindow();

	return ret;
}

LRESULT HistoryPage::OnExecSqlResultMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	auto runtimeResultInfo = (ResultInfo *)lParam;
	if (runtimeResultInfo == nullptr) {
		return 0;
	}
	
	std::wstring str = runtimeResultInfo->createdAt.empty() ? 
		DateUtil::getCurrentDateTime() : runtimeResultInfo->createdAt;
	runtimeResultInfo->createdAt = str;
	str.append(L" - ");
	str.append(StringUtil::formatBreak(runtimeResultInfo->sql));
	str.append(L"\r\n\r\n");
	if (runtimeResultInfo->code || !runtimeResultInfo->msg.empty()) {
		str.append(S(L"result")).append(L": [").append(std::to_wstring(runtimeResultInfo->code)).append(L"] ")
			.append(runtimeResultInfo->msg).append(L" \r\n\r\n");
	}
	str.append(S(L"affected-rows")).append(L": ").append(std::to_wstring(runtimeResultInfo->effectRows)).append(L"\r\n\r\n")
		.append(S(L"exec-time")).append(L": ").append(runtimeResultInfo->execTime).append(L"\r\n")
		.append(S(L"transfer-time")).append(L": ").append(runtimeResultInfo->transferTime).append(L"\r\n")
		.append(S(L"total-time")).append(L": ").append(runtimeResultInfo->totalTime);
	str.append(L"\r\n --------------------------------------------------------------------- \r\n\r\n");

	CString text;
	infoEdit.GetWindowText(text);
	text.Append(str.c_str());
	infoEdit.SetWindowText(text);
	infoEdit.UpdateWindow();
	return 0;
}

HBRUSH HistoryPage::OnCtlColorStatic(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, bkgColor);
	::SelectObject(hdc, textFont);
	return AtlGetStockBrush(WHITE_BRUSH);
}

HBRUSH HistoryPage::OnCtlColorEdit(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, bkgColor);
	::SelectObject(hdc, textFont);
	return AtlGetStockBrush(WHITE_BRUSH);
}

