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

class TableService : public BaseService<TableService, TableUserRepository>
{
public:
	TableService();
	~TableService();

	uint64_t getTableDataCount(uint64_t userDbId, std::wstring tblName);
	int getTableDataPageCount(uint64_t userDbId, std::wstring tblName, int perpage);
	DataList getTableDataList(uint64_t userDbId, std::wstring tblName, int page, int perpage);
	std::pair<Columns, DataList> getTableDataListWithColumns(uint64_t userDbId, std::wstring tblName, int page, int perpage);
private:
	ColumnUserRepository  * columnUserRepository = ColumnUserRepository::getInstance();
};
