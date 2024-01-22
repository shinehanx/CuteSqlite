#pragma once

#include <Windows.h>
#include <atlbase.h>
#include <atlapp.h>
#include <atlcrack.h>
#include <atlwin.h>
#include <atlframe.h>
#include <atlctrls.h>
#include <atlmisc.h>
#include <GdiPlus.h>
#include <string>

typedef std::pair<WNDPROC, HWND> PWC;

class QImageButton : public CWindowImpl<QImageButton, CButton>, public COwnerDraw<QImageButton> {
public:
	BOOL PreTranslateMessage(MSG* pMsg);
	BEGIN_MSG_MAP_EX(QImageButton)	
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_RANGE_HANDLER(WM_MOUSEFIRST, WM_MOUSELAST, OnMouseMessage)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSEHOVER,OnMouseHover) 
		MESSAGE_HANDLER(WM_MOUSELEAVE,OnMouseLeave)
		CHAIN_MSG_MAP_ALT(COwnerDraw<QImageButton>, 1)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	QImageButton();
	~QImageButton();

	// 重载COwnerDraw的自定义绘制函数，绘制图像
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	void SetIconPath(const std::wstring & normalPath, const std::wstring & pressedPath) {
		normal_path = normalPath;
		pressed_path = pressedPath;
		iconPath = normal_path;//默认使用normal
	}

	void SetCapturePath(const std::wstring & capturePath) {
		capture_path = capturePath;
	}

	void SetTextColor(COLORREF color){
		color_text = color;
	}

	void SetBkgColor(COLORREF color){
		color_backgroud = color;
	}

	void SetBorderColor(COLORREF color){
		color_border = color;
	}

	void SetFontColors(DWORD normal_font_color, DWORD pressed_font_color, DWORD disable_font_color){
		this->normal_font_color = normal_font_color;
		this->pressed_font_color = pressed_font_color;
		this->disable_font_color = disable_font_color;
	}

	void SetBkgColors(DWORD normal_bkg_color, DWORD pressed_bkg_color, DWORD disable_bkg_color){
		this->normal_bkg_color = normal_bkg_color;
		this->pressed_bkg_color = pressed_bkg_color;
		this->disable_bkg_color = disable_bkg_color;
	}

	void SetBorderColors(DWORD normal_border_color, DWORD pressed_border_color, DWORD disable_border_color){
		this->normal_border_color = normal_border_color;
		this->pressed_border_color = pressed_border_color;
		this->disable_border_color = disable_border_color;
	}

	void SetFont(Gdiplus::Font * theFont){
		DeleteObject(font);
		font = theFont;
	}

	void SetToolTip(std::wstring & tooltip);
private:
	std::wstring iconPath;
	std::wstring capture_path;
	std::wstring normal_path;
	std::wstring pressed_path;

	// tooltip是参考CBimapButton的思路搞出来的
	std::wstring tooltip;
	CToolTipCtrl tooltipCtrl;
	UINT tooltipId = 1024;

	bool	m_bTracking;
	int		status;

	Gdiplus::Font * font = nullptr;

	COLORREF color_text = RGB(153,153,153);
	COLORREF color_backgroud = RGB(255,255,255);
	COLORREF color_border = RGB(255,255,255);

	DWORD normal_bkg_color = RGB(255, 255, 255);
	DWORD normal_font_color = RGB(153, 153, 153);
	DWORD normal_border_color = RGB(255, 255, 255);

	DWORD pressed_bkg_color = RGB(255, 255, 255);
	DWORD pressed_border_color = RGB(255, 255, 255);
	DWORD pressed_font_color = RGB(0x12, 0x96, 0xdb);

	DWORD disable_bkg_color = RGB(255, 255, 255);
	DWORD disable_border_color = RGB(255, 255, 255);
	DWORD disable_font_color = RGB(153, 153, 153);

	

	void DrawRect(Gdiplus::Graphics& graphics, Gdiplus::Rect rect, DWORD color);
	void DrawCapture(Gdiplus::Graphics& graphics, Gdiplus::Rect rect, DWORD color);
protected:

	LRESULT OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT OnHotItemChange(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	
	virtual BOOL OnEraseBkgnd(CDCHandle dc);

	
};