#include "stdafx.h"
#include "ColorUtil.h"
#include <Windows.h>

ColorUtil::ColorUtil()
{
}

ColorUtil::~ColorUtil()
{
}

void ColorUtil::setTextColor(HWND hwnd, COLORREF rgb)
{
	if (::IsWindow(hwnd)) {
		HDC hdc = ::GetDC(hwnd);
		::SetTextColor(hdc, rgb); //区域前景色
	}
}

void ColorUtil::setBkColor(HWND hwnd, COLORREF rgb)
{
	if (::IsWindow(hwnd)) {
		HDC hdc = ::GetDC(hwnd);
		::SetBkColor(hdc, rgb); //区域背景色
	}
}

void ColorUtil::transparent(HWND hwnd)
{
	ColorUtil::setBkColor(hwnd, RGB(255,255,255));
}

