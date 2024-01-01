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

 * @file   ImportDatabaseAdapter.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-10-08
 *********************************************************************/
#include "stdafx.h"
#include "ImportDatabaseAdapter.h"
#include "common/AppContext.h"
#include "core/common/Lang.h"
#include "core/common/exception/QRuntimeException.h"
#include "ui/common/message/QPopAnimate.h"
#include "ui/common/message/QMessageBox.h"

#define CMD_BUFF_LEN 1024


ImportDatabaseAdapter::ImportDatabaseAdapter(HWND parentHwnd, ATL::CWindow * view)
{
	this->parentHwnd = parentHwnd;
	this->dataView = view;
}

ImportDatabaseAdapter::~ImportDatabaseAdapter()
{

}

UserDbList ImportDatabaseAdapter::getDbs()
{
	loadDbs();
	return dbs;
}

void ImportDatabaseAdapter::loadDbs()
{
	try {
		dbs.clear();
		dbs = databaseService->getAllUserDbs();
	}
	catch (QRuntimeException &ex) {
		Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::error(parentHwnd, S(L"error-text").append(ex.getMsg()).append(L",[code:").append(ex.getCode()).append(L"]"));
	}
}


bool ImportDatabaseAdapter::importFromSql(uint64_t userDbId, const std::wstring & importPath)
{
	ATLASSERT(userDbId > 0 && !importPath.empty());
	if (_waccess(importPath.c_str(), 0) != 0) {
		QPopAnimate::error(parentHwnd, S(L"error-text").append(S(L"import-file-not-exists")));
		return false;
	}

	try {
		UserDb userDb = databaseService->getUserDb(userDbId);
		std::wstring dbPath = userDb.path;
		std::wstring cmdline;
		std::wstring sqlite3exePath = L"sqlite3.exe"; 
		
		std::wstring importRepPath = importPath;
		// If the file path contains spaces, assign a copy to the local tmp directory
		// Sample command line��c:\system32\cmd.exe /c sqlite3.exe "HairAnalyzer.xlsx" ".read I:/hairanalysis-20230406-211747.sql"
		if (importPath.find_first_of(L' ') != -1) { 
			std::wstring tmpfn = FileUtil::getFileName(importPath,false);
			std::wstring tmpext = FileUtil::getFileExt(importPath);			
			std::wstring tmptime = DateUtil::getCurrentDateTime(L"%Y%m%d_%H%M%S");

			// Tmp file name ,such as "tmp_hairanalysis_20230406_211747.sql"
			std::wstring tmppath = ResourceUtil::getProductBinDir();
			tmppath.append(L"tmp\tmp_").append(tmpfn).append(tmptime).append(L".").append(tmpext);
			std::wstring tmpdir = FileUtil::getFileDir(tmppath);
			if (_waccess(tmpdir.c_str(), 0) != 0) {
				FileUtil::createDirectory(tmpdir);
			}
			// copy the import file to tmp file 
			FileUtil::copy(importPath, tmppath);
			importRepPath = tmppath;

			// delete tmp file
			//_wunlink(tmppath);
		}
		AppContext::getInstance()->dispatch(Config::MSG_IMPORT_PROCESS_ID, 0, 10);

		// ������Ҫ�滻Ŀ¼�ָ��,��Ȼ�����sqlite3 .read�����windows��Ŀ¼
		importRepPath = StringUtil::replace(importRepPath, L"\\", L"/");
		cmdline.append(L"/c ").append(sqlite3exePath).append(L" \"").append(dbPath).append(L"\" \".read ").append(importRepPath).append(L"\"");
		Q_DEBUG(L"Import file, cmd: [{}]", cmdline);
		execCommandLine(cmdline.c_str());
		AppContext::getInstance()->dispatch(Config::MSG_IMPORT_PROCESS_ID, 1, 100);
		return true;
	} catch (QRuntimeException &ex) {
		Q_ERROR(L"error{}, msg:{}", ex.getCode(), ex.getMsg());
		QPopAnimate::error(parentHwnd, S(L"error-text").append(ex.getMsg()).append(L",[code:").append(ex.getCode()).append(L"]"));
		return false;
	} catch (SQLite::QSqlException &ex) {
		std::wstring errCode = std::to_wstring(ex.getErrorCode());
		Q_ERROR(L"error{}, msg:{}", errCode, ex.getErrorStr());		
		QPopAnimate::error(parentHwnd, S(L"error-text").append(ex.getErrorStr()).append(L",[code:").append(errCode).append(L"]"));
		return false;
	}
}

std::wstring ImportDatabaseAdapter::readFromSqlFile(const std::wstring & importPath)
{
	if (importPath.empty()) {
		throw QRuntimeException(L"300011", S(L"import-path-empty-error"));
	}

	if (_waccess(importPath.c_str(), 0) != 0) { // file not exists
		throw QRuntimeException(L"300012", S(L"import-file-not-exists"));
	}

	std::wifstream ifs;
	auto codeccvt = new std::codecvt_utf8<wchar_t, 0x10ffff, std::codecvt_mode(std::generate_header | std::little_endian)>();
	std::locale utf8(std::locale("C"), codeccvt);
	ifs.imbue(utf8);
	ifs.open(importPath, std::ios::in);
	if (ifs.bad()) {
		return L"";
	}
	std::wstring result;
	wchar_t content[1025];
	while (!ifs.eof()) {		
		memset(content, 0, sizeof(wchar_t) * 1025);
		ifs.read(content, 1024);
		result.append(content);
	}
	
	ifs.close();
	return result;
}

/**
 * Exec the command line.
 * 
 * @param cmdline
 */
void ImportDatabaseAdapter::execCommandLine(const wchar_t * cmdline)
{
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(STARTUPINFO));

	HANDLE hStdInPipeRead = NULL;
    HANDLE hStdInPipeWrite = NULL;
    HANDLE hStdOutPipeRead = NULL;
    HANDLE hStdOutPipeWrite = NULL;

	 BOOL ok = TRUE;
	// Create two pipes.
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
	if (!CreatePipe(&hStdInPipeRead, &hStdInPipeWrite, &sa, 0)) {
		Q_ERROR(L"CreatePipe failed, ERROR:{}. cmdline:{}", GetLastError(), cmdline);
		return;
	}
	if (!CreatePipe(&hStdOutPipeRead, &hStdOutPipeWrite, &sa, 0)) {
		Q_ERROR(L"CreatePipe failed, ERROR:{}. cmdline:{}", GetLastError(), cmdline);
		return;
	}

    si.cb = sizeof(STARTUPINFO);
    si.hStdError = hStdOutPipeWrite;
    si.hStdOutput = hStdOutPipeWrite;
    si.hStdInput = hStdInPipeRead;
	si.wShowWindow = SW_HIDE; // ���������д���
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;

	PROCESS_INFORMATION pi = {};

	// ע��1�����system32·����ע��һ��Ҫ��new/delete������wchar_t���飬������CreateProcessW������ִ�������У���Ȼִ������������������ᱨ��
	wchar_t * exepath = new wchar_t[_MAX_PATH];
	memset(exepath, 0, sizeof(wchar_t) * sizeof(exepath));
	::GetSystemDirectoryW(exepath, _MAX_PATH);
	wcscat_s(exepath, _MAX_PATH, L"\\cmd.exe");

	// ע��2������1,2һ��Ҫ������,���ܱ�֤�ض���������������ȷ, 
	//   ���� c:\system32\cmd.exe \c sqlite3.exe > [����ļ�]����Ȼ�ض�����������ļ�����Ϊ�ض��������>���ز���ִ��
	if (!CreateProcessW(exepath, (LPWSTR)cmdline, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        Q_ERROR(L"CreateProcessW failed,ERROR:{}. cmdline:{}", GetLastError(), cmdline);
        return;
    }

	 // Close pipes we do not need.
    CloseHandle(hStdOutPipeWrite);
    CloseHandle(hStdInPipeRead);

	bool bStatus;
    CHAR aBuf[CMD_BUFF_LEN + 1];
    DWORD dwRead;
    // GetStdHandle(STD_OUTPUT_HANDLE)

	std::string outmsg;
    while (true) {
		ZeroMemory(aBuf, CMD_BUFF_LEN + 1);
        bStatus = ReadFile(hStdOutPipeRead, aBuf, sizeof(aBuf), &dwRead, NULL);
        if (!bStatus || dwRead == 0) {
            break;
        }
		outmsg.append(aBuf);
    }
	if (!outmsg.empty()) {
		std::wstring outstr = StringUtil::utf82Unicode(outmsg);
        Q_ERROR(L"message from console:{}", outstr);
	}

	WaitForSingleObject( pi.hProcess, INFINITE );
	delete[] exepath;

	// Clean up and exit.
    CloseHandle(hStdOutPipeRead);
    CloseHandle(hStdInPipeWrite);

    // Close process and thread handles. 
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
}
