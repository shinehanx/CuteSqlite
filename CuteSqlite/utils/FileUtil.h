#pragma once

#include <io.h>
#include <direct.h>
#include <string>
#include <fstream>
#include "StringUtil.h"
#include "DateUtil.h"
#include "ResourceUtil.h"
#include "Log.h"
#include "core/common/exception/QRuntimeException.h"

#define MAX_PATH_LEN 256
#define ACCESS(fileName, accessMode) _access(fileName, accessMode)
#define MKDIR(path) _mkdir(path)

class FileUtil
{
public:
	static unsigned int createDirectory(const std::string &directoryPath);
	static unsigned int createDirectory(const std::wstring &directoryPath);

	/**
	 * Get the file extension.
	 * 
	 * @param filePath
	 * @return 
	 */
	static std::wstring getFileExt(const std::wstring &filePath);

	/**
	 * ����ļ���.
	 * 
	 * @param filePath �ļ�·��
	 * @param includeExt �Ƿ������׺��
	 * @return 
	 */
	static std::wstring getFileName(const std::wstring &filePath, bool includeExt = true);

	// ����ļ�·��,·��������Ŀ¼�ָ��'\\' '/'��֧�������ļ�Ŀ¼�ָ����� '\\' '/'
	static std::wstring getFileDir(const std::wstring &filePath);
	

	static bool copy(const std::wstring & fromPath, const std::wstring & toPath);

	static void saveFile(std::wstring path, size_t size, void * buffer);

	/**
	 * ������ʱͼƬ�ļ�.
	 * 
	 * @param ext ͼƬ��׺��
	 */
	static std::wstring makeTempImagePath(std::wstring &ext);

	static std::wstring readFile(const std::wstring & path);

	static std::wstring readFirstLine(const std::wstring & path, const std::wstring & encoding = L"UTF-8");
};





