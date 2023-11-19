/*****************************************************************//**
 * @file   QProcessBar.h
 * @brief  进度条
 * @detail    $DETAIL
 * 
 * @author 覃学涵
 * @date   2023-04-24
 *********************************************************************/
#pragma once
#include <atlwin.h>
#include <atlcrack.h>
#include <atltypes.h>

class QProcessBar: public CWindowImpl<QProcessBar> {
public:
	BEGIN_MSG_MAP_EX(QProcessBar)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
	void run(int percent);
	void error(const std::wstring & err);
	void reset();

	void setColors(COLORREF bkgColor, COLORREF processColor);
private:
	int percent = 0;
	std::wstring err;

	COLORREF bkgColor =  RGB(192, 192, 192);
	CBrush bkgBrush;
	COLORREF processColor = RGB(49, 139, 202);
	COLORREF errorColor = RGB(255, 127, 39);
	CBrush processBrush;
	CBrush errorBrush;

	COLORREF textColor = RGB(255, 255, 255);
	HFONT textFont = nullptr;
	CPen textPen;

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};
