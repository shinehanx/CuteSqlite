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

 * @file   FileUtil.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-11-04
 *********************************************************************/
#include "stdafx.h"
#include "FileUtil.h"


unsigned int FileUtil::createDirectory(const std::string &directoryPath)
{
	size_t dirPathLen = directoryPath.length();
	if (dirPathLen > MAX_PATH_LEN) {
		return 1;
	}
	char tmpDirPath[MAX_PATH_LEN] = {0};
	for (uint32_t i=0; i< dirPathLen; ++i) {
		tmpDirPath[i] = directoryPath[i];
		if (tmpDirPath[i] == '\\' || tmpDirPath[i] == '/') {
			if (ACCESS(tmpDirPath, 0) != 0) {
				int32_t ret = MKDIR(tmpDirPath);
				if (ret != 0) {
					return ret;
				}
			}
		}
	}
	
	return 0;
}

unsigned int FileUtil::createDirectory(const std::wstring &directoryPath)
{
	size_t dirPathLen = directoryPath.length();
	if (dirPathLen > MAX_PATH_LEN) {
		return 1;
	}
	wchar_t tmpDirPath[MAX_PATH_LEN] = {0};
	for (uint32_t i=0; i< dirPathLen; ++i) {
		tmpDirPath[i] = directoryPath[i];
		if (tmpDirPath[i] == L'\\' || tmpDirPath[i] == L'/') {
			if (_waccess(tmpDirPath, 0) != 0) {
				int32_t ret = _wmkdir(tmpDirPath);
				if (ret != 0) {
					return ret;
				}
			}
		}
	}
	
	return 0;
}

/**
* Get the file extension.
* 
* @param filePath
* @return 
*/
std::wstring FileUtil::getFileExt(const std::wstring &filePath)
{
	size_t p = filePath.find_last_of(L'.');
	std::wstring ext = p != std::wstring::npos ? filePath.substr(p+1, -1) : L"";
	return ext;
}

/**
* 获得文件名.
* 
* @param filePath 文件路径
* @param includeExt 是否包含后缀名
* @return 
*/
std::wstring FileUtil::getFileName(const std::wstring &filePath, bool includeExt /*= true*/)
{
	size_t p = filePath.find_last_of(L'\\');
	if (p == std::wstring::npos) {
		p = filePath.find_last_of(L'/');
	}
	if (p != std::wstring::npos && includeExt) {
		std::wstring fileName = filePath.substr(p+1, -1);
		return fileName;
	}else if (p != std::wstring::npos && !includeExt) {
		size_t p2 = filePath.find_last_of(L'.');
		if (p2 > p) {
			return filePath.substr(p+1, p2 - p - 1);
		}
	}
		
	return L"";
}

/**
 * 获得文件路径,路径最后包含目录分割符'\\' '/'，支持两种文件目录分隔符符 '\\' '/'.
 * 
 * @param filePath
 * @return 
 */
std::wstring FileUtil::getFileDir(const std::wstring &filePath)
{
	size_t p1 = filePath.find_last_of(L'\\');
	size_t p2 = filePath.find_last_of(L'/');
	size_t p = 0;
	if (p1 == std::wstring::npos && p2 == std::wstring::npos) {
		return L"";
	}else if (p1 != std::wstring::npos && p2 != std::wstring::npos) {
		p = max(p1, p2);
	}else if (p1 == std::wstring::npos) {
		p = p2;
	}else if (p2 == std::wstring::npos) {
		p = p1;
	}
		
	std::wstring dir = p != std::wstring::npos ? filePath.substr(0, p+1) : L"";
	return dir;
}

bool FileUtil::copy(const std::wstring & fromPath, const std::wstring & toPath)
{
	std::wstring toUserDbDir = FileUtil::getFileDir(toPath);
	ATLASSERT(!toUserDbDir.empty());
	
	if (_waccess(toUserDbDir.c_str(), 0) != 0) { //文件目录不存在
		Q_INFO(L"mkpath:{}", toUserDbDir);
		//创建DB目录，子目录不存在，则创建
		FileUtil::createDirectory(toUserDbDir);
	}

	if (_waccess(toPath.c_str(), 0) == 0) { // 文件存在,删除后覆盖
		::DeleteFileW(toPath.c_str());
	}
	
	ATLASSERT(_waccess(fromPath.c_str(), 0) == 0);

	errno_t _err;
	wchar_t _err_buf[256] = { 0 };
	FILE * origFile, *destFile;
	_err = _wfopen_s(&origFile, fromPath.c_str(), L"rb"); //原文件
	if (_err != 0 || origFile == NULL) {
		_wcserror_s(_err_buf, 256, _err);
		Q_ERROR(L"orig db file open error,error:{},path:{}", _err_buf, fromPath);
		QRuntimeException ex(std::to_wstring(_err), _err_buf);
		throw ex;
	}
	_err = _wfopen_s(&destFile, toPath.c_str(), L"wb"); // 目标文件
	if (_err != 0 || destFile == NULL) {
		_wcserror_s(_err_buf, 256, _err);
		Q_ERROR(L"dest db file open error,error:{},path:{}", _err_buf, toPath);
		QRuntimeException ex(std::to_wstring(_err), _err_buf);
		throw ex;
	}
	wchar_t ch = fgetwc(origFile);
	while (!feof(origFile)) {
		_err = fputwc(ch, destFile);
		ch = fgetwc(origFile);
	}

	fclose(destFile);
	fclose(origFile);
	return true;
}

void FileUtil::saveFile(std::wstring path, size_t size, void * buffer)
{
	errno_t _err;
	wchar_t _err_buf[80] = { 0 };
	FILE * file;
	
    _err = _wfopen_s(&file, path.c_str(),  L"wb"); //目标文件
	if (_err != 0 || file == NULL) {
		_wcserror_s(_err_buf, 80, _err);
		ATLASSERT(_err_buf);
	}
		
	fwrite(buffer, size, 1, file);
	fclose(file);
}

/**
* 生成临时图片文件.
* 
* @param ext 图片后缀名
*/
std::wstring FileUtil::makeTempImagePath(std::wstring &ext)
{
	ATLASSERT(ext.empty() == false);
	std::wstring binDir = ResourceUtil::getProductBinDir(); 
	std::wstring tmpPath(binDir);
	tmpPath.append(L"tmp\\images\\").append(DateUtil::getCurrentDate()).append(L"\\");
	FileUtil::createDirectory(tmpPath);
	tmpPath.append(L"_tmp_").append(DateUtil::getCurrentDateTime(L"%Y%m%d_%H%M%S.")).append(ext);
	return tmpPath;
}

std::wstring FileUtil::readFile(const std::wstring & path)
{
	if (_waccess(path.c_str(), 0) != 0) {
		ATLASSERT(false);
		return std::wstring();
	}
	std::wifstream ifs;
	auto codeccvt = new std::codecvt_utf8<wchar_t, 0x10ffff, std::codecvt_mode(std::generate_header | std::little_endian)>();
	std::locale utf8(std::locale("C"), codeccvt);
	ifs.imbue(utf8);
	ifs.open(path, std::ios::in);
	if (ifs.bad()) {
		return L"";
	}
	std::wstring result;
	wchar_t buff[257];
	while (!ifs.eof()) {
		wmemset(buff, 0, 257);
		ifs.read(buff, 256);
		result.append(buff); 
	}

	ifs.close();
	return result;
}


std::wstring FileUtil::readFirstLine(const std::wstring & path, const std::wstring & encoding)
{
	std::wifstream ifs;
	if (encoding == L"UTF-16") {
		auto codeccvt = new std::codecvt_utf16<wchar_t, 0x10ffff, std::codecvt_mode(std::generate_header | std::little_endian)>();
		std::locale utf16(std::locale("C"), codeccvt);
		ifs.imbue(utf16);
	} else {
		auto codeccvt = new std::codecvt_utf8<wchar_t, 0x10ffff, std::codecvt_mode(std::generate_header | std::little_endian)>();
		std::locale utf8(std::locale("C"), codeccvt);
		ifs.imbue(utf8);
	}
	
	ifs.open(path, std::ios::in);
	if (ifs.bad()) {
		return L"";
	}
	std::wstring line;
	if (!ifs.eof()) {
		wchar_t buff[4096] = {0};
		ifs.getline(buff, 4096);
		line.append(buff);
	}
	ifs.close();

	return line;
}