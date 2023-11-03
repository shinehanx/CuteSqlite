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

 * @file   ImportDatabaseAdapter.h
 * @brief  Import the table(s),view(s),trigger(s) of the selected database from SQL file.
 * 
 * @author Xuehan Qin
 * @date   2023-10-08
 *********************************************************************/
#pragma once
#include "ui/common/adapter/QAdapter.h"
#include "core/entity/Entity.h"
#include "core/service/db/DatabaseService.h"
#include "core/service/db/TableService.h"
#include "core/service/db/SqlService.h"
#include "ui/common/listview/QListViewCtrl.h"

class ImportDatabaseAdapter : public QAdapter<ImportDatabaseAdapter>
{
public:
	ImportDatabaseAdapter(HWND parentHwnd, ATL::CWindow * view);
	~ImportDatabaseAdapter();

	//FOR IMPORT AS SQL 
	UserDbList getDbs();
	void loadDbs();
	bool importFromSql(uint64_t userDbId, const std::wstring & importPath);

protected:
	UserDbList dbs;

	DatabaseService * databaseService = DatabaseService::getInstance();
	TableService * tableService = TableService::getInstance();
	SqlService * sqlService = SqlService::getInstance();

	std::wstring readFromSqlFile(const std::wstring & importPath);
	void execCommandLine(const wchar_t * cmdline);
};
