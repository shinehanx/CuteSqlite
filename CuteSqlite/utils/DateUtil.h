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
	}

	static std::wstring getYestoday(const wchar_t * format = L"%Y-%m-%d") 
	{
		CTime curTime = CTime::GetCurrentTime();
		// CTimeSpan params : 0 - LONG lDays, 1 - int nHours, 2 - int nMins, 3 - int nSecs
		curTime -= CTimeSpan(1, 0, 0, 0); 
		CString time = curTime.Format(format);		
		return time.GetString(); 
	};

	static CTime getYestodayTime() 
	{
		CTime curTime = CTime::GetCurrentTime();
		// CTimeSpan params : 0 - LONG lDays, 1 - int nHours, 2 - int nMins, 3 - int nSecs
		curTime -= CTimeSpan(1, 0, 0, 0); 
		return curTime;
	};

	/**
	 * Get the date from datetime,such as "2023-11-16 17:41:22" -> "2023-11-16" .
	 * 
	 * @param datetime - Format must be "%Y-%m-%d %H:%M:%S"
	 * @return 
	 */
	static std::wstring getDateFromDateTime(const std::wstring & datetime)
	{
		if (datetime.empty()) {
			return L"";
		}
		int nYear, nMonth, nDate, nHour, nMin, nSec;
	    swscanf_s(datetime.c_str(), L"%d-%d-%d %d:%d:%d", &nYear, &nMonth, &nDate, &nHour, &nMin, &nSec); 
		CTime time(nYear, nMonth, nDate, nHour, nMin, nSec);
		const wchar_t * format = L"%Y-%m-%d";
		CString date = time.Format(format);
		return date.GetString();
	}

	/**
	 * Get the date from datetime,such as "2023-11-16 17:41:22" .
	 * 
	 * @param datetime - Format must be "%Y-%m-%d %H:%M:%S"
	 * @return 
	 */
	static CTime getTimeFromString(const std::wstring & datetime)
	{
		if (datetime.empty()) {
			ATLASSERT(false);
		}
		int nYear, nMonth, nDate, nHour, nMin, nSec;
	    swscanf_s(datetime.c_str(), L"%d-%d-%d %d:%d:%d", &nYear, &nMonth, &nDate, &nHour, &nMin, &nSec); 
		CTime time(nYear, nMonth, nDate, nHour, nMin, nSec);
		return time;
	}

	static std::wstring getWeekDay(CTime & time)
	{
		UINT dayOfWeek[] = {
		   LOCALE_SDAYNAME7,   // Sunday
		   LOCALE_SDAYNAME1,
		   LOCALE_SDAYNAME2,
		   LOCALE_SDAYNAME3,
		   LOCALE_SDAYNAME4,
		   LOCALE_SDAYNAME5,
		   LOCALE_SDAYNAME6   // Saturday
		};
		wchar_t strWeekday[256];
		CTime tm(time);   // Initialize CTime with current time
		::GetLocaleInfoW(LOCALE_USER_DEFAULT,   // Get string for day of the week from system
			dayOfWeek[tm.GetDayOfWeek() - 1],   // Get day of week from CTime
			strWeekday, sizeof(strWeekday) / sizeof(strWeekday[0]));

		return strWeekday;
	}
};
