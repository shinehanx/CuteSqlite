#pragma once

#include <string>
#include <atltypes.h>

using namespace std;

class FontUtil
{

public:
	FontUtil();
	~FontUtil();

	static HFONT getFont(int pixel, bool bold = false, const wchar_t* font_name = L"Microsoft Yahei UI");

	static CSize measureTextSize(const wchar_t * str, int pixel, bool bold = false, const wchar_t * font_name = L"Microsoft Yahei UI");
};

