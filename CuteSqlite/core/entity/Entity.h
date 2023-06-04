/*****************************************************************//**
 * @file   Entity.h
 * @brief  数据库使用的结构体
 * @detail    $DETAIL
 * 
 * @author 覃学涵
 * @date   2023-03-17
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
typedef struct _UserFields {
	uint32_t cid = 0;
	std::wstring name;
	std::wstring type;
	int notnull = 0;
	std::wstring dfltValue;
	int pk = 0;
} UserField;
typedef std::vector<UserField> UserFieldList;
#endif

