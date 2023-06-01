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
		::SetTextColor(hdc, rgb); //����ǰ��ɫ
	}
}

void ColorUtil::setBkColor(HWND hwnd, COLORREF rgb)
{
	if (::IsWindow(hwnd)) {
		HDC hdc = ::GetDC(hwnd);
		::SetBkColor(hdc, rgb); //���򱳾�ɫ
	}
}

void ColorUtil::transparent(HWND hwnd)
{
	ColorUtil::setBkColor(hwnd, RGB(255,255,255));
}

