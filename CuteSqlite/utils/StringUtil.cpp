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

 * @file   StringUtil.cpp
 * @brief  
 * 
 * @author Xuehan Qin
 * @date   2023-11-04
 *********************************************************************/
#include "stdafx.h"
#include "StringUtil.h"

/**
* convert Unicodes to UTF8..
* 
* @param widestring unicode string 
* @return 
*/
char * StringUtil::unicodeToUtf8(const std::wstring& widestring)
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

wchar_t * StringUtil::utf8ToUnicode(const char * utf8str)
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
std::wstring StringUtil::utf82Unicode(const std::string& utf8string)
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
std::string StringUtil::unicode2Utf8(const std::wstring& widestring)
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

// ascii ת Utf8
std::string StringUtil::ASCII2UTF_8(std::string& strAsciiCode)
{ 
	std::string strRet(""); 
	// �Ȱ� ascii תΪ unicode 
	std::wstring wstr = ascii2WideByte(strAsciiCode); 
	// ���� unicode תΪ utf8 
	strRet = unicode2Utf8(wstr); 
	return strRet; 
}

//unicode תΪ ascii 
std::string StringUtil::wideByte2Ascii(std::wstring& wstrcode)
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

// utf-8 ת ascii 
std::string StringUtil::UTF_82ASCII(std::string& strUtf8Code)
{ 
	std::string strRet(""); 
	// �Ȱ� utf8 תΪ unicode 
	std::wstring wstr = utf82Unicode(strUtf8Code); 
	// ���� unicode תΪ ascii 
	strRet = wideByte2Ascii(wstr); 
	return strRet; 
} 

// ascii ת Unicode 
std::wstring StringUtil::ascii2WideByte(std::string& strascii)
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
std::wstring StringUtil::mergeName(std::wstring &name1, std::wstring name2)
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
std::wstring StringUtil::cutLastChar(std::wstring &str)
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
* #param ignoreCase : ���Դ�Сд
* #return          : void
*/
std::wstring StringUtil::replace(const std::wstring & str, const std::wstring& before, const std::wstring& after, bool ignoreCase /*= false*/)
{		
	std::wstring text = str;
	if (!ignoreCase) {			
		for (std::wstring::size_type pos(0); pos != std::wstring::npos; pos += after.length()) {
			pos = text.find(before, pos);
			if (pos != std::wstring::npos)
				text.replace(pos, before.length(), after);
			else
				break;
		}
	} else {
		std::wregex pattern(before, std::wregex::icase);
		text = std::regex_replace(text, pattern, after, std::regex_constants::match_any);
	}
		
	return text;
}

/**
* Replace the the line break : \r\n.
* 
* @param str
* @return 
*/
std::wstring StringUtil::replaceBreak(const std::wstring & str)
{
	std::wstring res = replace(str, L"\\r", L"\r");
	res = replace(res, L"\\n", L"\n");
	return res;
}

/**
* search if the "search" string is exists in the "str" string.
* 
* @param str - original text
* @param search - search text
* @param ignoreCase - ignore case 
* @return 
*/
bool StringUtil::search(const std::wstring & str, const std::wstring & search, bool ignoreCase /*= false*/)
{
	if (!ignoreCase) {
		std::wstring text = str;
		size_t pos = text.find_first_of(search);
		return pos != std::wstring::npos;
	} else {
		std::wregex pattern(search, std::wregex::icase);
		return std::regex_search(str, pattern);
	}
}

/**
* ȡС�����λ����ת���ַ���
* 
* @param val
* @param fixed
* @return 
*/
std::wstring StringUtil::doubleToString(double val, int fixed)
{
	std::wostringstream oss;
	oss << std::setprecision(fixed) << val;
	return oss.str();
}

/**
* ����ַ�����.
* 
* @param str
* @param font
* @return 
*/
CSize StringUtil::getTextSize(const wchar_t * str, HFONT font)
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
* @param pattern �����ַ���,�綺��
* @return 
*/
std::vector<std::wstring> StringUtil::split(std::wstring str, const std::wstring & pattern, bool bTrim /*= true*/)
{
	std::wstring::size_type pos;
	std::vector<std::wstring> result;
	str += pattern; // ��չ�ַ����Է������
	int size = static_cast<int>(str.size());
	for (int i = 0; i < size; i++) {
		pos = str.find(pattern, i);
		if (pos < size) {
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

/**
 * �ַ����ָ��.
 * 
 * @param str �������ַ���
 * @param pattern �����ַ���,�綺��
 * @param cutEnclose ���ص��ַ������飬�ַ���ȥ��ָ����ǰ��ر��ַ�
 * @return 
 */
std::vector<std::wstring> StringUtil::splitCutEnclose(std::wstring str, const std::wstring & pattern, std::wstring & cutEnclose, bool bTrim /*= true*/)
{
	std::wstring::size_type pos;
	std::vector<std::wstring> result;
	str += pattern; // ��չ�ַ����Է������
	int size = static_cast<int>(str.size());
	for (int i = 0; i < size; i++) {
		pos = str.find(pattern, i);
		if (pos < size) {
			std::wstring s = str.substr(i, pos - i);
			if (s.empty()) {
				i = static_cast<int>(pos + pattern.size()) - 1;
				continue;
			}
			
			// ���ص��ַ��������е��ַ������������ڹر��ַ�����
			if (!cutEnclose.empty()) {
				size_t _begin = s.find(cutEnclose);
				size_t _end = s.rfind(cutEnclose);
				if (_begin != std::wstring::npos && _end != std::wstring::npos) {
					s = s.substr(_begin + 1, _end - _begin - 1);
				}
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

/**
* implode strings with symbol character.
* 
* @param strVec
* @param symbol - split character
* @param bTrim - whether trim the 
* @return 
*/
std::wstring StringUtil::implode(const std::vector<std::wstring> & strVec, const std::wstring & symbol, bool bTrim /*= true*/)
{
	if (strVec.empty()) {
		return L"";
	}
	std::wstring result;
	int i = 0;
	for (auto str : strVec) {
		if (i > 0 && !result.empty()) {
			result.append(symbol);
		}
		if (bTrim && !str.empty()) {
			StringUtil::trim(str);
		}
		if (!str.empty()) {
			result.append(str);
		}
		i++;
	}
	return result;
}

/**
 * Remove leading and trailing whitespace.
 * 
 * @param inout_str
 */
void StringUtil::trim(std::wstring &inout_str)
{
	static const wchar_t whitespace[] = L" \n\t\v\r\f";
	inout_str.erase(0, inout_str.find_first_not_of(whitespace));
	inout_str.erase(inout_str.find_last_not_of(whitespace) + 1U);
}

std::wstring StringUtil::escape(std::wstring &source)
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

std::wstring StringUtil::escapeXml(std::wstring & source)
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

std::wstring StringUtil::escapeSql(std::wstring &source)
{
	if (source.empty()) {
		return source;
	}

	// replace the escape character
	std::wstring result = replace(source, L"\'", L"\'\'");
	return result;
}

std::wstring StringUtil::toupper(std::wstring s)
{
	std::transform(s.begin(), s.end(), s.begin(), [](wchar_t c){ 
		return std::towupper(c); 
	});
	return s;
}

std::wstring StringUtil::tolower(std::wstring s)
{
	std::transform(s.begin(), s.end(), s.begin(), [](wchar_t c){ 
		return std::towlower(c); 
	});
	return s;
}

bool StringUtil::isDigit(std::wstring & s)
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

/**
 * append blank to string..
 * 
 * @param str
 * @param nBlk - the blank character length of appending
 * @return 
 */
std::wstring & StringUtil::blkToTail(std::wstring & str, int nBlk /*= 3*/)
{
	wchar_t * blk = L" "; // blank 
	for (int i = 0; i < nBlk; i++) {
		str.append(blk);
	}
	return str;
}

/**
* Get gid of the parens and quetes.
* 
* @param str
* @return 
*/
std::wstring & StringUtil::cutParensAndQuotes(std::wstring & str)
{
	if (str.empty()) {
		return str;
	}
		
	str = StringUtil::replace(str, L")", L"");
	str = StringUtil::replace(str, L"`", L"");
	str = StringUtil::replace(str, L"'", L"");
	str = StringUtil::replace(str, L"\"", L"");

	return str;
}

/**
 * Fetch a substring between begin pos and end pos that start find from offset position.
 * 
 * @param str - string
 * @param begin - begin symbol
 * @param end - end symbol
 * @param offset - offset to start finding
 * @param rescure - 
 * @return 
 */
std::wstring StringUtil::inSymbolString(const std::wstring & str, wchar_t begin, wchar_t end, size_t offset, bool rescure /*= true*/)
{
	if (str.empty()) {
		return str;
	}

	size_t _begin_pos = str.find_first_of(begin, offset);
	size_t _end_pos = str.find_first_of(end, offset);
	size_t _str_len = str.size();

	if (_begin_pos == std::wstring::npos || _end_pos == std::wstring::npos) {
		return str;
	}
	if (!rescure) { // not rescure search, then let end position = the last end symbol position 
		size_t last = str.find_last_of(end, offset);
		if (last == std::wstring::npos || last >= _str_len) {
			return str;
		}
		return str.substr(_begin_pos + 1, last - _begin_pos - 1);
	}

	std::stack<size_t> stack;
	size_t _find_pos = _begin_pos;
	while (_find_pos < _end_pos) {			
		stack.push(_find_pos);
		_find_pos = str.find_first_of(begin, _find_pos+1);
	}

	_find_pos = _end_pos;
	while (_find_pos < _str_len) {
		_end_pos = _find_pos;
		stack.pop();
		if (stack.empty()) {
			break;
		}			
		_find_pos = str.find_first_of(end, _find_pos+1);
	}

	if (_end_pos == std::wstring::npos || _end_pos >= _str_len) {
		return str;
	}
	return str.substr(_begin_pos + 1, _end_pos - _begin_pos - 1);
}

/**
 * Find the next character that it is not blank character including CR/LF/CRLF char and the space char.
 * 
 * @param line
 * @param word
 * @param offset
 * @return 
 */
wchar_t StringUtil::nextNotBlankChar(const std::wstring & line, wchar_t * word, size_t offset /*= 0*/)
{	
	if (line.empty() || word == nullptr || !wcslen(word)) {
		return 0;
	}
	size_t _line_len = line.size();
	size_t _word_pos = line.find(word, offset);
	if (_word_pos == std::wstring::npos) {
		return 0;
	}
	size_t pos = _word_pos + wcslen(word);
	if (pos >= _line_len) {
		return 0;
	}
	wchar_t nextChar = line.at(pos);
	// ignore the blank char
	while (nextChar == L' ' || nextChar == L'\t' 
		|| nextChar == L'\r' || nextChar == L'\n' ) {
		if (++pos > _line_len - 1) {
			break;
		}
		nextChar = line.at(pos);
	}
		
	return nextChar;
}