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
 
 * @file   TableIndexesPageAdapter.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-10-10
 *********************************************************************/
#include "stdafx.h"
#include "TableIndexesPageAdapter.h"
#include <Strsafe.h>
#include "core/common/Lang.h"
#include "ui/common/message/QMessageBox.h"
#include "ui/database/rightview/page/table/dialog/TableIndexColumnsDialog.h"

const Columns TableIndexesPageAdapter::headerColumns = { S(L"index-name"), S(L"columns"), S(L"index-type"), L"SQL"};
const std::vector<int> TableIndexesPageAdapter::columnSizes = { 150, 150, 150, 200 };
const std::vector<int> TableIndexesPageAdapter::columnFormats = { LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_LEFT };
const std::vector<std::wstring> TableIndexesPageAdapter::indexTypeList = {L"Unique", L"Primary Key", L"Foreign Key",  L"Check"};

TableIndexesPageAdapter::TableIndexesPageAdapter(HWND parentHwnd, QListViewCtrl * listView, TblOperateType operateType /*= NEW_TABLE*/)
{
	this->parentHwnd = parentHwnd;
	this->dataView = listView;
	this->operateType = operateType;
}

TableIndexesPageAdapter::~TableIndexesPageAdapter()
{
	
}

int TableIndexesPageAdapter::loadTblIndexesListView(uint64_t userDbId, const std::wstring & schema, const std::wstring & tblName)
{
	// headers
	loadHeadersForListView();

	// rows
	if (tblName.empty()) {
		return loadEmptyRowsForListView();
	}
	return loadIndexRowsForListView(userDbId, schema, tblName);
}

void TableIndexesPageAdapter::loadHeadersForListView()
{
	dataView->InsertColumn(0, L"", LVCFMT_LEFT, 26, -1, 0);
	
	int n = static_cast<int>(headerColumns.size());
	for (int i = 0; i < n; i++) {
		auto column = headerColumns.at(i);
		auto size = columnSizes.at(i);
		auto format = columnFormats.at(i);// LVCFMT_LEFT or LVCFMT_CENTER

		dataView->InsertColumn(i+1, column.c_str(), format, size);
	}
}

int TableIndexesPageAdapter::loadEmptyRowsForListView()
{
	
		IndexInfo index1, index2;
		index1.colums = L"id"; // todo..., remove debug 
		index1.type = indexTypeList.at(1); //type : Primary
		runtimeDatas.push_back(index1);
		index2.colums = L"id,name,created_at,updated_at"; //type : Unique
		index2.type = indexTypeList.at(0); //type : Unique
		runtimeDatas.push_back(index2);
	
	dataView->SetItemCount(2);
	return 1;
}


int TableIndexesPageAdapter::loadIndexRowsForListView(uint64_t userDbId, const std::wstring & schema, const std::wstring & tblName)
{
	runtimeDatas = databaseService->getIndexInfoList(userDbId, tblName);
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
bool TableIndexesPageAdapter::getIsChecked(int iItem)
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


int TableIndexesPageAdapter::getSelIndexType(const std::wstring & dataType)
{
	int nSelItem = 0;
	int n = static_cast<int>(indexTypeList.size());
	for (int i = 0; i < n; i++) {
		if (dataType == indexTypeList.at(i)) {
			nSelItem = i;
			break;
		}
	}
	return nSelItem;
}

/**
 * Fill the item and subitem data value of list view .
 * 
 * @param pLvdi
 * @return 
 */
LRESULT TableIndexesPageAdapter::fillDataInListViewSubItem(NMLVDISPINFO * pLvdi)
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
	} else  if (pLvdi->item.iSubItem == 3 && pLvdi->item.mask & LVIF_TEXT) { // set dataType - 2
		std::wstring & val = runtimeDatas.at(pLvdi->item.iItem).type;
		StringCchCopy(pLvdi->item.pszText, pLvdi->item.cchTextMax, val.c_str());
		dataView->createComboBox(iItem, pLvdi->item.iSubItem, val);		
		return 0;
	} else if (pLvdi->item.iSubItem == 1 && pLvdi->item.mask & LVIF_TEXT){ // column name
		std::wstring & val = runtimeDatas.at(pLvdi->item.iItem).name;	
		StringCchCopy(pLvdi->item.pszText, pLvdi->item.cchTextMax, val.c_str());
	} else if (pLvdi->item.iSubItem == 2 && pLvdi->item.mask & LVIF_TEXT){ // default value
		std::wstring & val = runtimeDatas.at(pLvdi->item.iItem).colums;	
		StringCchCopy(pLvdi->item.pszText, pLvdi->item.cchTextMax, val.c_str());
		dataView->createButton(iItem, pLvdi->item.iSubItem, L"...");
	} else if (pLvdi->item.iSubItem == 4 && pLvdi->item.mask & LVIF_TEXT){ // check 
		std::wstring & val = runtimeDatas.at(pLvdi->item.iItem).sql;
		StringCchCopy(pLvdi->item.pszText, pLvdi->item.cchTextMax, val.c_str());
	}

	return 0;
}

void TableIndexesPageAdapter::changeRuntimeDatasItem(int iItem, int iSubItem, std::wstring & origText, const std::wstring & newText)
{
	ATLASSERT(iItem >= 0 && iSubItem > 0);
	
	if (iSubItem == 1) { // column name
		runtimeDatas[iItem].name = newText;
	} else if (iSubItem == 2) { // columns
		runtimeDatas[iItem].colums = newText;
	} else if (iSubItem == 3) { // index type 
		runtimeDatas[iItem].type = newText;
	} else if (iSubItem == 4) { // 
		runtimeDatas[iItem].sql = newText;
	}

	SubItemValue subItemVal;
	subItemVal.iItem = iItem;
	subItemVal.iSubItem = iSubItem;
	subItemVal.origVal = origText;
	subItemVal.newVal = newText;

	dataView->setChangeVal(subItemVal);
}

void TableIndexesPageAdapter::invalidateSubItem(int iItem, int iSubItem)
{
	CRect subItemRect;
	dataView->GetSubItemRect(iItem, iSubItem, LVIR_BOUNDS, subItemRect);
	dataView->InvalidateRect(subItemRect, false);
}

void TableIndexesPageAdapter::createNewIndex()
{
	// 1.create a empty row and push it to runtimeDatas list
	IndexInfo row;
	row.name = L"New Index";
	runtimeDatas.push_back(row);

	// 2.update the item count and selected the new row	
	int n = static_cast<int>(runtimeDatas.size());
	dataView->SetItemCount(n);

}

bool TableIndexesPageAdapter::deleteSelIndexes(bool confirm)
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


std::wstring TableIndexesPageAdapter::getSubItemString(int iItem, int iSubItem)
{
	ATLASSERT(iItem >= 0 && iSubItem > 0);
	if (runtimeDatas.empty()) {
		return L"";
	}
	if (iSubItem == 1) {
		return runtimeDatas.at(iItem).name;
	} else if (iSubItem == 2) {
		return runtimeDatas.at(iItem).colums;
	} else if (iSubItem == 3) {
		return runtimeDatas.at(iItem).type;
	} else if (iSubItem == 4) {
		return runtimeDatas.at(iItem).sql;
	}
	return L"";
}

void TableIndexesPageAdapter::changeColumnText(int iItem, int iSubItem, const std::wstring & text)
{
	ATLASSERT(iItem >= 0 && iSubItem > 0);
	std::wstring origText = getSubItemString(iItem, iSubItem);
	changeRuntimeDatasItem(iItem, iSubItem, origText, text);
	invalidateSubItem(iItem, iSubItem);
}

void TableIndexesPageAdapter::clickListViewSubItem(NMITEMACTIVATE * clickItem)
{
	if (clickItem->iSubItem == 0) {
		return ;
	} else if (clickItem->iSubItem == 2) { // button
		return ;
	} else if (clickItem->iSubItem == 3) {
		dataView->showComboBox(clickItem->iItem, clickItem->iSubItem, indexTypeList, false);
		return ;	
	}

	// show the editor
	dataView->createOrShowEditor(clickItem->iItem, clickItem->iSubItem);
}

std::wstring TableIndexesPageAdapter::genderateIndexesSqlClause()
{
	std::wostringstream ss;
	int n = static_cast<int>(runtimeDatas.size());
	wchar_t blk[5] = { 0 };
	wmemset(blk, 0x20, 4); // 4 blank chars
	for (int i = 0; i < n; i++) {
		if (i > 0) {
			ss << L',' << std::endl;
		}
		auto item = runtimeDatas.at(i);
		ss << blk;
		if (!item.name.empty()) {
			ss << L"CONSTRAINT \"" << item.name << L"\"" << blk[0];
		}
		
		if (!item.type.empty()) {
			ss <<  item.type << L"(";
		}
		
		if (!item.colums.empty()) {
			ss  <<  item.colums ;
		}

		if (item.ai) {
			ss <<  L" AUTOINCREMENT" ;
		}

		if (!item.type.empty()) {
			ss << L")";
		}
	}
	return ss.str();
}