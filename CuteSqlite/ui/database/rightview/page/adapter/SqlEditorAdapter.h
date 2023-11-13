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

 * @file   SqlEditorAdapter.h
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-11-11
 *********************************************************************/
#pragma once
#include "ui/common/edit/adapter/QHelpEditAdapter.h"
#include "ui/database/rightview/page/supplier/QueryPageSupplier.h"
#include "core/service/db/TableService.h"
#include "core/service/db/DatabaseService.h"

class SqlEditorAdapter : public QHelpEditAdapter {
public:
	SqlEditorAdapter(QueryPageSupplier * supplier);
	virtual std::vector<std::wstring> getTags(const std::wstring & line, const std::wstring & preline, const std::wstring & word, size_t curPosInLine);
	
private:
	
	QueryPageSupplier * supplier = nullptr;
	TableService * tableService = TableService::getInstance();
	DatabaseService * databaseService = DatabaseService::getInstance();

	std::vector<std::wstring> getSelectTags(const std::wstring & upline, const std::wstring & upPreline, const std::wstring & upword, size_t curPosInLine);

	UserTableStrings & getCacheUserTableStrings(uint64_t userDbId);

	std::vector<std::wstring> getUpdateTags(const std::wstring & upline, const std::wstring & upPreline, const std::wstring & upword, size_t curPosInLine);

	Columns& getCacheTableColumns(uint64_t userDbId, const std::wstring & tblName);
};
