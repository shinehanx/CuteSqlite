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

 * @file   ExportDatabaseObjectsService.h
 * @brief  Export the database objects to file
 * 
 * @author Xuehan Qin
 * @date   2023-10-06
 *********************************************************************/
#pragma once
#include "core/common/service/BaseService.h"
#include "core/repository/db/UserDbRepository.h"
#include "core/entity/Entity.h"

#define EXPERT_PER_PAGE 100

class ExportDatabaseObjectsService : public BaseService<ExportDatabaseObjectsService, UserDbRepository>
{
public:
	ExportDatabaseObjectsService() {};
	~ExportDatabaseObjectsService() {};

	std::wstring readExportDatabaseTemplate();

	
};
