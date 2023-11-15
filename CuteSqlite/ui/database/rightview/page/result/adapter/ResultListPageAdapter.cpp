#include "stdafx.h"
#include "ResultListPageAdapter.h"
#include <Strsafe.h>
#include <CommCtrl.h>
#include "common/AppContext.h"
#include "core/common/Lang.h"
#include "core/common/repository/QSqlException.h"
#include "core/service/system/SettingService.h"
#include "ui/common/message/QPopAnimate.h"
#include "ui/common/message/QMessageBox.h"
#include "utils/SqlUtil.h"
#include "utils/ClipboardUtil.h"
#include "utils/PerformUtil.h"

ResultListPageAdapter::ResultListPageAdapter(HWND parentHwnd, QListViewCtrl * listView, ResultType resultType)
{
	this->parentHwnd = parentHwnd;
	this->dataView = listView;
	this->resultType = resultType;
	if (this->resultType == QUERY_TABLE_DATA) {
		settingPrefix = TABLE_DATA_SETTING_PREFIX;
	}
}

ResultListPageAdapter::~ResultListPageAdapter()
{

}

int ResultListPageAdapter::loadListView(uint64_t userDbId, std::wstring & sql)
{
	int nCols = static_cast<int>(runtimeColumns.size());
	for (int i = nCols; i >=0 ; i--) {
		dataView->DeleteColumn(i);
	}
	dataView->DeleteAllItems();
	dataView->clearChangeVals();
	runtimeTables.clear();
	runtimeDatas.clear();
	runtimeColumns.clear();
	runtimeFilters.clear();
	runtimeNewRows.clear();
	resetRuntimeResultInfo();

	runtimeUserDbId = userDbId;
	originSql = sql;
	runtimeSql = sql;

	if (sql.empty()) {
		return 0;
	}

	if (!SqlUtil::isPragmaStmt(originSql, false) && !SqlUtil::hasLimitClause(originSql)) {
		LimitParams limitParams;
		loadLimitParams(limitParams);
		if (limitParams.checked) {
			runtimeSql.append(L" LIMIT ").append(std::to_wstring(limitParams.rows))
				.append(L" OFFSET ").append(std::to_wstring(limitParams.offset));
		}		
	}
	runtimeResultInfo.userDbId = userDbId;
	runtimeResultInfo.sql = runtimeSql;
	auto bt = PerformUtil::begin();
	try {		
		QSqlStatement query = sqlService->tryExecuteSql(userDbId, runtimeSql);
		loadRuntimeTables(userDbId, runtimeSql); 
		loadRuntimeHeader(query);
		runtimeResultInfo.execTime = PerformUtil::end(bt);
		runtimeResultInfo.effectRows = loadRuntimeData(query);	
		runtimeResultInfo.transferTime = PerformUtil::end(bt);
		return runtimeResultInfo.effectRows;
	} catch (SQLite::QSqlException &ex) {
		std::wstring _err = ex.getErrorStr();
		Q_ERROR(L"query db has error:{}, msg:{}", ex.getErrorCode(), _err);		
		runtimeResultInfo.code = ex.getErrorCode();
		runtimeResultInfo.msg = _err;
		runtimeResultInfo.execTime = PerformUtil::end(bt);
		runtimeResultInfo.transferTime = PerformUtil::end(bt);
		throw QSqlExecuteException(std::to_wstring(ex.getErrorCode()), _err, runtimeSql);
	}
	
	return 0;
}

int ResultListPageAdapter::loadFilterListView()
{
	dataView->DeleteAllItems();
	dataView->clearChangeVals();
	runtimeTables.clear();
	runtimeDatas.clear();
	runtimeNewRows.clear();
	resetRuntimeResultInfo();

	if (originSql.empty()) {
		return 0;
	}
	runtimeSql = buildRungtimeSqlWithFilters();
	runtimeResultInfo.sql = runtimeSql;
	runtimeResultInfo.userDbId = runtimeUserDbId;
	auto bt = PerformUtil::begin();
	try {		
		QSqlStatement query = sqlService->tryExecuteSql(runtimeUserDbId, runtimeSql);
		runtimeResultInfo.execTime = PerformUtil::end(bt);

		loadRuntimeTables(runtimeUserDbId, runtimeSql); 		

		runtimeResultInfo.effectRows = loadRuntimeData(query);
		runtimeResultInfo.transferTime = PerformUtil::end(bt);
		return runtimeResultInfo.effectRows;
	} catch (SQLite::QSqlException &ex) {
		std::wstring _err = ex.getErrorStr();
		Q_ERROR(L"query db has error:{}, msg:{}", ex.getErrorCode(), _err);
		runtimeResultInfo.code = ex.getErrorCode();
		runtimeResultInfo.msg = _err;
		runtimeResultInfo.execTime = PerformUtil::end(bt);
		runtimeResultInfo.transferTime = PerformUtil::end(bt);
		throw QSqlExecuteException(std::to_wstring(ex.getErrorCode()), _err, runtimeSql);
	}
	return 0;
}

/**
 * Fill the item and subitem data value of list view .
 * 
 * @param pLvdi
 * @return 
 */
LRESULT ResultListPageAdapter::fillDataInListViewSubItem(NMLVDISPINFO * pLvdi)
{
	auto iItem = pLvdi->item.iItem;
	if (-1 == iItem)
		return 0;

	auto count = static_cast<int>(runtimeDatas.size());
	if (!count || count <= iItem)
		return 0;

	if (iItem == count - 1) {
		iItem = count - 1;
	}

	// set checked/unchecked image in the first column	
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
	UserTableStrings allTables = tableService->getUserTableStrings(userDbId);
	runtimeTables = SqlUtil::getTablesFromSelectSql(sql, allTables);
}

void ResultListPageAdapter::loadRuntimeHeader(QSqlStatement & query)
{
	dataView->InsertColumn(0, L"", LVCFMT_LEFT, 26, -1, 0);
	int n = query.getColumnCount();
	for (int i = 0; i < n; i++) {
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
	// trigger CListViewCtrl message LVN_GETDISPINFO to parent HWND, it will call this->fillListViewItemData(NMLVDISPINFO * pLvdi)
	dataView->SetItemCount(nRow);
	
	return nRow;
}

void ResultListPageAdapter::loadLimitParams(LimitParams & limitParams)
{
	std::wstring limitChecked = SettingService::getInstance()->getSysInit(settingPrefix + L"limit-checked");
	std::wstring offset = SettingService::getInstance()->getSysInit(settingPrefix + L"limit-offset");
	std::wstring rows = SettingService::getInstance()->getSysInit(settingPrefix + L"limit-rows");
	offset = StringUtil::isDigit(offset) ? offset : L"0";
	rows = StringUtil::isDigit(rows) ? rows : L"1000";
	limitParams.checked = limitChecked == L"true";
	limitParams.offset = std::stoi(offset);
	limitParams.rows = std::stoi(rows);
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
 * Buid runtime sql statement with origin sql statement.
 * 
 * @return new runtime sql
 */
std::wstring ResultListPageAdapter::buildRungtimeSqlWithFilters()
{
	std::wstring newSql;
	std::wstring whereClause = SqlUtil::getWhereClause(originSql);
	std::wstring fourthClause = SqlUtil::getFourthClause(originSql);

	std::wstring condition;
	int n = static_cast<int>(runtimeFilters.size());
	for (int i = 0; i < n; i++) {
		auto tuple = runtimeFilters.at(i);
		auto connect = std::get<0>(tuple);// connect such as "and/or"
		auto column = std::get<1>(tuple);// column
		auto operater =std::get<2>(tuple);// operator such as "=,>,<..."
		auto val = StringUtil::escapeSql(std::get<3>(tuple));// value

		// ignore if column is empty or operater is empty
		if (column.empty() || operater.empty()) {
			continue;
		}
		condition.append(connect).append(L" ") 
			.append(column).append(L" ") 
			.append(operater).append(L" '") // 
			.append(val).append(L"' "); 
	}

	std::wstring newWhereClause = whereClause;
	if (whereClause.empty() && !condition.empty()) {		
		if (fourthClause.empty()) {
			// for example : select * from analysis_hair_inspection;
			newWhereClause.append(L" WHERE ").append(condition);
		}else {
			// for example : select * from analysis_hair_inspection order by uid;
			newWhereClause.append(L" WHERE ").append(condition).append(L" ").append(fourthClause);
		}
		
	}else if (!whereClause.empty() && !condition.empty()) {
		// for example : select * from analysis_hair_inspection where id=1 order by uid;
		newWhereClause.append(L" AND (").append(condition).append(L")");
	}

	if (!whereClause.empty()) {
		newSql = StringUtil::replace(originSql, whereClause, newWhereClause);
	} else {
		// for example : select * from analysis_hair_inspection
		if (fourthClause.empty() && !newWhereClause.empty()) {
			newSql.assign(originSql).append(L" ").append(newWhereClause);
		}else if (!fourthClause.empty()){
			// for example : select * from analysis_hair_inspection order by uid;
			newSql = StringUtil::replace(originSql, fourthClause, newWhereClause);
		}else {
			newSql = originSql;
		}		
	}

	if (!SqlUtil::hasLimitClause(originSql)) {
		LimitParams limitParams;
		loadLimitParams(limitParams);
		if (limitParams.checked) {
			newSql.append(L" LIMIT ").append(std::to_wstring(limitParams.rows))
				.append(L" OFFSET ").append(std::to_wstring(limitParams.offset));
		}		
	}
	
	return newSql;
}

RowItem ResultListPageAdapter::getFirstSelectdRowItem()
{
	if (!dataView->GetSelectedCount()) {
		return RowItem();
	}
	
	int nSelItem = -1;
	if ((nSelItem = dataView->GetNextItem(nSelItem, LVNI_SELECTED)) != -1) {
		DataList::iterator itor = runtimeDatas.begin();		
		for (int i = 0; i < nSelItem && itor != runtimeDatas.end(); i++) {
			itor++;
		}
		return *itor;
	}
	return RowItem();
}

int ResultListPageAdapter::getFirstSelectdIndex()
{
	if (!dataView->GetSelectedCount()) {
		return -1;
	}

	int nSelItem = -1;
	return  dataView->GetNextItem(nSelItem, LVNI_SELECTED);
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

int ResultListPageAdapter::getSelectedItemCount()
{
	return dataView->GetSelectedCount();
}

void ResultListPageAdapter::setRuntimeUserDbId(uint64_t userDbId)
{
	runtimeUserDbId = userDbId;
}

UserTableStrings ResultListPageAdapter::getRuntimeTables()
{
	return runtimeTables;
}

void ResultListPageAdapter::setRuntimeTables(const UserTableStrings & val)
{
	runtimeTables = val;
}

Columns ResultListPageAdapter::getRuntimeColumns()
{
	return runtimeColumns;
}

void ResultListPageAdapter::setRuntimeColumns(const Columns & columns)
{
	runtimeColumns = columns;
}

DataList ResultListPageAdapter::getRuntimeDatas()
{
	return runtimeDatas;
}

void ResultListPageAdapter::setRuntimeDatas(const DataList & dataList)
{
	runtimeDatas = dataList;
}

/**
 * add subItemVal to variable changeVals of QListView.
 * 
 * @param subItemVal
 */
void ResultListPageAdapter::addListViewChangeVal(SubItemValue &subItemVal)
{
	dataView->setChangeVal(subItemVal);
}

ColumnInfoList ResultListPageAdapter::getRuntimeColumnInfos(std::wstring & tblName)
{
	ATLASSERT(runtimeUserDbId && !tblName.empty());
	return tableService->getUserColumns(runtimeUserDbId, tblName);
}

UserTable ResultListPageAdapter::getRuntimeUserTable(std::wstring & tblName)
{
	ATLASSERT(runtimeUserDbId && !tblName.empty());
	return tableService->getUserTable(runtimeUserDbId, tblName);
}

/**
 * Get the runtime valid query columns for filter the result list.
 * 
 * @return valid query columns
 */
Columns ResultListPageAdapter::getRuntimeValidFilterColumns()
{
	if (runtimeTables.empty() || runtimeColumns.empty()) {
		return Columns();
	}
	Columns validColums;

	for (auto tblName : runtimeTables) {
		ColumnInfoList columnInfos = getRuntimeColumnInfos(tblName);
		for (auto column : runtimeColumns) {
			auto iter = std::find_if(columnInfos.begin(), columnInfos.end(), [&column](ColumnInfo & columnInfo) {
				return column == columnInfo.name;
			});
			if (iter != columnInfos.end()) {
				validColums.push_back(column);
			}
		}
	}

	return validColums;
}

DataFilters ResultListPageAdapter::getRuntimeFilters()
{
	return runtimeFilters;
}

void ResultListPageAdapter::setRuntimeFilters(DataFilters & filters)
{
	runtimeFilters = filters;
}

void ResultListPageAdapter::clearRuntimeFilters()
{
	runtimeFilters.clear();
}

bool ResultListPageAdapter::isRuntimeFiltersEmpty()
{
	return std::empty(runtimeFilters);
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

void ResultListPageAdapter::setSettingPrefix(std::wstring &prefix)
{
	settingPrefix = prefix;
}

/**
 * change the runtimeDatas variable before changed the list view subitem text .
 * 
 * @param iItem
 * @param iSubItem
 * @param text
 */
void ResultListPageAdapter::changeRuntimeDatasItem(int iItem, int iSubItem, std::wstring & origText, std::wstring & newText)
{
	ATLASSERT(iItem >= 0 && iSubItem > 0);
	auto iter = runtimeDatas.begin();
	
	for (int i = 0; i < iItem; i++) {
		iter++;
	}
	RowItem & rowItem = *iter;
	// rowItem.index = listView.row.iSubItem - 1 
	rowItem[iSubItem - 1] = newText;
}

void ResultListPageAdapter::invalidateSubItem(int iItem, int iSubItem)
{
	CRect subItemRect;
	dataView->GetSubItemRect(iItem, iSubItem, LVIR_BOUNDS, subItemRect);
	dataView->InvalidateRect(subItemRect, false);
}

void ResultListPageAdapter::createNewRow()
{
	if (runtimeColumns.empty()) {
		return;
	}
	// 1.create a empty row and push it to runtimeDatas list
	RowItem row;
	std::wstring primaryKey = tableService->getPrimaryKeyColumn(runtimeUserDbId, runtimeTables.at(0), runtimeColumns);
	int n = static_cast<int>(runtimeColumns.size());
	for (int i = 0; i < n; i++) {
		auto colum = runtimeColumns.at(i);
		if (colum == primaryKey) {
			row.push_back(L"(Auto)");
		}else {
			row.push_back(std::wstring());
		}
		
	}
	runtimeDatas.push_back(row);

	// 2.update the item count and selected the new row	
	n = static_cast<int>(runtimeDatas.size());
	runtimeNewRows.push_back(n-1); // param - runtimeDatas index
	dataView->SetItemCount(n);
	dataView->SelectItem(n-1);

	// 3.show the editor on first column for the new row 
	dataView->createOrShowEditor(n - 1, 1);// the 1th column
}

void ResultListPageAdapter::copyNewRow()
{
	if (runtimeColumns.empty()) {
		return;
	}
	// 1.copy selected row and push it to runtimeDatas list
	RowItem row = getFirstSelectdRowItem();
	if (row.empty()) {
		return;
	}
	auto primaryKey = tableService->getPrimaryKeyColumn(runtimeUserDbId, runtimeTables.at(0), runtimeColumns);
	if (primaryKey == runtimeColumns.at(0)) {
		row[0] = L"(Auto)";
	}
	runtimeDatas.push_back(row);
	int n = static_cast<int>(runtimeDatas.size());
	runtimeNewRows.push_back(n-1); // runtimeDatas index
	dataView->SetItemCount(n);
	dataView->SelectItem(n-1);

	// 2.show the editor on first column for the new row 
	std::pair<int, int> subItemPos(n - 1, 1); // the 1th column
	dataView->createOrShowEditor(subItemPos);
}

/**
 * save the change data includes:
 * 1.The changeVals from QListView::getChangeVals
 * 2.The runtimeDates index from runtimeNewRows 
 * 
 * @return 
 */
bool ResultListPageAdapter::save()
{
	// 1.The changeVals from QListViewCtrl::getChangeVals function
	bool ret = saveChangeVals();
	if (!ret) {
		return ret;
	}
	// 2. The runtimeNewRows that it save runtimeDatas index  
	ret = saveNewRows();
	if (!ret) {
		return ret;
	}

	return true;
}

/**
 * The changeVals from QListViewCtrl::getChangeVals function will be save.
 * 
 */
bool ResultListPageAdapter::saveChangeVals()
{
	SubItemValues changeVals = dataView->getChangedVals();
	SubItemValues errorChanges;
	for (auto subItemVal : changeVals) {
		int iItem = subItemVal.iItem;
		int iSubItem = subItemVal.iSubItem;

		//iItem will be excludes if it's in the vector rumtimeNewRows
		auto iterItem = std::find(runtimeNewRows.begin(), runtimeNewRows.end(), iItem);
		if (iterItem != runtimeNewRows.end()) {
			continue;
		}

		auto iter = runtimeDatas.begin();
		for (int i = 0; i < iItem ; i++) {
			iter++;
		}
		// this row change vals vector
		SubItemValues rowChangeVals = dataView->getRowChangedVals(iItem);

		RowItem & rowItem = *iter;
		std::wstring tblName = runtimeTables.at(0);
		std::wstring primaryKey = tableService->getPrimaryKeyColumn(runtimeUserDbId, tblName, runtimeColumns);
		std::wstring whereClause;
		int nSelSubItem = iSubItem - 1;
		std::wstring & origVal = subItemVal.origVal;
		if (primaryKey.empty()) {
			whereClause = SqlUtil::makeWhereClause(runtimeColumns, rowItem, rowChangeVals);
		} else {
			whereClause = SqlUtil::makeWhereClauseByPrimaryKey(primaryKey, runtimeColumns, rowItem, rowChangeVals);
		}

		std::wstring newVal = StringUtil::escapeSql(subItemVal.newVal);
		std::wstring sqlUpdate(L"UPDATE ");
		sqlUpdate.append(tblName)
			.append(L" SET \"")
			.append(runtimeColumns.at(nSelSubItem))
			.append(L"\"='")
			.append(newVal).append(L"'")
			.append(whereClause);

		try {
			auto stmt = sqlService->tryExecuteSql(runtimeUserDbId, sqlUpdate);
			stmt.exec();
		}  catch (SQLite::QSqlException &ex) {
			errorChanges.push_back(subItemVal);
			std::wstring _err(L"Error:\r\n");
			_err.append(ex.getErrorStr()).append(L"\r\nSQL:\r\n").append(sqlUpdate);
			Q_ERROR(L"query db has error, code:{}, msg:{}", ex.getErrorCode(), _err);
			// QPopAnimate::error(parentHwnd, _err);
			QMessageBox::confirm(parentHwnd, _err, S(L"yes"), S(L"no"));
			dataView->createOrShowEditor(subItemVal.iItem, subItemVal.iSubItem);
		}
	}
	// clear changes
	dataView->clearChangeVals();

	// restore the changeVals if has error
	if (!errorChanges.empty()) {
		dataView->setChangedVals(errorChanges);
		return false;
	} 
	return true;
}

/**
 * The runtimeDates index from runtimeNewRows.
 * 
 */
bool ResultListPageAdapter::saveNewRows()
{
	if (runtimeNewRows.empty()) {
		return true;
	}
	std::vector<int> errorNewRows;
	std::wstring tblName = runtimeTables.at(0);
	for (auto nItem : runtimeNewRows) {
		auto itor = runtimeDatas.begin();
		for (int i = 0; i < nItem; i++) {
			itor++;
		}
		std::wstring colums = SqlUtil::makeInsertColumsClause(runtimeColumns);
		std::wstring values = SqlUtil::makeInsertValuesClause(*itor);
		if (colums.empty() || values.empty()) {
			continue;
		}
		
		std::wstring sqlInsert(L"INSERT INTO ");
		sqlInsert.append(tblName).append(colums).append(values);

		try {
			auto stmt = sqlService->tryExecuteSql(runtimeUserDbId, sqlInsert);
			stmt.exec();
		}  catch (SQLite::QSqlException &ex) {
			errorNewRows.push_back(nItem);
			std::wstring _err(L"Error:\r\n");
			_err.append(ex.getErrorStr()).append(L"\r\nSQL:\r\n").append(sqlInsert);
			Q_ERROR(L"query db has error, code:{}, msg:{}", ex.getErrorCode(), _err);
			QMessageBox::confirm(parentHwnd, _err, S(L"yes"), S(L"no"));
			dataView->createOrShowEditor(nItem, 1);
		}
	}

	// clear new rows
	runtimeNewRows.clear();

	// reset the runtimeNewRows if has error
	if (!errorNewRows.empty()) {
		runtimeNewRows = errorNewRows;
		return false;
	}

	try {
		loadFilterListView();
	} catch (QSqlExecuteException &ex) {
		QPopAnimate::report(ex);
	}
	
	return true;
}

bool ResultListPageAdapter::remove(bool confirm)
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
		RowItem & rowItem = (*iter);
		// 2.1 delete row from database 
		if (!removeRowFromDb(nSelItem, rowItem)) {
			return false;
		}

		// 2.2 delete row from runtimeDatas vector 
		runtimeDatas.erase(iter);

		// 2.3 delete row from dataView
		dataView->RemoveItem(nSelItem);
	}

	// 3.delete or subtract item index in runtimeNewRows and changeVals the item begin from the last selected item
	for (int i = n - 1; i >= 0; i--) {
		nSelItem = nSelItems.at(i);
		auto itor = runtimeNewRows.begin();
		while (itor != runtimeNewRows.end()) {
			if ((*itor) == nSelItem) {
				if (itor != runtimeNewRows.begin()) {
					runtimeNewRows.erase(itor--);
				} else {
					runtimeNewRows.erase(itor);
					itor = runtimeNewRows.begin();
					continue;
				}
				if (runtimeNewRows.empty()) {
					break;
				}
			} else if ((*itor) > nSelItem) {
				(*itor)--;
			}
			itor++;
		}
	}

	return true;
}

int ResultListPageAdapter::removeRowFromDb(int nSelItem, RowItem & rowItem)
{
	std::wstring & tblName = runtimeTables.at(0);

	// if the nSelItem row 
	auto iter = std::find(runtimeNewRows.begin(), runtimeNewRows.end(), nSelItem);
	if (iter != runtimeNewRows.end()) {
		return 1;
	}

	// this row change vals vector
	SubItemValues rowChangedVals = dataView->getRowChangedVals(nSelItem);

	// delete row from database 
	std::wstring sqlDelete = L"DELETE FROM ";
	std::wstring primaryKey = tableService->getPrimaryKeyColumn(runtimeUserDbId, tblName, runtimeColumns);
	std::wstring whereClause;
	if (primaryKey.empty()) {
		whereClause = SqlUtil::makeWhereClause(runtimeColumns, rowItem, rowChangedVals);
	}
	else {
		whereClause = SqlUtil::makeWhereClauseByPrimaryKey(primaryKey, runtimeColumns, rowItem, rowChangedVals);
	}
	sqlDelete.append(tblName).append(whereClause);
	try {
		auto stmt = sqlService->tryExecuteSql(runtimeUserDbId, sqlDelete);
		return stmt.exec();
	} catch (SQLite::QSqlException &ex) {

		std::wstring _err(L"Error:\r\n");
		_err.append(ex.getErrorStr()).append(L"\r\nSQL:\r\n").append(sqlDelete);
		Q_ERROR(L"query db has error, code:{}, msg:{}", ex.getErrorCode(), _err);
		QMessageBox::confirm(parentHwnd, _err, S(L"yes"), S(L"no"));
		return 0;
	}
}

bool ResultListPageAdapter::isDirty()
{
	bool hasEditSubItem = dataView && dataView->IsWindow() ? dataView->getChangedCount() : false;
	bool hasNewRow = !runtimeNewRows.empty(); 

	return hasEditSubItem || hasNewRow;
}

ResultInfo & ResultListPageAdapter::getRuntimeResultInfo()
{
	return runtimeResultInfo;
}

bool ResultListPageAdapter::cancel()
{
	// 1. delete the new item from runtimeNewRows
	if (!runtimeNewRows.empty()) {
		int n = static_cast<int>(runtimeNewRows.size());
		for (int i = n - 1; i >= 0; i--) {
			int nSelItem = runtimeNewRows.at(i);
			dataView->SelectItem(nSelItem);
			if (!remove(false)) {
				return false;
			}
		}		
	}	

	// 2. restore the text of subitem in the listview
	if (!restoreChangeVals()) {
		return false;
	}
	return true;
}

/**
 * restore the change values from list view.
 * 
 */
bool ResultListPageAdapter::restoreChangeVals()
{
	// 1.restore the runtimeDatas and refresh the text of the changed subitem 
	auto changeVals = dataView->getChangedVals();
	int n = static_cast<int>(changeVals.size());
	for (int i = n - 1; i >= 0; i--) {
		auto item = changeVals.at(i);
		auto iter = runtimeDatas.begin();
		for (int j = 0; j < item.iItem; j++) {
			iter++;
		}
		(*iter).at(item.iSubItem - 1) = item.origVal;

		// invalidate the subitem
		invalidateSubItem(item.iItem, item.iSubItem);
	}

	// 2. clear all change vals
	dataView->clearChangeVals();
	return true;
}

void ResultListPageAdapter::sendExecSqlMessage(ResultInfo & resultInfo)
{
	AppContext::getInstance()->dispatch(Config::MSG_EXEC_SQL_RESULT_MESSAGE_ID, WPARAM(NULL), LPARAM(&resultInfo));
}

void ResultListPageAdapter::resetRuntimeResultInfo()
{
	runtimeResultInfo.sql.clear();
	runtimeResultInfo.effectRows = 0;
	runtimeResultInfo.execTime.clear();
	runtimeResultInfo.totalTime.clear();
	runtimeResultInfo.code = 0;
	runtimeResultInfo.msg.clear();
}
