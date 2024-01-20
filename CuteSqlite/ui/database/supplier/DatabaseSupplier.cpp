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

 * @file   DatabaseSupplier.cpp
 * @brief  Holds temporary and persistent data across multiple windows
 * 
 * @author Xuehan Qin
 * @date   2023-05-23
 *********************************************************************/
#include "stdafx.h"
#include "DatabaseSupplier.h"
#include "utils/StringUtil.h"

DatabaseSupplier * DatabaseSupplier::theInstance = nullptr;

DatabaseSupplier::DatabaseSupplier()
{

}

DatabaseSupplier * DatabaseSupplier::getInstance()
{
	if (DatabaseSupplier::theInstance == nullptr) {
		DatabaseSupplier::theInstance = new DatabaseSupplier();
	}
	return DatabaseSupplier::theInstance;
}

void DatabaseSupplier::clearSelectedData()
{
	selectedUserDbId = 0;
	selectedSchema.clear();
	selectedTable.clear();
	selectedColumn.clear();
	selectedIndexName.clear();
	selectedTriggerName.clear();
	selectedViewName.clear();
}

