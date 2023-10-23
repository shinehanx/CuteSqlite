#pragma once

#include <io.h>
#include <direct.h>
#include <string>
#include <fstream>
#include "StringUtil.h"
#include "DateUtil.h"
#include "ResourceUtil.h"
#include "Log.h"

#define MAX_PATH_LEN 256
#define ACCESS(fileName, accessMode) _access(fileName, accessMode)
#define MKDIR(path) _mkdir(path)

class FileUtil
{
public:
	static unsigned int createDirectory(const std::string &directoryPath) 
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

	static unsigned int createDirectory(const std::wstring &directoryPath) 
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
	static std::wstring getFileExt(const std::wstring &filePath) 
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
	static std::wstring getFileName(const std::wstring &filePath, bool includeExt=true) 
	{
		size_t p = filePath.find_last_of(L'\\');
		if (p == std::wstring::npos) {
			p = filePath.find_last_of(L'/');
		}
		if (p != std::wstring::npos && includeExt) {
			std::wstring fileName = filePath.substr(p+1, -1);
			return fileName;
		}else if (p != std::wstring::npos && !includeExt) {
			size_t p2 = filePath.find_first_of(L'.');
			if (p2 > p) {
				return filePath.substr(p+1, p2 - p - 1);
			}
		}
		
		return L"";
	}

	// 获得文件路径,路径最后包含目录分割符'\\' '/'，支持两种文件目录分隔符符 '\\' '/'
	static std::wstring getFileDir(const std::wstring &filePath) 
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

	static bool copy(const std::wstring &src, const std::wstring &dst)
	{
		if (_waccess(src.c_str(), 0) != 0) {
			return false;
		}
		char * ansiSrcPath = StringUtil::unicodeToUtf8(src);
		char * ansiDstPath = StringUtil::unicodeToUtf8(dst);
		errno_t _err;
		char _err_buf[80] = { 0 };
		FILE * origFile, *destFile;
		_err = fopen_s(&origFile, ansiSrcPath, "rb"); //原文件
		if (_err != 0 || origFile == NULL) {
			_strerror_s(_err_buf, 80, NULL);
			std::wstring _err_msg = StringUtil::utf82Unicode(_err_buf);
			Q_ERROR(L"src file open error,error:{},path:{}", _err_msg, src);
			ATLASSERT(_err == 0);
		}
		_err = fopen_s(&destFile, ansiDstPath, "wb"); //目标文件
		if (_err != 0 || destFile == NULL) {
			_strerror_s(_err_buf, 80, NULL);
			std::wstring _err_msg = StringUtil::utf82Unicode(_err_buf);
			Q_ERROR(L"dst file open error,error:{},path:{}", _err_msg , dst);
			ATLASSERT(_err == 0);
		}
		char ch = fgetc(origFile);
		while (!feof(origFile)) {
			_err = fputc(ch, destFile);
			ch = fgetc(origFile);
		}

		fclose(destFile);
		fclose(origFile);
		free(ansiSrcPath);
		free(ansiDstPath);

		return true;
	}

	static void saveFile(std::wstring path, size_t size, void * buffer)
	{
		errno_t _err;
		char _err_buf[80] = { 0 };
		FILE * file;
		char * ansiPath = StringUtil::unicodeToUtf8(path);		
        _err = fopen_s(&file, ansiPath,  "wb"); //目标文件
		if (_err != 0 || file == NULL) {
			_strerror_s(_err_buf, 80, NULL);
			ATLASSERT(_err_buf);
		}
		
		fwrite(buffer, size, 1, file);

		fclose(file);
		free(ansiPath);
	}

	/**
	 * 生成临时图片文件.
	 * 
	 * @param ext 图片后缀名
	 */
	static std::wstring makeTempImagePath(std::wstring &ext)
	{
		ATLASSERT(ext.empty() == false);
		std::wstring binDir = ResourceUtil::getProductBinDir(); 
		std::wstring tmpPath(binDir);
		tmpPath.append(L"tmp\\images\\").append(DateUtil::getCurrentDate()).append(L"\\");
		FileUtil::createDirectory(tmpPath);
		tmpPath.append(L"_tmp_").append(DateUtil::getCurrentDateTime(L"%Y%m%d_%H%M%S.")).append(ext);
		return tmpPath;
	}

	static std::wstring readFile(const std::wstring & path)
	{
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
			wmemset(buff, 0, 256);
			ifs.read(buff, 256);
			result.append(buff);
		}

		ifs.close();
		return result;
	}
};





