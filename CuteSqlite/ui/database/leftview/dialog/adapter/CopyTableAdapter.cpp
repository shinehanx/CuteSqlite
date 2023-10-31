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

 * @file   CopyTableAdapter.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-11-01
 *********************************************************************/
#include "stdafx.h"
#include "CopyTableAdapter.h"


CopyTableAdapter::CopyTableAdapter(HWND parentHwnd, CopyTableSupplier * supplier)
{
	QAdapter::parentHwnd = parentHwnd;
	this->supplier = supplier;
}

CopyTableAdapter::CopyTableAdapter(HWND parentHwnd, ATL::CWindow * view)
{

}


CopyTableAdapter::~CopyTableAdapter()
{

}

UserDbList CopyTableAdapter::getDbs()
{
	return databaseService->getAllUserDbs();
}

std::wstring CopyTableAdapter::getDefaultShardingStrategyExpress(int suffixBegin, int suffixEnd)
{
	if (!(suffixBegin > 0 && suffixEnd > 0 && suffixBegin < suffixEnd)) {
		return L"";
	}

	Columns columns = 
		tableService->getUserColumnStrings(supplier->getRuntimeUserDbId(),supplier->getRuntimeTblName());

	std::wstring primaryKeyColumn =
		tableService->getPrimaryKeyColumn(supplier->getRuntimeUserDbId(),supplier->getRuntimeTblName(), columns);
	if (primaryKeyColumn.empty()) {
		return L"";
	}

	int capacity = suffixEnd - suffixBegin + 1;
	std::wstring express = primaryKeyColumn;
	express.append(L" % ").append(std::to_wstring(capacity));
	if (suffixBegin > 0) {
		express.append(L" + ").append(std::to_wstring(suffixBegin));
	}
	
	return express;
}
