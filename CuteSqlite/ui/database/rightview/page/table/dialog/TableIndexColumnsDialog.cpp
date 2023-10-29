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

 * @file   TableIndexColumnsDialog.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-10-16
 *********************************************************************/
#include "stdafx.h"
#include "TableIndexColumnsDialog.h"
#include "ui/common/message/QPopAnimate.h"

TableIndexColumnsDialog::TableIndexColumnsDialog(
	HWND parentHwnd, TableColumnsPageAdapter * columnPageAdapter, 
	TableIndexesPageAdapter * indexPageAdapter, CRect btnRect, int iItem, int iSubItem)
	:AbstractTableColumnsDialog(parentHwnd, btnRect, iItem, iSubItem)
{
	this->columnPageAdapter = columnPageAdapter;
	this->indexPageAdapter = indexPageAdapter;
}



void TableIndexColumnsDialog::loadLeftListBox()
{
	leftListBox.ResetContent();

	std::wstring columns = indexPageAdapter->getSubItemString(iItem, iSubItem);
	std::vector<std::wstring> selColumnNames = StringUtil::split(columns, L",");

	const std::vector<std::wstring> columnNames = columnPageAdapter->getAllColumnNames(selColumnNames);
	for (auto columnName : columnNames) {
		leftListBox.AddString(columnName.c_str());
	}
}


void TableIndexColumnsDialog::loadRightListBox()
{
	rightListBox.ResetContent();

	std::wstring columns = indexPageAdapter->getSubItemString(iItem, iSubItem);
	std::vector<std::wstring> selColumnNames = StringUtil::split(columns, L",");
	for (auto columnName : selColumnNames) {
		rightListBox.AddString(columnName.c_str());
	}
}



void TableIndexColumnsDialog::OnClickYesButton(UINT uNotifyCode, int nID, HWND hwnd)
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
	indexPageAdapter->changeColumnText(iItem, iSubItem, selColumns);
	IndexInfo & changeIndexInfo = indexPageAdapter->getSupplier()->getIdxRuntimeData(iItem);
	indexPageAdapter->getSupplier()->updateRelatedColumnsIfChangeIndex(changeIndexInfo);
	EndDialog(Config::QDIALOG_YES_BUTTON_ID);
}
