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

 * @file   QueryPageSupplier.h
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-10-30
 *********************************************************************/
#pragma once
#include "core/entity/Entity.h"
#include "ui/database/rightview/common/QPageSupplier.h"

class QueryPageSupplier : public QPageSupplier {
public:
	// sql statements
	std::vector<std::wstring> sqlVector;

	// Using semicolons to separate a SQL statement becomes a member variable sqlVector
	void splitToSqlVector(std::wstring sql);
};
