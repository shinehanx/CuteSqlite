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

 * @file   ExportDatabaseObjectsService.cpp
 * @brief  Export the database objects to file
 * 
 * @author Xuehan Qin
 * @date   2023-10-06
 *********************************************************************/
#include "stdafx.h"
#include "ExportDatabaseObjectsService.h"
#include <fstream>



std::wstring ExportDatabaseObjectsService::readExportDatabaseTemplate()
{
	std::wstring binDir = ResourceUtil::getProductBinDir();
	std::wstring tplPath = binDir + L"res\\tpl\\export-database-as-sql.tpl";

	std::wifstream ifs;
	auto codeccvt = new std::codecvt_utf8<wchar_t, 0x10ffff, std::codecvt_mode(std::generate_header | std::little_endian)>();
	std::locale utf8(std::locale("C"), codeccvt);
	ifs.imbue(utf8);
	ifs.open(tplPath, std::ios::in);
	if (ifs.bad()) {
		return L"";
	}

	wchar_t content[1024];
	memset(content, 0, sizeof(wchar_t) * 1024);
	ifs.read(content, 1024);
	std::wstring result(content);

	ifs.close();
	return result;
}
