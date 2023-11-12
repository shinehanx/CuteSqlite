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

 * @file   TableForeignkeyColumnsDialog.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-10-16
 *********************************************************************/
#include "stdafx.h"
#include "TableForeignkeyColumnsDialog.h"
#include "ui/common/message/QPopAnimate.h"
#include "ui/database/rightview/page/supplier/TableStructureSupplier.h"

TableForeignkeyColumnsDialog::TableForeignkeyColumnsDialog(
	HWND parentHwnd, 
	TableColumnsPageAdapter * columnPageAdapter,
	TableForeignkeysPageAdapter * foreignkeyPageAdapter, 
	CRect btnRect, int iItem, int iSubItem)
	:AbstractTableColumnsDialog(parentHwnd, btnRect, iItem, iSubItem)
{
	this->columnPageAdapter = columnPageAdapter;
	this->foreignkeyPageAdapter = foreignkeyPageAdapter;
}

/**
 * load the left list box columns
 * Condition: iSubItem == 2 or 4.
 * 
 */
void TableForeignkeyColumnsDialog::loadLeftListBox()
{
	ATLASSERT(iSubItem == 2 || iSubItem == 4);
	leftListBox.ResetContent();

	std::wstring columns = foreignkeyPageAdapter->getSubItemString(iItem, iSubItem);
	std::vector<std::wstring> selColumnNames = StringUtil::split(columns, L",");

	if (iSubItem == 2) { // this table columns
		const std::vector<std::wstring> columnNames = columnPageAdapter->getAllColumnNames(selColumnNames);
		for (auto columnName : columnNames) {
			leftListBox.AddString(columnName.c_str());
		}
		return;
	}

	// iSubItem == 4 - Referenced columns
	uint64_t userDbId = foreignkeyPageAdapter->getSupplier()->getRuntimeUserDbId();
	std::wstring tblName = foreignkeyPageAdapter->getSubItemString(iItem, 3); // 3. Referenced Table
	std::wstring shema = foreignkeyPageAdapter->getSupplier()->getRuntimeSchema();
	if (tblName.empty()) {
		return ;
	}
	ColumnInfoList columnInfoList = tableService->getUserColumns(userDbId, tblName, shema, true);
	
	for (auto & columnInfo : columnInfoList) {
		// exclude the selected columns
		auto iter = std::find(selColumnNames.begin(), selColumnNames.end(), columnInfo.name);
		if (iter != selColumnNames.end()) {
			continue;
		}
		leftListBox.AddString(columnInfo.name.c_str());
	}
}


void TableForeignkeyColumnsDialog::loadRightListBox()
{
	rightListBox.ResetContent();

	std::wstring columns = foreignkeyPageAdapter->getSubItemString(iItem, iSubItem);
	std::vector<std::wstring> selColumnNames = StringUtil::split(columns, L",");
	for (auto columnName : selColumnNames) {
		rightListBox.AddString(columnName.c_str());
	}
}

void TableForeignkeyColumnsDialog::OnClickYesButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	
	int nItemCount = rightListBox.GetCount();
	if (!nItemCount) {
		QPopAnimate::error(m_hWnd, S(L"no-columns-selected"));
		rightListBox.SetFocus();
		return;
	}
	std::wstring selColumns;
	for (int i = 0; i < nItemCount; i++) {
		wchar_t cch[256] = {0};
		rightListBox.GetText(i, cch);
		selColumns.append(cch).append(L",");
	}
	selColumns = StringUtil::cutLastChar(selColumns);
	foreignkeyPageAdapter->changeColumnText(iItem, iSubItem, selColumns);
	EndDialog(Config::QDIALOG_YES_BUTTON_ID);
}
