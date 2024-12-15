/*****************************************************************//**
 * Copyright 2023 Xuehan Qin (qinxuehan2018@gmail.com) 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *   http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 * @file   ResourceUtil.cpp
 * @brief  
 * 
 * @author Xuehan Qin (qinxuehan2018@gmail.com) 
 * @date   2024-10-30
 *********************************************************************/
#include "stdafx.h"
#include "ResourceUtil.h"
#include <Windows.h>

ResourceUtil::ResourceUtil()
{
}

ResourceUtil::~ResourceUtil()
{
}


wstring ResourceUtil::getResourceDir()
{
	wstring result;
	TCHAR modulePathName[_MAX_PATH] = { 0 };
	_wgetcwd(modulePathName, _MAX_PATH);
	result.append(modulePathName);
	result.append(L"\\res");
	return result;
}

wstring ResourceUtil::getResourcePath(wchar_t * fileName)
{
	wstring result;
	TCHAR modulePathName[_MAX_PATH] = { 0 };
	_wgetcwd(modulePathName, _MAX_PATH);
	result.append(modulePathName);
	result.append(L"\\res\\");
	result.append(fileName);
	return result;
}

wstring ResourceUtil::getProductBinDir()
{
	static wchar_t szbuf[MAX_PATH];  
	GetModuleFileName(NULL,szbuf,MAX_PATH);  
    PathRemoveFileSpec(szbuf);
	int length = lstrlen(szbuf);
	szbuf[length] = L'\\';
	szbuf[length+1] = 0;
	return std::wstring(szbuf);
}

std::wstring ResourceUtil::getProductResDir()
{
	std::wstring dir = ResourceUtil::getProductBinDir();
	return dir + L"res\\";
}

std::wstring ResourceUtil::getProductImagesDir()
{
	std::wstring dir = ResourceUtil::getProductBinDir();
	return dir + L"res\\images\\";
}

std::wstring ResourceUtil::getProductUploadDir()
{
	std::wstring dir = ResourceUtil::getProductBinDir();
	return dir + L"upload\\";
}

/**
 * 获得产品目录下的所有文件.
 * 
 * @return 
 */
std::wstring ResourceUtil::getProductTmpDir()
{
	std::wstring dir = ResourceUtil::getProductBinDir();
	return dir + L"tmp\\";
}

/**
 * 获得桌面的目录路径.
 * 
 * @return 
 */
std::wstring ResourceUtil::getDesktopDir()
{
	std::wstring ret;
	WCHAR lpszDesktopPath[MAX_PATH];
	HRESULT result = ::SHGetFolderPathW(NULL, CSIDL_DESKTOPDIRECTORY, NULL, 0, lpszDesktopPath);
	if (SUCCEEDED(result)) {
		ret.append(lpszDesktopPath);
	}

	return ret;
}

/**
 * UNICODE转成ANSI，GLOG不支持UNICODE目录
 */
char* ResourceUtil::convertUnicodeToANSI(const wchar_t* unicode)  
{  
    if(unicode == NULL)
    {
        return _strdup("\0");
    }

    int len;  
    len = WideCharToMultiByte(CP_ACP, 0,unicode, -1, NULL, 0, NULL, NULL);  
    char *szANSI = (char*)malloc(len + 1);  
    memset(szANSI, 0, len + 1);  
    WideCharToMultiByte(CP_ACP, 0,unicode, -1, szANSI, len, NULL,NULL);  
    return szANSI;  
}