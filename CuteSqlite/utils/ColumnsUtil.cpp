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

 * @file   ColumnsUtil.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-12-22
 *********************************************************************/
#include "stdafx.h"
#include "ColumnsUtil.h"
#include <algorithm>


Columns ColumnsUtil::mergeColumns(const Columns & columns1, const Columns & columns2)
{
	Columns result = columns1;

	for (auto column : columns2) {
		auto iter = std::find_if(result.begin(), result.end(), [&column](auto & item) {
			return column == item;
		});

		if (iter == result.end()) {
			result.push_back(column);
		}
	}
	return result;
}
