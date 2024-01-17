/*****************************************************************//**
 * @file   QHorizontalBar.h
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

class QHorizontalBar: public CWindowImpl<QHorizontalBar> {
public:
	BEGIN_MSG_MAP_EX(QHorizontalBar)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
	QHorizontalBar(const std::wstring & val, float percent = 0, COLORREF processColor = RGB(49, 139, 202));
	~QHorizontalBar();
	
	void draw(const std::wstring & val, float percent);
	void error(const std::wstring & err);
	void reset();

	void setColors(COLORREF bkgColor, COLORREF processColor);
private:
	float percent = 0; // such as 99.70
	std::wstring val;
	std::wstring err;

	HRGN hRgn = nullptr;

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
	BOOL OnEraseBkgnd(CDCHandle dc);
};
