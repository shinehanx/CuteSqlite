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
 
 * @file   TableForeignkeysPageAdapter.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-10-10
 *********************************************************************/
#include "stdafx.h"
#include "TableForeignkeysPageAdapter.h"
#include <Strsafe.h>
#include <stack>
#include "core/common/Lang.h"
#include "ui/common/message/QMessageBox.h"
#include "ui/database/rightview/page/table/dialog/TableIndexColumnsDialog.h"
#include "utils/EntityUtil.h"
#include "ui/common/message/QPopAnimate.h"

TableForeignkeysPageAdapter::TableForeignkeysPageAdapter(HWND parentHwnd, QListViewCtrl * listView, TableStructureSupplier * supplier)
{
	ATLASSERT(parentHwnd && listView && supplier);
	this->parentHwnd = parentHwnd;
	this->dataView = listView;
	this->supplier = supplier;
}

TableForeignkeysPageAdapter::~TableForeignkeysPageAdapter()
{
	
}

int TableForeignkeysPageAdapter::loadTblForeignkeysListView(uint64_t userDbId,const std::wstring & tblName, const std::wstring & schema)
{
	// headers
	loadHeadersForListView();

	// rows
	if (tblName.empty()) {
		return loadEmptyRowsForListView();
	}
	return loadForeignkeyRowsForListView(userDbId, tblName, schema);
}

void TableForeignkeysPageAdapter::loadHeadersForListView()
{
	dataView->InsertColumn(0, L"", LVCFMT_LEFT, 26, -1, 0);
	
	int n = static_cast<int>(TableStructureSupplier::frkHeadColumns.size());
	for (int i = 0; i < n; i++) {
		auto column = TableStructureSupplier::frkHeadColumns.at(i);
		auto size = TableStructureSupplier::frkHeadSizes.at(i);
		auto format = TableStructureSupplier::frkHeadFormats.at(i);// LVCFMT_LEFT or LVCFMT_CENTER

		dataView->InsertColumn(i+1, column.c_str(), format, size);
	}
}

int TableForeignkeysPageAdapter::loadEmptyRowsForListView()
{	
	
	return 1;
}


int TableForeignkeysPageAdapter::loadForeignkeyRowsForListView(uint64_t userDbId, const std::wstring & tblName, const std::wstring & schema)
{
	auto frkRuntimeDatas = tableService->getForeignKeyList(userDbId, tblName, schema);
	supplier->setFrkRuntimeDatas(frkRuntimeDatas);
	supplier->setFrkOrigDatas(frkRuntimeDatas);
	int n = static_cast<int>(supplier->getFrkRuntimeDatas().size());
	dataView->SetItemCount(n);
	return n;
}

/**
 * if the row of index=iItem is selected.
 * 
 * @param iItem the row index
 * @return 
 */
bool TableForeignkeysPageAdapter::getIsChecked(int iItem)
{
	if (dataView->GetSelectedCount() == 0) {
		return false;
	}
	int nSelItem = dataView->GetNextItem(-1, LVNI_ALL | LVNI_SELECTED); // ��������ѡ�е��� -1��ʾ���ҳ���һ��
	
	while (nSelItem != -1) {
		if (nSelItem == iItem) {
			return true;
		}
		nSelItem = dataView->GetNextItem(nSelItem, LVNI_ALL | LVNI_SELECTED); // ������������ѡ����
	}

	return false;
}


int TableForeignkeysPageAdapter::getSelIndexType(const std::wstring & dataType)
{
	int nSelItem = 0;
	int n = static_cast<int>(TableStructureSupplier::idxTypeList.size());
	for (int i = 0; i < n; i++) {
		if (dataType == TableStructureSupplier::idxTypeList.at(i)) {
			nSelItem = i;
			break;
		}
	}
	return nSelItem;
}


void TableForeignkeysPageAdapter::changePrimaryKey(ColumnInfoList & pkColumns)
{
	int n = static_cast<int>(supplier->getIdxRuntimeDatas().size());
	std::vector<int> nSelItems;
	for (int i = 0; i < n; i++) {
		auto item = supplier->getIdxRuntimeDatas().at(i);
		if (StringUtil::toupper(item.type) == StringUtil::toupper(TableStructureSupplier::idxTypeList.at(1))) { // indexTypeList[1] : primary key
			nSelItems.push_back(i);
		}
	}

	if (pkColumns.empty()) {		
		n = static_cast<int>(nSelItems.size());
		for (int i = n - 1; i >= 0; i--) {
			removeSelectedItem(i);
		}
		return;
	}

	// Generate columns string, such as "id,name,..."
	IndexInfo row;
	row.type = TableStructureSupplier::idxTypeList.at(1);// indexTypeList[1] : primary key
	int nCols = static_cast<int>(pkColumns.size());
	for (int i = 0; i < nCols; i++) {
		if (i > 0) {
			row.columns.append(L",");
		}
		auto item = pkColumns.at(i);
		row.columns.append(item.name);
	}

	// insert/or modify the primary key row to ListView
	if (nSelItems.empty()) {
		supplier->getIdxRuntimeDatas().insert(supplier->getIdxRuntimeDatas().begin(), row); // insert to the first
	} else {
		auto & oneIndex = supplier->getIdxRuntimeDatas().at(nSelItems.at(0));
		oneIndex.columns = row.columns; // modify columns string
		invalidateSubItem(nSelItems.at(0), 1); // 2th param = 1 - primary key
	}

	// update the item count and selected the new row	
	n = static_cast<int>(supplier->getIdxRuntimeDatas().size());
	dataView->SetItemCount(n);
}

/**
 * Fill the item and subitem data value of list view .
 * 
 * @param pLvdi
 * @return 
 */
LRESULT TableForeignkeysPageAdapter::fillDataInListViewSubItem(NMLVDISPINFO * pLvdi)
{
	auto iItem = pLvdi->item.iItem;
	if (-1 == iItem)
		return 0;

	auto count = static_cast<int>(supplier->getIdxRuntimeDatas().size());
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
		std::wstring & val = supplier->getIdxRuntimeDatas().at(pLvdi->item.iItem).type;
		StringCchCopy(pLvdi->item.pszText, pLvdi->item.cchTextMax, val.c_str());
		dataView->createComboBox(iItem, pLvdi->item.iSubItem, val);		
		return 0;
	} else if (pLvdi->item.iSubItem == 1 && pLvdi->item.mask & LVIF_TEXT){ // index name
		std::wstring & val = supplier->getIdxRuntimeDatas().at(pLvdi->item.iItem).name;	
		StringCchCopy(pLvdi->item.pszText, pLvdi->item.cchTextMax, val.c_str());
	} else if (pLvdi->item.iSubItem == 2 && pLvdi->item.mask & LVIF_TEXT){ // columns
		std::wstring & val = supplier->getIdxRuntimeDatas().at(pLvdi->item.iItem).columns;	
		StringCchCopy(pLvdi->item.pszText, pLvdi->item.cchTextMax, val.c_str());
		dataView->createButton(iItem, pLvdi->item.iSubItem, L"...");
	}

	return 0;
}

void TableForeignkeysPageAdapter::changeRuntimeDatasItem(int iItem, int iSubItem, std::wstring & origText, const std::wstring & newText)
{
	ATLASSERT(iItem >= 0 && iSubItem > 0);
	
	auto & runtimeDatas = supplier->getIdxRuntimeDatas();
	if (iSubItem == 1) { // column name
		runtimeDatas[iItem].name = newText;
	} else if (iSubItem == 2) { // columns
		runtimeDatas[iItem].columns = newText;
		if (runtimeDatas[iItem].type == getSupplier()->idxTypeList.at(1)) { // 1 - Primary Key
			auto colVector = StringUtil::split(newText, L",");
			if (colVector.size() > 1) {
				runtimeDatas[iItem].ai = 0;
			}
		}
	} else if (iSubItem == 3) { // index type 
		runtimeDatas[iItem].type = newText;
	} 

	SubItemValue subItemVal;
	subItemVal.iItem = iItem;
	subItemVal.iSubItem = iSubItem;
	subItemVal.origVal = origText;
	subItemVal.newVal = newText;

	dataView->setChangeVal(subItemVal);
}

void TableForeignkeysPageAdapter::invalidateSubItem(int iItem, int iSubItem)
{
	CRect subItemRect;
	dataView->GetSubItemRect(iItem, iSubItem, LVIR_BOUNDS, subItemRect);
	dataView->InvalidateRect(subItemRect, false);
}

void TableForeignkeysPageAdapter::createNewIndex()
{
	// 1.create a empty row and push it to runtimeDatas list
	IndexInfo row;
	row.name = L"";
	supplier->getIdxRuntimeDatas().push_back(row);

	// 2.update the item count and selected the new row	
	int n = static_cast<int>(supplier->getIdxRuntimeDatas().size());
	dataView->SetItemCount(n);

}

bool TableForeignkeysPageAdapter::deleteSelIndexes(bool confirm)
{
	if (!dataView->GetSelectedCount()) {
		return false;
	}
	if (confirm && QMessageBox::confirm(parentHwnd, S(L"delete-index-confirm-text"), S(L"yes"), S(L"no")) == Config::CUSTOMER_FORM_NO_BUTTON_ID) {
		return false;
	}
		
	// 1. delete the changeVals from dataView
	std::vector<int> nSelItems;
	int nSelItem = -1;
	while ((nSelItem = dataView->GetNextItem(nSelItem, LVNI_SELECTED)) != -1) {
		nSelItems.push_back(nSelItem);
	}

	if (nSelItems.empty()) {
		return false;
	}

	// 2.delete from runtimeDatas and database and dataView that the item begin from the last selected item
	int n = static_cast<int>(nSelItems.size());
	for (int i = n - 1; i >= 0; i--) {
		nSelItem = nSelItems.at(i);
		auto & indexInfo = supplier->getIdxRuntimeDatas().at(nSelItem);
		// 2.0 update related TableColumnsPage runtime data through indexInfo.columns
		supplier->updateRelatedColumnsIfDeleteIndex(indexInfo);
		
		// 2.1 delete row from runtimeDatas vector and origDatas
		supplier->eraseIdxRuntimeData(nSelItem);
		supplier->eraseIdxOrigData(nSelItem);

		// 2.2 delete row from dataView 
		dataView->RemoveItem(nSelItem);		
	}
	
	return true;
}


void TableForeignkeysPageAdapter::removeSelectedItem(int nSelItem)
{
	auto iter = supplier->getIdxRuntimeDatas().begin();
	for (int j = 0; j < nSelItem; j++) {
		iter++;
	}

	// 1 delete row from runtimeDatas vector 
	supplier->getIdxRuntimeDatas().erase(iter);

	// 2 delete row from dataView
	dataView->RemoveItem(nSelItem);
}


/**
 * change columns value in listView of TableForeinkeysPage when TableColumnsPage changing column name.
 * 
 * @param oldColumnName
 * @param newColumnName
 */
void TableForeignkeysPageAdapter::changeTableColumnName(const std::wstring & oldColumnName, const std::wstring & newColumnName)
{
	ATLASSERT(!oldColumnName.empty() && !newColumnName.empty() && oldColumnName != newColumnName);
	IndexInfoList & indexes = supplier->getIdxRuntimeDatas();
	int n = static_cast<int>(indexes.size());
	for (int i = 0; i < n; i++) {
		auto & item = indexes.at(i);
		auto columns = StringUtil::split(item.columns, L",");
		auto iter = std::find(columns.begin(), columns.end(), oldColumnName);
		if (iter == columns.end()) {
			continue;
		}
		(*iter) = newColumnName;
		item.columns = StringUtil::implode(columns, L",");

		invalidateSubItem(i, 2); // 2 - columns 
	}
}


void TableForeignkeysPageAdapter::deleteTableColumnName(const std::wstring & columnName)
{
	ATLASSERT(!columnName.empty());
	IndexInfoList & indexes = supplier->getIdxRuntimeDatas();

	int n = static_cast<int>(indexes.size());
	std::stack<int> delItemStack;
	for (int i = 0; i < n; i++) {
		auto & item = indexes.at(i);
		auto columns = StringUtil::split(item.columns, L",");
		auto iter = std::find(columns.begin(), columns.end(), columnName);
		if (iter == columns.end()) {
			continue;
		}
		columns.erase(iter);
		if (columns.empty()) {
			delItemStack.push(i);
		}
		item.columns = StringUtil::implode(columns, L","); 
		invalidateSubItem(i, 2); // 2 - columns 
	}
	
	while (!delItemStack.empty()) {
		int nSelItem = delItemStack.top();
		delItemStack.pop();

		removeSelectedItem(nSelItem);
	}
}


std::wstring TableForeignkeysPageAdapter::getSubItemString(int iItem, int iSubItem)
{
	ATLASSERT(iItem >= 0 && iSubItem > 0);
	if (supplier->getIdxRuntimeDatas().empty()) {
		return L"";
	}
	if (iSubItem == 1) {
		return supplier->getIdxRuntimeDatas().at(iItem).name;
	} else if (iSubItem == 2) {
		return supplier->getIdxRuntimeDatas().at(iItem).columns;
	} else if (iSubItem == 3) {
		return supplier->getIdxRuntimeDatas().at(iItem).type;
	} 
	return L"";
}

void TableForeignkeysPageAdapter::changeColumnText(int iItem, int iSubItem, const std::wstring & text)
{
	ATLASSERT(iItem >= 0 && iSubItem > 0);
	std::wstring origText = getSubItemString(iItem, iSubItem);
	changeRuntimeDatasItem(iItem, iSubItem, origText, text);
	invalidateSubItem(iItem, iSubItem);
}

void TableForeignkeysPageAdapter::clickListViewSubItem(NMITEMACTIVATE * clickItem)
{
	if (clickItem->iSubItem == 0) { // 0 - row checkBox
		return ;
	} else if (clickItem->iSubItem == 2) { // button
		dataView->activeSubItem(clickItem->iItem, clickItem->iSubItem);
		return ;
	} else if (clickItem->iSubItem == 3) {
		dataView->showComboBox(clickItem->iItem, clickItem->iSubItem, TableStructureSupplier::idxTypeList, false);
		return ;	
	}

	// show the editor
	dataView->createOrShowEditor(clickItem->iItem, clickItem->iSubItem);
}

/**
 * Generate indexes clause when generating create table DDL.
 * 
 * @param hasAutoIncrement
 * @return 
 */
std::wstring TableForeignkeysPageAdapter::genderateCreateIndexesSqlClause(bool hasAutoIncrement)
{
	std::wstring ss;
	int n = static_cast<int>(supplier->getIdxRuntimeDatas().size());
	wchar_t blk[5] = { 0, 0, 0, 0, 0 };
	wmemset(blk, 0x20, 4); // 4 blank chars
	for (int i = 0; i < n; i++) {
		if (i > 0) {
			ss.append(L",").append(L"\n");
		}
		auto item = supplier->getIdxRuntimeDatas().at(i);
		ss.append(blk);
		generateOneIndexSqlClause(item, ss, hasAutoIncrement);
	}
	return ss;
}

/**
 * Generate ONE index SQL clause.
 * 
 * @param item
 * @param ss
 * @param hasAutoIncrement
 */
void TableForeignkeysPageAdapter::generateOneIndexSqlClause(IndexInfo &item, std::wstring &ss, bool hasAutoIncrement)
{
	if (!item.name.empty()) {
		ss.append(L"CONSTRAINT \"").append(item.name).append(L"\"").append(L" ");
	}
	if (!item.type.empty()) {
		ss.append(StringUtil::toupper(item.type)).append(L"(");
	}
	if (!item.columns.empty()) {
		ss.append(item.columns);
	}

	if (hasAutoIncrement && item.type == TableStructureSupplier::idxTypeList[1]) {// idxTypeList[1] - Primary key
		ss.append(L" AUTOINCREMENT");
	}

	if (!item.type.empty()) {
		ss.append(L")");
	}
}

/**
 * verify if has duplicatedPrimary key in other row.
 * 
 * @param iItem - but not in iItem row
 * @return true - not duplicated, false - duplicated
 */
bool TableForeignkeysPageAdapter::verifyIfDuplicatedPrimaryKey(int iItem)
{
	auto & idxRuntimeDatas = supplier->getIdxRuntimeDatas();
	int n = static_cast<int>(idxRuntimeDatas.size());
	for (int i = 0; i < n; i++) {
		if (i == iItem) {
			continue;
		}
		auto & item = idxRuntimeDatas.at(i);
		if (item.type == supplier->idxTypeList.at(1)) { // 1 - primary key
			return false;
		}
	}

	return true;
}
