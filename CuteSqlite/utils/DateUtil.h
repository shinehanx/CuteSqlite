#pragma once
#include <string>
#include <sstream>
#include <atltime.h>
class DateUtil
{
public:
	static std::wstring getCurrentDateTime(const wchar_t * format = L"%Y-%m-%d %H:%M:%S") 
	{
		CTime curTime = CTime::GetCurrentTime();
		CString time = curTime.Format(format);		
		return time.GetString(); 
	}

	static std::wstring getCurrentDate(const wchar_t * format = L"%Y-%m-%d") 
	{
		CTime curTime = CTime::GetCurrentTime();
		CString time = curTime.Format(format);		
		return time.GetString(); 
	};
};
