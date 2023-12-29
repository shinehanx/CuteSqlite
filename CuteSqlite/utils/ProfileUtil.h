/*****************************************************************//**
 * @file   ProfileUtil.h
 * @brief  ����ini�����ļ��Ĺ�����
 * @detail    $DETAIL
 * 
 * @author ��ѧ��
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
	 * ���ͳ�ini�ļ������е�section����.
	 * 
	 * @param [in]iniProfilePath  ini�ļ�·��
	 * @param [out]sections ���ص�sections����
	 */
	static void parseSections(std::wstring & iniProfilePath, std::vector<std::wstring> &sections)
	{
		// �ο��ֲ᣺https://learn.microsoft.com/zh-cn/windows/win32/api/winbase/nf-winbase-getprivateprofilesectionnames
		wchar_t buff[1024] = {0}; // ���ص������У���'\0'����һ��section; ���е�section�ַ�������������ټ�һ��\0;
		DWORD buff_len = GetPrivateProfileSectionNamesW(buff,  1024, iniProfilePath.c_str());
		if (buff_len == 0) {
			return ;
		}

		ProfileUtil::parseReturnBuffer(buff, buff_len, sections);

	}

	/**
	 * ���ͳ�ini�ļ���section�µ�������<key,val>.
	 * 
	 * @param [in]iniProfilePath - ini�ļ�
	 * @param [in]section - ��
	 * @param [out]settings - <key,val>��map
	 */
	static void parseSectionSettings(std::wstring & iniProfilePath, std::wstring &section, Setting &settings)
	{
		// �ο��ֲ᣺https://learn.microsoft.com/zh-cn/windows/win32/api/winbase/nf-winbase-getprivateprofilesectionw
		wchar_t buff[17408] = {0}; // ���ص������У���'\0'����һ��section; ���е�section�ַ�������������ټ�һ��\0;
		memset(buff, 0, sizeof(buff));
		DWORD buff_len = GetPrivateProfileSectionW(section.c_str(), buff,  17408, iniProfilePath.c_str());
		if (buff_len == 0) {
			return ;
		}

		std::vector<std::wstring> itemList ; //key=val���ַ�������
		ProfileUtil::parseReturnBuffer(buff, buff_len, itemList); //�Ƚ��ͳ�ÿһ����
		for (std::wstring item : itemList) {
			std::pair<std::wstring, std::wstring> pair;
			if (ProfileUtil::explode(item.c_str(),  L'=',  pair)) {
				settings.push_back(pair);
			}
			
		}
	}

	/**
	 * ����ָ��Ŀ¼�����е��ļ�.
	 * 
	 * @param [in]iniProfileDir  ָ��Ŀ¼�²���ini�ļ�
	 * @param [out]languages ���Ե����飬pair<std::wstring, std::wstring> ����<language, filepath>
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
	 * �����ݽ��ͳ���.
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
		wchar_t * _start = buff; //��ʼ���Ƶ��ַ���
		wchar_t * p = buff;
		for (DWORD n = 0; n < buff_len; p++, n++) {
			if (*p == L'\0') { //�����ַ�\0�ͱ�ʾ�ַ������� 
				wchar_t * _dest = new wchar_t[cp_size + 1];
				memset(_dest, 0, (cp_size + 1) * sizeof(wchar_t));
				memcpy(_dest, _start, cp_size * sizeof(wchar_t));
				vec.push_back(_dest);
				delete[] _dest;
				cp_size = 0;
				if (n < buff_len) {					
					_start = p + 1;
				}

				if (*(p + 1) == L'\0') { //pָ��֮��һ���ַ�Ϊ\0���ͱ�ʾ����section�ַ���ȫ������
					break;
				}
			}
			cp_size++;
		}
	}

	/**
	 * �����ַ�����KV�ķ�ʽ.
	 * 
	 * @param [in]buff �ַ���
	 * @param [in]ch �ָ���
	 * @param [out]pair ��ֵ�� 
	 * @return ����ɹ�/ʧ��
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
		wchar_t * _start = tmpbuf; //��ʼ���Ƶ��ַ���
		for (DWORD n = 0; n < buff_len+1; p++, n++) {
			if (*p == ch || *p == L'\0') { //�����ַ�=0�ͱ�ʾ�ַ������� 
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
