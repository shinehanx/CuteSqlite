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

 * @file   ImportAsCsvAdapter.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-11-04
 *********************************************************************/
#include "stdafx.h"
#include "ImportFromCsvAdapter.h"
#include "core/common/Lang.h"
#include "ui/common/message/QPopAnimate.h"
#include <strsafe.h>

ImportFromCsvAdapter::ImportFromCsvAdapter(HWND parentHwnd, ImportFromCsvSupplier * supplier)
	:ImportDatabaseAdapter(parentHwnd, nullptr)
{
	this->parentHwnd = parentHwnd;
	this->supplier = supplier;
}


UserTableStrings ImportFromCsvAdapter::getTables(uint64_t userDbid)
{
	try {
		return tableService->getUserTableStrings(userDbid);
	} catch (QRuntimeException &ex) {
		Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::error(S(L"error-text").append(ex.getMsg()).append(L",[code:").append(ex.getCode()).append(L"]"));
	}
	return UserTableStrings();
}

void ImportFromCsvAdapter::loadHeaderForColumnListView(QListViewCtrl & columnListView)
{
	columnListView.InsertColumn(0, L"", LVCFMT_LEFT, 26, -1, 0);
	std::wstring headColumns[2] = { L"Source Column", L"Target Column" };
	int headSize[2] = { 150, 150};
	
	for (int i = 0; i < 2; i++) {
		columnListView.InsertColumn(i+1, headColumns[i].c_str(), LVCFMT_LEFT, headSize[i]);
	}
}

int ImportFromCsvAdapter::loadCsvFileToColumnListView(const std::wstring &importPath, QListViewCtrl & columnListView)
{
	if (importPath.empty() || _waccess(importPath.c_str(), 0) != 0) {
		QPopAnimate::error(E(L"200002"));
		return 0;
	}
	
	std::wstring firstLine = FileUtil::readFirstLine(importPath, supplier->csvCharset);
	
	if (firstLine.empty()) {
		QPopAnimate::error(E(L"200002"));
		return 0;
	}

	std::wstring sep = supplier->csvFieldTerminateBy;
	sep = sep.empty() ? L"," : (sep == L"TAB" ? L"\t" : sep);
	auto fields = StringUtil::split(firstLine, sep, true);
	supplier->setCsvRuntimeColumns(fields);
	int rowCount = static_cast<int>(fields.size());

	Columns targetColumns = tableService->getUserColumnStrings(supplier->getRuntimeUserDbId(), supplier->getRuntimeTblName());
	supplier->setTblRuntimeColumns(targetColumns);
	int targetLen = static_cast<int>(targetColumns.size());
	
	columnListView.SetItemCount(rowCount);
	return rowCount;
}

int ImportFromCsvAdapter::loadCsvFileToDataListView(const std::wstring & importPath, QListViewCtrl & dataListView)
{
	return 0;
}

void ImportFromCsvAdapter::fillSubItemForColumnListView(NMLVDISPINFO* pLvdi, QListViewCtrl & columnListView)
{
	auto iItem = pLvdi->item.iItem;
	if (-1 == iItem)
		return ;

	auto count = static_cast<int>(supplier->getCsvRuntimeColumns().size());
	if (!count || count <= iItem)
		return ;

	// set checked/unchecked image in the first column	
	if (pLvdi->item.iSubItem == 0 && pLvdi->item.mask & LVIF_TEXT) {
		pLvdi->item.mask = LVIF_IMAGE;
		pLvdi->item.iIndent = 1;
		if (getIsChecked(&columnListView, pLvdi->item.iItem)) { 
			pLvdi->item.iImage = 1;
		} else {
			pLvdi->item.iImage = 0;
		}
		
		return ;
	} else  if (pLvdi->item.iSubItem == 1 && pLvdi->item.mask & LVIF_TEXT) { // set dataType - 2
		std::wstring val;
		if (supplier->csvColumnNameOnTop) {
			val = supplier->getCsvRuntimeColumns().at(iItem);
		} else {
			val = L"Column##" + std::to_wstring(iItem + 1);
		}
		
		StringCchCopy(pLvdi->item.pszText, pLvdi->item.cchTextMax, val.c_str());
		return ;
	} else  if (pLvdi->item.iSubItem == 2 && pLvdi->item.mask & LVIF_TEXT) { // set dataType - 2
		std::wstring & val = supplier->getTblRuntimeColumns().at(pLvdi->item.iItem);
		StringCchCopy(pLvdi->item.pszText, pLvdi->item.cchTextMax, val.c_str());
		columnListView.createComboBox(iItem, pLvdi->item.iSubItem, val);
		return ;
	}
}

void ImportFromCsvAdapter::fillSubItemForDataListView(NMLVDISPINFO* plvdi, QListViewCtrl & dataListView)
{
	
}

/**
 * if the row of index=iItem is selected.
 * 
 * @param iItem the row index
 * @return 
 */
bool ImportFromCsvAdapter::getIsChecked(QListViewCtrl * listView, int iItem)
{
	if (listView->GetSelectedCount() == 0) {
		return false;
	}
	int nSelItem = listView->GetNextItem(-1, LVNI_ALL | LVNI_SELECTED); // 向下搜索选中的项 -1表示先找出第一个
	
	while (nSelItem != -1) {
		if (nSelItem == iItem) {
			return true;
		}
		nSelItem = listView->GetNextItem(nSelItem, LVNI_ALL | LVNI_SELECTED); // 继续往下搜索选中项
	}

	return false;
}