#include "stdafx.h"
#include "ResultListPageAdapter.h"
#include <Strsafe.h>
#include <CommCtrl.h>
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

	auto count = static_cast<int>(datas.size());
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

	auto iter = datas.begin();
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

void ResultListPageAdapter::loadHeader(QSqlStatement & query)
{
	dataView->InsertColumn(0, L"", LVCFMT_LEFT, 24, -1, 0);
	int n = query.getColumnCount();
	for (int i = 0; i < n - 1; i++) {
		std::wstring columnName = query.getColumnName(i);
		dataView->InsertColumn(i+1, columnName.c_str(), LVCFMT_LEFT, 100);
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
		for (int i = 0; i < cols - 1; i++) {
			std::wstring columnVal = query.getColumn(i).isNull() ? L"" : query.getColumn(i).getText();
			rowItem.push_back(columnVal);
		}
		datas.push_back(rowItem);
	}
	int nRow = static_cast<int>(datas.size());
	// trigger CListViewCtrl message LVN_GETDISPINFO, will call functon this->onGetListViewData(NMLVDISPINFO * pLvdi)
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