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

 * @file   ResultInfoPage.cpp
 * @brief  Execute sql statement and show the info of query result
 * 
 * @author Xuehan Qin
 * @date   2023-05-22
 *********************************************************************/

#include "stdafx.h"
#include "ResultInfoPage.h"
#include "ui/common/QWinCreater.h"
#include "core/common/Lang.h"
#include "common/AppContext.h"

void ResultInfoPage::clear()
{
	infoEdit.Clear();
}

void ResultInfoPage::createOrShowUI()
{
	QPage::createOrShowUI();

	CRect clientRect;
	GetClientRect(clientRect);
	createOrShowInfoEditor(clientRect);
}

void ResultInfoPage::loadWindow()
{
	QPage::loadWindow();
}

void ResultInfoPage::createOrShowInfoEditor(CRect & clientRect)
{
	CRect rect = clientRect;
	QWinCreater::createOrShowEdit(m_hWnd, infoEdit, Config::INFOPAGE_INFO_EDIT_ID, L"", rect, clientRect, textFont,  WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL);
	infoEdit.BringWindowToTop();
}

void ResultInfoPage::paintItem(CDC & dc, CRect & paintRect)
{
	dc.FillRect(paintRect, bkgColor);
}

int ResultInfoPage::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	bool ret = QPage::OnCreate(lpCreateStruct);
	textFont = FT(L"form-text-size");

	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_EXEC_SQL_RESULT_MESSAGE_ID);
	return ret;
}

int ResultInfoPage::OnDestroy()
{
	bool ret = QPage::OnDestroy();
	if (textFont) ::DeleteObject(textFont);
	return ret;
}

LRESULT ResultInfoPage::OnExecSqlResultMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	auto runtimeResultInfo = (ResultInfo *)lParam;
	if (runtimeResultInfo == nullptr) {
		return 0;
	}

	CString msg;
	infoEdit.GetWindowText(msg);
	std::wstring str(msg.GetString());
	str.append(S(L"query")).append(L": ").append(runtimeResultInfo->sql).append(L"\r\n\r\n");
	if (runtimeResultInfo->code || !runtimeResultInfo->msg.empty()) {
		str.append(S(L"result")).append(L": [").append(std::to_wstring(runtimeResultInfo->code)).append(L"] ")
			.append(runtimeResultInfo->msg).append(L" \r\n\r\n");
	}
	str.append(S(L"affected-rows")).append(L": ").append(std::to_wstring(runtimeResultInfo->effectRows)).append(L"\r\n\r\n")
		.append(S(L"exec-time")).append(L": ").append(runtimeResultInfo->execTime).append(L"\r\n")
		.append(S(L"transfer-time")).append(L": ").append(runtimeResultInfo->transferTime).append(L"\r\n")
		.append(S(L"total-time")).append(L": ").append(runtimeResultInfo->totalTime).append(L"\r\n\r\n");

	infoEdit.SetWindowText(str.c_str());
	infoEdit.UpdateWindow();
	return 0;
}

HBRUSH ResultInfoPage::OnCtlColorStatic(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, bkgColor);
	::SelectObject(hdc, textFont);
	return AtlGetStockBrush(WHITE_BRUSH);
}

HBRUSH ResultInfoPage::OnCtlColorEdit(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, bkgColor);
	::SelectObject(hdc, textFont);
	return AtlGetStockBrush(WHITE_BRUSH);
}

