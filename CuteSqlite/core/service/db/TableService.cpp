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

 * @file   TableService.cpp
 * @brief  The Sqlite table management class
 * 
 * @author Xuehan Qin
 * @date   2023-10-07
 *********************************************************************/
#include "stdafx.h"
#include "TableService.h"
#include "utils/Log.h"
#include "utils/SqlUtil.h"
#include "core/common/Lang.h"
#include "core/common/exception/QRuntimeException.h"
#include "core/common/exception/QSqlExecuteException.h"

TableService::TableService()
{

}

TableService::~TableService()
{

}

uint64_t TableService::getTableDataCount(uint64_t userDbId, std::wstring tblName, const std::wstring & schema)
{
	ATLASSERT(userDbId > 0 && !tblName.empty());
	return getRepository()->getDataCount(userDbId, tblName, schema);
}

int TableService::getTableDataPageCount(uint64_t userDbId, std::wstring tblName, int perpage, const std::wstring & schema)
{
	ATLASSERT(userDbId > 0 && !tblName.empty() && perpage > 0);
	uint64_t rowCount = getTableDataCount(userDbId, tblName, schema);
	return rowCount % perpage ? static_cast<int>(rowCount / perpage + 1) : static_cast<int>(rowCount / perpage);
}

DataList TableService::getTableDataList(uint64_t userDbId, std::wstring tblName, int page, int perpage, const std::wstring & schema)
{
	ATLASSERT(userDbId > 0 && !tblName.empty() && page > 0 && perpage > 0);
	return getRepository()->getPageDataList(userDbId, tblName, page, perpage, schema);
}

std::pair<Columns, DataList> TableService::getTableDataListWithColumns(uint64_t userDbId, std::wstring tblName, int page, int perpage, const std::wstring & schema)
{
	ATLASSERT(userDbId > 0 && !tblName.empty() && page > 0 && perpage > 0);
	std::pair<Columns, DataList> result;
	ColumnInfoList columnInfoList = columnUserRepository->getListByTblName(userDbId, tblName, schema);
	Columns columns;
	for (auto columnInfo : columnInfoList) {
		columns.push_back(columnInfo.name);
	}
	DataList dataList = getTableDataList(userDbId, tblName, page, perpage, schema);
	result = { columns, dataList };
	return result;
}

bool TableService::isExistsTblName(uint64_t userDbId, const std::wstring & tblName, const std::wstring & schema)
{
	UserTable userTable = getRepository()->getTable(userDbId, tblName, schema);
	if (userTable.name.empty()) {
		return false;
	}

	return true;
}

bool TableService::execBySql(uint64_t userDbId, const std::wstring & sql)
{
	ATLASSERT(userDbId > 0 && !sql.empty());
	try {
		getRepository()->execBySql(userDbId, sql);
	} catch (QSqlExecuteException & ex) {
		throw ex;
		return false;
	}
	return true;
}


UserTableList TableService::getUserTables(uint64_t userDbId)
{
	ATLASSERT(userDbId > 0);
	return tableUserRepository->getListByUserDbId(userDbId);
}

UserTable TableService::getUserTable(uint64_t userDbId, const std::wstring & tblName, const std::wstring & schema)
{
	ATLASSERT(userDbId > 0 && !tblName.empty());
	return tableUserRepository->getTable(userDbId, tblName, schema);
}

UserTableStrings TableService::getUserTableStrings(uint64_t userDbId, const std::wstring & schema)
{
	ATLASSERT(userDbId > 0);
	UserTableStrings result;
	UserTableList userTableList = tableUserRepository->getListByUserDbId(userDbId, schema);
	for (auto userTable : userTableList) {
		result.push_back(userTable.name);
	}
	return result;
}


ColumnInfoList TableService::getUserColumns(uint64_t userDbId, const std::wstring & tblName, const std::wstring & schema, bool isSimple)
{
	ATLASSERT(userDbId > 0 && !tblName.empty());
	ColumnInfoList result = columnUserRepository->getListByTblName(userDbId, tblName, schema);
	if (isSimple) {
		return result;
	}

	UserTable userTable = getUserTable(userDbId, tblName, schema);
	IndexInfo primaryKey = SqlUtil::parseConstraintsForPrimaryKey(userTable.sql);
	ColumnInfoList ddlColumns = SqlUtil::parseColumnsByTableDDL(userTable.sql);
	
	auto colVec = StringUtil::split(primaryKey.columns, L",");
	// supplemented the ai and pk properties
	for (ColumnInfo & columnInfo : result) {
		// supplemented the auto increment property
		if (primaryKey.columns == columnInfo.name) { // only one column in Primary key
			columnInfo.ai = primaryKey.ai;
		}

		// supplemented the primary key property		
		auto iter = std::find(colVec.begin(), colVec.end(), columnInfo.name);
		if (iter != colVec.end()) {
			columnInfo.pk = 1;
		}

		auto ddlIter = std::find_if(ddlColumns.begin(), ddlColumns.end(), [&columnInfo](const ColumnInfo & info) {
			return columnInfo.name == info.name;
		});
		if (ddlIter != ddlColumns.end()) {
			columnInfo.un = (*ddlIter).un;
			columnInfo.ai = columnInfo.ai ? columnInfo.ai : (*ddlIter).ai;
		}
	}
	return result;
}

UserIndexList TableService::getUserIndexes(uint64_t userDbId, const std::wstring & tblName, const std::wstring & schema)
{
	ATLASSERT(userDbId > 0 && !tblName.empty());
	return indexUserRepository->getListByTblName(userDbId, tblName, schema);
}

IndexInfoList TableService::getIndexInfoList(uint64_t userDbId, const std::wstring & tblName, const std::wstring & schema)
{
	ATLASSERT(userDbId > 0 && !tblName.empty());
	UserTable userTable = tableUserRepository->getTable(userDbId, tblName, schema);
	if (userTable.name.empty() || userTable.sql.empty()) {
		return IndexInfoList();
	}

	std::wstring & createTblSql = userTable.sql;
	IndexInfoList indexInfoList = SqlUtil::parseConstraints(createTblSql);

	return indexInfoList;
}

std::wstring TableService::getPrimaryKeyColumn(uint64_t userDbId, const std::wstring & tblName, Columns & columns, const std::wstring & schema)
{
	ATLASSERT(userDbId > 0 && !tblName.empty());
	UserTable userTable = tableUserRepository->getTable(userDbId, tblName, schema);
	if (userTable.name.empty() || userTable.sql.empty()) {
		return L"";
	}

	std::wstring & createTblSql = userTable.sql;
	std::wstring primaryKey = SqlUtil::parsePrimaryKey(createTblSql);
	if (primaryKey.empty()) {
		return primaryKey;
	}

	for (auto column : columns) {
		if (column == primaryKey) {
			return primaryKey;
		}
	}
	return L"";
}

ForeignKeyList TableService::getForeignKeyList(uint64_t userDbId, const std::wstring & tblName, const std::wstring & schema /*= std::wstring()*/)
{
	ATLASSERT(userDbId > 0 && !tblName.empty());
	UserTable userTable = tableUserRepository->getTable(userDbId, tblName, schema);
	if (userTable.name.empty() || userTable.sql.empty()) {
		return ForeignKeyList();
	}

	std::wstring & createTblSql = userTable.sql;
	ForeignKeyList foreignKeyList = SqlUtil::parseForeignKeysByTableDDL(createTblSql);

	return foreignKeyList;
}

void TableService::renameTable(uint64_t userDbId, const std::wstring & oldTableName, const std::wstring & newTableName, const std::wstring & schema /*= std::wstring()*/)
{
	UserTable userTable = tableUserRepository->getTable(userDbId, newTableName, schema);
	if (!userTable.name.empty()) {
		throw QRuntimeException(L"200026");
	}

	tableUserRepository->renameTable(userDbId, oldTableName, newTableName, schema);
}

void TableService::truncateTable(uint64_t userDbId, const std::wstring & tblName, const std::wstring & schema)
{
	UserTable userTable = tableUserRepository->getTable(userDbId, tblName, schema);
	if (userTable.name.empty()) {
		throw QRuntimeException(L"200027");
	}

	tableUserRepository->truncateTable(userDbId, tblName, schema);
}
