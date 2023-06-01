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

 * @file   SqlService.h
 * @brief  Execute sql service class
 * 
 * @author Xuehan Qin
 * @date   2023-05-28
 *********************************************************************/
#pragma once
#include "core/common/service/BaseService.h"
#include "core/repository/user/SqlExecutorUserRepository.h"
#include "core/common/repository/QSqlStatement.h"

class SqlService : public BaseService<SqlService, SqlExecutorUserRepository>
{
public:
	SqlService() {};
	~SqlService() {};

	QSqlStatement executeSql(uint64_t userDbId, std::wstring &sql);
};
