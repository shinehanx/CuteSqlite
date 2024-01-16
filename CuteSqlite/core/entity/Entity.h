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
	uint64_t rowId = 0;
	std::wstring type;
	std::wstring name;
	std::wstring tblName;
	uint64_t rootpage = 0;
	std::wstring sql;	
} UserTable, UserView, UserTrigger, UserIndex;

typedef std::vector<UserTable> UserTableList;
typedef std::vector<UserView> UserViewList;
typedef std::vector<UserTrigger> UserTriggerList;
typedef std::vector<UserIndex> UserIndexList;

typedef struct {
	int seqno = 0;
	int cid = 0;
	std::wstring name;
} PragmaIndexColumn;
typedef std::vector<PragmaIndexColumn> PragmaIndexColumns;

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
	std::wstring type; // Primary Key,Unique,Foreign Key,Checks,Index
	uint8_t pk = 0;  // primary key
	uint8_t ai = 0; // Auto increment
	uint8_t un = 0; // unique
	uint8_t fk = 0; // foreign key
	uint8_t ck = 0; // checks
	std::wstring columns; // columns
	std::wstring partialClause; // The partial index clause
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
	std::wstring partialClause; // The partial index clause
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

// the data structure for show in list view or export
typedef std::vector<std::wstring> RowItem, Columns, Functions, UserTableStrings, WhereExpresses;
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

	// Extend for sql log
	uint64_t id = 0;
	uint64_t userDbId = 0;
	int top = 0;
	std::wstring createdAt;
	DWORD data = 0;
} ResultInfo, SqlLog;
// Store the sql log list for execute result
typedef std::list<SqlLog> SqlLogList;

// performance analysis report
typedef struct {
	uint64_t id = 0;
	uint64_t userDbId = 0;
	uint64_t sqlLogId = 0;

	std::wstring createdAt;
	std::wstring updatedAt;
} PerfAnalysisReport;
// Store the perf analysis report list for execute result
typedef std::list<PerfAnalysisReport> PerfAnalysisReportList;

// Explain query plan
typedef struct {
	int id = 0;
	int parent = 0;
	int notused = 0;
	std::wstring detail;
} ExplainQueryPlan;
typedef std::vector<ExplainQueryPlan> ExplainQueryPlans;

// Explain sql to this struct
typedef struct {
	//BASIC INFORMATION
	int no; // OpenRead/OpenWrite p1
	uint64_t userDbId;
	std::wstring addr;
	std::wstring name;
	std::wstring type; // index, table, view, trigger
	uint64_t rootPage;

	// FOR INDEX ANALYSIS
	std::vector<std::pair<int, std::wstring>> useIndexes; // useing index in this table, pair item params: first(int) - index no, second(std::wstring) - index name
	Columns whereColumns; // where clause used columns in this table
	Columns orderColumns; // order clause used columns in this table
	std::vector<std::pair<int, std::wstring>> whereIndexColumns; // using index columns of where clause in this table, pair item params: first(int) - index no, second(std::wstring) - column name
	std::vector<std::pair<int, std::wstring>> orderIndexColumns; // using index columns of order clause in this table, pair item params: first(int) - index no, second(std::wstring) - column name
	
	Columns mergeColumns; // merge whereColumns  and orderColumns
	std::wstring coveringIndexName; // covering index name
	std::vector<std::pair<int, std::wstring>> coveringIndexColumns; // covering index columns of wherer and orde clause in this table, pair item params: first(int) - index no, second(std::wstring) - column name

	// FOR JOIN ANALYSIS
	WhereExpresses whereExpresses;
	uint64_t effectRows;

} ByteCodeResult;
typedef std::vector<ByteCodeResult> ByteCodeResults;

// Select column for select sql statement
typedef struct {
	int regNo = 0; // register no
	std::wstring fullName;
	std::wstring name;
	std::wstring alias;
	std::wstring tblName;
	std::wstring tblAlias;	
} SelectColumn;
typedef std::vector<SelectColumn> SelectColumns;

typedef struct {
	int tblNo; // OpenRead/OpenWrite p1
	std::wstring tblName;
	int idxNo;
	std::wstring idxName;
	std::wstring columnName;
} ByteCodeUseColumn;
typedef std::vector<ByteCodeUseColumn> ByteCodeUseColumns;
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
} StructAndDataSetting;

typedef enum {
	TABLE_COLUMNS_PAGE = 0,
	TABLE_INDEXS_PAGE = 1,
	TABLE_FOREIGN_KEYS_PAGE = 2
} TableStructurePageType;

typedef enum {
	SQL_LOG_PAGE,
	ANALYSIS_REPORT_PAGE,
	STORE_ANALYSIS_PAGE,
	DATABASE_PARAMS_PAGE
} AnalysisPageType;

typedef enum {
	FROM_CLAUSE,
	WHERE_CLAUSE,
	ORDER_CLAUSE,
	INSERT_COLUMNS_CLAUSE,
	INSERT_VALUES_CLAUSE,
	GROUP_CLAUSE,
	HAVING_CLAUSE,
	LIMIT_CLAUSE,

	COVERING_INDEXES
} SqlClauseType;

// Explain sql statement column idx
typedef enum  {
	EXP_ADDR = 0, // addr
	EXP_OPCODE, // opcode
	EXP_P1,	// p1
	EXP_P2,	// p2
	EXP_P3,	// p3
	EXP_P4,	// p4
	EXP_P5,	// p5
	EXP_COMMENT, //comment
} ExplainColumn;

// use for select statement such as "tbl1 as m1 left join tbl2 as m2"
typedef struct {
	std::wstring tbl;
	std::wstring alias;
} TableAlias;
typedef std::vector<TableAlias> TableAliasVector;

typedef struct {
	std::wstring name;				// Name of a table or index in the database file
	std::wstring tblName;			// Name of associated table
	uint8_t isIndex = 0;			// TRUE if it is an index, false for a table
	uint8_t isWithoutRowid = 0;		// TRUE if WITHOUT ROWID table
	uint64_t nentry = 0;			// Number of entries in the BTree
	uint64_t leafEntries = 0;		// Number of leaf entries
	uint8_t depth = 0;				// Depth of the b-tree
	uint64_t payload = 0;			// Total amount of data stored in this table or index
	uint64_t ovflPayload = 0;		// Total amount of data stored on overflow pages
	uint64_t ovflCnt = 0;			// Number of entries that use overflow
	uint64_t mxPayload = 0;			// Maximum payload size
	uint64_t intPages = 0;			// Number of interior pages used
	uint64_t leafPages = 0;			// Number of leaf pages used
	uint64_t ovflPages = 0;			// Number of overflow pages used
	uint64_t intUnused = 0;			// Number of unused bytes on interior pages
	uint64_t leafUnused = 0;		// Number of unused bytes on primary pages
	uint64_t ovflUnused = 0;		// Number of unused bytes on overflow pages
	uint64_t gapCnt = 0;			// Number of gaps in the page layout
	uint64_t compressedSize = 0;	// Total bytes stored on disk
} SpaceUsed;
typedef std::list<SpaceUsed> SpaceUsedList;

typedef struct {
	std::wstring name;			// item name
	std::wstring description;	// item description
	double val;					// item value
	double maxVal;				// max value
} StoreAnalysisItem;
typedef std::vector<StoreAnalysisItem> StoreAnalysisItems;
