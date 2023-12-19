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

 * @file   SqlParser.h
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-12-19
 *********************************************************************/
#pragma once
#include <string>
#include <vector>
#include "SQLParserResult.h"

class SQLParser {
public:
	// Parses a given constant character SQL string into the result object.
    // This does NOT mean that the SQL string was valid SQL. To check that
    // you need to check result->isValid();
    static bool parse(const std::wstring & sql, SQLParserResult * result);

	// Run tokenization on the given string and store the tokens in the output vector.
    static bool tokenize(const std::wstring& sql, std::vector<std::wstring>* tokens);

	
private:
	static std::wstring tokenFromSql(const std::wstring &sql, size_t & offset);
};
