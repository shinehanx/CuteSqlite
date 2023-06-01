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

 * @file   SqlExecutorUserRepository.h
 * @brief  Exeute sql from user db
 * 
 * @author Xuehan Qin
 * @date   2023-05-28
 *********************************************************************/
#pragma once
#include <string>
#include "core/common/repository/BaseUserRepository.h"
#include "core/common/repository/QSqlDatabase.h"
#include "core/common/repository/QSqlStatement.h"

class SqlExecutorUserRepository : public BaseUserRepository<SqlExecutorUserRepository>
{
public:
	SqlExecutorUserRepository() {};
	~SqlExecutorUserRepository() {};

	QSqlStatement execSql(uint64_t userDbId, std::wstring &sql);
};