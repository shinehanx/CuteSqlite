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

void DatabaseUserRepository::create(uint64_t userDbId, std::wstring & userDbPath)
{
	std::wstring userDbDir = FileUtil::getFileDir(userDbPath);
	ATLASSERT(!userDbDir.empty());

	char * ansiDbDir = StringUtil::unicodeToUtf8(userDbDir);
	if (_waccess(userDbDir.c_str(), 0) != 0) { //文件目录不存在
		Q_INFO(L"mkpath:{}", userDbDir);
		//创建DB目录，子目录不存在，则创建
		FileUtil::createDirectory(ansiDbDir);
	}
	free(ansiDbDir);


	std::wstring path = userDbPath;
	char * ansiPath = StringUtil::unicodeToUtf8(path);
	if (_access(ansiPath, 0) == 0) { // 文件存在,删除后覆盖
		::DeleteFile(path.c_str());
	}
	if (localDir.empty()) {
		localDir = ResourceUtil::getProductBinDir();
	}
	std::wstring origPath = localDir + L"res\\db\\UserDb.s3db";
	char * ansiOrigPath = StringUtil::unicodeToUtf8(origPath.c_str());
	ATLASSERT(_access(ansiOrigPath, 0) == 0);

	errno_t _err;
	char _err_buf[80] = { 0 };
	FILE * origFile, *destFile;
	_err = fopen_s(&origFile, ansiOrigPath, "rb"); //原文件
	if (_err != 0 || origFile == NULL) {
		_strerror_s(_err_buf, 80, NULL);
		std::wstring _err_msg = StringUtil::utf82Unicode(_err_buf);
		Q_ERROR(L"orgin db file open error,error:{},path:{}", _err_msg, origPath);
		ATLASSERT(_err == 0);
	}
	_err = fopen_s(&destFile, ansiPath, "wb"); // 目标文件
	if (_err != 0 || destFile == NULL) {
		_strerror_s(_err_buf, 80, NULL);
		std::wstring _err_msg = StringUtil::utf82Unicode(_err_buf);
		Q_ERROR(L"dest db file open error,error:{},path:{}", _err_msg, path);
		ATLASSERT(_err == 0);
	}
	char ch = fgetc(origFile);
	while (!feof(origFile)) {
		_err = fputc(ch, destFile);
		ch = fgetc(origFile);
	}

	fclose(destFile);
	fclose(origFile);
	free(ansiOrigPath);	

	free(ansiPath);
}
