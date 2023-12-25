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
	static char * unicodeToUtf8(const std::wstring& widestring);

	
	/**
	 * UTF8s to unicode..
	 * 
	 * @param utf8str
	 * @return 
	 */
	static wchar_t * utf8ToUnicode(const char * utf8str);
	//Utf8 ת unicode 
	static std::wstring utf82Unicode(const std::string& utf8string);

	//Unicode ת Utf8 
	static std::string unicode2Utf8(const std::wstring& widestring);

	//ascii ת Utf8 
	static std::string ASCII2UTF_8(std::string& strAsciiCode);

	//unicode תΪ ascii 
	static std::string wideByte2Ascii(std::wstring& wstrcode);
	

	//utf-8 ת ascii 
	static std::string UTF_82ASCII(std::string& strUtf8Code);
	
	/// 
	//ascii ת Unicode 
	static std::wstring ascii2WideByte(std::string& strascii);

	/**
	 * �ϲ�����.
	 * 
	 * @param name1
	 * @param name2
	 * @return 
	 */
	static std::wstring mergeName(std::wstring &name1, std::wstring name2);

	/**
	 * ȥ�����һ���ַ�.
	 * 
	 * @param name1
	 * @param name2
	 * @return 
	 */
	static std::wstring cutLastChar(std::wstring &str);

	/**
	 * �ַ����滻����
	 * #function name   : replace_str()
	 * #param str       : ����֮ǰ���ַ���
	 * #param before    : ��Ҫ���滻���ַ���
	 * #param after     : �滻Ŀ���ַ���
	 * #param ignoreCase : ���Դ�Сд
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
	 * ȡС�����λ����ת���ַ���
	 * 
	 * @param val
	 * @param fixed
	 * @return 
	 */
	static std::wstring doubleToString(double val, int fixed);

	/**
	 * ����ַ����.
	 * 
	 * @param str
	 * @param font
	 * @return 
	 */
	static CSize getTextSize(const wchar_t * str, HFONT font);

	/**
	 * �ַ����ָ��.
	 * 
	 * @param str �������ַ���
	 * @param pattern �����ַ���,�綺��
	 * @return 
	 */
	static std::vector<std::wstring> split(std::wstring str, const std::wstring & pattern, bool bTrim = true);

	/**
	 * �ַ����ָ��.
	 * 
	 * @param str �������ַ���
	 * @param pattern �����ַ���,�綺��
	 * @param cutEnclose ���ص��ַ��������е��ַ������������ڹر��ַ�����
	 * @return 
	 */
	static std::vector<std::wstring> splitCutEnclose(std::wstring str, const std::wstring & pattern, std::wstring & cutEnclose, bool bTrim = true);

	/**
	 * ʹ�ÿ��ַ����ָ��.
	 * 
	 * @param str �������ַ���
	 * @return 
	 */
	static std::vector<std::wstring> splitByBlank(std::wstring str, bool bTrim = true);

	/**
	 * �ַ����ָ����pattern��������notBegin, notEnd.
	 * 
	 * @param str �������ַ���
	 * @param pattern �����ַ���,�綺��
	 * @param notBegin �����ַ����������ڴ��ַ�����ʼ����ʼ�ͽ���ǰ����pattern�ַ���������ԣ�
	 * @param notEnd �����ַ����������ڴ��ַ�����������ʼ�ͽ���ǰ����pattern�ַ���������ԣ�
	 * @param notContain notBegin��notEnd֮���������notContain�ַ���������Ա��ε�notBegin��notEnd
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