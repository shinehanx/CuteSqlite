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
#include <regex>
#include <stack>

/// <summary>
/// 字符串转换工具类，
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
	static char * unicodeToUtf8(const std::wstring& widestring);

	
	/**
	 * UTF8s to unicode..
	 * 
	 * @param utf8str
	 * @return 
	 */
	static wchar_t * utf8ToUnicode(const char * utf8str);
	//Utf8 转 unicode 
	static std::wstring utf82Unicode(const std::string& utf8string);

	//Unicode 转 Utf8 
	static std::string unicode2Utf8(const std::wstring& widestring);

	//ascii 转 Utf8 
	static std::string ASCII2UTF_8(std::string& strAsciiCode);

	//unicode 转为 ascii 
	static std::string wideByte2Ascii(std::wstring& wstrcode);
	

	//utf-8 转 ascii 
	static std::string UTF_82ASCII(std::string& strUtf8Code);
	
	/// 
	//ascii 转 Unicode 
	static std::wstring ascii2WideByte(std::string& strascii);

	/**
	 * 合并名字.
	 * 
	 * @param name1
	 * @param name2
	 * @return 
	 */
	static std::wstring mergeName(std::wstring &name1, std::wstring name2);

	/**
	 * 去掉最后一个字符.
	 * 
	 * @param name1
	 * @param name2
	 * @return 
	 */
	static std::wstring cutLastChar(std::wstring &str);

	/**
	 * 字符串替换函数
	 * #function name   : replace_str()
	 * #param str       : 操作之前的字符串
	 * #param before    : 将要被替换的字符串
	 * #param after     : 替换目标字符串
	 * #param ignoreCase : 忽略大小写
	 * #return          : void
	 */
	static std::wstring replace(const std::wstring & str, const std::wstring& before, const std::wstring& after, bool ignoreCase = false);

	/**
	 * Replace the the line break : \r\n.
	 * 
	 * @param str
	 * @return 
	 */
	static std::wstring replaceBreak(const std::wstring & str);

	/**
	 * Format the the line break : \n  -> \r\n.
	 * 
	 * @param str
	 * @return 
	 */
	static std::wstring formatBreak(const std::wstring & str);

	/**
	 * search if the "search" string is exists in the "str" string..
	 * 
	 * @param str - original text
	 * @param search - search text
	 * @param ignoreCase - ignore case 
	 * @return 
	 */
	static bool search(const std::wstring & str, const std::wstring & search, bool ignoreCase = false);

	/**
	 * search if the "search" string is start of the "str" string..
	 * 
	 * @param str - original text
	 * @param search - search text
	 * @param ignoreCase - ignore case 
	 * @return 
	 */
	static bool startWith(const std::wstring & str, const std::wstring & search, bool ignoreCase = false);

	/**
	 * search if the "search" string is end of the "str" string..
	 * 
	 * @param str - original text
	 * @param search - search text
	 * @param ignoreCase - ignore case 
	 * @return 
	 */
	static bool endWith(const std::wstring & str, const std::wstring & search, bool ignoreCase = false);


	/**
	 * 取小数点后几位，并转成字符串
	 * 
	 * @param val
	 * @param fixed
	 * @return 
	 */
	static std::wstring doubleToString(double val, int fixed);

	/**
	 * 获得字符宽高.
	 * 
	 * @param str
	 * @param font
	 * @return 
	 */
	static CSize getTextSize(const wchar_t * str, HFONT font);

	/**
	 * 字符串分割函数.
	 * 
	 * @param str 待分离字符串
	 * @param pattern 分离字符串,如逗号
	 * @return 
	 */
	static std::vector<std::wstring> split(std::wstring str, const std::wstring & pattern, bool bTrim = true);

	/**
	 * 字符串分割函数.
	 * 
	 * @param str 待分离字符串
	 * @param pattern 分离字符串,如逗号
	 * @param cutEnclose 返回的字符串数组中的字符串，不包含在关闭字符串中
	 * @return 
	 */
	static std::vector<std::wstring> splitCutEnclose(std::wstring str, const std::wstring & pattern, std::wstring & cutEnclose, bool bTrim = true);

	/**
	 * 使用空字符串分割函数.
	 * 
	 * @param str 待分离字符串
	 * @return 
	 */
	static std::vector<std::wstring> splitByBlank(std::wstring str, bool bTrim = true);

	/**
	 * 字符串分割函数，pattern不包含在notBegin, notEnd.
	 * 
	 * @param str 待分离字符串
	 * @param pattern 分离字符串,如逗号
	 * @param notBegin 分离字符串不包含在此字符串开始（开始和结束前遇到pattern字符串，则忽略）
	 * @param notEnd 分离字符串不包含在此字符串结束（开始和结束前遇到pattern字符串，则忽略）
	 * @param notContain notBegin和notEnd之间如果存在notContain字符串，则忽略本次的notBegin和notEnd
	 * @param bTrim
	 * @return 
	 */
	static std::vector<std::wstring> splitNotIn(std::wstring str, const std::wstring & pattern, 
		const std::wstring & notBegin, const std::wstring & notEnd, 
		const std::wstring & notContainIn, bool bTrim = true);

	/**
	 * implode strings with symbol character.
	 * 
	 * @param strVec
	 * @param symbol - split character
	 * @param bTrim - whether trim the 
	 * @return 
	 */
	static std::wstring implode(const std::vector<std::wstring> & strVec, const std::wstring & symbol, bool bTrim = true);

	/**
	 * add a character in front of every word and back of every word. the words is split by splitSymbol
	 * such as "id,name,gender" => "'id','name','word'"
	 */
	static std::wstring addSymbolToWords(const std::wstring & str, const std::wstring & splitSymbol, const std::wstring & addSymbol);

	// Remove leading and trailing whitespace
	static void trim(std::wstring &inout_str);

	static std::wstring escape(std::wstring &source);
	static std::wstring escapeXml(std::wstring & source);	
	static std::wstring escapeSql(std::wstring &source);

	static std::wstring toupper(std::wstring s);
	static std::wstring tolower(std::wstring s);

	static bool isDigit(std::wstring & s);
	static bool isDecimal(std::wstring & s);

	/**
	 * append blank to string.
	 */
	static std::wstring & blkToTail(std::wstring & str, int nBlk = 3);

	/**
	 * Get gid of the parens and quetes.
	 * 
	 * @param str
	 * @return 
	 */
	static std::wstring & cutParensAndQuotes(std::wstring & str);

	/**
	 * Get gid of the parens and quotes.
	 * 
	 * @param str
	 * @return 
	 */
	static std::wstring & cutQuotes(std::wstring & str);

	/**
	 * Fetch a substring between begin char and end char that start find from offset position.
	 * 
	 * @param str - string
	 * @param begin - begin symbol
	 * @param end - end symbol
	 * @param offset - offset to start finding
	 * @param rescure - 
	 * @return 
	 */
	static std::wstring inSymbolString(const std::wstring & str, wchar_t begin, wchar_t end, size_t offset, bool rescure = true);

	//  Fetch a substring between begin char and end char that start find from offset position.
	static std::vector<std::wstring> inSymbolStrings(const std::wstring & str, wchar_t begin, wchar_t end, size_t offset, bool rescure = true);

	// Fetch a substring between begin char and end char that start find from offset position.
	static std::wstring notInSymbolString(const std::wstring & str, wchar_t begin, wchar_t end, size_t offset, bool rescure = true);

	/**
	 * Find the next character that it is not blank character.
	 * 
	 * @param line
	 * @param word
	 * @param offset
	 * @return 
	 */
	static wchar_t nextNotBlankChar(const std::wstring & line, wchar_t * word, size_t offset = 0);

	/**
	 * Convert std::wstring vector to std::string vector.
	 * 
	 * @param tags
	 * @return 
	 */
	static std::vector<std::string> wstringsToStrings(const std::vector<std::wstring> & wstrs);

	static bool existsIn(const std::wstring & str, const std::vector<std::wstring> & strVector);
};



#endif