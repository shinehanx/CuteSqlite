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

 * @file   TableColumnsPageAdapter.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-10-10
 *********************************************************************/
#include "stdafx.h"
#include "TableColumnsPageAdapter.h"
#include <Strsafe.h>
#include "core/common/Lang.h"


const Columns TableColumnsPageAdapter::columns = { S(L"column-name"), S(L"data-type"), L"Not Null", L"Unsigned", L"PK", L"Auto Insc?", S(L"default"), S(L"check")};
const std::vector<int> TableColumnsPageAdapter::columnSizes = { 150, 100, 80, 80, 80, 80, 150, 150 };
const std::vector<int> TableColumnsPageAdapter::columnFormats = { LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_LEFT, LVCFMT_LEFT };
const std::vector<std::wstring> TableColumnsPageAdapter::dataTypeList = { L"INTEGER", L"TEXT", L"BLOB", L"REAL", L"NUMERIC"};

TableColumnsPageAdapter::TableColumnsPageAdapter(HWND parentHwnd, QListViewCtrl * listView, TblOperateType operateType /*= NEW_TABLE*/)
{
	this->parentHwnd = parentHwnd;
	this->dataView = listView;
	this->operateType = operateType;
}

TableColumnsPageAdapter::~TableColumnsPageAdapter()
{
	
}

int TableColumnsPageAdapter::loadNewTblListView(uint64_t userDbId, const std::wstring & schema)
{
	loadColumnsForListView();

	return loadEmptyRowsForListView();
}

void TableColumnsPageAdapter::loadColumnsForListView()
{
	dataView->InsertColumn(0, L"", LVCFMT_CENTER, 24, -1, 0);
	
	int n = static_cast<int>(columns.size());
	for (int i = 0; i < n; i++) {
		auto column = columns.at(i);
		auto size = columnSizes.at(i);
		auto format = columnFormats.at(i);// LVCFMT_LEFT or LVCFMT_CENTER

		dataView->InsertColumn(i+1, column.c_str(), format, size);
	}
}

int TableColumnsPageAdapter::loadEmptyRowsForListView()
{
	dataView->SetItemCount(NEW_TBL_EMPTY_COLUMN_SIZE);
	return NEW_TBL_EMPTY_COLUMN_SIZE;
}

/**
 * if the row of index=iItem is selected.
 * 
 * @param iItem the row index
 * @return 
 */
bool TableColumnsPageAdapter::getIsChecked(int iItem)
{
	if (dataView->GetSelectedCount() == 0) {
		return false;
	}
	int nSelItem = dataView->GetNextItem(-1, LVNI_ALL | LVNI_SELECTED); // 向下搜索选中的项 -1表示先找出第一个
	
	while (nSelItem != -1) {
		if (nSelItem == iItem) {
			return true;
		}
		nSelItem = dataView->GetNextItem(nSelItem, LVNI_ALL | LVNI_SELECTED); // 继续往下搜索选中项
	}

	return false;
}

/**
 * Fill the item and subitem data value of list view .
 * 
 * @param pLvdi
 * @return 
 */
LRESULT TableColumnsPageAdapter::fillListViewWithEmptyItemData(NMLVDISPINFO * pLvdi)
{
	auto iItem = pLvdi->item.iItem;
	if (-1 == iItem)
		return 0;

	auto count = NEW_TBL_EMPTY_COLUMN_SIZE;
	if (!count || count <= iItem)
		return 0;

	if (iItem == count - 1) {
		iItem = count - 1;
	}

	// set checked/unchecked image in the first column	
	if (pLvdi->item.iSubItem == 0 && pLvdi->item.mask & LVIF_TEXT) {
		pLvdi->item.mask = LVIF_IMAGE;
		pLvdi->item.iIndent = 1;
		if (getIsChecked(pLvdi->item.iItem)) {
			pLvdi->item.iImage = 1;
		} else {
			pLvdi->item.iImage = 0;
		}
		
		return 0;
	}

	// set dataType - 2
	if (pLvdi->item.iSubItem == 2 && pLvdi->item.mask & LVIF_TEXT) {
		dataView->createOrShowComboBox(iItem, pLvdi->item.iSubItem, dataTypeList, 0);
		return 0;
	}

	if (pLvdi->item.iSubItem >= 3 && pLvdi->item.iSubItem <= 6 && (pLvdi->item.mask & LVIF_TEXT)) {
		/*
		pLvdi->item.mask = LVIF_IMAGE;
		pLvdi->item.iIndent = 1;
		pLvdi->item.iImage = 0;		
		*/
		if (pLvdi->item.iItem == 1 && pLvdi->item.iSubItem == 3) {
			Q_DEBUG(L"item.iItem:{}, item.iSubItem:{}", pLvdi->item.iItem, pLvdi->item.iSubItem);
		}
		dataView->createOrShowCheckBox(iItem, pLvdi->item.iSubItem);
		return 0;
	}

	return 0;
}
