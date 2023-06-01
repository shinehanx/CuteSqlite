/*****************************************************************//**
 * @file   QPopAnimate.h
 * @brief  动画提示框
 * @detail    $DETAIL
 * 
 * @author 覃学涵
 * @date   2023-04-23
 *********************************************************************/
#pragma once
#include <atlwin.h>
#include <atltypes.h>
#include <atlcrack.h>
#include <string>
#include "resource.h"
#include "ui/common/image/QStaticImage.h"

#define QPOPANIMATE_WIDTH 300
#define QPOPANIMATE_HEIGHT 70
class QPopAnimate : public CDialogImpl<QPopAnimate> {
public:
	enum { IDD = IDD_QFORM_DIALOG };

	BEGIN_MSG_MAP_EX(QPopAnimate)
		//MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MSG_WM_CTLCOLOREDIT(OnCtlEditColor)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	static void error(HWND parentHwnd, std::wstring & text);
	static void success(HWND parentHwnd, std::wstring & text);
	static void warn(HWND parentHwnd, std::wstring & text);

	void setup(std::wstring & text);
	void setSize(int w, int h);
	void createOrShowUI();
	void setImagePath(std::wstring & imagePath);
private:
	int width = QPOPANIMATE_WIDTH;
	int height = QPOPANIMATE_HEIGHT;
	CRect winRect;
	CRect destopRect;
	std::wstring text;
	std::wstring imagePath;

	COLORREF textColor = RGB(8, 8, 8);
	COLORREF bkgColor = RGB(238, 238, 238);
	HBRUSH bkgBrush = nullptr;
	HFONT textFont = nullptr;
	QStaticImage image;

	CEdit textEdit;

	void createOrShowImage(CRect & clientRect);
	void createOrShowTextEdit(CRect & clientRect);

	CRect getInitRect();
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	HBRUSH OnCtlEditColor(HDC hdc, HWND hwnd);
};


