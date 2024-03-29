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

 * @file   IndexUserRepository.h
 * @brief  Operations on indexes in the user database
 * 
 * @author Xuehan Qin
 * @date   2023-05-20
 *********************************************************************/
#pragma once
#include "core/common/repository/BaseUserRepository.h"
#include "core/entity/Entity.h"

class IndexUserRepository : public BaseUserRepository<IndexUserRepository>
{
public:
	IndexUserRepository() {}
	~IndexUserRepository() {}

	UserIndexList getListByTblName(uint64_t userDbId, const std::wstring &tblName, const std::wstring & schema = std::wstring());
	IndexInfoList getInfoListByTblName(uint64_t userDbId, const std::wstring &tblName, const std::wstring & schema = std::wstring());
	UserIndexList getListByUserDbId(uint64_t userDbId);
	UserIndex getByRowId(uint64_t userDbId, uint64_t rowId);
	UserIndex getByIndexName(uint64_t userDbId, const std::wstring &tblName, const std::wstring & schema = std::wstring());
	PragmaIndexColumns getPragmaIndexColumns(uint64_t userDbId, const std::wstring &indexName);
private:
	UserIndex toUserIndex(QSqlStatement &query);
	IndexInfo toIndexInfo(QSqlStatement &query);
	PragmaIndexColumn toPragmaIndexColumn(QSqlStatement &query);
};
