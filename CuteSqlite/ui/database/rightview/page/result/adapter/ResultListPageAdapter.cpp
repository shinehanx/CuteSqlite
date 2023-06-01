#include "stdafx.h"
#include "ResultListPageAdapter.h"
#include <Strsafe.h>
#include "core/common/repository/QSqlException.h"
#include "ui/common/message/QPopAnimate.h"

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
	datas.clear();

	if (sql.empty()) {
		return 0;
	}
	try {
		QSqlStatement query = sqlService->executeSql(userDbId, sql);
		loadHeader(query);
		return loadData(query);
	} catch (SQLite::QSqlException &ex) {
		std::wstring _err = ex.getErrorStr();
		Q_ERROR(L"query db has error:{}, msg:{}", ex.getErrorCode(), _err);
		//supplier.
	}
	
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

	auto count = static_cast<int>(datas.size());
	if (!count || count <= iItem)
		return 0;

	auto iter = datas.begin();
	for (int i = 0; i < iItem; i++) {
		iter++;
	}
	RowItem & rowItem = *iter;
	if (pLvdi->item.mask & LVIF_TEXT) {
		std::wstring val = rowItem.at(pLvdi->item.iSubItem);
		StringCchCopy(pLvdi->item.pszText, pLvdi->item.cchTextMax, val.c_str());		
	}

	return 0;
}

void ResultListPageAdapter::loadHeader(QSqlStatement & query)
{
	int n = query.getColumnCount();
	for (int i = 0; i < n; i++) {
		std::wstring columnName = query.getColumnName(i);
		dataView->InsertColumn(i, columnName.c_str(), LVCFMT_LEFT, 100);
		columns.push_back(columnName);
	}
}

int ResultListPageAdapter::loadData(QSqlStatement & query)
{
	CRect rectList;
	dataView->GetClientRect(rectList);

	
	int cols = dataView->GetHeader().GetItemCount();
	while (query.executeStep()) {
		RowItem rowItem;
		int rows = static_cast<int>(dataView->GetItemCount());
		for (int i = 0; i < cols; i++) {
			std::wstring colomnVal = query.getColumn(i).isNull() ? L"" : query.getColumn(i).getText();
			rowItem.push_back(colomnVal);
		}
		datas.push_back(rowItem);
	}
	int nRow = static_cast<int>(datas.size());
	// trigger CListViewCtrl message LVN_GETDISPINFO, will call functon this->onGetListViewData(NMLVDISPINFO * pLvdi)
	dataView->SetItemCount(nRow);
	
	return nRow;
}
