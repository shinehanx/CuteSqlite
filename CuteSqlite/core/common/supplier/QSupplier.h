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

 * @file   QSupplier.h
 * @brief  The base class of all XXSupplier child class 
 * 
 * @author Xuehan Qin
 * @date   2023-11-01
 *********************************************************************/
#pragma once
#include <stdint.h>
#include <string>

class QSupplier {
public:
	// CSV Options
	static std::wstring csvFieldSeperators[5];
	static std::wstring csvLineSeperators[3];
	static std::wstring csvNullAsKeywords[2];
	static std::wstring csvEncodings[2];

	uint64_t getRuntimeUserDbId() const { return runtimeUserDbId; }
	void setRuntimeUserDbId(uint64_t val) { runtimeUserDbId = val; }

	std::wstring & getRuntimeTblName() { return runtimeTblName; }
	void setRuntimeTblName(const std::wstring & val) { runtimeTblName = val; }

	std::wstring & getRuntimeSchema() { return runtimeSchema; }
	void setRuntimeSchema(const std::wstring & val) { runtimeSchema = val; }

protected:
	// Runtime variables
	uint64_t runtimeUserDbId = 0;
	std::wstring runtimeSchema;
	std::wstring runtimeTblName;

	
};
