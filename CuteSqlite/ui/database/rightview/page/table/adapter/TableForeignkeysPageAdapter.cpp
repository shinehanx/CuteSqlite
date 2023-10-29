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
	ForeignKey item1{L"ctsqlite_auto_foreign_key_1", L"Foreign Key", L"class_id", L"analysis_sample_class", L"id", L"CASCADE", L"SET NULL"};
	ForeignKey item2{L"ctsqlite_auto_foreign_key_2", L"Foreign Key", L"inspection_id", L"analysis_hair_inspection", L"id", L"CASCADE", L"SET NULL"};
	supplier->setFrkRuntimeDatas(ForeignKeyList({ item1 , item2 }));
	dataView->SetItemCount(2);
	return 1;
}


int TableForeignkeysPageAdapter::loadForeignkeyRowsForListView(uint64_t userDbId, const std::wstring & tblName, const std::wstring & schema)
{
	auto frkRuntimeDatas = tableService->getForeignKeyList(userDbId, tblName, schema);
	supplier->setFrkRuntimeDatas(frkRuntimeDatas);
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
	int nSelItem = dataView->GetNextItem(-1, LVNI_ALL | LVNI_SELECTED); // 向下搜索选中的项 -1表示先找出第一个
	
	while (nSelItem != -1) {
		if (nSelItem == iItem) {
			return true;
		}
		nSelItem = dataView->GetNextItem(nSelItem, LVNI_ALL | LVNI_SELECTED); // 继续往下搜索选中项
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
	int n = static_cast<int>(supplier->getFrkRuntimeDatas().size());
	std::vector<int> nSelItems;
	for (int i = 0; i < n; i++) {
		auto item = supplier->getFrkRuntimeDatas().at(i);
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
	ForeignKey row;
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
		supplier->getFrkRuntimeDatas().insert(supplier->getFrkRuntimeDatas().begin(), row); // insert to the first
	} else {
		auto & oneIndex = supplier->getFrkRuntimeDatas().at(nSelItems.at(0));
		oneIndex.columns = row.columns; // modify columns string
		invalidateSubItem(nSelItems.at(0), 1); // 2th param = 1 - primary key
	}

	// update the item count and selected the new row	
	n = static_cast<int>(supplier->getFrkRuntimeDatas().size());
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

	auto count = static_cast<int>(supplier->getFrkRuntimeDatas().size());
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
	} else  if (pLvdi->item.iSubItem == 1 && pLvdi->item.mask & LVIF_TEXT) { // name
		std::wstring & val = supplier->getFrkRuntimeDatas().at(pLvdi->item.iItem).name;
		StringCchCopy(pLvdi->item.pszText, pLvdi->item.cchTextMax, val.c_str());
		return 0;
	} else if (pLvdi->item.iSubItem == 2 && pLvdi->item.mask & LVIF_TEXT){ // columns
		std::wstring & val = supplier->getFrkRuntimeDatas().at(pLvdi->item.iItem).columns;	
		StringCchCopy(pLvdi->item.pszText, pLvdi->item.cchTextMax, val.c_str());
		dataView->createButton(iItem, pLvdi->item.iSubItem, L"...");
	} else if (pLvdi->item.iSubItem == 3 && pLvdi->item.mask & LVIF_TEXT){ // referenced table
		std::wstring & val = supplier->getFrkRuntimeDatas().at(pLvdi->item.iItem).referencedTable;
		StringCchCopy(pLvdi->item.pszText, pLvdi->item.cchTextMax, val.c_str());
		dataView->createComboBox(iItem, pLvdi->item.iSubItem, val);
	} else if (pLvdi->item.iSubItem == 4 && pLvdi->item.mask & LVIF_TEXT){ // referenced columns
		std::wstring & val = supplier->getFrkRuntimeDatas().at(pLvdi->item.iItem).referencedColumns;	
		StringCchCopy(pLvdi->item.pszText, pLvdi->item.cchTextMax, val.c_str());
		dataView->createButton(iItem, pLvdi->item.iSubItem, L"...");
	} else if (pLvdi->item.iSubItem == 5 && pLvdi->item.mask & LVIF_TEXT){ // referenced columns
		std::wstring & val = supplier->getFrkRuntimeDatas().at(pLvdi->item.iItem).onUpdate;	
		StringCchCopy(pLvdi->item.pszText, pLvdi->item.cchTextMax, val.c_str());
		dataView->createComboBox(iItem, pLvdi->item.iSubItem, val);
	} else if (pLvdi->item.iSubItem == 6 && pLvdi->item.mask & LVIF_TEXT){ // referenced columns
		std::wstring & val = supplier->getFrkRuntimeDatas().at(pLvdi->item.iItem).onDelete;	
		StringCchCopy(pLvdi->item.pszText, pLvdi->item.cchTextMax, val.c_str());
		dataView->createComboBox(iItem, pLvdi->item.iSubItem, val);
	}

	return 0;
}

void TableForeignkeysPageAdapter::changeRuntimeDatasItem(int iItem, int iSubItem, std::wstring & origText, const std::wstring & newText)
{
	ATLASSERT(iItem >= 0 && iSubItem > 0);
	
	auto & runtimeDatas = supplier->getFrkRuntimeDatas();
	if (iSubItem == 1) { // column name
		runtimeDatas[iItem].name = newText;
	} else if (iSubItem == 2) { // columns
		runtimeDatas[iItem].columns = newText;
	} else if (iSubItem == 3) { // referenced column
		runtimeDatas[iItem].referencedTable = newText;
	} else if (iSubItem == 4) { // referenced column
		runtimeDatas[iItem].referencedColumns = newText;
	} else if (iSubItem == 5) { // on update
		runtimeDatas[iItem].onUpdate = newText;
	} else if (iSubItem == 6) { // on delete
		runtimeDatas[iItem].onDelete = newText;
	}

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
	ForeignKey row;
	row.name = L"";
	supplier->getFrkRuntimeDatas().push_back(row);

	// 2.update the item count and selected the new row	
	int n = static_cast<int>(supplier->getFrkRuntimeDatas().size());
	dataView->SetItemCount(n);

}

bool TableForeignkeysPageAdapter::deleteSelForeignKeys(bool confirm)
{
	if (!dataView->GetSelectedCount()) {
		return false;
	}
	if (confirm && QMessageBox::confirm(parentHwnd, S(L"delete-foreign-confirm-text"), S(L"yes"), S(L"no")) == Config::CUSTOMER_FORM_NO_BUTTON_ID) {
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
		auto & foreignKey = supplier->getFrkRuntimeDatas().at(nSelItem);
	
		// 2.1 delete row from runtimeDatas vector and origDatas
		supplier->eraseFrkRuntimeData(nSelItem);

		// 2.2 delete row from dataView 
		dataView->RemoveItem(nSelItem);		
	}
	
	return true;
}


void TableForeignkeysPageAdapter::removeSelectedItem(int nSelItem)
{
	supplier->eraseFrkRuntimeData(nSelItem);

	// 2 delete row from dataView
	dataView->RemoveItem(nSelItem);
}

void TableForeignkeysPageAdapter::deleteTableColumnName(const std::wstring & columnName)
{
	ATLASSERT(!columnName.empty());
	ForeignKeyList & indexes = supplier->getFrkRuntimeDatas();

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
	if (supplier->getFrkRuntimeDatas().empty()) {
		return L"";
	}
	if (iSubItem == 1) {
		return supplier->getFrkRuntimeDatas().at(iItem).name;
	} else if (iSubItem == 2) {
		return supplier->getFrkRuntimeDatas().at(iItem).columns;
	} else if (iSubItem == 3) {
		return supplier->getFrkRuntimeDatas().at(iItem).referencedTable;
	} else if (iSubItem == 4) {
		return supplier->getFrkRuntimeDatas().at(iItem).referencedColumns;
	} else if (iSubItem == 5) {
		return supplier->getFrkRuntimeDatas().at(iItem).onUpdate;
	} else if (iSubItem == 6) {
		return supplier->getFrkRuntimeDatas().at(iItem).onDelete;
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
	} else if (clickItem->iSubItem == 2 || clickItem->iSubItem == 4) { // 2 - columns, 4 - referenced columns
		dataView->activeSubItem(clickItem->iItem, clickItem->iSubItem);
		return ;
	} else if (clickItem->iSubItem == 3) { // 3 - a
		UserTableList userTableList = tableService->getUserTables(supplier->getRuntimeUserDbId());
		std::vector<std::wstring> tblNames;
		for (auto & table : userTableList) {
			if (table.name == supplier->getRuntimeTblName()) {
				continue;
			}
			tblNames.push_back(table.name);
		}
		dataView->showComboBox(clickItem->iItem, clickItem->iSubItem, tblNames, false);
		return ;	
	} else if (clickItem->iSubItem == 5) { // 5. on update
		dataView->showComboBox(clickItem->iItem, clickItem->iSubItem, supplier->frkOnUpdateTypeList, false);
		return ;
	} else if (clickItem->iSubItem == 6) { // 5. on delete
		dataView->showComboBox(clickItem->iItem, clickItem->iSubItem, supplier->frkOnDeleteTypeList, false);
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
std::wstring TableForeignkeysPageAdapter::genderateCreateForeignKeyClause()
{
	std::wstring ss;
	int n = static_cast<int>(supplier->getFrkRuntimeDatas().size());
	wchar_t blk[5] = { 0, 0, 0, 0, 0 };
	wmemset(blk, 0x20, 4); // 4 blank chars
	for (int i = 0; i < n; i++) {
		if (i > 0) {
			ss.append(L",").append(L"\n");
		}
		auto item = supplier->getFrkRuntimeDatas().at(i);
		ss.append(blk);
		generateOneForeignKeyClause(item, ss);
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
void TableForeignkeysPageAdapter::generateOneForeignKeyClause(ForeignKey &item, std::wstring &ss)
{
	if (!item.name.empty()) {
		ss.append(L"CONSTRAINT \"").append(item.name).append(L"\"").append(L" ");
	}
	ss.append(L"FOREIGN KEY ");
	if (!item.columns.empty()) {
		ss.append(L"(").append(item.columns).append(L") ");
	}
	ss.append(L"REFERENCES ");
	if (!item.referencedTable.empty()) {
		ss.append(L"\"").append(item.referencedTable).append(L"\" ");
	}
	if (!item.referencedColumns.empty()) {
		ss.append(L"(").append(item.referencedColumns).append(L") ");
	}

	if (!item.onDelete.empty()) {
		ss.append(L"ON DELETE ").append(item.onDelete).append(L" ");
	}

	if (!item.onUpdate.empty()) {
		ss.append(L"ON UPDATE ").append(item.onUpdate).append(L" ");
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
	auto & idxRuntimeDatas = supplier->getFrkRuntimeDatas();
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

