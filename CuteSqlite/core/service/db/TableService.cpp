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
#include "core/common/exception/QRuntimeException.h"

TableService::TableService()
{

}

TableService::~TableService()
{

}

uint64_t TableService::getTableDataCount(uint64_t userDbId, std::wstring tblName)
{
	ATLASSERT(userDbId > 0 && !tblName.empty());
	return getRepository()->getDataCount(userDbId, tblName);
}

int TableService::getTableDataPageCount(uint64_t userDbId, std::wstring tblName, int perpage)
{
	ATLASSERT(userDbId > 0 && !tblName.empty() && perpage > 0);
	uint64_t rowCount = getTableDataCount(userDbId, tblName);
	return rowCount % perpage ? static_cast<int>(rowCount / perpage + 1) : static_cast<int>(rowCount / perpage);
}

DataList TableService::getTableDataList(uint64_t userDbId, std::wstring tblName, int page, int perpage)
{
	ATLASSERT(userDbId > 0 && !tblName.empty() && page > 0 && perpage > 0);
	return getRepository()->getPageDataList(userDbId, tblName, page, perpage);
}

std::pair<Columns, DataList> TableService::getTableDataListWithColumns(uint64_t userDbId, std::wstring tblName, int page, int perpage)
{
	ATLASSERT(userDbId > 0 && !tblName.empty() && page > 0 && perpage > 0);
	std::pair<Columns, DataList> result;
	ColumnInfoList columnInfoList = columnUserRepository->getListByTblName(userDbId, tblName);
	Columns columns;
	for (auto columnInfo : columnInfoList) {
		columns.push_back(columnInfo.name);
	}
	DataList dataList = getTableDataList(userDbId, tblName, page, perpage);
	result = { columns, dataList };
	return result;
}

