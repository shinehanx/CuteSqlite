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

 * @file   QueryPageEditorAdapter.h
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-11-13
 *********************************************************************/
#pragma once
#include "core/service/db/TableService.h"
#include "core/service/db/DatabaseService.h"
#include "ui/common/adapter/QAdapter.h"
#include "ui/common/edit/adapter/QHelpEditAdapter.h"
#include "ui/database/rightview/page/supplier/QueryPageSupplier.h"

class QueryPageEditorAdapter : public QAdapter<QueryPageEditorAdapter>, public QHelpEditAdapter
{
public:
	QueryPageEditorAdapter(HWND parentHwnd, QueryPageSupplier * supplier);
	~QueryPageEditorAdapter();
	virtual std::vector<std::wstring> getTags(const std::wstring & line, const std::wstring & preline, const std::wstring & word, size_t curPosInLine);
	virtual std::vector<std::wstring> & getCacheUserTableStrings(uint64_t userDbId);
	virtual uint64_t getCurrentUserDbId();

	QueryPageSupplier * getSupplier() { return supplier; }

	// For menus
	void createMenus();	
	void createTemplatesMenu();
	void createPragmasMenu();
	void createSqlLogMenu();
	// Pop up menu
	void popupTemplatesMenu(CPoint & pt);
	void popupPragmasMenu(CPoint & pt);

	// Use sql log
	void clearUseSql();
	void setUseSql(const std::wstring & useSql);
	std::wstring & getUseSql() const;
private:
	HWND parentHwnd;
	CMenu templatesMenu;
	CMenu pragmasMenu;
	CMenu sqlLogMenu;

	QueryPageSupplier * supplier = nullptr;
	TableService * tableService = TableService::getInstance();
	DatabaseService * databaseService = DatabaseService::getInstance();

	// For auto complete
	std::vector<std::wstring> getSelectTags(const std::wstring & upline, const std::wstring & upPreline, const std::wstring & upword, size_t curPosInLine);
	
	std::vector<std::wstring> getUpdateTags(const std::wstring & upline, const std::wstring & upPreline, const std::wstring & upword, size_t curPosInLine);
	Columns& getCacheTableColumns(uint64_t userDbId, const std::wstring & tblName);
};
