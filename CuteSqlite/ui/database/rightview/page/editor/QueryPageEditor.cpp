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

 * @file   QueryPageEditor.cpp
 * @brief   Editor for QueryPage class, extended the toolbar buttons and menus
 * 
 * @author Xuehan Qin
 * @date   2023-11-13
 *********************************************************************/
#include "stdafx.h"
#include "QueryPageEditor.h"
#include <utility>
#include "core/common/Lang.h"
#include "ui/common/QWinCreater.h"
#include "ui/common/message/QPopAnimate.h"
#include "ui/database/rightview/page/editor/dialog/SqlLogDialog.h"
#include "common/AppContext.h"

void QueryPageEditor::setup(QueryPageSupplier * supplier)
{
	this->supplier = supplier;

}


void QueryPageEditor::createOrShowUI()
{
	QHelpEdit::createOrShowUI();

	CRect clientRect;
	GetClientRect(clientRect);
	createOrShowToolBarElems(clientRect);
}


void QueryPageEditor::createOrShowToolBarElems(CRect & clientRect)
{
	CRect topbarRect = getTopRect(clientRect);
	int x = 20, y = 5, w = PAGE_BUTTON_WIDTH + 8, h = PAGE_BUTTON_HEIGHT;
	CRect rect(x, y, x + w, y + h);

	// create or show button
	std::wstring imgDir = ResourceUtil::getProductImagesDir();
	std::wstring normalImagePath, pressedImagePath;
	if (!templatesButton.IsWindow()) {
		normalImagePath = imgDir + L"database\\editor\\button\\templates-button-normal.png";
		pressedImagePath = imgDir + L"database\\editor\\button\\templates-button-pressed.png";
		templatesButton.SetIconPath(normalImagePath, pressedImagePath);
		templatesButton.SetBkgColors(buttonColor, buttonColor, buttonColor);
	}
	QWinCreater::createOrShowButton(m_hWnd, templatesButton, Config::EDITOR_TEMPLATES_BUTTON_ID, L"", rect, clientRect);
	templatesButton.SetToolTip(S(L"sql-templates"));
	templatesButton.BringWindowToTop();

	rect.OffsetRect(w + 20, 0);
	if (!pragmasButton.IsWindow()) {
		normalImagePath = imgDir + L"database\\editor\\button\\pragmas-button-normal.png";
		pressedImagePath = imgDir + L"database\\editor\\button\\pragmas-button-pressed.png";
		pragmasButton.SetIconPath(normalImagePath, pressedImagePath);
		pragmasButton.SetBkgColors(buttonColor, buttonColor, buttonColor);
	}
	QWinCreater::createOrShowButton(m_hWnd, pragmasButton, Config::EDITOR_PRAGMAS_BUTTON_ID, L"", rect, clientRect);
	pragmasButton.SetToolTip(S(L"pragma-templates"));
	pragmasButton.BringWindowToTop();

	rect.OffsetRect(w + 20, 0);
	rect.right = rect.left + 16;
	if (!clearAllButton.IsWindow()) {
		normalImagePath = imgDir + L"database\\editor\\button\\clear-all-button-normal.png";
		pressedImagePath = imgDir + L"database\\editor\\button\\clear-all-button-pressed.png"; 
		clearAllButton.SetIconPath(normalImagePath, pressedImagePath);
		clearAllButton.SetBkgColors(buttonColor, buttonColor, buttonColor); 
	}
	QWinCreater::createOrShowButton(m_hWnd, clearAllButton, Config::EDITOR_CLEAR_ALL_BUTTON_ID, L"", rect, clientRect);
	clearAllButton.SetToolTip(S(L"clear-all"));
	clearAllButton.BringWindowToTop();

	rect.left = topbarRect.right - 30 - w;
	rect.right = rect.left + w;	
	if (!sqlLogButton.IsWindow()) {
		normalImagePath = imgDir + L"database\\editor\\button\\sql-log-button-normal.png";
		pressedImagePath = imgDir + L"database\\editor\\button\\sql-log-button-pressed.png";
		sqlLogButton.SetIconPath(normalImagePath, pressedImagePath);
		sqlLogButton.SetBkgColors(buttonColor, buttonColor, buttonColor); 
	}
	QWinCreater::createOrShowButton(m_hWnd, sqlLogButton, Config::EDITOR_SQL_LOG_BUTTON_ID, L"", rect, clientRect);
	sqlLogButton.SetToolTip(S(L"sql-log"));
	sqlLogButton.BringWindowToTop();

	
}

int QueryPageEditor::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	int ret = QHelpEdit::OnCreate(lpCreateStruct);

	adapter = new QueryPageEditorAdapter(m_hWnd, supplier);
	AppContext::getInstance()->subscribe(m_hWnd, Config::MSG_USE_SQL_ID);
	return ret;
}

int QueryPageEditor::OnDestroy()
{
	AppContext::getInstance()->unsubscribe(m_hWnd, Config::MSG_USE_SQL_ID);

	int ret = QHelpEdit::OnDestroy();
	if (templatesButton.IsWindow()) templatesButton.DestroyWindow();
	if (pragmasButton.IsWindow()) pragmasButton.DestroyWindow();
	if (clearAllButton.IsWindow()) clearAllButton.DestroyWindow();
	if (sqlLogButton.IsWindow()) sqlLogButton.DestroyWindow();

	if (adapter) {
		delete adapter;
		adapter = nullptr;
	}
	return ret;
}

void QueryPageEditor::OnClickTemplatesButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	CRect rect;
	templatesButton.GetWindowRect(rect);
	CPoint pt(rect.left, rect.bottom + 5);

	((QueryPageEditorAdapter *)adapter)->createTemplatesMenu();
	((QueryPageEditorAdapter *)adapter)->popupTemplatesMenu(pt);
}


void QueryPageEditor::OnClickPragmasButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	CRect rect; 
	pragmasButton.GetWindowRect(rect);
	CPoint pt(rect.left, rect.bottom + 5);

	((QueryPageEditorAdapter *)adapter)->createPragmasMenu();
	((QueryPageEditorAdapter *)adapter)->popupPragmasMenu(pt);
}


void QueryPageEditor::OnClickClearAllButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	editor.clearText();
}


void QueryPageEditor::OnClickSqlLogButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	CRect winRect;
	sqlLogButton.GetWindowRect(winRect);
	supplier->clearCacheUseSql();

	SqlLogDialog dialog;	
	dialog.setup(m_hWnd, (QueryPageEditorAdapter *)adapter, winRect);		
	if (dialog.DoModal(m_hWnd) == Config::QDIALOG_YES_BUTTON_ID) {
		std::wstring useSql = supplier->getCacheUseSql();
		if (!useSql.empty()) {
			replaceSelText(useSql);
		}
	}
}

void QueryPageEditor::OnClickTemplatesMenu(UINT uNotifyCode, int nID, HWND hwnd)
{
	std::wstring binDir = ResourceUtil::getProductBinDir();
	std::wstring templatePath = binDir + L"res\\tpl\\stmt-template-" + std::to_wstring(nID - WM_USER) + L".tpl";
	if (_waccess(templatePath.c_str(), 0) != 0) {
		Q_ERROR(L"File not found:" + templatePath);
		QPopAnimate::error(templatePath + L", File not found!");
		return;
	}

	std::wstring content = FileUtil::readFile(templatePath);
	if (content.empty()) {
		Q_ERROR(L"File empty:" + templatePath);
		QPopAnimate::error(templatePath + L" is empty file!");
		return;
	}
	replaceSelText(content);
}

void QueryPageEditor::OnClickPragmasMenu(UINT uNotifyCode, int nID, HWND hwnd)
{
	auto & pragmas = supplier->pragmas;
	int nSelMenuId = nID - WM_USER;
	auto iter = std::find_if(pragmas.begin(), pragmas.end(), [&nSelMenuId](const std::tuple<int, std::wstring, std::wstring> & item) {
		return nSelMenuId == std::get<0>(item);
	});
	if (iter == pragmas.end()) {
		return;
	}
	std::wstring content = std::get<2>(*iter);
	if (content.empty()) {
		Q_ERROR(L"Pragma empty, menu id:" + nID);
		QPopAnimate::error(std::wstring(L"Pragma function empty, menu id:") + std::to_wstring(nID));
		return;
	}
	replaceSelText(content);
}

LRESULT QueryPageEditor::OnHandleUseSql(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	std::wstring sql = (const wchar_t *)wParam;
	HWND hwnd = (HWND)lParam;
	if (hwnd != m_hWnd || sql.empty()) {
		return 0;
	}
	replaceSelText(sql);
	return 0;
}
