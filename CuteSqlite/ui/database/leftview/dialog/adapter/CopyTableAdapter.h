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

 * @file   CopyTableAdapter.h
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-10-31
 *********************************************************************/
#pragma once
#include "ui/common/adapter/QAdapter.h"
#include <atlwin.h>
#include "core/entity/Entity.h"
#include "core/service/db/DatabaseService.h"
#include "core/service/db/TableService.h"
#include "ui/database/leftview/dialog/supplier/CopyTableSupplier.h"

class CopyTableAdapter : public QAdapter<CopyTableAdapter>
{
public:
	CopyTableAdapter(HWND parentHwnd, ATL::CWindow * view);
	CopyTableAdapter(HWND parentHwnd, CopyTableSupplier * supplier);
	~CopyTableAdapter();

	UserDbList getDbs();
	std::wstring getDefaultShardingStrategyExpress(int suffixBegin, int suffixEnd);
	bool matchDefaultExpress(const std::wstring & str, int & capacity, int & offset);
	std::wstring getPreviewSqlInSameDb();
	std::list<std::wstring> getSqlsInSameDb();

	bool verifyParams();
	bool startCopy();

	CopyTableSupplier * getSupplier() { return supplier; }

	std::wstring generateCreateDdlForTargetTable(uint16_t suffix, const std::wstring & targetTblName);
	std::wstring genderatePageDataSql(const DataList & pageDataList, const std::wstring & targetTblName);
private:
	DatabaseService * databaseService = DatabaseService::getInstance();
	TableService * tableService = TableService::getInstance();
	CopyTableSupplier * supplier = nullptr;

	// same db
	bool doExecSqlsInSameDb();
	std::wstring generateCopyDataSqlInSameDb(uint16_t suffix, const std::wstring & targetTblName);

	// same db
	bool doExecSqlsInOtherDb();
	bool doExecCopyDataSqlInOtherDb(uint16_t suffix, const std::wstring & targetTblName);
};
