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

 * @file   PreviewSqlDialog.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-10-24
 *********************************************************************/
#include "stdafx.h"
#include "PreviewSqlDialog.h"
#include "ui/common/message/QPopAnimate.h"
#include "utils/SavePointUtil.h"


PreviewSqlDialog::PreviewSqlDialog(HWND parentHwnd, CopyTableAdapter * adapter)
{
	setFormSize(PREVIEW_SQL_DIALOG_WIDTH, PREVIEW_SQL_DIALOG_HEIGHT);
	caption = S(L"preview-sql");
	this->sql.clear();
	this->adapter = adapter;
	this->supplier = adapter ? adapter->getSupplier() : nullptr;
}

void PreviewSqlDialog::createOrShowUI()
{
	QDialog::createOrShowUI();

	CRect clientRect;
	GetClientRect(clientRect);

	createOrShowSqlPreviewEdit(clientRect);
}


void PreviewSqlDialog::createOrShowSqlPreviewEdit(CRect & clientRect)
{
	CRect rect = clientRect;
	rect.DeflateRect(2, 2, 2, 2);
	rect.bottom = rect.top + (rect.Height() - 30);
	createOrShowSqlEditor(sqlPreviewEdit, Config::PREVIEW_SQL_EDITOR_ID, L"", rect, clientRect);
}


void PreviewSqlDialog::createOrShowSqlEditor(QHelpEdit & win, UINT id, const std::wstring & text, CRect & rect, CRect & clientRect, DWORD exStyle /*= 0*/)
{
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		std::wstring editorTitle = L"Source table:" + supplier->getRuntimeTblName() + L", Target table:" +  supplier->getTargetTable();
		win.setup(editorTitle, std::wstring(L""));
		win.Create(m_hWnd, rect, text.c_str(), WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0, Config::TABLE_SQL_PREVIEW_EDIT_ID);		
		return;
	} else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
	}
}

void PreviewSqlDialog::loadWindow()
{
	if (!isNeedReload) {
		return ;
	}

	isNeedReload = false;
	loadSqlPreviewEdit();
}


void PreviewSqlDialog::loadSqlPreviewEdit()
{
	if (supplier->getRuntimeUserDbId() == supplier->getTargetUserDbId()) {
		std::wstring sql = adapter->getPreviewSqlInSameDb();
		sqlPreviewEdit.setText(sql);
	} else {
		loadTargetTableSqlForOtherDb();
	}
}


void PreviewSqlDialog::loadTargetTableSqlForOtherDb()
{
	uint64_t targetUserDbId = supplier->getTargetUserDbId();
	int percent = 0;

	std::wstring savePoint = SavePointUtil::create(L"copy_table");
	std::wstring sql = L"SAVEPOINT \"" + savePoint + L"\";\n";
	sqlPreviewEdit.addText(sql);

	try {
		auto tableMap = supplier->getShardingTables();

		for (auto & tblItem : tableMap) {
			// 1. create table sql
			sql = adapter->generateCreateDdlForTargetTable(tblItem.first, tblItem.second);
			if (!sql.empty()) {
				sqlPreviewEdit.addText(sql);
				sqlPreviewEdit.addText(L"\n");
			}
			
			// 2. append table data sql
			doAppendCopyDataSql(tblItem.first, tblItem.second);			
		}
		
		// 4.Release the SAVEPOINT
		sql = L"RELEASE \"" + savePoint + L"\";\n";
		sqlPreviewEdit.addText(sql);
	} catch (QSqlExecuteException &ex) {
		QPopAnimate::report(ex);
		return ;
	} catch (QRuntimeException &ex) {
		QPopAnimate::report(ex);
		return ;
	}
}

bool PreviewSqlDialog::doAppendCopyDataSql(uint16_t suffix, const std::wstring & targetTblName)
{
	// SQL : INSERT INTO target_tbl SELECT * FROM source_tbl [WHERE {express} = {suffix}]
	if (supplier->getStructAndDataSetting() == STRUCT_ONLY 
		|| supplier->getStructAndDataSetting() == UNKOWN || targetTblName.empty()) {
		return false;
	}

	int page = 1;
	int perpage = 100;

	// Get data from the source table of source database, 
	// Get data from the source table of source database, 
	std::wstring whereClause;
	bool checked1 = supplier->getStructAndDataSetting() == DATA_ONLY || supplier->getStructAndDataSetting() == STRUCTURE_AND_DATA;
	bool checked2 = supplier->getEnableTableSharding();
	bool checked3 = supplier->getEnableShardingStrategy();
	bool checked4 = !supplier->getShardingStrategyExpress().empty();
	if (checked1 && checked2 && checked3 && checked4) { 
		whereClause.append(L"(").append(supplier->getShardingStrategyExpress()).append(L")").append(L"=").append(std::to_wstring(suffix));
	}
	
	int totalPage = tableService->getTableWhereDataPageCount(supplier->getRuntimeUserDbId(), supplier->getRuntimeTblName(), whereClause, perpage);
	for (page = 1; page <= totalPage; page++) {
		// Get data from source table 
		DataList pageDataList = tableService->getTableWhereDataList(supplier->getRuntimeUserDbId(), supplier->getRuntimeTblName(), whereClause, page, perpage);

		// Generate a sql statement for the page data list. and then execute the sql statement.
		std::wstring sql = adapter->genderatePageDataSql(pageDataList, targetTblName);
		if (!sql.empty()) {
			sqlPreviewEdit.addText(sql);
			sqlPreviewEdit.addText(L"\n");
		}
	}
	return true;
}

LRESULT PreviewSqlDialog::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	QDialog::OnInitDialog(uMsg, wParam, lParam, bHandled); 
	return 0;
}

LRESULT PreviewSqlDialog::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	QDialog::OnDestroy(uMsg, wParam, lParam, bHandled);
	
	if (sqlPreviewEdit.IsWindow()) sqlPreviewEdit.DestroyWindow();
	return 0;
}

LRESULT PreviewSqlDialog::OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	// invisible then return 
	if (!wParam) { 
		return 0;
	}
	loadWindow();
	return 0;
}

void PreviewSqlDialog::OnClickYesButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	EndDialog(Config::QDIALOG_YES_BUTTON_ID);
}
