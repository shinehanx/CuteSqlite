#include "stdafx.h"
#include "QImageButton.h"
#include <sstream>
#include <iostream>
#include <winuser.h>
#include "utils/GdiPlusUtil.h"
#include "utils/Log.h"
#include "core/common/Lang.h"

namespace
{
	enum
	{
		kButtonStatusNormal,
		kButtonStatusPressed,
		kButtonStatusHover,
		kButtonStatusDisable
	};
}

BOOL QImageButton::PreTranslateMessage(MSG* pMsg)
{
	if (tooltipCtrl.IsWindow()) {
		tooltipCtrl.RelayEvent(pMsg);
	}
	return FALSE;
}

QImageButton::QImageButton()
{

}

QImageButton::~QImageButton()
{
	Detach();	
}

LRESULT QImageButton::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (font == nullptr) {
		font = GDI_PLUS_FT(L"image-button-size");
	}

	
	bHandled = FALSE;
	status = kButtonStatusNormal;
	m_bTracking = false;

	//icon
	iconPath = normal_path;

	// create a tool tip
	tooltipCtrl.Create(this->m_hWnd);
	ATLASSERT(tooltipCtrl.IsWindow());
	if(tooltipCtrl.IsWindow() && (tooltip.empty() != false)) {
		tooltipCtrl.Activate(TRUE);
		tooltipCtrl.AddTool(this->m_hWnd, tooltip.c_str());
	}
	return 0;
}

LRESULT QImageButton::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	::DeleteObject(font);

	if(tooltipCtrl.IsWindow()) {
		tooltipCtrl.DestroyWindow();
		tooltipCtrl.m_hWnd = NULL;
	}

	return 0;
}

void QImageButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	//Q_DEBUG(L"call QImageButton::DrawItem()");
	// Ellipse
	HDC hdc = lpDrawItemStruct->hDC;
	
	CRect rect;
	GetClientRect(&rect);
	Gdiplus::Bitmap bmp(int(rect.Width()),int(rect.Height()));
	Gdiplus::Graphics graphics(&bmp);
	graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintSystemDefault);
	graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

	// 按钮窗口创建可能有1像素的边框区域，需要夸扩大左上区域1像素才可以把边界绘制到.
	// 不清楚原因.
	rect.InflateRect(1,1,0,0);
	Gdiplus::Rect client_rect(rect.left,rect.top,rect.Width(),rect.Height());

	switch(status) {
	case kButtonStatusNormal:
		{
			SetTextColor(normal_font_color);
			DrawRect(graphics, client_rect, normal_bkg_color);
			break;
		}
	case kButtonStatusPressed:
		{
			SetTextColor(pressed_font_color);
			DrawRect(graphics, client_rect, pressed_bkg_color);
			break;
		}
	case kButtonStatusHover:
		{
			SetTextColor(pressed_font_color);
			DrawRect(graphics, client_rect, pressed_bkg_color);
			break;
		}
	case kButtonStatusDisable:
		{
			SetTextColor(disable_font_color);
			DrawRect(graphics, client_rect, disable_bkg_color);
			break;
		}
	}

	CString winText;
	GetWindowText(winText);
	std::wstring text = winText.GetString();
	if(text.size()){
		Gdiplus::RectF rect(static_cast<Gdiplus::REAL>(client_rect.X), 
			static_cast<Gdiplus::REAL>(client_rect.Y), 
			static_cast<Gdiplus::REAL>(client_rect.Width), 
			static_cast<Gdiplus::REAL>(client_rect.Height));
		Gdiplus::Color gdiTextColor;
		gdiTextColor.SetFromCOLORREF(color_text);
		GdiPlusUtil::DrawTextCenter(graphics, rect, text.c_str(), font, &gdiTextColor);
	}

	// 画图片
	if (iconPath.size()) {
		Gdiplus::Image iconImage(iconPath.c_str(), false);
		Gdiplus::SizeF textSize(0.0, 0.0);
		UINT splitSize = 0; // 文字和图片间距
		if (text.size()) {
			textSize = GdiPlusUtil::GetTextBounds(font, text);
			splitSize = 5; // 文字和图片间距为5
		}
		UINT imageWidth = iconImage.GetWidth();
		UINT imageHeight = iconImage.GetHeight();

		if (static_cast<UINT>(client_rect.Width) < imageWidth 
			&& static_cast<UINT>(client_rect.Height) < imageHeight) { // 图片超大，图片压扁显示
			graphics.DrawImage(&iconImage, client_rect, 0, 0, imageWidth, imageHeight, Gdiplus::UnitPixel);
		} else {
			int ix = 0;
			if ((UINT)(textSize.Width) > 0) {
				ix = ((client_rect.Width - (UINT)textSize.Width) / 2) - imageWidth - splitSize;
			} else if (client_rect.Width - imageWidth * 1.0 > 0.0){
				ix = (int)((client_rect.Width - imageWidth * 1.0) / 2.0);
			} else {
				ix = -1 * (int)((imageWidth * 1.0 - client_rect.Width) / 2.0);
			}
			int iy = (client_rect.Height - imageHeight) /2;
			GdiPlusUtil::DrawImage(graphics, &iconImage, ix, iy);
		}
	}
	// 画遮罩
	if (capture_path.size()) {
		DrawCapture(graphics, client_rect, RGB(177, 177, 177));
	}

	///*Drawing on DC*/
	Gdiplus::Graphics graphshow(hdc);
	///*Important! Create a CacheBitmap object for quick drawing*/
	Gdiplus::CachedBitmap cachedBmp(&bmp, &graphshow);
	graphshow.DrawCachedBitmap(&cachedBmp, 0, 0);


	SetMsgHandled(FALSE);
}

/**
 * 参考CBitmapButton，弹出tooltip.
 * 
 * @param tooltip
 */
void QImageButton::SetToolTip(std::wstring & tooltip)
{
	this->tooltip = tooltip;

	if(tooltip.empty()) {
		if(tooltipCtrl.IsWindow())
			tooltipCtrl.Activate(FALSE);
		return ;
	}

	if(tooltipCtrl.IsWindow()) {
		tooltipCtrl.Pop();
		tooltipCtrl.Activate(TRUE);
		tooltipCtrl.AddTool(this->m_hWnd, tooltip.c_str());
	}	
}

LRESULT QImageButton::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!m_bTracking) {  
        TRACKMOUSEEVENT tme;  
        tme.cbSize = sizeof(TRACKMOUSEEVENT);  
        tme.dwFlags = TME_LEAVE | TME_HOVER;//要触发的消息类型  
        tme.hwndTrack = m_hWnd;  
        tme.dwHoverTime = 10;// 如果不设此参数,无法触发mouseHover  
  
		 //MOUSELEAVE|MOUSEHOVER消息由此函数触发.  
        if (::_TrackMouseEvent(&tme)) {  
            m_bTracking = true;     
        }  
    }
	
    return 0;  
}

LRESULT QImageButton::OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(status != kButtonStatusPressed){
		status = kButtonStatusHover;
	}
	Invalidate(FALSE);
	bHandled = FALSE;

	//背景色/文字颜色/边框颜色
	color_backgroud  = RGB(GetRValue(pressed_bkg_color), GetGValue(pressed_bkg_color), GetBValue(pressed_bkg_color));
	color_border = RGB(GetRValue(pressed_border_color), GetGValue(pressed_border_color), GetBValue(pressed_border_color));
	color_text = RGB(GetRValue(pressed_font_color), GetGValue(pressed_font_color), GetBValue(pressed_font_color));
	
	//icon
	iconPath = pressed_path;
	return 0;
}

LRESULT QImageButton::OnMouseLeave(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	status = kButtonStatusNormal;
	Invalidate(FALSE);
	m_bTracking = false;
	bHandled = FALSE;

	//背景色/文字颜色/边框颜色
	color_backgroud  = RGB(GetRValue(normal_bkg_color), GetGValue(normal_bkg_color), GetBValue(normal_bkg_color));
	color_border = RGB(GetRValue(pressed_border_color), GetGValue(pressed_border_color), GetBValue(pressed_border_color));
	color_text = RGB(GetRValue(normal_font_color), GetGValue(normal_font_color), GetBValue(normal_font_color));

	//icon
	iconPath = normal_path;
	return 0; 
}

LRESULT QImageButton::OnHotItemChange(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
	return TRUE;
}

BOOL QImageButton::OnEraseBkgnd(CDCHandle dc)
{
	return TRUE;
}

/**
 * 捕捉这个消息是参考CBitmapButton的思路弹出tooltip.
 * 
 * @param uMsg
 * @param wParam
 * @param lParam
 * @param bHandled
 * @return 
 */
LRESULT QImageButton::OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	MSG msg = { this->m_hWnd, uMsg, wParam, lParam };
	if(tooltipCtrl.IsWindow())
		tooltipCtrl.RelayEvent(&msg);
	bHandled = FALSE;
	return 1;
}

void QImageButton::DrawRect(Gdiplus::Graphics& graphics, Gdiplus::Rect rect, DWORD color)
{
	Gdiplus::GraphicsPath m_pPath;
	m_pPath.AddRectangle(rect);
	m_pPath.CloseFigure();
	Gdiplus::Color bgkColor(GetRValue(color_backgroud), GetGValue(color_backgroud),
		GetBValue(color_backgroud));
	Gdiplus::SolidBrush bkgBrush(bgkColor);
	graphics.FillPath(&bkgBrush,&m_pPath);

	Gdiplus::Pen *pen = nullptr;
	Gdiplus::Color gcolor(GetRValue(color),GetGValue(color), GetBValue(color));
	if(color_border == -1){
		pen = new Gdiplus::Pen(gcolor, 1);
	}else{
		Gdiplus::Color gdi_color_border(GetRValue(color_border), GetGValue(color_border),
			GetBValue(color_border));
		pen = new Gdiplus::Pen(gdi_color_border, 1);
	}

	Gdiplus::SolidBrush colorBrush(gcolor);
	Gdiplus::GraphicsPath borderPath;
	
	borderPath.AddRectangle(rect);
	borderPath.CloseFigure();
	
	graphics.FillPath(&colorBrush, &borderPath);

	if(color_border != -1)
		graphics.DrawPath(pen, &borderPath);

	
	if (pen) {
		delete pen;
	}
}

/**
 * 画右下角遮罩的拍照三角形
 */
void QImageButton::DrawCapture(Gdiplus::Graphics& graphics, Gdiplus::Rect rect, DWORD color)
{
	Gdiplus::GraphicsPath polygonPath;
	//绘制遮罩三角形
	Gdiplus::Point points[] = { 
		{rect.GetRight(), rect.GetBottom() - 40}, 
		{rect.GetRight() - 40, rect.GetBottom()}, 
		{rect.GetRight(), rect.GetBottom()},
		{rect.GetRight(), rect.GetBottom() - 40} 
	};
	Gdiplus::Color blushColor(GetRValue(color), GetGValue(color), GetBValue(color));
	Gdiplus::SolidBrush brush(blushColor);
	graphics.FillPolygon(&brush, points, 4);

	//绘制照相机图片
	Gdiplus::Image captureImage(capture_path.c_str(), false);
	UINT imageWidth = captureImage.GetWidth();
	UINT imageHeight = captureImage.GetHeight();

	int ix = rect.GetRight() - 5 - imageWidth;
	int iy = rect.GetBottom() - 5 - imageHeight; 
	GdiPlusUtil::DrawImage(graphics, &captureImage, ix, iy);
}
