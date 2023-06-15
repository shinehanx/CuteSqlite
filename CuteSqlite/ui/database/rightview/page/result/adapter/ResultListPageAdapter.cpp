#include "stdafx.h"
#include "ResultListPageAdapter.h"
#include <Strsafe.h>
#include <CommCtrl.h>
#include "core/common/repository/QSqlException.h"
#include "ui/common/message/QPopAnimate.h"
#include "utils/SqlUtil.h"
#include "utils/ClipboardUtil.h"

ResultListPageAdapter::ResultListPageAdapter(HWND parentHwnd, CListViewCtrl * listView)
{
	this->parentHwnd = parentHwnd;
	this->dataView = listView;
}

ResultListPageAdapter::~ResultListPageAdapter()
{

}

int ResultListPageAdapter::loadListView(uint64_t userDbId, std::wstring & sql)
{
	dataView->DeleteAllItems();

	runtimeUserDbId = userDbId;
	runtimeTables.clear();
	runtimeDatas.clear();
	
	if (sql.empty()) {
		return 0;
	}
	try {
		QSqlStatement query = sqlService->executeSql(userDbId, sql);
		loadRuntimeTables(userDbId, sql); 
		loadRuntimeHeader(query);
		return loadRuntimeData(query);
	} catch (SQLite::QSqlException &ex) {
		std::wstring _err = ex.getErrorStr();
		Q_ERROR(L"query db has error:{}, msg:{}", ex.getErrorCode(), _err);
		//supplier.
	}
	return 0;
}

/**
 * Fill the item and subitem data value of list view .
 * 
 * @param pLvdi
 * @return 
 */
LRESULT ResultListPageAdapter::fillListViewItemData(NMLVDISPINFO * pLvdi)
{
	auto iItem = pLvdi->item.iItem;
	if (-1 == iItem)
		return 0;

	auto count = static_cast<int>(runtimeDatas.size());
	if (!count || count <= iItem)
		return 0;

	// set check image in the first column
	
	if (pLvdi->item.iSubItem == 0 && pLvdi->item.mask & LVIF_TEXT) {
		pLvdi->item.mask = LVIF_IMAGE;
		if (getIsChecked(pLvdi->item.iItem)) {
			pLvdi->item.iImage = 1;
		} else {
			pLvdi->item.iImage = 0;
		}
		
		return 0;
	}

	auto iter = runtimeDatas.begin();
	for (int i = 1; i < iItem + 1; i++) {
		iter++;
	}
	RowItem & rowItem = *iter;
	if (pLvdi->item.iSubItem > 0 && (pLvdi->item.mask & LVIF_TEXT)) {
		std::wstring val = rowItem.at(pLvdi->item.iSubItem - 1);	
		StringCchCopy(pLvdi->item.pszText, pLvdi->item.cchTextMax, val.c_str());		
	}

	return 0;
}

/**
 * load the query table from sql in runtime.
 * 
 * @param userDbId the select db id
 * @param sql
 */
void ResultListPageAdapter::loadRuntimeTables(uint64_t userDbId, std::wstring & sql)
{
	ATLASSERT(!sql.empty());
	runtimeTables.clear();
	if (!SqlUtil::isSelectSql(sql)) {
		return ;
	}
	UserTableStrings allTables = databaseService->getUserTableStrings(userDbId);
	runtimeTables = SqlUtil::getTablesFromSelectSql(sql, allTables);
}

void ResultListPageAdapter::loadRuntimeHeader(QSqlStatement & query)
{
	dataView->InsertColumn(0, L"", LVCFMT_LEFT, 24, -1, 0);
	int n = query.getColumnCount();
	for (int i = 0; i < n - 1; i++) {
		std::wstring columnName = query.getColumnName(i);
		dataView->InsertColumn(i+1, columnName.c_str(), LVCFMT_LEFT, 100);
		runtimeColumns.push_back(columnName);
	}
}

int ResultListPageAdapter::loadRuntimeData(QSqlStatement & query)
{
	CRect rectList;
	dataView->GetClientRect(rectList);

	int cols = dataView->GetHeader().GetItemCount();
	while (query.executeStep()) {
		RowItem rowItem;
		for (int i = 0; i < cols - 1; i++) {
			std::wstring columnVal = query.getColumn(i).isNull() ? L"" : query.getColumn(i).getText();
			rowItem.push_back(columnVal);
		}
		runtimeDatas.push_back(rowItem);
	}
	int nRow = static_cast<int>(runtimeDatas.size());
	// trigger CListViewCtrl message LVN_GETDISPINFO to parent HWND, will call functon this->fillListViewItemData(NMLVDISPINFO * pLvdi)
	dataView->SetItemCount(nRow);
	
	return nRow;
}

/**
 * if the row of index=iItem is selected.
 * 
 * @param iItem the row index
 * @return 
 */
bool ResultListPageAdapter::getIsChecked(int iItem)
{
	if (dataView->GetSelectedCount() == 0) {
		return false;
	}
	int nSelItem = dataView->GetNextItem(-1, LVNI_ALL | LVNI_SELECTED); //向下搜索选中的项 -1表示先找出第一个
	
	while (nSelItem != -1) {
		if (nSelItem == iItem) {
			return true;
		}
		nSelItem = dataView->GetNextItem(nSelItem, LVNI_ALL | LVNI_SELECTED); // 继续往下搜索选中项
	}

	return false;
}

void ResultListPageAdapter::changeSelectAllItems()
{
	CHeaderCtrl headerCtrl = dataView->GetHeader();

	HDITEM headerItem;
	headerItem.mask = HDI_IMAGE;
	headerCtrl.GetItem(0, &headerItem);
	if (headerItem.iImage == 1) {
		headerItem.iImage = 0;
		dataView->SelectAllItems(false);
	} else {
		headerItem.iImage = 1;
		dataView->SelectAllItems(true);
	}
	headerItem.fmt = HDF_LEFT;
	headerCtrl.SetItem(0, &headerItem);
}

DataList ResultListPageAdapter::getSelectedDatas()
{
	if (!dataView->GetSelectedCount()) {
		return DataList();
	}
	DataList result;
	int nSelItem = -1;
	while ((nSelItem = dataView->GetNextItem(nSelItem, LVNI_SELECTED)) != -1) {
		DataList::iterator itor = runtimeDatas.begin();		
		for (int i = 0; i < nSelItem && itor != runtimeDatas.end(); i++) {
			itor++;
		}
		if (itor != runtimeDatas.end()) {
			result.push_back(*itor);
		}
		
	}
	return result;
}

UserTableStrings ResultListPageAdapter::getRuntimeTables()
{
	return runtimeTables;
}

Columns ResultListPageAdapter::getRuntimeColumns()
{
	return runtimeColumns;
}

DataList ResultListPageAdapter::getRuntimeDatas()
{
	return runtimeDatas;
}

UserColumnList ResultListPageAdapter::getRuntimeUserColumns(std::wstring & tblName)
{
	ATLASSERT(runtimeUserDbId && !tblName.empty());
	return databaseService->getUserColumns(runtimeUserDbId, tblName);
}

UserTable ResultListPageAdapter::getRuntimeUserTable(std::wstring & tblName)
{
	ATLASSERT(runtimeUserDbId && !tblName.empty());
	return databaseService->getUserTable(runtimeUserDbId, tblName);
}

void ResultListPageAdapter::copyAllRowsToClipboard()
{
	int n = static_cast<int>(runtimeColumns.size());
	std::wostringstream oss;
	// 1.write the columns to stringstream
	for (int i = 0; i < n; i++) {
		auto column = runtimeColumns.at(i);
		if (i > 0) {
			oss << L",";
		}
		oss << L'"' << column << L'"';
	}
	oss << endl;

	// 2.write the datas to stringstream
	n = 0;	
	for (auto vals : runtimeDatas) {
		if (vals.empty()) {
			continue;
		}
		
		int i = 0;
		for (auto val : vals) {
			if (i > 0) {
				oss << L",";
			}

			std::wstring rval = StringUtil::escape(val);
			oss << L'"' << rval << L'"';
			i++;
		}
		oss << endl;
	}
	oss.flush();

	// 3. copy the stringstream to clipboard
	ClipboardUtil::copyToClipboard(oss.str());
}

void ResultListPageAdapter::copySelRowsToClipboard()
{
	int n = static_cast<int>(runtimeColumns.size());
	std::wostringstream oss;
	// 1.write the columns to stringstream
	for (int i = 0; i < n; i++) {
		auto column = runtimeColumns.at(i);
		if (i > 0) {
			oss << L",";
		}
		oss << L'"' << column << L'"';
	}
	oss << endl;

	// 2.write the selected datas to stringstream
	n = 0;	
	DataList selDatas = getSelectedDatas();
	for (auto vals : selDatas) {
		if (vals.empty()) {
			continue;
		}
		
		int i = 0;
		for (auto val : vals) {
			if (i > 0) {
				oss << L",";
			}

			std::wstring rval = StringUtil::escape(val);
			oss << L'"' << rval << L'"';
			i++;
		}
		oss << endl;
		n++;
	}
	oss.flush();

	// 3. copy the stringstream to clipboard
	ClipboardUtil::copyToClipboard(oss.str());
}

void ResultListPageAdapter::copyAllRowsAsSql()
{
	if (runtimeTables.empty() || runtimeTables.size() > 1) {
		return ;
	}

	std::wstring tbl = runtimeTables.at(0);
	int n = static_cast<int>(runtimeColumns.size());
	std::wostringstream oss;
	// 1.write the data to stringstream
	n = 0;
	for (auto vals : runtimeDatas) {
		if (vals.empty()) {
			continue;
		}
		int i = 0;
		std::wostringstream dataSql, columnStmt, valuesStmt;
		dataSql << L"INSERT INTO " << tbl << L' ';

		columnStmt << L"(";
		valuesStmt << L" VALUES (";
		// write the selected column data value
		for (auto column : runtimeColumns) {			
			std::wstring val = StringUtil::escapeSql(vals.at(i));

			if (i > 0) {
				columnStmt << L", ";
				valuesStmt <<  L", ";
			}
			columnStmt << L"\"" << column << "\"";
			valuesStmt << L"'" << val <<  L"'";
			i++;
		}
		columnStmt <<  L')';
		valuesStmt << L')';
		dataSql << columnStmt.str() << valuesStmt.str() << L';';
		oss << dataSql.str() << endl;
		n++;
	}
	
	oss.flush();

	// 2. copy the stringstream to clipboard
	ClipboardUtil::copyToClipboard(oss.str());
}

void ResultListPageAdapter::copySelRowsAsSql()
{
	if (runtimeTables.empty() || runtimeTables.size() > 1) {
		return ;
	}

	std::wstring tbl = runtimeTables.at(0);
	int n = static_cast<int>(runtimeColumns.size());
	std::wostringstream oss;
	// 1.write the data to stringstream
	n = 0;
	DataList selDatas = getSelectedDatas();
	for (auto vals : selDatas) {
		if (vals.empty()) {
			continue;
		}
		int i = 0;
		std::wostringstream dataSql, columnStmt, valuesStmt;
		dataSql << L"INSERT INTO " << tbl << L' ';

		columnStmt << L"(";
		valuesStmt << L" VALUES (";
		// write the selected column data value
		for (auto column : runtimeColumns) {			
			std::wstring val = StringUtil::escapeSql(vals.at(i));

			if (i > 0) {
				columnStmt << L", ";
				valuesStmt <<  L", ";
			}
			columnStmt << L"\"" << column << "\"";
			valuesStmt << L"'" << val <<  L"'";
			i++;
		}
		columnStmt <<  L')';
		valuesStmt << L')';
		dataSql << columnStmt.str() << valuesStmt.str() << L';';
		oss << dataSql.str() << endl;
		n++;
	}
	
	oss.flush();

	// 2. copy the stringstream to clipboard
	ClipboardUtil::copyToClipboard(oss.str());
}
