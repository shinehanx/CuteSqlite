/*****************************************************************//**
 * @file   Lang.h
 * @brief  读取语言配置文件的选项
 * @detail    $DETAIL
 * 
 * @author 覃学涵
 * @date   2023-04-05
 *********************************************************************/
#pragma once
#include <string>
#include <GdiPlus.h>

// 取指定语言的字符串
#define S(key) Lang::lang(key)

// 取指定语言的字体
#define FT(key) Lang::font(key)
#define FTB(key, bold) Lang::font(key, bold)
#define FTBP(key, bold, pixed) Lang::font(key, bold, pixed)
#define FTBPF(key, bold, pixed, fontName) Lang::font(key, bold, pixed, fontName)

// 取指定语言的字体(gdi+版本)
#define GDI_PLUS_FT(key) Lang::gdiplusFont(key)

class Lang {
public:
	/**
	 * 获得指定语言的文本.
	 */
	static std::wstring lang(const wchar_t * key);

	/**
	 * 获得指定语言的字体.
	 * 
	 * @param key
	 * @return 
	 */
	static HFONT font(const wchar_t * key, bool bold = false, int defPixed = 20, const wchar_t * defFontName = L"Microsoft Yahei UI");

	/**
	 * 获得指定语言的字体.
	 * 
	 * @param key
	 * @return 
	 */
	static Gdiplus::Font * gdiplusFont(const wchar_t * key, bool bold = false, int defPixed = 20, const wchar_t * defFontName = L"Microsoft Yahei UI");

	/**
	 * 获得字体名.
	 * 
	 * @return 
	 */
	static std::wstring fontName(const wchar_t * defFontName = L"Microsoft Yahei UI");

	/**
	 * 获得指定的字体大小.
	 * 
	 * @param key
	 * @param defPixed
	 * @return 
	 */
	static int fontSize(const wchar_t * key, int defPixed = 20);
};





