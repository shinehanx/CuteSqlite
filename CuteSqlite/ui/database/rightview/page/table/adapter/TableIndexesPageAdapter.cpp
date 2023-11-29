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
#include <stack>
#include "core/common/Lang.h"
#include "ui/common/message/QMessageBox.h"
#include "ui/database/rightview/page/table/dialog/TableIndexColumnsDialog.h"
#include "utils/EntityUtil.h"
#include "ui/common/message/QPopAnimate.h"

TableIndexesPageAdapter::TableIndexesPageAdapter(HWND parentHwnd, QListViewCtrl * listView, TableStructureSupplier * supplier)
{
	ATLASSERT(parentHwnd && listView && supplier);
	this->parentHwnd = parentHwnd;
	this->dataView = listView;
	this->supplier = supplier;
}

TableIndexesPageAdapter::~TableIndexesPageAdapter()
{
	
}

int TableIndexesPageAdapter::loadTblIndexesListView(uint64_t userDbId,const std::wstring & tblName, const std::wstring & schema)
{
	// headers
	loadHeadersForListView();

	// rows
	if (tblName.empty()) {
		return loadEmptyRowsForListView();
	}
	return loadIndexRowsForListView(userDbId, tblName, schema);
}

void TableIndexesPageAdapter::loadHeadersForListView()
{
	dataView->InsertColumn(0, L"", LVCFMT_LEFT, 26, -1, 0);
	
	int n = static_cast<int>(TableStructureSupplier::idxHeadColumns.size());
	for (int i = 0; i < n; i++) {
		auto column = TableStructureSupplier::idxHeadColumns.at(i);
		auto size = TableStructureSupplier::idxHeadSizes.at(i);
		auto format = TableStructureSupplier::idxHeadFormats.at(i);// LVCFMT_LEFT or LVCFMT_CENTER

		dataView->InsertColumn(i+1, column.c_str(), format, size);
	}
}

int TableIndexesPageAdapter::loadEmptyRowsForListView()
{
	/*
	// test data
	IndexInfo index1, index2;
	index1.columns = L"id"; // todo..., remove debug 
	index1.type = TableStructureSupplier::idxTypeList.at(1); //type 1: Primary
	index1.seq = std::chrono::system_clock::now(); // seq = current time 
	supplier->getIdxRuntimeDatas().push_back(index1);
	index2.columns = L"id,name,created_at,updated_at"; //type : Unique
	index2.type = TableStructureSupplier::idxTypeList.at(0); //type 0: Unique
	index2.seq = std::chrono::system_clock::now(); // seq = current time 
	supplier->getIdxRuntimeDatas().push_back(index2);
	dataView->SetItemCount(2);
	return 2;
	*/
	dataView->SetItemCount(0);
	return 0;
}


int TableIndexesPageAdapter::loadIndexRowsForListView(uint64_t userDbId, const std::wstring & tblName, const std::wstring & schema)
{
	auto idxRuntimeDatas = tableService->getIndexInfoList(userDbId, tblName, schema);
	supplier->setIdxRuntimeDatas(idxRuntimeDatas);
	supplier->setIdxOrigDatas(idxRuntimeDatas);
	int n = static_cast<int>(supplier->getIdxRuntimeDatas().size());
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
	int n = static_cast<int>(TableStructureSupplier::idxTypeList.size());
	for (int i = 0; i < n; i++) {
		if (dataType == TableStructureSupplier::idxTypeList.at(i)) {
			nSelItem = i;
			break;
		}
	}
	return nSelItem;
}


void TableIndexesPageAdapter::changePrimaryKey(ColumnInfoList & pkColumns)
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
LRESULT TableIndexesPageAdapter::fillDataInListViewSubItem(NMLVDISPINFO * pLvdi)
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
	} else if (pLvdi->item.iSubItem == 4 && pLvdi->item.mask & LVIF_TEXT){ // unique
		uint8_t val = supplier->getIdxRuntimeDatas().at(pLvdi->item.iItem).un;
		dataView->createCheckBox(iItem, pLvdi->item.iSubItem, val);
	} else if (pLvdi->item.iSubItem == 5 && pLvdi->item.mask & LVIF_TEXT){ // partial clause
		std::wstring & val = supplier->getIdxRuntimeDatas().at(pLvdi->item.iItem).partialClause;	
		StringCchCopy(pLvdi->item.pszText, pLvdi->item.cchTextMax, val.c_str());
	}

	return 0;
}

void TableIndexesPageAdapter::changeRuntimeDatasItem(int iItem, int iSubItem, std::wstring & origText, const std::wstring & newText)
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
	} else if (iSubItem == 4) { // index unique
		runtimeDatas[iItem].un = newText.empty() ? 0 
			: static_cast<uint8_t>(std::stoi(newText));
	} else if (iSubItem == 5) { // partial clause
		runtimeDatas[iItem].partialClause = newText; 
	}

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
	row.name = L"";
	supplier->getIdxRuntimeDatas().push_back(row);

	// 2.update the item count and selected the new row	
	int n = static_cast<int>(supplier->getIdxRuntimeDatas().size());
	dataView->SetItemCount(n);

}

bool TableIndexesPageAdapter::deleteSelIndexes(bool confirm)
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

		// 2.2 delete row from dataView 
		dataView->RemoveItem(nSelItem);		
	}
	
	return true;
}


void TableIndexesPageAdapter::removeSelectedItem(int nSelItem)
{
	supplier->eraseIdxRuntimeData(nSelItem);

	// 2 delete row from dataView
	dataView->RemoveItem(nSelItem);
}

void TableIndexesPageAdapter::deleteTableColumnName(const std::wstring & columnName)
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


std::wstring TableIndexesPageAdapter::getSubItemString(int iItem, int iSubItem)
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
	} else if (iSubItem == 4) {
		return std::to_wstring(supplier->getIdxRuntimeDatas().at(iItem).un);
	} else if (iSubItem == 5) {
		return supplier->getIdxRuntimeDatas().at(iItem).partialClause;
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
	if (clickItem->iSubItem == 2) { // button
		dataView->activeSubItem(clickItem->iItem, clickItem->iSubItem);
		return ;
	} else if (clickItem->iSubItem == 3) {
		dataView->showComboBox(clickItem->iItem, clickItem->iSubItem, TableStructureSupplier::idxTypeList, false);
		return ;	
	} else if (clickItem->iSubItem == 4) {
		dataView->activeSubItem(clickItem->iItem, clickItem->iSubItem);
		if (!changeListViewCheckBox(clickItem->iItem, clickItem->iSubItem)) {
			return;
		}
		return ;
	}

	if (clickItem->iSubItem > 0) {
		// show the editor
		dataView->createOrShowEditor(clickItem->iItem, clickItem->iSubItem);
	}
	
	// Clicked the next row of bottom item , will insert a new index row
	CPoint pt = clickItem->ptAction;
	CRect lastRowRect;
	dataView->GetItemRect(dataView->GetItemCount() - 1, lastRowRect, LVIR_BOUNDS);
	lastRowRect.OffsetRect(0, lastRowRect.Height());
	if (lastRowRect.PtInRect(pt)) {
		createNewIndex();
	}
}

/**
 * Check/Uncheck the checkBox in ListView.
 * 
 * @param iItem
 * @param iSubItem
 * @return 
 */
bool TableIndexesPageAdapter::changeListViewCheckBox(int iItem, int iSubItem)
{
	bool isChecked = dataView->getCheckBoxIsChecked(iItem, iSubItem);
	dataView->setCheckBoxIsChecked(iItem, iSubItem, !isChecked);
	std::wstring origVal = std::to_wstring((int)isChecked);
	std::wstring newVal = std::to_wstring((int)!isChecked);
	changeRuntimeDatasItem(iItem, iSubItem, origVal, newVal);
	invalidateSubItem(iItem, iSubItem);

	refreshPreviewSql();
	return true;
}

void TableIndexesPageAdapter::refreshPreviewSql()
{
	// send msg to TableStructurePage, class chain : TableIndexesPage($parentHwnd)->QTabView($tabView)->TableTabView->TableStructurePage
	HWND pHwnd = ::GetParent(::GetParent(::GetParent(parentHwnd)));
	::PostMessage(pHwnd, Config::MSG_TABLE_PREVIEW_SQL_ID, NULL, NULL);
}

/**
 * Generate indexes clause when generating create table DDL.
 * 
 * @param hasAutoIncrement
 * @return 
 */
std::wstring TableIndexesPageAdapter::generateConstraintsClause(bool hasAutoIncrement)
{
	std::wstring ss;
	int n = static_cast<int>(supplier->getIdxRuntimeDatas().size());
	wchar_t blk[5] = { 0, 0, 0, 0, 0 };
	wmemset(blk, 0x20, 4); // 4 blank chars

	int useIdx = 0;
	for (int i = 0; i < n; i++) {
		auto item = supplier->getIdxRuntimeDatas().at(i);
		if ((item.type.empty() && item.columns.empty() && item.name.empty()) 
			|| item.type == supplier->idxTypeList.at(3)) { // 3 - Index, ignore Index in create table ddl
			continue;
		}

		if (useIdx > 0) {
			ss.append(L",").append(L"\r\n");
		}
		ss.append(blk);
		generateOneConstraintClause(item, hasAutoIncrement, ss);
		useIdx++;
	}
	return ss;
}

std::vector<std::wstring> TableIndexesPageAdapter::generateCreateIndexesDDL(const std::wstring & schema, const std::wstring & tblName)
{
	std::vector<std::wstring> result;
	int n = static_cast<int>(supplier->getIdxRuntimeDatas().size());
	wchar_t blk[5] = { 0, 0, 0, 0, 0 };
	wmemset(blk, 0x20, 4); // 4 blank chars

	int useIdx = 0;
	for (int i = 0; i < n; i++) {
		auto item = supplier->getIdxRuntimeDatas().at(i);
		if (item.type != supplier->idxTypeList.at(3)) { // 3 - Index, ignore other constraints
			continue;
		}
		std::wstring ss;
		generateOneCreateIndexDDL(item, schema, tblName, ss);
		if (!ss.empty()) {
			result.push_back(ss);
		}
	}
	
	return result;
}

/**
 * Generate ONE index SQL clause.
 * 
 * @param item
 * @param ss
 * @param hasAutoIncrement
 */
void TableIndexesPageAdapter::generateOneConstraintClause(const IndexInfo &item, bool hasAutoIncrement, std::wstring &ss)
{
	if (!item.name.empty()) {
		ss.append(L"CONSTRAINT \"").append(item.name).append(L"\"").append(L" ");
	}
	if (!item.type.empty()) {
		ss.append(StringUtil::toupper(item.type)).append(L"(");
	}
	if (!item.columns.empty()) {
		std::wstring columns = StringUtil::addSymbolToWords(item.columns, L",", L"\"");
		ss.append(columns);
	}

	if (hasAutoIncrement && item.type == TableStructureSupplier::idxTypeList[1]) {// idxTypeList[1] - Primary key
		ss.append(L" AUTOINCREMENT");
	}

	if (!item.type.empty()) {
		ss.append(L")");
	}

	if (!item.partialClause.empty()) {
		ss.append(L" ").append(item.partialClause);
	}
}


void TableIndexesPageAdapter::generateOneCreateIndexDDL(const IndexInfo &item, const std::wstring & schema, const std::wstring &tblName, std::wstring &ss)
{
	const wchar_t * blk = L" ";
	const wchar_t * cma = L"\"";
	const wchar_t * cmb = L"(";
	const wchar_t * cmc = L")";
	ss.append(L"CREATE ");
	if (item.un) {
		ss.append(L"UNIQUE ");
	}
	ss.append(L"INDEX ");
	if (!schema.empty() && schema != L"main") {
		ss.append(schema).append(L".");
	}		
	ss.append(cma).append(item.name).append(cma).append(blk); 
	

	ss.append(L"ON ").append(cma).append(tblName).append(cma).append(blk);

	if (!item.columns.empty()) {
		std::wstring columns = StringUtil::addSymbolToWords(item.columns, L",", L"\"");
		ss.append(cmb).append(columns).append(cmc).append(blk);
	}
	
	if (!item.partialClause.empty()) {
		ss.append(item.partialClause);
	}
	ss.append(L";");
}

/**
 * verify if has duplicatedPrimary key in other row.
 * 
 * @param iItem - but not in iItem row
 * @return true - not duplicated, false - duplicated
 */
bool TableIndexesPageAdapter::verifyIfDuplicatedPrimaryKey(int iItem)
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

void TableIndexesPageAdapter::selectListViewItemForManage()
{
	auto & idxRuntimeDatas = supplier->getIdxRuntimeDatas();
	int n = static_cast<int>(idxRuntimeDatas.size());
	// check if send the message of delete columns from LeftTreeView, then select the associated row from this list view
	if (n > 0 && !databaseSupplier->selectedIndexName.empty() 
		&& supplier->getRuntimeUserDbId() == databaseSupplier->getSelectedUserDbId()
		&& supplier->getRuntimeTblName() == databaseSupplier->selectedTable) {
		int i = 0;
		for (int i = 0; i < n; i++) {
			auto & item = idxRuntimeDatas.at(i);
			std::wstring name = !item.name.empty() ? item.name
				: item.type + L"(" + item.columns + L")";
			if (name == databaseSupplier->selectedIndexName) {
				dataView->SelectItem(i);
				break;
			} else if (!databaseSupplier->selectedIndexName.empty() 
				&& databaseSupplier->selectedIndexName.find(L"sqlite_autoindex_") != std::wstring::npos
				&& item.name.empty() && item.type == L"Primary Key" && item.ai == 1) {
				dataView->SelectItem(i);
			}
		}		
	}
}

bool TableIndexesPageAdapter::isDirty()
{
	bool hasChangeSubItem = dataView && dataView->IsWindow() ? dataView->getChangedCount() : false;	
	return hasChangeSubItem || !supplier->compareIdxDatas();
}

