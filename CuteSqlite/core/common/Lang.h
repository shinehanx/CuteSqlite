/*****************************************************************//**
 * @file   Lang.h
 * @brief  ��ȡ���������ļ���ѡ��
 * @detail    $DETAIL
 * 
 * @author ��ѧ��
 * @date   2023-04-05
 *********************************************************************/
#pragma once
#include <string>
#include <GdiPlus.h>

// ȡָ�����Ե��ַ���
#define S(key) Lang::lang(key)

// ȡָ�����Ե�����
#define FT(key) Lang::font(key)
#define FTB(key, bold) Lang::font(key, bold)
#define FTBP(key, bold, pixed) Lang::font(key, bold, pixed)
#define FTBPF(key, bold, pixed, fontName) Lang::font(key, bold, pixed, fontName)

// ȡָ�����Ե�����(gdi+�汾)
#define GDI_PLUS_FT(key) Lang::gdiplusFont(key)

class Lang {
public:
	/**
	 * ���ָ�����Ե��ı�.
	 */
	static std::wstring lang(const wchar_t * key);

	/**
	 * ���ָ�����Ե�����.
	 * 
	 * @param key
	 * @return 
	 */
	static HFONT font(const wchar_t * key, bool bold = false, int defPixed = 20, const wchar_t * defFontName = L"Microsoft Yahei UI");

	/**
	 * ���ָ�����Ե�����.
	 * 
	 * @param key
	 * @return 
	 */
	static Gdiplus::Font * gdiplusFont(const wchar_t * key, bool bold = false, int defPixed = 20, const wchar_t * defFontName = L"Microsoft Yahei UI");

	/**
	 * ���������.
	 * 
	 * @return 
	 */
	static std::wstring fontName(const wchar_t * defFontName = L"Microsoft Yahei UI");

	/**
	 * ���ָ���������С.
	 * 
	 * @param key
	 * @param defPixed
	 * @return 
	 */
	static int fontSize(const wchar_t * key, int defPixed = 20);
};





