/*****************************************************************//**
 * @file   ProfileUtil.h
 * @brief  解释ini配置文件的工具类
 * @detail    $DETAIL
 * 
 * @author 覃学涵
 * @date   2023-03-03
 *********************************************************************/
#pragma once
#include "stdafx.h"
#include <stdio.h> 
#include <vector>
#include <string>
#include <utility>
#include "utils/Log.h"
class ProfileUtil {
public:
	/**
	 * 解释出ini文件中所有的section数组.
	 * 
	 * @param [in]iniProfilePath  ini文件路径
	 * @param [out]sections 返回的sections数组
	 */
	static void parseSections(std::wstring & iniProfilePath, std::vector<std::wstring> &sections)
	{
		// 参考手册：https://learn.microsoft.com/zh-cn/windows/win32/api/winbase/nf-winbase-getprivateprofilesectionnames
		wchar_t buff[1024] = {0}; // 返回的数据中，以'\0'结束一个section; 所有的section字符串结束后，最后再加一个\0;
		DWORD buff_len = GetPrivateProfileSectionNamesW(buff,  1024, iniProfilePath.c_str());
		if (buff_len == 0) {
			return ;
		}

		ProfileUtil::parseReturnBuffer(buff, buff_len, sections);

	}

	/**
	 * 解释出ini文件中section下的配置项<key,val>.
	 * 
	 * @param [in]iniProfilePath - ini文件
	 * @param [in]section - 节
	 * @param [out]settings - <key,val>的map
	 */
	static void parseSectionSettings(std::wstring & iniProfilePath, std::wstring &section, Setting &settings)
	{
		// 参考手册：https://learn.microsoft.com/zh-cn/windows/win32/api/winbase/nf-winbase-getprivateprofilesectionw
		wchar_t buff[17408] = {0}; // 返回的数据中，以'\0'结束一个section; 所有的section字符串结束后，最后再加一个\0;
		memset(buff, 0, sizeof(buff));
		DWORD buff_len = GetPrivateProfileSectionW(section.c_str(), buff,  17408, iniProfilePath.c_str());
		if (buff_len == 0) {
			return ;
		}

		std::vector<std::wstring> itemList ; //key=val的字符串数组
		ProfileUtil::parseReturnBuffer(buff, buff_len, itemList); //先解释出每一行来
		for (std::wstring item : itemList) {
			std::pair<std::wstring, std::wstring> pair;
			if (ProfileUtil::explode(item.c_str(),  L'=',  pair)) {
				settings.push_back(pair);
			}
			
		}
	}

	/**
	 * 查找指定目录下所有的文件.
	 * 
	 * @param [in]iniProfileDir  指定目录下查找ini文件
	 * @param [out]languages 语言的数组，pair<std::wstring, std::wstring> 参数<language, filepath>
	 */
	static void parseLanguagesFromDir(std::wstring & iniProfileDir, std::vector<std::pair<std::wstring,std::wstring>> &languages)
	{
		WIN32_FIND_DATA wfd;
		HANDLE hFind = INVALID_HANDLE_VALUE;

		std::wstring findPath = iniProfileDir;
		findPath.append(L"*.ini");
		hFind = ::FindFirstFile(findPath.c_str(), &wfd);

		languages.clear();
		if (hFind == INVALID_HANDLE_VALUE) {
			Q_ERROR(L"Has error :INVALID_HANDLE_VALUE, path:{}", iniProfileDir.c_str());
			return ;
		}

		do {
			std::wstring iniFindPath = iniProfileDir;
			iniFindPath.append(wfd.cFileName);
			if (_waccess(iniFindPath.c_str(), 0) != 0) {
				Q_ERROR(L"file not exisits, path:{}", iniFindPath.c_str());
				continue;
			}
			Setting langSetting;
			parseSectionSettings(iniFindPath, std::wstring(L"LANG"), langSetting);
			if (langSetting.empty())  continue;
			
			std::pair<std::wstring, std::wstring> pair;
			std::wstring language = langSetting.at(0).second;
			if (language.empty()) continue;
			pair.first = language;
			pair.second = iniFindPath;
			languages.push_back(pair);
		} while (FindNextFile(hFind, &wfd) != 0);

		::FindClose(hFind);

	}
private:
	/**
	 * 把数据解释出来.
	 * 
	 * @param [in]buff
	 * @param [in]buff_len
	 * @param [out]vec
	 */
	static void parseReturnBuffer(wchar_t * buff, DWORD buff_len, std::vector<std::wstring> &vec)
	{
		if (buff == nullptr || buff_len <=0 ) {
			return ;
		}

		size_t cp_size = 0;
		wchar_t * _start = buff; //开始复制的字符串
		wchar_t * p = buff;
		for (DWORD n = 0; n < buff_len; p++, n++) {
			if (*p == L'\0') { //遇到字符\0就表示字符串结束 
				wchar_t * _dest = new wchar_t[cp_size + 1];
				memset(_dest, 0, (cp_size + 1) * sizeof(wchar_t));
				memcpy(_dest, _start, cp_size * sizeof(wchar_t));
				vec.push_back(_dest);
				delete[] _dest;
				cp_size = 0;
				if (n < buff_len) {					
					_start = p + 1;
				}

				if (*(p + 1) == L'\0') { //p指针之后一个字符为\0，就表示所有section字符串全部结束
					break;
				}
			}
			cp_size++;
		}
	}

	/**
	 * 分离字符串成KV的方式.
	 * 
	 * @param [in]buff 字符串
	 * @param [in]ch 分隔符
	 * @param [out]pair 键值对 
	 * @return 分离成功/失败
	 */
	static bool explode(const wchar_t * buff, wchar_t ch, std::pair<std::wstring, std::wstring> & pair)
	{
		if (buff == nullptr || sizeof(buff) == 0) {
			return false;
		}
		std::vector<std::wstring> vec;
		size_t cp_size = 0;
		size_t buff_len = wcslen(buff) ;

		wchar_t * tmpbuf = new wchar_t[buff_len+1];
		memset(tmpbuf, 0, (buff_len +1) * sizeof(wchar_t));
		memcpy(tmpbuf, buff, buff_len * sizeof(wchar_t));

		wchar_t * p = tmpbuf;
		wchar_t * _start = tmpbuf; //开始复制的字符串
		for (DWORD n = 0; n < buff_len+1; p++, n++) {
			if (*p == ch || *p == L'\0') { //遇到字符=0就表示字符串结束 
				wchar_t * _dest = new wchar_t[cp_size + 1];
				memset(_dest, 0, (cp_size + 1) * sizeof(wchar_t));
				memcpy(_dest, _start, cp_size * sizeof(wchar_t));
				vec.push_back(_dest);
				delete[] _dest;
				cp_size = 0;
				if (n < buff_len+1) {					
					_start = p + 1;
				}				
			}
			cp_size++;
		}
		delete[] tmpbuf;
		if (vec.empty() || vec.size() < 1) {
			return false;
		}
		pair.first = vec[0];
		pair.second = vec[1];
		return true;
	}

};
