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
#include <chrono>

typedef struct {
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
typedef struct {
	uint64_t id = 0;
	std::wstring name;
	std::wstring path;
	int isActive = 0;
	std::wstring createdAt;
	std::wstring updatedAt;
} UserDb;

typedef std::vector<UserDb> UserDbList;

// tables,views,triggers,index
typedef struct {
	std::wstring name;
	std::wstring sql;
	std::wstring tblName;
} UserTable, UserView, UserTrigger, UserIndex;

typedef std::vector<UserTable> UserTableList;
typedef std::vector<UserView> UserViewList;
typedef std::vector<UserTrigger> UserTriggerList;
typedef std::vector<UserIndex> UserIndexList;

// table fields
typedef struct {
	uint32_t cid = 0;
	uint8_t notnull = 0; //not null
	uint8_t pk = 0; // primary key
	uint8_t ai = 0; // auto increment
	uint8_t un = 0; // unsigned

	std::wstring name;
	std::wstring type;
	std::wstring defVal;
	std::wstring checks;
	
	std::chrono::system_clock::time_point seq; //system assign a sequence, for alter table
} ColumnInfo;
typedef std::vector<ColumnInfo> ColumnInfoList;

typedef struct {
	std::wstring name; // constrain name
	std::wstring type; // Primary Key,Unique,Foreign Key,Checks
	uint8_t pk = 0;  // primary key
	uint8_t ai = 0; // Auto increment
	uint8_t un = 0; // unique
	uint8_t fk = 0; // foreign key
	uint8_t ck = 0; // checks
	std::wstring columns; // columns
	std::wstring sql;
	std::chrono::system_clock::time_point seq; //system assign a sequence, for alter table
} IndexInfo;
typedef std::vector<IndexInfo> IndexInfoList;

typedef struct  {
	std::wstring name; // constrain name
	std::wstring type = L"Foreign Key"; // must be Foreign Key
	std::wstring columns; // columns
	std::wstring referencedTable;
	std::wstring referencedColumns;
	std::wstring onUpdate;
	std::wstring onDelete;
	std::chrono::system_clock::time_point seq; //system assign a sequence, for alter table
} ForeignKey;
typedef std::vector<ForeignKey> ForeignKeyList;

//Export to CSV params
typedef struct {
	std::wstring csvFieldTerminatedBy;
	std::wstring csvFieldEnclosedBy;
	std::wstring csvFieldEscapedBy;
	std::wstring csvLineTerminatedBy;
	bool hasColumnOnTop = false;
} ExportCsvParams;

//Export to Excel XML params
typedef struct {
	int excelComlumnMaxSize = 0;
	int excelDecimalPlaces = 0;
} ExportExcelParams;

//Export to sql params
typedef struct {
	std::wstring sqlSetting;
} ExportSqlParams, StructAndDataParams;

typedef std::vector<std::wstring> ExportSelectedColumns;

// the data structure for show in listview or export
typedef std::vector<std::wstring> RowItem, Columns, Functions, UserTableStrings;
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

//Insert statement params for export as sql 
typedef struct {
	bool retainColumn = false;
	bool multiRows = false;
} InsertStatementParams;

//Insert statement params for export as sql 
typedef struct {
	std::wstring param;
} TblStatementParams;

// Execute sql result
typedef struct {
	int effectRows = 0;
	std::wstring execTime;
	std::wstring transferTime;
	std::wstring totalTime;
	std::wstring sql;
	int code = 0;
	std::wstring msg;
} ResultInfo;
#endif

// Support create/modify table
typedef enum {
	QUERY_DATA,
	TABLE_DATA,
	CREATE_VIEW,
	CREATE_TRIGGER,
	MODIFY_VIEW,
	MODIFY_TRIGGER,
	NEW_TABLE,
	MOD_TABLE,
} PageOperateType;

typedef enum {
	UNKOWN = 0,
	STRUCT_ONLY = 1,
	DATA_ONLY = 2,
	STRUCTURE_AND_DATA = 3
}StructAndDataSetting;

typedef enum {
	TABLE_COLUMNS_PAGE = 0,
	TABLE_INDEXS_PAGE = 1,
	TABLE_FOREIGN_KEYS_PAGE = 2
}TableStructurePageType;

