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

 * @file   TableService.h
 * @brief  The Sqlite table management class
 * 
 * @author Xuehan Qin
 * @date   2023-10-07
 *********************************************************************/
#pragma once
#include "core/common/service/BaseService.h"
#include "core/repository/user/TableUserRepository.h"
#include "core/repository/user/ColumnUserRepository.h"
#include "core/repository/user/IndexUserRepository.h"

class TableService : public BaseService<TableService, TableUserRepository>
{
public:
	TableService();
	~TableService();

	uint64_t getTableDataCount(uint64_t userDbId, const std::wstring & tblName, const std::wstring & schema = std::wstring());
	int getTableDataPageCount(uint64_t userDbId, const std::wstring & tblName, int perpage, const std::wstring & schema = std::wstring());
	uint64_t getTableWhereDataCount(uint64_t userDbId, const std::wstring &  tblName, const std::wstring & whereClause, const std::wstring & schema = std::wstring());
	int getTableWhereDataPageCount(uint64_t userDbId, const std::wstring & tblName, const std::wstring & whereClause, int perpage, const std::wstring & schema = std::wstring());
	DataList getTableDataList(uint64_t userDbId, const std::wstring & tblName, int page, int perpage, const std::wstring & schema = std::wstring());
	DataList getTableWhereDataList(uint64_t userDbId, const std::wstring & tblName, const std::wstring & whereClause, int page, int perpage, const std::wstring & schema = std::wstring());
	std::pair<Columns, DataList> getTableDataListWithColumns(uint64_t userDbId, const std::wstring & tblName, int page, int perpage, const std::wstring & schema = std::wstring());

	bool isExistsTblName(uint64_t userDbId, const std::wstring & tblName, const std::wstring & schema = std::wstring());
	bool execBySql(uint64_t userDbId, const std::wstring & sql);

	// user table operations
	UserTableList getUserTables(uint64_t userDbId);
	UserTable getUserTable(uint64_t userDbId, const std::wstring & tblName, const std::wstring & schema = std::wstring());

	// user table operations
	UserTableStrings getUserTableStrings(uint64_t userDbId, const std::wstring & schema = std::wstring());

	// user columns operations
	ColumnInfoList getUserColumns(uint64_t userDbId, const std::wstring & tblName, const std::wstring & schema = std::wstring(), bool isSimple = true);
	Columns getUserColumnStrings(uint64_t userDbId, const std::wstring & tblName, const std::wstring & schema = std::wstring());

	// user indexes operations
	UserIndexList getUserIndexes(uint64_t userDbId, const std::wstring & tblName, const std::wstring & schema = std::wstring());
	UserIndex getUserIndexByRowId(uint64_t userDbId, uint64_t rowId);
	IndexInfoList getIndexInfoList(uint64_t userDbId, const std::wstring & tblName, const std::wstring & schema = std::wstring());

	
	// get runtime user unique or index columns
	std::wstring getPrimaryKeyColumn(uint64_t userDbId, const std::wstring & tblName, Columns & columns, const std::wstring & schema = std::wstring());

	ForeignKeyList getForeignKeyList(uint64_t userDbId, const std::wstring & tblName, const std::wstring & schema = std::wstring());


	void renameTable(uint64_t userDbId, const std::wstring & oldTableName, const std::wstring & newTableName, const std::wstring & schema = std::wstring());
	void truncateTable(uint64_t userDbId, const std::wstring & tblName, const std::wstring & schema = std::wstring());
	void dropTable(uint64_t userDbId, const std::wstring & tblName, const std::wstring & schema = std::wstring());
private:
	TableUserRepository * tableUserRepository = TableUserRepository::getInstance();
	ColumnUserRepository * columnUserRepository = ColumnUserRepository::getInstance();
	IndexUserRepository * indexUserRepository = IndexUserRepository::getInstance();

};
