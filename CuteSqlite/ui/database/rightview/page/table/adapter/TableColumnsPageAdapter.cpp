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
#include "ui/common/message/QMessageBox.h"

const Columns TableColumnsPageAdapter::headerColumns = { S(L"column-name"), S(L"data-type"), L"Not Null", L"PK", L"Auto Insc", L"Unique", S(L"default"), S(L"check")};
const std::vector<int> TableColumnsPageAdapter::columnSizes = { 150, 100, 70, 70, 70, 70, 150, 100 };
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

int TableColumnsPageAdapter::loadTblColumnsListView(uint64_t userDbId, const std::wstring & schema, const std::wstring & tblName)
{
	// headers
	loadHeadersForListView();

	// rows
	if (tblName.empty()) {
		return loadEmptyRowsForListView();
	}
	return loadColumnRowsForListView(userDbId, schema, tblName);
}

void TableColumnsPageAdapter::loadHeadersForListView()
{
	dataView->InsertColumn(0, L"", LVCFMT_CENTER, 26, -1, 0);
	
	int n = static_cast<int>(headerColumns.size());
	for (int i = 0; i < n; i++) {
		auto column = headerColumns.at(i);
		auto size = columnSizes.at(i);
		auto format = columnFormats.at(i);// LVCFMT_LEFT or LVCFMT_CENTER

		dataView->InsertColumn(i+1, column.c_str(), format, size);
	}
}

int TableColumnsPageAdapter::loadEmptyRowsForListView()
{
	std::vector<std::wstring> ss = StringUtil::split(L"id,name,type,remark,phone,email,created_at,updated_at",L","); // tmp
	for (int i = 0; i < NEW_TBL_EMPTY_COLUMN_SIZE; i++) {
		ColumnInfo columnInfo;
		columnInfo.name = ss.at(i);
		runtimeDatas.push_back(columnInfo);
	}
	dataView->SetItemCount(NEW_TBL_EMPTY_COLUMN_SIZE);
	dataView->Invalidate(true);
	return NEW_TBL_EMPTY_COLUMN_SIZE;
}


int TableColumnsPageAdapter::loadColumnRowsForListView(uint64_t userDbId, const std::wstring & schema, const std::wstring & tblName)
{
	runtimeDatas = databaseService->getUserColumns(userDbId, tblName);
	int n = static_cast<int>(runtimeDatas.size());
	dataView->SetItemCount(n);
	return n;
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


int TableColumnsPageAdapter::getSelDataType(const std::wstring & dataType)
{
	int nSelItem = 0;
	int n = static_cast<int>(dataTypeList.size());
	for (int i = 0; i < n; i++) {
		if (dataType == dataTypeList.at(i)) {
			nSelItem = i;
			break;
		}
	}
	return nSelItem;
}


ColumnInfo TableColumnsPageAdapter::getRuntimeData(int nItem) const 
{
	ATLASSERT(nItem < static_cast<int>(runtimeDatas.size()));
	return runtimeDatas.at(nItem);
}

/**
 * Fill the item and subitem data value of list view .
 * 
 * @param pLvdi
 * @return 
 */
LRESULT TableColumnsPageAdapter::fillDataInListViewSubItem(NMLVDISPINFO * pLvdi)
{
	auto iItem = pLvdi->item.iItem;
	if (-1 == iItem)
		return 0;

	auto count = static_cast<int>(runtimeDatas.size());
	if (!count || count <= iItem)
		return 0;

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
	} else  if (pLvdi->item.iSubItem == 2 && pLvdi->item.mask & LVIF_TEXT) { // set dataType - 2
		std::wstring & val = runtimeDatas.at(pLvdi->item.iItem).type;
		dataView->createComboBox(iItem, pLvdi->item.iSubItem, val);
		StringCchCopy(pLvdi->item.pszText, pLvdi->item.cchTextMax, val.c_str());
		return 0;
	} else if (pLvdi->item.iSubItem >= 3 && pLvdi->item.iSubItem <= 6 && (pLvdi->item.mask & LVIF_TEXT)) {
		uint8_t val = 0;
		if (pLvdi->item.iSubItem == 3) {
			val = runtimeDatas.at(pLvdi->item.iItem).notnull;
		} else if (pLvdi->item.iSubItem == 4) {
			val = runtimeDatas.at(pLvdi->item.iItem).pk;
		} else if (pLvdi->item.iSubItem == 5) {
			val = runtimeDatas.at(pLvdi->item.iItem).ai;
		} else if (pLvdi->item.iSubItem == 6) {
			val = runtimeDatas.at(pLvdi->item.iItem).un;
		}
		dataView->createCheckBox(iItem, pLvdi->item.iSubItem, val);
		return 0;
	} else if (pLvdi->item.iSubItem == 1 && pLvdi->item.mask & LVIF_TEXT){ // column name
		std::wstring & val = runtimeDatas.at(pLvdi->item.iItem).name;	
		StringCchCopy(pLvdi->item.pszText, pLvdi->item.cchTextMax, val.c_str());
	} else if (pLvdi->item.iSubItem == 7 && pLvdi->item.mask & LVIF_TEXT){ // default value
		std::wstring & val = runtimeDatas.at(pLvdi->item.iItem).defVal;	
		StringCchCopy(pLvdi->item.pszText, pLvdi->item.cchTextMax, val.c_str());
	} else if (pLvdi->item.iSubItem == 8 && pLvdi->item.mask & LVIF_TEXT){ // check 
		std::wstring & val = runtimeDatas.at(pLvdi->item.iItem).checks;	
		StringCchCopy(pLvdi->item.pszText, pLvdi->item.cchTextMax, val.c_str());
	}

	return 0;
}

void TableColumnsPageAdapter::changeRuntimeDatasItem(int iItem, int iSubItem, std::wstring & newText)
{
	ATLASSERT(iItem >= 0 && iSubItem > 0);
	
	if (iSubItem == 1) { // column name
		runtimeDatas[iItem].name = newText;
	} else if (iSubItem == 2) { // column name
		runtimeDatas[iItem].type = newText;
	} else if (iSubItem == 3) { // not null
		runtimeDatas[iItem].notnull = newText.empty() ? 0 
			: static_cast<uint8_t>(std::stoi(newText));
	} else if (iSubItem == 4) { // pk : primary key
		runtimeDatas[iItem].pk = newText.empty() ? 0 
			: static_cast<uint8_t>(std::stoi(newText));
	} else if (iSubItem == 5) { // ai : auto increment
		runtimeDatas[iItem].ai = newText.empty() ? 0 
			: static_cast<uint8_t>(std::stoi(newText));
	} else if (iSubItem == 6) { // un : unique
		runtimeDatas[iItem].un = newText.empty() ? 0 
			: static_cast<uint8_t>(std::stoi(newText));
	}  else if (iSubItem == 7) { // default value
		runtimeDatas[iItem].defVal = newText;
	} else if (iSubItem == 8) { // check 
		runtimeDatas[iItem].checks = newText;
	}
}

void TableColumnsPageAdapter::invalidateSubItem(int iItem, int iSubItem)
{
	CRect subItemRect;
	dataView->GetSubItemRect(iItem, iSubItem, LVIR_BOUNDS, subItemRect);
	dataView->InvalidateRect(subItemRect, false);
}

void TableColumnsPageAdapter::createNewColumn()
{
	// 1.create a empty row and push it to runtimeDatas list
	ColumnInfo columnRow;
	columnRow.name = L"New Column";
	runtimeDatas.push_back(columnRow);

	// 2.update the item count and selected the new row	
	int n = static_cast<int>(runtimeDatas.size());
	dataView->SetItemCount(n);

}

bool TableColumnsPageAdapter::deleteSelColumns(bool confirm)
{
	if (!dataView->GetSelectedCount()) {
		return false;
	}
	if (confirm && QMessageBox::confirm(parentHwnd, S(L"delete-confirm-text"), S(L"yes"), S(L"no")) == Config::CUSTOMER_FORM_NO_BUTTON_ID) {
		return false;
	}
		
	// 1. delete the changeVals from dataView
	std::vector<int> nSelItems;
	int nSelItem = -1;
	while ((nSelItem = dataView->GetNextItem(nSelItem, LVNI_SELECTED)) != -1) {
		nSelItems.push_back(nSelItem);
		dataView->removeChangedValsItems(nSelItem);
	}

	if (nSelItems.empty()) {
		return false;
	}

	// 2.delete from runtimeDatas and database and dataView that the item begin from the last selected item
	int n = static_cast<int>(nSelItems.size());
	for (int i = n - 1; i >= 0; i--) {
		nSelItem = nSelItems.at(i);
		auto iter = runtimeDatas.begin();
		for (int j = 0; j < nSelItem; j++) {
			iter++;
		}
		auto & rowItem = (*iter);		

		// 2.1 delete row from runtimeDatas vector 
		runtimeDatas.erase(iter);

		// 2.2 delete row from dataView
		dataView->RemoveItem(nSelItem);
	}
	
	return true;
}

bool TableColumnsPageAdapter::moveUpSelColumns()
{
	if (!dataView->GetSelectedCount()) {
		return false;
	}

	// 1. delete the changeVals from dataView
	std::vector<int> nSelItems;
	int nSelItem = -1;
	while ((nSelItem = dataView->GetNextItem(nSelItem, LVNI_SELECTED)) != -1) {
		nSelItems.push_back(nSelItem);
		dataView->moveUpChangeValsItem(nSelItem);
	}

	if (nSelItems.empty()) {
		return false;
	}

	// 2.delete from runtimeDatas and database and dataView that the item begin from the last selected item
	int n = static_cast<int>(nSelItems.size());
	for (int i = n - 1; i >= 0; i--) {
		nSelItem = nSelItems.at(i);
		auto iter = runtimeDatas.begin();
		for (int j = 0; j < nSelItem; j++) {
			iter++;
		}
		auto & rowItem = (*iter);
		auto prevIter = iter == std::begin(runtimeDatas) ? 
			runtimeDatas.end() : std::prev(iter);		

		if (prevIter != runtimeDatas.end()) {
			// 2.1 delete row from runtimeDatas vector 
			std::swap(rowItem, (*prevIter));

			// 2.2 move up the child elements from dataView
			dataView->moveUpComboBoxes(nSelItem);
			dataView->moveUpCheckBoxes(nSelItem);

			if (nSelItem - 1 >= 0 ) {
				dataView->SelectItem(nSelItem - 1);
			}
		}		
	}

	return true;
}

bool TableColumnsPageAdapter::moveDownSelColumns()
{
	if (!dataView->GetSelectedCount()) {
		return false;
	}

	// 1. delete the changeVals from dataView
	std::vector<int> nSelItems;
	int nSelItem = -1;
	while ((nSelItem = dataView->GetNextItem(nSelItem, LVNI_SELECTED)) != -1) {
		nSelItems.push_back(nSelItem);
		dataView->moveDownChangeValsItem(nSelItem);
	}

	if (nSelItems.empty()) {
		return false;
	}

	int maxRows = dataView->GetItemCount();
	// 2.delete from runtimeDatas and database and dataView that the item begin from the last selected item
	int n = static_cast<int>(nSelItems.size());
	for (int i = n - 1; i >= 0; i--) {
		nSelItem = nSelItems.at(i);
		auto iter = runtimeDatas.begin();
		for (int j = 0; j < nSelItem; j++) {
			iter++;
		}
		auto & rowItem = (*iter);
		auto nextIter = iter == std::end(runtimeDatas) ? 
			runtimeDatas.end() : std::next(iter);

		if (nextIter != runtimeDatas.end()) {
			// 2.1 swap row from runtimeDatas vector 
			std::swap(rowItem, (*nextIter));

			// 2.2 move down the child elements from dataView
			dataView->moveDownComboBoxes(nSelItem);
			dataView->moveDownCheckBoxes(nSelItem);

			if (nSelItem + 1 < maxRows ) {
				dataView->SelectItem(nSelItem + 1);
			}
		}		
	}

	return true;
}


void TableColumnsPageAdapter::clickListViewSubItem(NMITEMACTIVATE * clickItem)
{
	if (clickItem->iSubItem == 0 || (clickItem->iSubItem >= 2 && clickItem->iSubItem <= 6)) {
		if (clickItem->iSubItem == 2) {
			dataView->showComboBox(clickItem->iItem, clickItem->iSubItem, dataTypeList, true);
		} else if (clickItem->iSubItem >= 3 && clickItem->iSubItem <= 6) {
			bool isChecked = dataView->getCheckBoxIsChecked(clickItem->iItem, clickItem->iSubItem);
			dataView->setCheckBoxIsChecked(clickItem->iItem, clickItem->iSubItem, !isChecked);
			std::wstring origVal = std::to_wstring((int)isChecked);
			std::wstring newVal = std::to_wstring((int)!isChecked);
			changeRuntimeDatasItem(clickItem->iItem, clickItem->iSubItem, newVal);
			invalidateSubItem(clickItem->iItem, clickItem->iSubItem);
		}
		
		return ;
	}

	// show the editor
	dataView->createOrShowEditor(clickItem->iItem, clickItem->iSubItem);
}

/**
 * Get all column names from dataView.
 * 
 * @param excludeNames
 * @return 
 */
std::vector<std::wstring> TableColumnsPageAdapter::getAllColumnNames(const std::vector<std::wstring> & excludeNames /*= std::vector<std::wstring>()*/) const
{
	std::vector<std::wstring> result;

	int n = static_cast<int>(runtimeDatas.size());
	for (int i = 0; i < n; i++) {
		auto data = runtimeDatas.at(i);
		std::wstring columnName = data.name;
		if (columnName.empty()) {
			continue;
		}

		if (!excludeNames.empty() 
			&& excludeNames.end() != std::find(excludeNames.begin(), excludeNames.end(), columnName)) {
			continue;
		}

		result.push_back(columnName);
	}

	return result;
}

