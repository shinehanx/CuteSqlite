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

 * @file   CopyTableSupplier.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-11-01
 *********************************************************************/
#include "stdafx.h"
#include "CopyTableSupplier.h"
#include "utils/StringUtil.h"

CopyTableSupplier::CopyTableSupplier(DatabaseSupplier * databaseSupplier)
{
	this->runtimeUserDbId = databaseSupplier->getSelectedUserDbId();
	this->runtimeTblName = databaseSupplier->selectedTable;
	this->runtimeSchema = databaseSupplier->selectedSchema;
}

CopyTableSupplier::CopyTableSupplier()
{

}

CopyTableSupplier::~CopyTableSupplier()
{

}

/**
 * Get sharding tables.
 * 
 * @return map - first(uint16_t) : table suffix, second(string) : table name
 */
ShardingTableMap CopyTableSupplier::getShardingTables()
{
	ShardingTableMap result;
	if (!targetTable.empty() && 
		(targetTable.find(L"{n}") == std::wstring::npos || !enableTableSharding)) {
		result[0] = targetTable; // single table
	}

	if (tblSuffixBegin < 0 || tblSuffixEnd < 0 || 
		tblSuffixBegin >= tblSuffixEnd || !enableTableSharding ||
		targetTable.empty() || targetTable.find(L"{n}") == std::wstring::npos) {
		return result;
	}

	for (int16_t i = tblSuffixBegin; i <= tblSuffixEnd; i++) {
		std::wstring tblName = StringUtil::replace(targetTable, L"{n}", std::to_wstring(i));
		result[i] = tblName;
	}

	return result;
}