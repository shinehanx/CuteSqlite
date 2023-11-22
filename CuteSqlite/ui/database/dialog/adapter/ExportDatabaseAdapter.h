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

 * @file   ExportDatabaseAdapter.h
 * @brief  Export the table(s),view(s),trigger(s) of the selected database in SQL.
 * 
 * @author Xuehan Qin
 * @date   2023-10-04
 *********************************************************************/
#pragma once
#pragma once
#include "ui/common/adapter/QAdapter.h"
#include "core/entity/Entity.h"
#include "core/service/db/DatabaseService.h"
#include "core/service/db/TableService.h"
#include "core/service/export/ExportDatabaseObjectsService.h"
#include "ui/database/supplier/DatabaseSupplier.h"

class ExportDatabaseAdapter : public QAdapter<ExportDatabaseAdapter>
{
public:
	ExportDatabaseAdapter(HWND parentHwnd, ATL::CWindow * view);
	~ExportDatabaseAdapter();

	UserDbList getDbs();
	void loadDbs();
	uint64_t getSeletedUserDbId();

	void exportObjectsToSql(uint64_t userDbId,
		std::wstring & exportPath,
		UserTableList & tblList,
		UserViewList & viewList,
		UserTriggerList & triggerList,
		StructAndDataParams & structureAndDataParams,
		InsertStatementParams & insertStatementParams,
		TblStatementParams & tblStatementParams);
private:
	UserDbList dbs;
	ExportDatabaseObjectsService * exportDatabaseObjectsService = ExportDatabaseObjectsService::getInstance();	
	DatabaseService * databaseService = DatabaseService::getInstance();
	TableService * tableService = TableService::getInstance();
	DatabaseSupplier * databaseSupplier = DatabaseSupplier::getInstance();

	int exportTablesToSql(uint64_t userDbId,
		std::wofstream & ofs, 
		UserTableList & tblList, 
		StructAndDataParams & structureAndDataParams,
		InsertStatementParams & insertStatementParams,
		TblStatementParams & tblStatementParams, int & percent);
	void exportViewsToSql(uint64_t userDbId, std::wofstream & ofs, UserViewList & viewList, 
		TblStatementParams & tblStatementParams, int & percent);
	void exportTriggersToSql(uint64_t userDbId, std::wofstream & ofs, UserTriggerList & triggerList, 
		TblStatementParams & tblStatementParams, int & percent);

	void doExportCreateTableStructure(std::wofstream & ofs, uint64_t userDbId, 
		const UserTable tbl, const StructAndDataParams & structureAndDataParams, const TblStatementParams & tblStatementParams);
	void doExportCreateIndex(std::wofstream & ofs, uint64_t userDbId, 
		const UserTable tbl, const StructAndDataParams & structureAndDataParams, const TblStatementParams & tblStatementParams);

	void doExportTableInsertStatement(std::wofstream & ofs, uint64_t userDbId, 
		const UserTable &tbl, const StructAndDataParams & structureAndDataParams, const InsertStatementParams & insertStatementParams);

	void replaceCreateTableClause(std::wstring &sql);	
	void replaceCreateIndexClause(std::wstring &sql);	
	void replaceCreateViewClause(std::wstring &sql);	
	void replaceCreateTriggerClause(std::wstring &sql);	
};

