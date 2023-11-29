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

 * @file   QueryPageSupplier.h
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-10-30
 *********************************************************************/
#pragma once
#include "core/entity/Entity.h"
#include "ui/database/rightview/common/QPageSupplier.h"

class QueryPageSupplier : public QPageSupplier {
public:
	QueryPageSupplier();
	~QueryPageSupplier();
	

	static const std::vector<std::wstring> sqlTags;
	static const std::list<std::tuple<int, std::wstring, std::wstring>> pragmas;

	// sql statements
	std::vector<std::wstring> sqlVector;

	// sql log dialog use bitmap
	HBITMAP topBitmap = nullptr;
	HBITMAP sucessBitmap = nullptr;
	HBITMAP errorBitmap = nullptr;

	// Using semicolons to separate a SQL statement becomes a member variable sqlVector
	void splitToSqlVector(std::wstring sql);

	// tables
	UserTableStrings & getCacheUserTableStrings(uint64_t userDbId);
	void setCacheUserTableStrings(uint64_t userDbId, UserTableStrings & tblStrs);

	// table columns
	Columns & getCacheTableColumns(uint64_t userDbId, const std::wstring & tblName);
	void setCacheTableColumns(uint64_t userDbId, const std::wstring & tblName, const Columns & columns);

	std::wstring & getCacheUseSql() { return cacheUseSql; }
	void setCacheUseSql(const std::wstring & val) { cacheUseSql = val; }
	void clearCacheUseSql() { cacheUseSql.clear(); }

	
	HWND getActiveResultTabPageHwnd() const { return activeResultTabPageHwnd; }
	void setActiveResultTabPageHwnd(HWND val) { activeResultTabPageHwnd = val; }
private:
	// template params: first - userDbId, second - user table strings
	std::map<uint64_t, UserTableStrings> cacheUserTableMap;
	
	// template params:  first - userDbId, second - table name, third - columns strings
	std::map<std::pair<uint64_t, std::wstring>, Columns> cacheTableColumnsMap;

	// 
	std::wstring cacheUseSql;

	// The HWND of active page in ResultTabView
	HWND activeResultTabPageHwnd = nullptr;
};
