#include "stdafx.h"
#include "FontUtil.h"
#include <Windows.h>
#include <atlgdi.h>


FontUtil::FontUtil()
{
}

FontUtil::~FontUtil()
{
}

HFONT FontUtil::getFont(int pixel, bool bold, const wchar_t* font_name)
{
	LOGFONT lf; 
	memset(&lf, 0, sizeof(LOGFONT)); // zero out structure 
	lf.lfHeight = pixel; // request a 8-pixel-height font
	if(bold) {
		lf.lfWeight = FW_BOLD;  
	}
	lstrcpy(lf.lfFaceName, font_name); // request a face name "Microsoft Yahei UI"
	
	HFONT font = ::CreateFontIndirect(&lf);
	return font;
}

/**
 * 丈量文字占据的宽高.
 * 
 * @param text
 * @param pixel
 * @param bold
 * @param font_name
 * @return 
 */
CSize FontUtil::measureTextSize(const wchar_t * text, int pixel, bool bold /*= fasle*/, const wchar_t * font_name /*= L"Microsoft Yahei UI"*/)
{
	// 获得文本的宽高
	CClientDC clientDc(NULL);
	HFONT textFont = FontUtil::getFont(pixel, bold, font_name);
	clientDc.SelectFont(textFont);
	CSize size;
	::GetTextExtentPoint32(clientDc, text, static_cast<int>(wcslen(text)), &size);
	
	DeleteObject(textFont);
	return size;
}

