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

 * @file   DatabaseUserRepository.cpp
 * @brief  DatabaseUserRepository is a user repository class to handle user database
 * 
 * @author Xuehan Qin
 * @date   2023-05-20
 *********************************************************************/
#include "stdafx.h"
#include "DatabaseUserRepository.h"
#include <utility>
#include "utils/ResourceUtil.h"
#include "core/common/exception/QSqlExecuteException.h"

void DatabaseUserRepository::create(uint64_t userDbId, std::wstring & userDbPath)
{
	std::wstring userDbDir = FileUtil::getFileDir(userDbPath);
	ATLASSERT(!userDbDir.empty());
	
	if (_waccess(userDbPath.c_str(), 0) == 0) { // 文件存在,删除后覆盖
		::_wunlink(userDbPath.c_str());
	}
	if (localDir.empty()) {
		localDir = ResourceUtil::getProductBinDir();
	}
	std::wstring origPath = localDir + L"res\\db\\UserDb.s3db";
	ATLASSERT(_waccess(origPath.c_str(), 0) == 0);

	FileUtil::copy(origPath, userDbPath);
}

void DatabaseUserRepository::copy(const std::wstring & fromDbPath, const std::wstring & toDbPath)
{
	FileUtil::copy(fromDbPath, toDbPath);
}

Functions DatabaseUserRepository::getFunctions(int64_t userDbId)
{
	std::wstring sql = L"PRAGMA function_list;";
	try {
		QSqlStatement query(getUserConnect(userDbId), sql.c_str());
		
		Functions result;
		while (query.executeStep()) {
			std::wstring item = query.getColumn(L"name").getText();
			result.push_back(item);
		}
		return result;
	}
	catch (SQLite::QSqlException &ex) {
		std::wstring _err = ex.getErrorStr();
		Q_ERROR(L"query db has error:{}, msg:{}", ex.getErrorCode(), _err);
		throw QSqlExecuteException(std::to_wstring(ex.getErrorCode()), ex.getErrorStr(), sql);
	}
}
