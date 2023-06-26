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

 * @file   Entity.h
 * @brief  The entities struct of the system database tables or ui used
 * 
 * @author Xuehan Qin
 * @date   2023-06-10
 *********************************************************************/
#ifndef _ENTIRY_H_
#define _ENTIRY_H_
#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <map>



typedef struct _SysInit
{
	std::wstring name;
	std::wstring val;
} SysInit;
/**
 * 系统加载配置项列表
 */
typedef std::vector<SysInit> SysInitList;

// 配置项(<key,val>)，存放于数据库表sys_init以及系统的ini文件中，同事具备排序功能
typedef std::vector<std::pair<std::wstring, std::wstring>> Setting;

// INI的配置项，有分组功能<group, Setting> 
typedef std::unordered_map<std::wstring, Setting> IniSetting;


// databases
typedef struct _UserDb {
	uint64_t id = 0;
	std::wstring name;
	std::wstring path;
	int isActive = 0;
	std::wstring createdAt;
	std::wstring updatedAt;
} UserDb;

typedef std::vector<UserDb> UserDbList;

// tables,views,triggers,index
typedef struct _UserTable {
	std::wstring name;
	std::wstring sql;
} UserTable, UserView, UserTrigger, UserIndex;

typedef std::vector<UserTable> UserTableList;
typedef std::vector<UserView> UserViewList;
typedef std::vector<UserTrigger> UserTriggerList;
typedef std::vector<UserIndex> UserIndexList;

// table fields
typedef struct _ColumnInfo {
	uint32_t cid = 0;
	std::wstring name;
	std::wstring type;
	int notnull = 0;
	std::wstring dfltValue;
	int pk = 0;
} ColumnInfo;
typedef std::vector<ColumnInfo> ColumnInfoList;

//Export to CSV params
typedef struct _ExportCsvParams {
	std::wstring csvFieldTerminatedBy;
	std::wstring csvFieldEnclosedBy;
	std::wstring csvFieldEscapedBy;
	std::wstring csvLineTerminatedBy;
	bool hasColumnOnTop = false;
} ExportCsvParams;

//Export to Excel XML params
typedef struct _ExportExcelParams {
	int excelComlumnMaxSize = 0;
	int excelDecimalPlaces = 0;
} ExportExcelParams;

//Export to sql params
typedef struct _ExportSqlParams {
	std::wstring sqlSetting;
} ExportSqlParams;

typedef std::vector<std::wstring> ExportSelectedColumns;

// the data structure for show in listview or export
typedef std::vector<std::wstring> RowItem, Columns, UserTableStrings;
// data items list
typedef std::list<RowItem> DataList;

//select sql - limit clause params
typedef struct _LimitParams {
	bool checked = false;
	int offset = 0;
	int rows = 0;
} LimitParams;

typedef struct {
	int iItem;
	int iSubItem;
	std::wstring origVal;
	std::wstring newVal;
} SubItemValue;
typedef std::vector<SubItemValue> SubItemValues;
#endif

