#include "stdafx.h"
#include "Lang.h"
#include "core/service/system/SettingService.h"

/**
 * ���ini���������ļ�[STRING]�ڵ��е��ı�.
 * 
 * @param key �ı���key
 * @return val 
 */
std::wstring Lang::lang(const wchar_t * key)
{
	Setting langSetting = SettingService::getInstance()->getSettingBySection(std::wstring(L"STRING"));

	auto iterator = std::find_if(langSetting.begin(), langSetting.end(), [&key](std::pair<std::wstring, std::wstring> & pair) {
		return pair.first == std::wstring(key);
	});
	if (iterator == langSetting.end()) {
		return L"";
	}

	return (*iterator).second;
}

/**
 * ���ini���������ļ�[ERROR]�ڵ��еĴ����ı�.
 * 
 * @param key �ı���key
 * @return val 
 */
std::wstring Lang::error(const std::wstring & key)
{
	Setting langSetting = SettingService::getInstance()->getSettingBySection(std::wstring(L"ERROR"));

	auto iterator = std::find_if(langSetting.begin(), langSetting.end(), [&key](std::pair<std::wstring, std::wstring> & pair) {
		return pair.first == key;
	});
	if (iterator == langSetting.end()) {
		return L"";
	}

	return (*iterator).second;
}

std::wstring Lang::langNoTab(const std::wstring & key)
{
	auto str = lang(key.c_str());
	if (str.empty()) {
		return str;
	}
	size_t tabPos = str.find_last_of(L'\t');
	if (tabPos != std::wstring::npos) {
		std::wstring pre = str.substr(0, tabPos);
		std::wstring tail = str.substr(tabPos+1, -1);
		str = pre;
		str.append(L"\t(").append(tail).append(L")");
	}
	str = StringUtil::replace(str, L"\t", L" ");
	return str;
}

/**
 * ���ini���������ļ�[FONT]�ڵ��е��ı�����ʹ�С.HFONT����
 * 
 * @param key ini�ļ�ָ���������key�����磺left-panel-size
 * @param bold �Ƿ�Ӵ�
 * @param defPixed Ĭ�ϵ������С
 * @param defFontName Ĭ�ϵ����� 
 * @return HFONT����
 */
HFONT Lang::font(const wchar_t * key, bool bold /*= false*/, int defPixed, const wchar_t * defFontName)
{
	Setting fontSetting = SettingService::getInstance()->getSettingBySection(std::wstring(L"FONT"));
	int pixel = defPixed; // Ĭ��ֵ
	
	std::wstring fontName = defFontName;
	for (auto iterator = fontSetting.begin(); iterator != fontSetting.end(); iterator++) {
		if ((*iterator).first == key) {
			pixel = std::stoi((*iterator).second);
		}
		if ((*iterator).first == L"font-name") {
			fontName = (*iterator).second;
		}
	}	

	LOGFONT lf; 
	memset(&lf, 0, sizeof(LOGFONT)); // zero out structure 
	lf.lfHeight = pixel; // request a 8-pixel-height font
	if(bold) {
		lf.lfWeight = FW_BOLD;  
	}
	lstrcpy(lf.lfFaceName, fontName.c_str()); // request a face name "Microsoft Yahei UI"
	
	HFONT font = ::CreateFontIndirect(&lf);
	return font;
}

/**
 * ���ini���������ļ�[FONT]�ڵ��е��ı�����ʹ�С�� ��GDI FONT����
 * 
 * @param key
 * @param bold
 * @param defPixed
 * @param defFontName
 * @return ��GDI FONT����
 */
Gdiplus::Font * Lang::gdiplusFont(const wchar_t * key, bool bold /*= false*/, int defPixed /*= 20*/, const wchar_t * defFontName /*= L"Microsoft Yahei UI"*/)
{
	Setting fontSetting = SettingService::getInstance()->getSettingBySection(std::wstring(L"FONT"));
	int pixel = defPixed; // Ĭ��ֵ
	
	std::wstring fontName = defFontName;
	for (auto iterator = fontSetting.begin(); iterator != fontSetting.end(); iterator++) {
		if ((*iterator).first == key) {
			pixel = std::stoi((*iterator).second);
		}
		if ((*iterator).first == L"font-name") {
			fontName = (*iterator).second;
		}
	}
	Gdiplus::FontFamily fontFamily(fontName.c_str());
	Gdiplus::Font * font = new Gdiplus::Font(&fontFamily, static_cast<Gdiplus::REAL>(pixel * 1.0), 
			(bold)?Gdiplus::FontStyleBold : Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

	return font;
}

/**
 * ���ini���������ļ�[FONT]�ڵ��е��ı�����.
 * 
 * @param defFontName
 * @return 
 */
std::wstring Lang::fontName(const wchar_t * defFontName /*= L"Microsoft Yahei UI"*/)
{
	std::wstring fn = SettingService::getInstance()->getValBySectionAndKey(std::wstring(L"FONT"), std::wstring(L"font-name"));
	if (fn.empty()) {
		return std::wstring(defFontName);
	}
	return fn;
}

/**
 * ���ini���������ļ�[FONT]�ڵ��е��ı���С.
 * 
 * @param key
 * @param defPixed
 * @return 
 */
int Lang::fontSize(const wchar_t * key, int defPixed /*= 20*/)
{
	Setting fontSetting = SettingService::getInstance()->getSettingBySection(std::wstring(L"FONT"));
	int pixel = defPixed; // Ĭ��ֵ
	
	
	for (auto iterator = fontSetting.begin(); iterator != fontSetting.end(); iterator++) {
		if ((*iterator).first == key) {
			pixel = std::stoi((*iterator).second);
			break;
		}
	}	

	return pixel;
}
