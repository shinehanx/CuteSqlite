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

 * @file   AddSqlDialog.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-10-24
 *********************************************************************/
#include "stdafx.h"
#include "AddSqlDialog.h"
#include "ui/common/message/QPopAnimate.h"
#include "utils/SavePointUtil.h"
#include "utils/SqlUtil.h"
#include "utils/PerformUtil.h"
#include "common/AppContext.h"

AddSqlDialog::AddSqlDialog(HWND parentHwnd, const std::wstring &content /*= std::wstring()*/, const std::wstring &sqlTitle)
{
	setFormSize(PREVIEW_SQL_DIALOG_WIDTH, PREVIEW_SQL_DIALOG_HEIGHT);
	caption = S(L"analysis-sql");
	this->parentHwnd = parentHwnd;
	this->content = content;
}

void AddSqlDialog::createOrShowUI()
{
	QDialog::createOrShowUI();

	CRect clientRect;
	GetClientRect(clientRect);
	createOrShowSelectDbElems(clientRect);
	createOrShowSqlPreviewEdit(clientRect);
}

void AddSqlDialog::createOrShowSelectDbElems(CRect & clientRect)
{
	int x = 10, y = 5, w = 250, h = 20;
	CRect rect(x, y, x + w, y + h);
	createOrShowFormLabel(selectDbLabel, S(L"select-database").append(L":"), rect, clientRect, SS_LEFT, elemFont);

	rect.OffsetRect(0, h + 5);
	createOrShowFormComboBox(selectDbComboBox, Config::EXPORT_DB_AS_SQL_SELECT_DB_COMBOBOX_ID,  L"", rect, clientRect);
}

void AddSqlDialog::createOrShowSqlPreviewEdit(CRect & clientRect)
{
	CRect rcLast = GdiPlusUtil::GetWindowRelativeRect(selectDbComboBox.m_hWnd);
	int x = 2, y = rcLast.bottom + 5, w = clientRect.Width() - 4, h = clientRect.Height() - y - 2;
	CRect rect(x, y, x + w, y + h);
	
	rect.bottom = rect.top + (rect.Height() - 30);
	createOrShowSqlEditor(sqlPreviewEdit, Config::PREVIEW_SQL_EDITOR_ID, L"", rect, clientRect);
}


void AddSqlDialog::createOrShowSqlEditor(QHelpEdit & win, UINT id, const std::wstring & text, CRect & rect, CRect & clientRect, DWORD exStyle /*= 0*/)
{
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		std::wstring editorTitle = S(L"add-sql-analysis");
		win.setup(editorTitle, std::wstring(L""));
		win.Create(m_hWnd, rect, text.c_str(), WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, id);
		return;
	} else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
	}
}

void AddSqlDialog::loadWindow()
{
	if (!isNeedReload) {
		return ;
	}

	isNeedReload = false;
	loadSelectDbComboBox();
	loadSqlPreviewEdit();
}


void AddSqlDialog::loadSelectDbComboBox()
{
	// db list
	UserDbList dbs;
	try {
		dbs = databaseService->getAllUserDbs();
	} catch (QRuntimeException &ex) {
		Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::error(parentHwnd, S(L"error-text").append(ex.getMsg()).append(L",[code:").append(ex.getCode()).append(L"]"));
	}

	selectDbComboBox.ResetContent();
	int n = static_cast<int>(dbs.size());
	int nSelItem = -1;
	for (int i = 0; i< n; i++) {
		auto item = dbs.at(i);
		int nItem = selectDbComboBox.AddString(item.name.c_str());
		selectDbComboBox.SetItemData(nItem, item.id);
		if (item.isActive) {
			nSelItem = i;
		}
	}
	selectDbComboBox.SetCurSel(nSelItem);
}

void AddSqlDialog::loadSqlPreviewEdit()
{
	sqlPreviewEdit.setText(content);
	// parent window may be handle the message to append this specify sql statements to preview editor
	::PostMessage(parentHwnd, Config::MSG_LOADING_SQL_PREVIEW_EDIT_ID, (WPARAM)&sqlPreviewEdit, NULL);
}

LRESULT AddSqlDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	QDialog::OnInitDialog(uMsg, wParam, lParam, bHandled); 
	yesButton.SetWindowText(S(L"analysis").c_str());
	elemFont = FT(L"elem-size");
	return 0;
}

LRESULT AddSqlDialog::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	QDialog::OnDestroy(uMsg, wParam, lParam, bHandled);
	
	if (elemFont) ::DeleteObject(elemFont);
	if (selectDbLabel.IsWindow()) selectDbLabel.DestroyWindow();
	if (selectDbComboBox.IsWindow()) selectDbComboBox.DestroyWindow();
	if (sqlPreviewEdit.IsWindow()) sqlPreviewEdit.DestroyWindow();
	return 0;
}

LRESULT AddSqlDialog::OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// invisible then return 
	if (!wParam) { 
		return 0;
	}
	loadWindow();
	return 0;
}

void AddSqlDialog::OnClickYesButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	std::wstring sqls = sqlPreviewEdit.getText();
	if (sqls.empty()) {
		QPopAnimate::report(E(L"300002"));
		return ;
	}

	std::vector<std::wstring> sqlVector;
	
	if (sqls.find(L';') == std::wstring::npos) {
		sqlVector.push_back(sqls);
	} else {
		sqlVector = StringUtil::splitNotIn(sqls, L";", L"BEGIN", L"END", L"TRANSACTION", true);
	}

	for (auto & sql : sqlVector) {
		if (!SqlUtil::isSelectSql(sql)) {
			QPopAnimate::report(E(L"300003")); // Only SELECT statements are supported
			continue;
		}

		saveSqlToAnalysis(sql);
	}
	

	EndDialog(Config::QDIALOG_YES_BUTTON_ID);
}

void AddSqlDialog::saveSqlToAnalysis(const std::wstring& sql)
{
	if (sql.empty()) {
		return ;
	}
	int nSelItem = selectDbComboBox.GetCurSel();
	if (nSelItem == CB_ERR) {
		return ;
	}
	uint64_t userDbId = static_cast<uint64_t>(selectDbComboBox.GetItemData(nSelItem));
	if (!userDbId) {
		return ;
	}

	DataList dataList;
	auto _begin = PerformUtil::begin();
	try {
		dataList = selectSqlAnalysisService->explainSql(userDbId, sql);
		
		if (dataList.empty()) {
			QPopAnimate::report(E(L"300004"));
			return;
		}

		SqlLog sqlLog;
		sqlLog.userDbId = userDbId;
		sqlLog.effectRows = 0;
		sqlLog.execTime = PerformUtil::end(_begin);
		sqlLog.transferTime = PerformUtil::end(_begin);
		sqlLog.totalTime = PerformUtil::end(_begin);
		sqlLog.sql = sql;
		sqlLog.createdAt = DateUtil::getCurrentDateTime();
		sqlLog.createdAt = DateUtil::getCurrentDateTime();
		uint64_t sqlLogId = sqlLogService->createSqlLog(sqlLog);

		if (!sqlLogId) {
			return;
		}

		AppContext::getInstance()->dispatchForResponse(Config::MSG_ANALYSIS_SQL_ID, WPARAM(userDbId), LPARAM(sqlLogId));
	} catch (QSqlExecuteException &ex) {
		QPopAnimate::report(ex);
		return;
	} catch (QRuntimeException &ex) {
		QPopAnimate::report(ex);
		return;
	}
}
