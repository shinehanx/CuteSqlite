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

 * @file   RightWorkViewToolBarAdapter.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-11-22
 *********************************************************************/
#include "stdafx.h"
#include "RightWorkViewAdapter.h"
#include "core/common/Lang.h"
#include "ui/database/rightview//page/dialog/ViewDialog.h"
#include "ui/database/rightview//page/dialog/TriggerDialog.h"
#include <ui/common/message/QPopAnimate.h>

/**
 * Construct function.
 * 
 * @param parentHwnd - RightWorkView HWND
 * @param dataView - tabView
 */
RightWorkViewAdapter::RightWorkViewAdapter(HWND parentHwnd, QTabView & _tabView,
	std::vector<QueryPage *> & _queryPagePtrs, std::vector<TableStructurePage *> & _tablePagePtrs)
	:tabView(_tabView), queryPagePtrs(_queryPagePtrs), tablePagePtrs(_tablePagePtrs)
{
	this->parentHwnd = parentHwnd;
	this->dataView = &_tabView;
}

void RightWorkViewAdapter::execSelectedSql()
{
	int nPage = tabView.GetActivePage();
	if (nPage < 0) {
		return ;
	}
	HWND activeHwnd = tabView.GetPageHWND(nPage);
	for (auto pagePtr : queryPagePtrs) {
		if (pagePtr && pagePtr->IsWindow () && activeHwnd == pagePtr->m_hWnd) {
			pagePtr->execAndShow(true);
		}		
	}
}

void RightWorkViewAdapter::execAllSql()
{
	int nPage = tabView.GetActivePage();
	if (nPage < 0) {
		return ;
	}
	HWND activeHwnd = tabView.GetPageHWND(nPage);
	for (auto pagePtr : queryPagePtrs) {
		if (pagePtr && pagePtr->IsWindow () && activeHwnd == pagePtr->m_hWnd) {
			pagePtr->execAndShow(false);
		}		
	}
}

void RightWorkViewAdapter::explainSelectedSql()
{
	int nPage = tabView.GetActivePage();
	if (nPage < 0) {
		return ;
	}
	HWND activeHwnd = tabView.GetPageHWND(nPage);
	for (auto pagePtr : queryPagePtrs) {
		if (pagePtr && pagePtr->IsWindow () && activeHwnd == pagePtr->m_hWnd) {
			pagePtr->explainAndShow(); 
		}		
	}
}


void RightWorkViewAdapter::createFirstQueryPage(CRect & tabRect, bool isInitedPages)
{
	//queryPagePtrs.clear();
	QueryPage * firstQueryPage = nullptr;
	if (queryPagePtrs.empty()) {
		if (isInitedPages) {
			return;
		}
		firstQueryPage = new QueryPage();
		firstQueryPage->setup(QUERY_DATA);
		createOrShowQueryPage(*firstQueryPage, tabRect, true); 
		queryPagePtrs.push_back(firstQueryPage);
	} else {
		firstQueryPage = queryPagePtrs.at(0);
		createOrShowQueryPage(*firstQueryPage, tabRect, true);
	}
}

void RightWorkViewAdapter::createNewQueryPage(CRect & tabRect)
{
	//queryPagePtrs.clear();
	QueryPage * newQueryPage  = new QueryPage();
	newQueryPage->setup(QUERY_DATA);
	createOrShowQueryPage(*newQueryPage, tabRect, true); 
	queryPagePtrs.push_back(newQueryPage);
	tabView.AddPage(newQueryPage->m_hWnd, StringUtil::blkToTail(S(L"query-editor")).c_str(), 0, newQueryPage);
}

void RightWorkViewAdapter::createOrShowQueryPage(QueryPage &win, CRect & tabRect, bool isAllowCreate)
{
	int x = 1, y = tabView.m_cyTabHeight + 1, w = tabRect.Width() - 2, h = tabRect.Height() - tabView.m_cyTabHeight - 2;
	CRect rect(x, y, x + w, y + h);
	if (::IsWindow(tabView.m_hWnd) && !win.IsWindow() && isAllowCreate) {
		win.Create(tabView.m_hWnd, rect, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN , 0);
	} else if (::IsWindow(tabView.m_hWnd) && win.IsWindow()) {
		win.MoveWindow(rect);
		win.ShowWindow(true);
	}
}

void RightWorkViewAdapter::createOrShowTableStructurePage(TableStructurePage &win, CRect & tabRect)
{
	int x = 1, y = tabView.m_cyTabHeight + 1, 
		w = tabRect.Width() - 2, 
		h = tabRect.Height() - tabView.m_cyTabHeight - 2;
	CRect rect(x, y, x + w, y + h);
	if (::IsWindow(tabView.m_hWnd) && !win.IsWindow()) {
		win.Create(tabView.m_hWnd, rect, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN , 0);
	} else if (::IsWindow(tabView.m_hWnd) && win.IsWindow()) {
		win.MoveWindow(rect);
		win.ShowWindow(true);
	}
}

void RightWorkViewAdapter::addNewTable(CRect & tabRect)
{
	TableStructurePage * newTablePage = new TableStructurePage();
	newTablePage->setup(PageOperateType::NEW_TABLE);
	createOrShowTableStructurePage(*newTablePage, tabRect);
	tablePagePtrs.push_back(newTablePage);

	// nImage = 3 : VIEW 
	tabView.AddPage(newTablePage->m_hWnd, StringUtil::blkToTail(S(L"new-table")).c_str(), 8, newTablePage);

	databaseSupplier->activeTabPageHwnd = newTablePage->m_hWnd;
}

void RightWorkViewAdapter::addNewView(CRect & tabRect)
{
	ViewDialog viewDialog(parentHwnd);
	if (viewDialog.DoModal(parentHwnd) != Config::QDIALOG_YES_BUTTON_ID) {
		return ;
	}
	
	CRect clientRect;
	::GetClientRect(parentHwnd, clientRect);
	std::wstring binDir = ResourceUtil::getProductBinDir();
	std::wstring tplPath = binDir + L"res\\tpl\\create-view-sql.tpl";
	std::wstring content = FileUtil::readFile(tplPath);
	std::wstring viewName = L"\"" + databaseSupplier->newViewName + L"\"";
	content = StringUtil::replace(content, L"{<!--view_name-->}", viewName);

	QueryPage * newViewPage = new QueryPage();
	newViewPage->setup(CREATE_VIEW, content);
	createOrShowQueryPage(*newViewPage, tabRect, true);
	queryPagePtrs.push_back(newViewPage);

	// nImage = 3 : view 
	tabView.AddPage(newViewPage->m_hWnd, StringUtil::blkToTail(S(L"new-view")).c_str(), 3, newViewPage);
	databaseSupplier->activeTabPageHwnd = newViewPage->m_hWnd;
}

void RightWorkViewAdapter::addNewTrigger(CRect & tabRect)
{
	TriggerDialog triggerDialog(parentHwnd);
	if (triggerDialog.DoModal(parentHwnd) != Config::QDIALOG_YES_BUTTON_ID) {
		return ;
	}

	std::wstring binDir = ResourceUtil::getProductBinDir();
	std::wstring tplPath = binDir + L"res\\tpl\\create-trigger-sql.tpl";
	std::wstring content = FileUtil::readFile(tplPath);
	std::wstring viewName = L"\"" + databaseSupplier->newViewName + L"\"";
	content = StringUtil::replace(content, L"{<!--trigger_name-->}", viewName);

	QueryPage * newTriggerPage = new QueryPage();
	newTriggerPage->setup(CREATE_TRIGGER, content);
	createOrShowQueryPage(*newTriggerPage, tabRect, true); 
	queryPagePtrs.push_back(newTriggerPage); 

	// nImage = 2 : table 
	tabView.AddPage(newTriggerPage->m_hWnd, StringUtil::blkToTail(S(L"new-trigger")).c_str(), 4, newTriggerPage);

	databaseSupplier->activeTabPageHwnd = newTriggerPage->m_hWnd;
}

void RightWorkViewAdapter::openView(CRect & tabRect)
{
	try {
		UserView userView = databaseService->getUserView(databaseSupplier->getSelectedUserDbId(), databaseSupplier->selectedViewName);
		if (userView.name.empty()) {
			QPopAnimate::error(E(L"200004"));
			return ;
		}
		std::wstring & content = userView.sql;
	
		QueryPage * newViewPage = new QueryPage();
		newViewPage->setup(MODIFY_VIEW, content);
		createOrShowQueryPage(*newViewPage, tabRect, true);
		queryPagePtrs.push_back(newViewPage);

		// nImage = 3 : view 
		std::wstring name = L"[View]";
		name.append(userView.name);
		tabView.AddPage(newViewPage->m_hWnd, StringUtil::blkToTail(name).c_str(), 3, newViewPage);
		databaseSupplier->activeTabPageHwnd = newViewPage->m_hWnd;
	} catch (QSqlExecuteException &ex) {
		Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::report(ex);
	}
}

void RightWorkViewAdapter::dropView()
{
	int n = tabView.GetPageCount();
	uint64_t userDbId = databaseSupplier->getSelectedUserDbId();
	std::wstring viewName =  L"[View]" + databaseSupplier->selectedViewName;
	std::stack<int> delPageStack;
	for (int i = 0; i < n; i++) {
		HWND pageHwnd = tabView.GetPageHWND(i);
		std::wstring pageTitle = tabView.GetPageTitle(i);
		StringUtil::trim(pageTitle);
		// same name
		if (pageTitle != viewName) {
			continue;
		}
		// query page has properties of TABLE_DATA and  same user db id
		auto iter = std::find_if(queryPagePtrs.begin(), queryPagePtrs.end(), [&pageHwnd, &userDbId](QueryPage * page) {
			if (page && page->IsWindow() 
				&& page->m_hWnd == pageHwnd 
				&& page->getSupplier()->getOperateType() == MODIFY_VIEW 
				&& page->getSupplier()->getRuntimeUserDbId() == userDbId) {
				page->DestroyWindow();
				delete page;
				page = nullptr;
				return true;
			}
			return false;
		});

		// reload table data
		if (iter != queryPagePtrs.end()) {
			queryPagePtrs.erase(iter);
			delPageStack.push(i);
			continue;
		}
	}

	while (!delPageStack.empty()) {
		int nPage = delPageStack.top();
		delPageStack.pop();
		tabView.RemovePage(nPage);
	}
}

void RightWorkViewAdapter::openTrigger(CRect & tabRect)
{
	UserTrigger userTrigger;
	try {
		userTrigger = databaseService->getUserTrigger(databaseSupplier->getSelectedUserDbId(), databaseSupplier->selectedTriggerName);
	} catch (QSqlExecuteException &ex) {
		Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::report(ex);
		return;
	}
	if (userTrigger.name.empty()) {
		QPopAnimate::error(E(L"200004"));
		return ;
	}
	std::wstring & content = userTrigger.sql;
	QueryPage * newViewPage = new QueryPage();
	newViewPage->setup(MODIFY_TRIGGER, content);
	createOrShowQueryPage(*newViewPage, tabRect, true);
	queryPagePtrs.push_back(newViewPage);

	// nImage = 4 : trigger 
	std::wstring name = L"[Trigger]";
	name.append(userTrigger.name);
	tabView.AddPage(newViewPage->m_hWnd, StringUtil::blkToTail(name).c_str(), 4, newViewPage);
	databaseSupplier->activeTabPageHwnd = newViewPage->m_hWnd;
}

void RightWorkViewAdapter::dropTrigger()
{
	int n = tabView.GetPageCount();
	uint64_t userDbId = databaseSupplier->getSelectedUserDbId();
	std::wstring & triggerName = databaseSupplier->selectedTriggerName;
	std::wstring vname = L"[Trigger]" + triggerName;
	std::stack<int> delPageStack;
	for (int i = 0; i < n; i++) {
		HWND pageHwnd = tabView.GetPageHWND(i);
		std::wstring pageTitle = tabView.GetPageTitle(i);
		StringUtil::trim(pageTitle);		
		// same name
		if (pageTitle != vname) {
			continue;
		}
		// query page has properties of MODIFY_TRIGGER and  same user db id
		auto iter = std::find_if(queryPagePtrs.begin(), queryPagePtrs.end(), [&pageHwnd, &userDbId](QueryPage * page) {
			if (page && page->IsWindow() 
				&& page->m_hWnd == pageHwnd 
				&& page->getSupplier()->getOperateType() == MODIFY_TRIGGER
				&& page->getSupplier()->getRuntimeUserDbId() == userDbId) {
				page->DestroyWindow();
				delete page;
				page = nullptr;
				return true;
			}
			return false;
		});

		// erase from queryPagePtrs
		if (iter != queryPagePtrs.end()) {
			queryPagePtrs.erase(iter);
			delPageStack.push(i);
			continue;
		}
	}

	// erase from tabView pages
	while (!delPageStack.empty()) {
		int nPage = delPageStack.top();
		delPageStack.pop();
		tabView.RemovePage(nPage);
	}
}

void RightWorkViewAdapter::showTableData(CRect & tabRect, bool isPropertyPage)
{
	if (databaseSupplier->selectedTable.empty()) {
		return ;
	}
	// 1.find the queryPage title is equal select table name
	int n = tabView.GetPageCount();
	int foundQueryPage = -1;
	for (int i = 0; i < n; i++) {
		HWND pageHwnd = tabView.GetPageHWND(i);
		std::wstring title = tabView.GetPageTitle(i);
		StringUtil::trim(title); // trim the title
		if (title != databaseSupplier->selectedTable) {
			continue;
		}

		auto iter = std::find_if(queryPagePtrs.begin(), queryPagePtrs.end(), [&pageHwnd](QueryPage * ptr) {
			return ptr->m_hWnd == pageHwnd ;
		});
		if (iter == queryPagePtrs.end()) {
			continue;
		}
		foundQueryPage = i; // found, and active this page and load table data
		tabView.SetActivePage(foundQueryPage);
		QueryPage * tableDataPage = (*iter);
		if (isPropertyPage) { // 1- properties page
			//active table info page
			tableDataPage->getResultTabView().activeTablePropertiesPage(); // active table properties page first
		} else {
			tableDataPage->getResultTabView().activeTableDataPage(); // active table data page first
		}
		
		tableDataPage->getResultTabView().loadTableDatas(databaseSupplier->selectedTable);
		return ;
	}

	// 2.check if exists a queryPage of same table name in tabView, then active this queryPage, 
	//   otherwise create a new queryPage for show table data
	if (foundQueryPage == -1) { // not found, create
		CRect clientRect;
		QueryPage * tableDataPage = new QueryPage();
		tableDataPage->setup(TABLE_DATA);
		createOrShowQueryPage(*tableDataPage, clientRect, true);
		queryPagePtrs.push_back(tableDataPage);

		// nImage = 3 : view 
		std::wstring tblName = databaseSupplier->selectedTable;
		tabView.AddPage(tableDataPage->m_hWnd, StringUtil::blkToTail(tblName).c_str(), 5, tableDataPage);
		foundQueryPage = tabView.GetPageCount() - 1;
		databaseSupplier->activeTabPageHwnd = tableDataPage->m_hWnd;
		tabView.SetActivePage(foundQueryPage);	
		if (isPropertyPage) { // 1- properties page
			//active table info page
			tableDataPage->getResultTabView().activeTablePropertiesPage(); // active table properties page first
		} else {
			tableDataPage->getResultTabView().activeTableDataPage(); // active table data page first
		}
		tableDataPage->getResultTabView().loadTableDatas(databaseSupplier->selectedTable); 
	}

}

void RightWorkViewAdapter::alterTable(CRect & tabRect, TableStructurePageType tblStructPageType)
{
	ATLASSERT(!databaseSupplier->selectedTable.empty()); 

	// 1.find the queryPage title is equal select table name
	int n = tabView.GetPageCount();
	int foundPage = -1;
	for (int i = 0; i < n; i++) {
		HWND pageHwnd = tabView.GetPageHWND(i);
		std::wstring title = tabView.GetPageTitle(i);
		StringUtil::trim(title); // trim the title
		if (title != databaseSupplier->selectedTable) {
			continue;
		}
		uint64_t userDbId = databaseSupplier->getSelectedUserDbId();
		auto iter = std::find_if(tablePagePtrs.begin(), tablePagePtrs.end(), [&pageHwnd, &userDbId](TableStructurePage * ptr) {
			return ptr->m_hWnd == pageHwnd && userDbId == ptr->getSupplier()->getRuntimeUserDbId();
		});
		if (iter == tablePagePtrs.end()) {
			continue;
		}
		foundPage = i; // found, and active this page and load table data
		
		TableStructurePage * tableStructPage = (*iter);
		tableStructPage->activePage(tblStructPageType);
		databaseSupplier->activeTabPageHwnd = tableStructPage->m_hWnd;
		tabView.SetActivePage(foundPage);
		return ;
	}

	// 2.check if exists a tableStructPage of same table name in tabView, then active this tableStructPage, 
	//   otherwise create a new tableStructPage for show table data
	if (foundPage == -1) { // not found, create
		
		TableStructurePage * tableStructPage = new TableStructurePage();
		tableStructPage->setup(PageOperateType::MOD_TABLE, databaseSupplier->selectedTable, tblStructPageType);
		createOrShowTableStructurePage(*tableStructPage, tabRect);
		tablePagePtrs.push_back(tableStructPage);

		std::wstring tblName = databaseSupplier->selectedTable;
		tabView.AddPage(tableStructPage->m_hWnd, StringUtil::blkToTail(tblName).c_str(), 2, tableStructPage);
		foundPage = tabView.GetPageCount() - 1;
		databaseSupplier->activeTabPageHwnd = tableStructPage->m_hWnd;
		tabView.SetActivePage(foundPage);
	}
}

void RightWorkViewAdapter::renameTable()
{
	int n = tabView.GetPageCount();
	uint64_t userDbId = databaseSupplier->getSelectedUserDbId();
	std::wstring & oldTableName = databaseSupplier->oldTableName;
	std::wstring & newTableName = databaseSupplier->newTableName;
	for (int i = 0; i < n; i++) {
		HWND pageHwnd = tabView.GetPageHWND(i);
		std::wstring pageTitle = tabView.GetPageTitle(i);
		StringUtil::trim(pageTitle);
		// same name
		if (pageTitle != oldTableName) {
			continue;
		}
		// query page has properties of TABLE_DATA and  same user db id
		auto iter = std::find_if(queryPagePtrs.begin(), queryPagePtrs.end(), [&pageHwnd, &userDbId](QueryPage * page) {
			if (page && page->IsWindow() 
				&& page->m_hWnd == pageHwnd 
				&& page->getSupplier()->getOperateType() == TABLE_DATA 
				&& page->getSupplier()->getRuntimeUserDbId() == userDbId) {
				return true;
			}
			return false;
		});
		if (iter != queryPagePtrs.end()) {
			tabView.SetPageTitle(i, newTableName.c_str());
			(*iter)->getResultTabView().loadTableDatas(newTableName);
			continue;
		}

		// database page has properties of TABLE_DATA and  same user db id
		auto iter2 = std::find_if(tablePagePtrs.begin(), tablePagePtrs.end(), [&pageHwnd, &userDbId](TableStructurePage * page) {
			if (page && page->IsWindow() 
				&& page->m_hWnd == pageHwnd 
				&& page->getSupplier()->getOperateType() == MOD_TABLE 
				&& page->getSupplier()->getRuntimeUserDbId() == userDbId) {
				return true;
			}
			return false;
		});
		if (iter2 != tablePagePtrs.end()) {
			tabView.SetPageTitle(i, newTableName.c_str());
			(*iter2)->renameTable(oldTableName, newTableName);
		}
	}
}


void RightWorkViewAdapter::dropTable()
{
	int n = tabView.GetPageCount();
	uint64_t userDbId = databaseSupplier->getSelectedUserDbId();
	std::wstring & tblName = databaseSupplier->selectedTable;
	std::stack<int> delPageStack;
	for (int i = 0; i < n; i++) {
		HWND pageHwnd = tabView.GetPageHWND(i);
		std::wstring pageTitle = tabView.GetPageTitle(i);
		StringUtil::trim(pageTitle);
		// same name
		if (pageTitle != tblName) {
			continue;
		}
		// query page has properties of TABLE_DATA and  same user db id
		auto iter = std::find_if(queryPagePtrs.begin(), queryPagePtrs.end(), [&pageHwnd, &userDbId](QueryPage * page) {
			if (page && page->IsWindow() 
				&& page->m_hWnd == pageHwnd 
				&& page->getSupplier()->getOperateType() == TABLE_DATA 
				&& page->getSupplier()->getRuntimeUserDbId() == userDbId) {
				page->DestroyWindow();
				delete page;
				page = nullptr;
				return true;
			}
			return false;
		});

		// reload table data
		if (iter != queryPagePtrs.end()) {
			queryPagePtrs.erase(iter);
			delPageStack.push(i);
			continue;
		}

		// database page has properties of TABLE_DATA and  same user db id
		auto iter2 = std::find_if(tablePagePtrs.begin(), tablePagePtrs.end(), [&pageHwnd, &userDbId](TableStructurePage * page) {
			if (page && page->IsWindow() 
				&& page->m_hWnd == pageHwnd 
				&& page->getSupplier()->getOperateType() == MOD_TABLE 
				&& page->getSupplier()->getRuntimeUserDbId() == userDbId) {
				page->DestroyWindow();
				delete page;
				page = nullptr;
				return true;
			}
			return false;
		});
		if (iter2 != tablePagePtrs.end()) {
			tablePagePtrs.erase(iter2);
			delPageStack.push(i);	
		}
	}

	while (!delPageStack.empty()) {
		int nPage = delPageStack.top();
		delPageStack.pop();
		tabView.RemovePage(nPage);
	}	
}

LRESULT RightWorkViewAdapter::refreshTableDataForSameDbTablePage(uint64_t userDbId, const std::wstring & theTblName)
{
	int n = tabView.GetPageCount();
	userDbId = userDbId ? userDbId : databaseSupplier->getSelectedUserDbId();
	std::wstring tblName = !theTblName.empty() ? theTblName : databaseSupplier->selectedTable;
	for (int i = 0; i < n; i++) {
		HWND pageHwnd = tabView.GetPageHWND(i);
		std::wstring pageTitle = tabView.GetPageTitle(i);
		StringUtil::trim(pageTitle);
		// same name
		if (pageTitle != tblName) {
			continue;
		}
		// query page has properties of TABLE_DATA and  same user db id
		auto iter = std::find_if(queryPagePtrs.begin(), queryPagePtrs.end(), [&pageHwnd, &userDbId](QueryPage * page) {
			if (page && page->IsWindow()
				&& page->m_hWnd == pageHwnd
				&& page->getSupplier()->getOperateType() == TABLE_DATA
				&& page->getSupplier()->getRuntimeUserDbId() == userDbId) {
				return true;
			}
			return false;
		});

		// reload table data
		if (iter != queryPagePtrs.end()) {
			(*iter)->getResultTabView().loadTableDatas(tblName);
		}
	}
	return true;
}

void RightWorkViewAdapter::createTabMenu()
{
	if (tabMenu.IsMenu()) {
		return;
	}
	tabMenu.CreatePopupMenu();
	tabMenu.AppendMenu(MF_STRING, Config::TABVIEW_CLOSE_THIS_MENU_ID, S(L"tab-close-this").c_str());
	tabMenu.AppendMenu(MF_STRING, Config::TABVIEW_CLOSE_OTHERS_MENU_ID, S(L"tab-close-others").c_str());
	tabMenu.AppendMenu(MF_STRING, Config::TABVIEW_CLOSE_LEFT_MENU_ID, S(L"tab-close-left").c_str());
	tabMenu.AppendMenu(MF_STRING, Config::TABVIEW_CLOSE_RIGHT_MENU_ID, S(L"tab-close-right").c_str());
}

void RightWorkViewAdapter::popupTabMenu(CPoint & pt)
{
	tabMenu.TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, parentHwnd);
}
