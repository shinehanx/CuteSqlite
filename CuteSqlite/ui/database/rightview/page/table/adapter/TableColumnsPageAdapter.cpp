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
#include <sstream>
#include <Strsafe.h>
#include "common/AppContext.h"
#include "core/common/Lang.h"
#include "ui/common/message/QMessageBox.h"
#include "ui/common/message/QPopAnimate.h"

TableColumnsPageAdapter::TableColumnsPageAdapter(HWND parentHwnd, QListViewCtrl * listView, TableStructureSupplier * supplier)
{
	ATLASSERT(parentHwnd && listView && supplier);
	this->parentHwnd = parentHwnd;
	this->dataView = listView;
	this->supplier = supplier;
}

TableColumnsPageAdapter::~TableColumnsPageAdapter()
{
	
}

int TableColumnsPageAdapter::loadTblColumnsListView()
{
	// headers
	loadHeadersForListView();

	// rows
	if (supplier->getRuntimeTblName().empty()) {
		return loadEmptyRowsForListView();
	}
	return loadColumnRowsForListView(supplier->getRuntimeUserDbId(), supplier->getRuntimeSchema(), supplier->getRuntimeTblName());
}

void TableColumnsPageAdapter::loadHeadersForListView()
{
	dataView->InsertColumn(0, L"", LVCFMT_CENTER, 26, -1, 0);
	
	int n = static_cast<int>(TableStructureSupplier::colsHeadColumns.size());
	for (int i = 0; i < n; i++) {
		auto column = TableStructureSupplier::colsHeadColumns.at(i);
		auto size = TableStructureSupplier::colsHeadSizes.at(i);
		auto format = TableStructureSupplier::colsHeadFormats.at(i);// LVCFMT_LEFT or LVCFMT_CENTER

		dataView->InsertColumn(i+1, column.c_str(), format, size);
	}
}

int TableColumnsPageAdapter::loadEmptyRowsForListView()
{
	std::vector<std::wstring> ss = StringUtil::split(L"id,name,type,class_id,inspection_id,remark,phone,email,created_at,updated_at",L","); // tmp
	int n = static_cast<int>(ss.size());
	for (int i = 0; i < n; i++) {
		ColumnInfo columnInfo;
		columnInfo.name = ss.at(i);
		if (i == 0 || i== 3 || i == 4) {
			columnInfo.type = L"INTEGER";
			columnInfo.defVal = L"0";
		}else {
			columnInfo.type = L"TEXT";
			columnInfo.defVal = L"''";
		}
		columnInfo.notnull = 1;
		columnInfo.seq = std::chrono::system_clock::now(); // seq = current time 
		supplier->getColsRuntimeDatas().push_back(columnInfo);
	}
	dataView->SetItemCount(n);
	dataView->Invalidate(true);
	return n;
}


int TableColumnsPageAdapter::loadColumnRowsForListView(uint64_t userDbId, const std::wstring & schema, const std::wstring & tblName)
{
	auto colsRuntimeDatas = tableService->getUserColumns(userDbId, tblName, schema, false);
	supplier->setColsRuntimeDatas(colsRuntimeDatas);
	supplier->setColsOrigDatas(colsRuntimeDatas);
	int n = static_cast<int>(supplier->getColsRuntimeDatas().size());
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
	int n = static_cast<int>(TableStructureSupplier::colsDataTypeList.size());
	for (int i = 0; i < n; i++) {
		if (dataType == TableStructureSupplier::colsDataTypeList.at(i)) {
			nSelItem = i;
			break;
		}
	}
	return nSelItem;
}

/**
 * Verify the auto increment has exists that item.ai has checked through search the runtimeDatas.
 * 
 * @param iItem
 * @return iItem that other row index, -1 is not found
 */
int TableColumnsPageAdapter::verifyExistsOtherAutoIncrement(int iItem)
{
	ATLASSERT(iItem >= 0 && iItem < static_cast<int>(supplier->getColsRuntimeDatas().size()));
	int n = static_cast<int>(supplier->getColsRuntimeDatas().size());
	for (int i = 0; i < n; i++) {
		if (i == iItem) {
			continue;
		}
		auto item = supplier->getColsRuntimeDatas().at(i);
		if (item.ai) { // The other item that it's ai state is checked will be forbidden 
			return i;
		}
		
	}
	return -1; // not found
}

/**
 * Verify the data type is allow auto increment in the same row.
 * The data type must be INTEGER
 * 
 * @param iItem
 * @return true - allow auto increment, otherwise not allowed
 */
bool TableColumnsPageAdapter::verifyDataTypeAllowAutoIncrement(int iItem)
{
	ATLASSERT(iItem >= 0 && iItem < static_cast<int>(supplier->getColsRuntimeDatas().size()));
	auto item = supplier->getColsRuntimeDatas().at(iItem);
	if (item.type == TableStructureSupplier::colsDataTypeList.at(0)) { // datatypeList[0] - INTEGER;
		return true; // 3 - Primary key's iSubItem
	}
	return false; // not found
}

/**
 * Verify the primary key has checked in the same row.
 * 
 * @param iItem
 * @return if found return iSubItem = 3, then return -1 that be not found
 */
int TableColumnsPageAdapter::verifyExistsPrimaryKeyInSameRow(int iItem)
{
	ATLASSERT(iItem >= 0 && iItem < static_cast<int>(supplier->getColsRuntimeDatas().size()));
	auto item = supplier->getColsRuntimeDatas().at(iItem);
	if (item.pk) {
		return 4; // 4 - Primary key's iSubItem
	}
	return -1; // not found
}

/**
 * Valid primary key in same row .
 * 
 * @param iItem
 */
void TableColumnsPageAdapter::validPrimaryKeyInSameRow(int iItem)
{
	int pkSubItem = -1;
	if ((pkSubItem = verifyExistsPrimaryKeyInSameRow(iItem)) == -1) {
		pkSubItem = 4; // 4 - The subitem index of Primary key
		dataView->setCheckBoxIsChecked(iItem, pkSubItem, true);
		changeRuntimeDatasItem(iItem, pkSubItem, std::wstring(L"1"));
		invalidateSubItem(iItem, pkSubItem);// set the ai column be checked
	}
}

/**
 * Invalid exists primary key in other row .
 * 
 * @param iItem
 */
void TableColumnsPageAdapter::invalidExistsPrimaryKeyInOtherRow(int iItem)
{
	ATLASSERT(iItem >= 0 && iItem < static_cast<int>(supplier->getColsRuntimeDatas().size()));
	int n = static_cast<int>(supplier->getColsRuntimeDatas().size());
	for (int i = 0; i < n; i++) {
		if (i == iItem) {
			continue;
		}
		ColumnInfo & item = supplier->getColsRuntimeDatas().at(i);
		if (item.pk) { // The other item that it's ai state is checked will be forbidden 
			item.pk = 0;
			dataView->setCheckBoxIsChecked(i, 4, 0); //iSubItem = 4 - primary key
			invalidateSubItem(i, 4);
		}
		
	}
}

/**
 * Invalid exists primary key in other row .
 * The function for invalid primary key in same row
 * 
 * @param iItem
 */
void TableColumnsPageAdapter::invalidExistsAutoIncrementInSameRow(int iItem)
{
	ATLASSERT(iItem >= 0 && iItem < static_cast<int>(supplier->getColsRuntimeDatas().size()));
	ColumnInfo & item = supplier->getColsRuntimeDatas().at(iItem);
	if (item.ai) { 
		item.ai = 0;
		dataView->setCheckBoxIsChecked(iItem, 5, 0); //iSubItem = 5 - auto increment
		invalidateSubItem(iItem, 5);
	}
}

ColumnInfo TableColumnsPageAdapter::getRuntimeData(int nItem) const 
{
	ATLASSERT(nItem < static_cast<int>(supplier->getColsRuntimeDatas().size()));
	return supplier->getColsRuntimeDatas().at(nItem);
}

std::wstring TableColumnsPageAdapter::genderateCreateColumnsSqlClause() const
{
	std::wstring ss;
	int n = static_cast<int>(supplier->getColsRuntimeDatas().size());
	wchar_t blk[5] = { 0 };
	wchar_t * blkc = L" ";
	wmemset(blk, 0x20, 4); // 4 blank chars
	for (int i = 0; i < n; i++) {
		if (i > 0) {
			ss.append(L",").append(L"\n");
		}
		auto item = supplier->getColsRuntimeDatas().at(i);
		ss.append(blk).append(L"\"").append(item.name).append(L"\"").append(blkc).append(item.type) ;
		if (item.notnull) {
			ss.append(blkc).append(L"NOT NULL");
		}
		if (!item.defVal.empty()) {
			ss.append(blkc).append(L"DEFAULT").append(blkc).append(L"(").append(item.defVal).append(L")");
		}
		if (item.un) {
			ss.append(blkc).append(L"UNIQUE");
		}

		if (!item.checks.empty()) {
			ss.append(blkc).append(L"CHECK(").append(item.checks).append(L")");
		}
	}
	return ss;
}

/**
 * Generate columns clause for insert into statement , such as "Insert into tbl_name (**here is pair->first**) SELECT **here is pair->second**".
 * 
 * @return pair - first is new columns , second is old columns
 */
std::pair<std::wstring,std::wstring> TableColumnsPageAdapter::generateInsertColumnsClause()
{
	std::wstring str1, str2;
	auto & colsRuntimeDatas = supplier->getColsRuntimeDatas();
	auto & colsOrigDatas = supplier->getColsOrigDatas();
	wchar_t blk[5] = { 0 };
	wchar_t * blkc = L" ";
	wchar_t * quo = L"\"";
	int n = static_cast<int>(colsRuntimeDatas.size());
	for (int i = 0; i < n; i++) {		
		auto & item = colsRuntimeDatas.at(i);
		auto iter = std::find_if(colsOrigDatas.begin(), colsOrigDatas.end(), [&item](ColumnInfo & origInfo) {
			if (item.seq == origInfo.seq) {
				return true;
			}
			return false;
		});

		if (iter == colsOrigDatas.end()) {
			continue;
		}
		if (i > 0) {
			str1.append(L",");
			str2.append(L",");
		}
		str1.append(quo).append(item.name).append(quo);
		str2.append(quo).append((*iter).name).append(quo);

	}
	return {str1, str2};
}

std::wstring TableColumnsPageAdapter::genderateAlterColumnsSqlClauseForMysql()
{
	std::wstring ss;
	auto & colsRuntimeDatas = supplier->getColsRuntimeDatas();
	auto & colsOrigDatas = supplier->getColsOrigDatas();

	wchar_t blk[5] = { 0 };
	wchar_t * blkc = L" ";
	// 1.alter table by change column name or change column properties 
	int n = static_cast<int>(colsRuntimeDatas.size());
	for (int i = 0; i < n; i++) {
		if (i > 0) {
			ss.append(L",").append(L"\n");
		}

		auto & item = colsRuntimeDatas.at(i);
		auto iter = std::find_if(colsOrigDatas.begin(), colsOrigDatas.end(), [&item](ColumnInfo & info) {
			if (item.seq == info.seq) {
				return true;
			}
			return false;
		});
		if (iter == colsOrigDatas.end()) {
			ss.append(L" ADD COLUMN \"").append(item.name).append(L"\"");
		} else {
			if (item.name != (*iter).name) {
				ss.append(L" RENAME COLUMN \"").append((*iter).name).append(L"\" TO \"")
					.append(item.name);
			} else if (item.name != (*iter).name && (
				item.notnull != (*iter).notnull || item.pk != (*iter).pk && item.ai != (*iter).ai || item.un != (*iter).un
				)){
				ss.append(L" ALTER COLUMN \"").append((*iter).name).append(L"\"");
			}
		}

		ss.append(blkc).append(L"\"").append(item.name).append(L"\"").append(blkc).append(item.type) ;
		if (item.notnull) {
			ss.append(L" NOT NULL");
		}
		if (!item.defVal.empty()) {
			ss.append(L" DEFAULT").append(L" (").append(item.defVal).append(L")");
		}
		if (item.un) {
			ss.append(L" UNIQUE");
		}

		if (!item.checks.empty()) {
			ss.append(blkc).append(L"CHECK(").append(item.checks).append(L")");
		}
	}
	return ss;
}

ColumnInfoList TableColumnsPageAdapter::getPrimaryKeyColumnInfoList()
{
	ColumnInfoList result;
	if (supplier->getColsRuntimeDatas().empty()) {
		return result;
	}

	for (auto item : supplier->getColsRuntimeDatas()) {
		if (item.pk) {
			result.push_back(item);
		}
	}

	return result;
}


bool TableColumnsPageAdapter::verifyExistsAutoIncrement()
{
	ColumnInfoList result;
	if (supplier->getColsRuntimeDatas().empty()) {
		return false;
	}

	for (auto item : supplier->getColsRuntimeDatas()) {
		if (item.ai) {
			return true;
		}
	}

	return false;
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

	auto count = static_cast<int>(supplier->getColsRuntimeDatas().size());
	if (!count || count <= iItem)
		return 0;

	// set checked/unchecked image in the first column	
	if (pLvdi->item.iSubItem == 0 && pLvdi->item.mask & LVIF_TEXT) { // row checkBox - 0
		pLvdi->item.mask = LVIF_IMAGE;
		pLvdi->item.iIndent = 1;
		if (getIsChecked(pLvdi->item.iItem)) { 
			pLvdi->item.iImage = 1;
		} else {
			pLvdi->item.iImage = 0;
		}
		return 0;
	} else  if (pLvdi->item.iSubItem == 2 && pLvdi->item.mask & LVIF_TEXT) { // set dataType - 2
		std::wstring & val = supplier->getColsRuntimeDatas().at(pLvdi->item.iItem).type;
		dataView->createComboBox(iItem, pLvdi->item.iSubItem, val);
		StringCchCopy(pLvdi->item.pszText, pLvdi->item.cchTextMax, val.c_str());
		return 0;
	} else if (pLvdi->item.iSubItem >= 3 && pLvdi->item.iSubItem <= 6 && (pLvdi->item.mask & LVIF_TEXT)) {
		uint8_t val = 0;
		if (pLvdi->item.iSubItem == 3) { // notnull - 3
			val = supplier->getColsRuntimeDatas().at(pLvdi->item.iItem).notnull;
		} else if (pLvdi->item.iSubItem == 4) { // primary key - 4
			val = supplier->getColsRuntimeDatas().at(pLvdi->item.iItem).pk;
		} else if (pLvdi->item.iSubItem == 5) { // auto increment - 5
			val = supplier->getColsRuntimeDatas().at(pLvdi->item.iItem).ai;
		} else if (pLvdi->item.iSubItem == 6) { // unique - 6
			val = supplier->getColsRuntimeDatas().at(pLvdi->item.iItem).un;
		}
		dataView->createCheckBox(iItem, pLvdi->item.iSubItem, val);
		return 0;
	} else if (pLvdi->item.iSubItem == 1 && pLvdi->item.mask & LVIF_TEXT){ // column name - 1
		std::wstring & val = supplier->getColsRuntimeDatas().at(pLvdi->item.iItem).name;	
		StringCchCopy(pLvdi->item.pszText, pLvdi->item.cchTextMax, val.c_str());
	} else if (pLvdi->item.iSubItem == 7 && pLvdi->item.mask & LVIF_TEXT){ // default value - 7
		std::wstring & val = supplier->getColsRuntimeDatas().at(pLvdi->item.iItem).defVal;	
		StringCchCopy(pLvdi->item.pszText, pLvdi->item.cchTextMax, val.c_str());
	} else if (pLvdi->item.iSubItem == 8 && pLvdi->item.mask & LVIF_TEXT){ // check - 8
		std::wstring & val = supplier->getColsRuntimeDatas().at(pLvdi->item.iItem).checks;	
		StringCchCopy(pLvdi->item.pszText, pLvdi->item.cchTextMax, val.c_str());
	}

	return 0;
}

void TableColumnsPageAdapter::changeRuntimeDatasItem(int iItem, int iSubItem, const std::wstring & newText)
{
	ATLASSERT(iItem >= 0 && iSubItem > 0);
	
	if (iSubItem == 1) { // column name
		supplier->getColsRuntimeDatas()[iItem].name = newText;
	} else if (iSubItem == 2) { // column name
		supplier->getColsRuntimeDatas()[iItem].type = newText;
	} else if (iSubItem == 3) { // not null
		supplier->getColsRuntimeDatas()[iItem].notnull = newText.empty() ? 0 
			: static_cast<uint8_t>(std::stoi(newText));
	} else if (iSubItem == 4) { // pk : primary key
		supplier->getColsRuntimeDatas()[iItem].pk = newText.empty() ? 0 
			: static_cast<uint8_t>(std::stoi(newText));
	} else if (iSubItem == 5) { // ai : auto increment
		supplier->getColsRuntimeDatas()[iItem].ai = newText.empty() ? 0 
			: static_cast<uint8_t>(std::stoi(newText));
	} else if (iSubItem == 6) { // un : unique
		supplier->getColsRuntimeDatas()[iItem].un = newText.empty() ? 0 
			: static_cast<uint8_t>(std::stoi(newText));
	}  else if (iSubItem == 7) { // default value
		supplier->getColsRuntimeDatas()[iItem].defVal = newText;
	} else if (iSubItem == 8) { // check 
		supplier->getColsRuntimeDatas()[iItem].checks = newText;
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
	supplier->getColsRuntimeDatas().push_back(columnRow);

	// 2.update the item count and selected the new row	
	int n = static_cast<int>(supplier->getColsRuntimeDatas().size());
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
	bool foundIndex = false;
	int nSelItem = -1;
	while ((nSelItem = dataView->GetNextItem(nSelItem, LVNI_SELECTED)) != -1) {
		nSelItems.push_back(nSelItem);

		std::wstring columnName = supplier->getColsRuntimeDatas().at(nSelItem).name;
		if (existsColumnNameInRuntimeIndexes(columnName)) {
			foundIndex = true;
		}
	}

	if (foundIndex && QMessageBox::confirm(parentHwnd, S(L"delete-column-confirm-text"), S(L"yes"), S(L"no")) == Config::CUSTOMER_FORM_NO_BUTTON_ID) {
		return false;
	}

	if (nSelItems.empty()) {
		return false;
	}

	// 2.delete from runtimeDatas and database and dataView that the item begin from the last selected item
	int n = static_cast<int>(nSelItems.size());
	for (int i = n - 1; i >= 0; i--) {
		nSelItem = nSelItems.at(i);
		auto iter = supplier->getColsRuntimeDatas().begin();
		for (int j = 0; j < nSelItem; j++) {
			iter++;
		}
		auto & rowItem = (*iter);
		std::wstring * columnName1 = new std::wstring(rowItem.name); // will be delete by TableIndexesPage::OnTableColumsDeleteColumnName
		std::wstring * columnName2 = new std::wstring(rowItem.name); // will be delete by TableForeignkeysPage::OnTableColumsDeleteColumnName

		// 2.1 delete row from runtimeDatas vector 
		supplier->getColsRuntimeDatas().erase(iter);

		// 2.2 delete row from dataView
		dataView->RemoveItem(nSelItem);

		// TableColumnsPage($parentHwnd)->QTabView($tabView)->QTableTabView  ------ then trans to --> TableIndexesPage and TableForeignkeysPage
		HWND pHwnd = ::GetParent(::GetParent(parentHwnd));
		::PostMessage(pHwnd, Config::MSG_TABLE_COLUMNS_DELETE_COLUMN_NAME_ID, WPARAM(columnName1), LPARAM(columnName2));
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
		auto iter = supplier->getColsRuntimeDatas().begin();
		for (int j = 0; j < nSelItem; j++) {
			iter++;
		}
		auto & rowItem = (*iter);
		auto prevIter = iter == std::begin(supplier->getColsRuntimeDatas()) ? 
			supplier->getColsRuntimeDatas().end() : std::prev(iter);		

		if (prevIter != supplier->getColsRuntimeDatas().end()) {
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
		auto iter = supplier->getColsRuntimeDatas().begin();
		for (int j = 0; j < nSelItem; j++) {
			iter++;
		}
		auto & rowItem = (*iter);
		auto nextIter = iter == std::end(supplier->getColsRuntimeDatas()) ? 
			supplier->getColsRuntimeDatas().end() : std::next(iter);

		if (nextIter != supplier->getColsRuntimeDatas().end()) {
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
			dataView->showComboBox(clickItem->iItem, clickItem->iSubItem, TableStructureSupplier::colsDataTypeList, true);
		} else if (clickItem->iSubItem >= 3 && clickItem->iSubItem <= 6) {
			dataView->activeSubItem(clickItem->iItem, clickItem->iSubItem);
			if (!changeListViewCheckBox(clickItem->iItem, clickItem->iSubItem)) {
				return;
			}
		}
		
		return ;
	}

	// show the editor
	dataView->createOrShowEditor(clickItem->iItem, clickItem->iSubItem);
}

/**
 * Check/Uncheck the checkBox in ListView.
 * 
 * @param iItem
 * @param iSubItem
 * @return 
 */
bool TableColumnsPageAdapter::changeListViewCheckBox(int iItem, int iSubItem)
{
	bool isChecked = dataView->getCheckBoxIsChecked(iItem, iSubItem);

	// verify if there is auto increment in other row
	if (iSubItem == 4 && !isChecked) {
		if (verifyExistsOtherAutoIncrement(iItem) != -1) {
			QPopAnimate::error(parentHwnd, S(L"ai-fobidden-multi-pk"));
			return false;
		}
	} else if (iSubItem == 4 && isChecked) {
		// invalid auto increment in same row
		invalidExistsAutoIncrementInSameRow(iItem);
	}

	// check other row has auto increment
	if (iSubItem == 5 && !isChecked) {
		if (verifyExistsOtherAutoIncrement(iItem) != -1) {
			QPopAnimate::error(parentHwnd, S(L"ai-column-exists"));
			return false;
		}
		// data type must INTEGER
		if (!verifyDataTypeAllowAutoIncrement(iItem)) {
			QPopAnimate::error(parentHwnd, S(L"ai-column-must-integer"));
			return false;
		}
		// valid primary key in same row
		validPrimaryKeyInSameRow(iItem);
		// invalid primary key in other row
		invalidExistsPrimaryKeyInOtherRow(iItem);
	}
	dataView->setCheckBoxIsChecked(iItem, iSubItem, !isChecked);
	std::wstring origVal = std::to_wstring((int)isChecked);
	std::wstring newVal = std::to_wstring((int)!isChecked);
	changeRuntimeDatasItem(iItem, iSubItem, newVal);
	invalidateSubItem(iItem, iSubItem);

	// Send this msg when changing the table index in the TableColumnsPage to TableIndexesPage, wParam=NULL, lParam=NULL
	if (iSubItem == 4 || iSubItem == 5) {
		//TableColumnsPage($parentHwnd)->QTabView($tabView)->TableTabView ------ trans to --> TableIndexesPage
		HWND pHwnd = ::GetParent(::GetParent(parentHwnd));
		::PostMessage(pHwnd, Config::MSG_TABLE_COLUMNS_CHANGE_PRIMARY_KEY_ID, NULL, NULL);
	}

	// send msg to TableStructurePage, class chain : TableColumnsPage($parentHwnd)->QTabView($tabView)->TableTabView->TableStructurePage
	HWND pHwnd = ::GetParent(::GetParent(::GetParent(parentHwnd)));
	::PostMessage(pHwnd, Config::MSG_TABLE_PREVIEW_SQL_ID, NULL, NULL);

	return true;
}


bool TableColumnsPageAdapter::existsColumnNameInRuntimeIndexes(const std::wstring & columnName)
{
	ATLASSERT(!columnName.empty());
	IndexInfoList & indexes = supplier->getIdxRuntimeDatas();
	int n = static_cast<int>(indexes.size());
	for (int i = 0; i < n; i++) {
		auto & item = indexes.at(i);
		auto columns = StringUtil::split(item.columns, L",");
		auto iter = std::find(columns.begin(), columns.end(), columnName);
		if (iter == columns.end()) {
			continue;
		}
		return true;
	}
	return false;
}

/**
 * Get all column names from dataView.
 * 
 * @param excludeNames
 * @return 
 */
std::vector<std::wstring> TableColumnsPageAdapter::getAllColumnNames(const std::vector<std::wstring> & excludeNames) const
{
	std::vector<std::wstring> result;

	int n = static_cast<int>(supplier->getColsRuntimeDatas().size());
	for (int i = 0; i < n; i++) {
		auto data = supplier->getColsRuntimeDatas().at(i);
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

