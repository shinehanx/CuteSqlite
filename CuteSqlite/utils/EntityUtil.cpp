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

 * @file   EntityUtil.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-10-28
 *********************************************************************/
#include "stdafx.h"
#include "EntityUtil.h"


IndexInfo EntityUtil::copy(const IndexInfo & item)
{
	IndexInfo result;
	result.name = item.name;
	result.type = item.type;
	result.pk = item.pk;
	result.ai = item.ai;
	result.un = item.un;
	result.fk = item.fk;
	result.ck = item.ck;
	result.columns = item.columns;
	result.sql = item.sql;
	result.seq = item.seq;

	return result;
}

ResultInfo EntityUtil::copy(const ResultInfo & item)
{
	ResultInfo result;	
	result.id = item.id;
	result.createdAt = item.createdAt;
	result.effectRows = item.effectRows;
	result.code = item.code;
	result.msg = item.msg;
	result.sql = item.sql;
	result.top = item.top;
	result.execTime = item.execTime;
	result.transferTime = item.transferTime;
	result.totalTime = item.totalTime;
	return result;
}

UserDb EntityUtil::copy(const UserDb & item)
{
	UserDb result;	
	result.id = item.id;
	result.name = item.name;
	result.path = item.path;
	result.isActive = item.isActive;
	result.createdAt = item.createdAt;
	result.updatedAt = item.updatedAt;
	
	return result;
}

bool EntityUtil::compare(const ColumnInfo & item1, const ColumnInfo & item2)
{
	if (item1.ai != item2.ai
		|| item1.notnull != item2.notnull
		|| item1.pk != item2.pk
		|| item1.un != item2.un
		|| item1.name != item2.name
		|| item1.type != item2.type
		|| item1.defVal != item2.defVal
		|| item1.checks != item2.checks) {
		return false;
	}
	return true;
}

bool EntityUtil::compare(const IndexInfo & item1, const IndexInfo & item2)
{
	if (item1.ai != item2.ai
		|| item1.fk != item2.fk
		|| item1.pk != item2.pk
		|| item1.un != item2.un
		|| item1.name != item2.name
		|| item1.type != item2.type
		|| item1.columns != item2.columns
		|| item1.partialClause != item2.partialClause) {
		return false;
	}
	return true;
}

bool EntityUtil::compare(const ForeignKey & item1, const ForeignKey & item2)
{
	if (item1.name != item2.name
		|| item1.type != item2.type
		|| item1.columns != item2.columns
		|| item1.referencedTable != item2.referencedTable
		|| item1.referencedColumns != item2.referencedColumns
		|| item1.onUpdate != item2.onUpdate
		|| item1.onDelete != item2.onDelete
		|| item1.partialClause != item2.partialClause) {
		return false;
	}
	return true;
}

