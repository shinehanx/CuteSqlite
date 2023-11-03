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

 * @file   DataBaseSupplier.h
 * @brief  Holds runtime and persistent data across multiple windows
 * 
 * @author Xuehan Qin
 * @date   2023-05-23
 *********************************************************************/
#pragma once
#include <string>
#include <vector>

class DatabaseSupplier
{
public:
	// TabView page type 
	typedef enum {
		QUERY_PAGE,
		HISTORY_PAGE,
		RESULT_PAGE,
		INFO_PAGE,
		TABLE_DATA_PAGE,		
		TABLE_PAGE,
		VIEW_PAGE,
		TRIGGER_PAGE
	} TabPageType;

	
	DatabaseSupplier();
	~DatabaseSupplier() {}

	static DatabaseSupplier * getInstance();

	

	// -----------------------------------
	// rumtime data
	// -----------------------------------
	// selected user db id
	uint64_t selectedUserDbId = 0;

	HWND activeTabPageHwnd = nullptr;
	
	// selected schema
	std::wstring selectedSchema;

	// selected table name
	std::wstring selectedTable;

	// New view name	
	std::wstring newViewName;
	std::wstring newTriggerName;

	// Rename table
	std::wstring oldTableName;
	std::wstring newTableName;

	// // --------------------------------

	void setSeletedUserDbId(uint64_t userDbId) { selectedUserDbId = userDbId; }
	uint64_t getSelectedUserDbId() { return selectedUserDbId; }

private:
	static DatabaseSupplier * theInstance;

	
	
};
