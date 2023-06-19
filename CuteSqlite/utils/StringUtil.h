#ifndef _STRINGUTIL_H
#define _STRINGUTIL_H

#include <Windows.h>
#include <string>
#include <locale>
#include <codecvt>
#include <vector>
#include <iomanip>
#include <sstream>
#include <atltypes.h>
#include <cwctype>
#include <algorithm>

/// <summary>
/// �ַ���ת�������࣬
/// </summary>
class StringUtil
{
public:

	/**
	 * convert Unicodes to UTF8..
	 * 
	 * @param widestring unicode string 
	 * @return 
	 */
	static char * unicodeToUtf8(const std::wstring& widestring)
	{
		if (widestring.empty()) {
			return "";
		}

		int utf8size = ::WideCharToMultiByte(CP_UTF8, 0, widestring.c_str(), -1, NULL, 0, NULL, NULL);
		if (utf8size == 0)
		{
			throw std::exception("Error in conversion.");
		}
		char * buff = new char[utf8size + 1]{ 0 };
		int convresult = ::WideCharToMultiByte(CP_UTF8, 0, widestring.c_str(), -1, buff, utf8size, NULL, NULL);
		if (convresult != utf8size)
		{
			throw std::exception("La falla!");
		}
		buff[utf8size] = '\0';
		
		return buff;
	}

	
	/**
	 * UTF8s to unicode..
	 * 
	 * @param utf8str
	 * @return 
	 */
	static wchar_t * utf8ToUnicode(const char * utf8str)
	{
		int len = static_cast<int>(strlen(utf8str));
		if (!len) {
			return nullptr;
		}
		//Ԥת�����õ�����ռ�Ĵ�С;
		int wcsLen = ::MultiByteToWideChar(CP_UTF8, NULL, utf8str, len, NULL, 0);

		//����ռ�Ҫ��'\0'�����ռ䣬MultiByteToWideChar�����'\0'�ռ�
		wchar_t * buff = new wchar_t[wcsLen + 1]{ 0 };

		//ת��
		int convresult =::MultiByteToWideChar(CP_UTF8, NULL, utf8str, len, buff, wcsLen);
		if (convresult != wcsLen) {
			throw std::exception("La falla!");
		}
		//������'\0'
		buff[wcsLen] = '\0';
		return buff;
	}

	//Utf8 ת unicode 
	static std::wstring utf82Unicode(const std::string& utf8string)
	{
		if (utf8string.empty()) {
			return std::wstring(L"");
		}
		int widesize = ::MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), -1, NULL, 0);
		if (widesize == ERROR_NO_UNICODE_TRANSLATION)
		{
			throw std::exception("Invalid UTF-8 sequence.");
		}
		if (widesize == 0)
		{
			throw std::exception("Error in conversion.");
		}
		std::vector<wchar_t> resultstring(widesize);
		int convresult = ::MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), -1, &resultstring[0], widesize);
		if (convresult != widesize)
		{
			throw std::exception("La falla!");
		}
		return std::wstring(&resultstring[0]);
	}

	//Unicode ת Utf8 
	static std::string unicode2Utf8(const std::wstring& widestring) 
	{ 
		if (widestring.empty()) {
			return std::string("");
		}
		int utf8size = ::WideCharToMultiByte(CP_UTF8, 0, widestring.c_str(), -1, NULL, 0, NULL, NULL); 
		if (utf8size == 0) 
		{ 
			throw std::exception("Error in conversion."); 
		} 
		std::vector<char> resultstring(utf8size); 
		int convresult = ::WideCharToMultiByte(CP_UTF8, 0, widestring.c_str(), -1, &resultstring[0], utf8size, NULL, NULL); 
		if (convresult != utf8size) 
		{ 
			throw std::exception("La falla!"); 
		} 
		
		return std::string(&resultstring[0]);
	} 

	//ascii ת Utf8 
	static std::string ASCII2UTF_8(std::string& strAsciiCode) 
	{ 
		std::string strRet(""); 
		//�Ȱ� ascii תΪ unicode 
		std::wstring wstr = ascii2WideByte(strAsciiCode); 
		//���� unicode תΪ utf8 
		strRet = unicode2Utf8(wstr); 
		return strRet; 
	}

	//unicode תΪ ascii 
	static std::string wideByte2Ascii(std::wstring& wstrcode) 
	{ 
		if (wstrcode.empty()) {
			return std::string("");
		}
		int asciisize = ::WideCharToMultiByte(CP_OEMCP, 0, wstrcode.c_str(), -1, NULL, 0, NULL, NULL); 
		if (asciisize == ERROR_NO_UNICODE_TRANSLATION) 
		{ 
			throw std::exception("Invalid UTF-8 sequence."); 
		} 
		if (asciisize == 0) 
		{ 
		throw std::exception("Error in conversion."); 
		} 
		std::vector<char> resultstring(asciisize); 
		int convresult =::WideCharToMultiByte(CP_OEMCP, 0, wstrcode.c_str(), -1, &resultstring[0], asciisize, NULL, NULL); 
		if (convresult != asciisize) 
		{ 
		throw std::exception("La falla!"); 
		} 
		return std::string(&resultstring[0]); 
	} 

	//utf-8 ת ascii 
	static std::string UTF_82ASCII(std::string& strUtf8Code) 
	{ 
		std::string strRet(""); 
		//�Ȱ� utf8 תΪ unicode 
		std::wstring wstr = utf82Unicode(strUtf8Code); 
		//���� unicode תΪ ascii 
		strRet = wideByte2Ascii(wstr); 
		return strRet; 
	} 
	/// 
	//ascii ת Unicode 
	static std::wstring ascii2WideByte(std::string& strascii) 
	{ 
		if (strascii.empty()) {
			return std::wstring(L"");
		}
		int widesize = MultiByteToWideChar (CP_ACP, 0, (char*)strascii.c_str(), -1, NULL, 0); 
		if (widesize == ERROR_NO_UNICODE_TRANSLATION) 
		{ 
			throw std::exception("Invalid UTF-8 sequence."); 
		} 
		if (widesize == 0) 
		{ 
			throw std::exception("Error in conversion."); 
		} 
		std::vector<wchar_t> resultstring(widesize); 
		int convresult = MultiByteToWideChar (CP_ACP, 0, (char*)strascii.c_str(), -1, &resultstring[0], widesize); 
		if (convresult != widesize) 
		{ 
			throw std::exception("La falla!"); 
		} 
		return std::wstring(&resultstring[0]); 
	}

	/**
	 * �ϲ�����.
	 * 
	 * @param name1
	 * @param name2
	 * @return 
	 */
	static std::wstring mergeName(std::wstring &name1, std::wstring name2)
	{
		std::wstring name = name1;
		if (name1.empty() == false && name2.empty() == false) {
			name.append(L" ");
		}
		name.append(name2);
		return name;
	}

	/**
	 * ȥ�����һ���ַ�.
	 * 
	 * @param name1
	 * @param name2
	 * @return 
	 */
	static std::wstring cutLastChar(std::wstring &str)
	{
		if (str.empty()) {
			return str;
		}

		return str.substr(0, str.size() - 1);
	}

	/**
	 * �ַ����滻����
	 * #function name   : replace_str()
	 * #param str       : ����֮ǰ���ַ���
	 * #param before    : ��Ҫ���滻���ַ���
	 * #param after     : �滻Ŀ���ַ���
	 * #return          : void
	 */
	static std::wstring replace(std::wstring str, const std::wstring& before, const std::wstring& after)
	{
		for (std::wstring::size_type pos(0); pos != std::wstring::npos; pos += after.length())
		{
			pos = str.find(before, pos);
			if (pos != std::wstring::npos)
				str.replace(pos, before.length(), after);
			else
				break;
		}
		return str;
	}

	/**
	 * ȡС�����λ����ת���ַ���
	 * 
	 * @param val
	 * @param fixed
	 * @return 
	 */
	static std::wstring doubleToString(double val, int fixed)
	{
		std::wostringstream oss;
		oss << std::setprecision(fixed) << val;
		return oss.str();
	}

	/**
	 * ����ַ����.
	 * 
	 * @param str
	 * @param font
	 * @return 
	 */
	static CSize getTextSize(const wchar_t * str, HFONT font) 
	{
		CClientDC cdc(NULL);
		HFONT oldFont = cdc.SelectFont(font);
		CSize size;
		cdc.GetTextExtent(str, static_cast<int>(wcslen(str)), &size);
		cdc.SelectFont(oldFont);
		return size;
	}

	/**
	 * �ַ����ָ��.
	 * 
	 * @param str �������ַ���
	 * @param pattern �����ַ���
	 * @return 
	 */
	static std::vector<std::wstring> split(std::wstring str, std::wstring pattern, bool bTrim = true)
	{

		std::wstring::size_type pos;
		std::vector<std::wstring> result;
		str += pattern; // ��չ�ַ����Է������
		int size = static_cast<int>(str.size());
		for (int i = 0; i < size; i++)
		{

			pos = str.find(pattern, i);
			if (pos < size)
			{
				std::wstring s = str.substr(i, pos - i);
				if (s.empty()) {
					i = static_cast<int>(pos + pattern.size()) - 1;
					continue;
				}
				if (bTrim && !s.empty()) {
					StringUtil::trim(s);
				}
				result.push_back(s);
				i = static_cast<int>(pos + pattern.size()) - 1;
			}
		}
		return result;
	}

	// Remove leading and trailing whitespace
	static void trim(std::wstring &inout_str) 
	{
		static const wchar_t whitespace[] = L" \n\t\v\r\f";
		inout_str.erase(0, inout_str.find_first_not_of(whitespace));
		inout_str.erase(inout_str.find_last_not_of(whitespace) + 1U);
	}

	static std::wstring escape(std::wstring &source) 
	{
		if (source.empty()) {
			return source;
		}

		// replace the escapre sequance
		std::wstring result = replace(source, L"\\", L"\\\\");
		result = replace(result, L"\"", L"\\\""); 
		result = replace(result, L"\r", L"\\r");
		result = replace(result, L"\n", L"\\n");
		return result;
	}

	static std::wstring escapeXml(std::wstring & source) 
	{
		if (source.empty()) {
			return source;
		}

		std::wostringstream wis;
		// replace the escapre sequance
		if (source.find_first_of(L'<') != -1 
			|| source.find_first_of(L'>') != -1
			|| source.find_first_of(L'\r') != -1
			|| source.find_first_of(L'\n') != -1
			|| source.find_first_of(L'[') != -1
			|| source.find_first_of(L']') != -1) {
			wis << L"<![CDATA[" << source << L"]]>";
			return wis.str();
		}
		return source;		
	}

	static std::wstring escapeSql(std::wstring &source) 
	{
		if (source.empty()) {
			return source;
		}

		// replace the escapre sequance
		std::wstring result = replace(source, L"\'", L"\'\'");
		return result;
	}

	static std::wstring toupper(std::wstring s)
	{
		std::transform(s.begin(), s.end(), s.begin(), [](wchar_t c){ 
			return std::towupper(c); 
		});
		return s;
	}

	static std::wstring tolower(std::wstring s)
	{
		std::transform(s.begin(), s.end(), s.begin(), [](wchar_t c){ 
			return std::towlower(c); 
		});
		return s;
	}

	static bool isDigit(std::wstring & s)
	{
		if (s.empty()) {
			return false;
		}

		int n = static_cast<int>(s.length());
		for (int i = 0; i < n; i++) {
			wchar_t ch = s.at(i);
			if (!std::iswdigit(ch)) {
				return false;
			}
		}
		return true;
	}
};



#endif