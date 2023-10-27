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
