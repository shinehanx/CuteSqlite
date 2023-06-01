#pragma once

#include <string>

using namespace std;

class ColorUtil
{

public:
	ColorUtil();
	~ColorUtil();

	//设置文本颜色
	static void setTextColor(HWND hwnd , COLORREF rgb);

	//设置背景颜色
	static void setBkColor(HWND hwnd , COLORREF rgb);

	//transparent color
	static void transparent(HWND hwnd);
};

