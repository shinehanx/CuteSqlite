#pragma once

#include <string>

using namespace std;

class ColorUtil
{

public:
	ColorUtil();
	~ColorUtil();

	//�����ı���ɫ
	static void setTextColor(HWND hwnd , COLORREF rgb);

	//���ñ�����ɫ
	static void setBkColor(HWND hwnd , COLORREF rgb);

	//transparent color
	static void transparent(HWND hwnd);
};

