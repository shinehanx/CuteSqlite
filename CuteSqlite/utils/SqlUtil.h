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

 * @file   SqlUtil.h
 * @brief  sql statement utils
 * 
 * @author Xuehan Qin
 * @date   2023-05-28
 *********************************************************************/
#pragma once
#include <regex>
#include <string>

class SqlUtil {
public:
	// select sql regex pattern
	static std::wregex selectPat;

	// field regex pattern
	static std::wregex fieldPat;

	static bool isSelectSql(std::wstring & sql);
	static std::wstring getFieldName(std::wstring & str);
};
