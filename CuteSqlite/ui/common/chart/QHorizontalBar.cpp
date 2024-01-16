#include "stdafx.h"
#include <string>
#include "QHorizontalBar.h"
#include "core/common/Lang.h"

QHorizontalBar::QHorizontalBar(double val, double maxVal)
{
	if (maxVal == 0 || val > maxVal) {
		percent = 0;
		return;
	}
	if (val == 1) {
		percent = 1;
		return;
	}
	this->percent = int(round(val * 100.0 / maxVal));
}

QHorizontalBar::~QHorizontalBar()
{
	m_hWnd = nullptr;
}

void QHorizontalBar::draw(double val, double maxVal)
{
	if (maxVal == 0 || val > maxVal) {
		percent = 0;
		return;
	}
	if (val == 1) {
		percent = 1;
		return;
	}
	this->percent = int(round(val * 100.0 / maxVal));	
	Invalidate(true);
}

void QHorizontalBar::error(const std::wstring & err)
{
	this->err = err;
	Invalidate(true);
}

void QHorizontalBar::reset()
{
	this->percent = 0;
	this->err.clear();
	Invalidate(true);
}

/**
 * ��ɫ��������ɫ�ͽ�����ɫ.
 * 
 * @param bkgColor ������ɫ
 * @param processColor ������ɫ.
 */
void QHorizontalBar::setColors(COLORREF bkgColor, COLORREF processColor)
{
	this->bkgColor = bkgColor;
	this->processColor = processColor;

	if (!bkgBrush.IsNull()) bkgBrush.DeleteObject();
	if (!processBrush.IsNull()) processBrush.DeleteObject();

	bkgBrush.CreateSolidBrush(bkgColor);
	processBrush.CreateSolidBrush(processColor);
}

LRESULT QHorizontalBar::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bkgBrush.CreateSolidBrush(bkgColor);
	processBrush.CreateSolidBrush(processColor);
	errorBrush.CreateSolidBrush(errorColor);
	textFont = FT(L"process-text-size"); 
	textPen.CreatePen(PS_SOLID, 1, textColor);

	CRect clientRect;
	GetClientRect(clientRect);
	// ��Բ��
	// Բ�Ǵ���
	HRGN hRgn = ::CreateRoundRectRgn(0, 0, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, 20, 20);
	::SetWindowRgn(m_hWnd, hRgn, TRUE);

	return 0;
}

LRESULT QHorizontalBar::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!bkgBrush.IsNull()) bkgBrush.DeleteObject();
	if (!processBrush.IsNull()) processBrush.DeleteObject();
	if (!errorBrush.IsNull()) errorBrush.DeleteObject();
	if (textFont) ::DeleteObject(textFont);
	if (!textPen.IsNull()) textPen.DeleteObject();
	return 0;
}

LRESULT QHorizontalBar::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CPaintDC pdc(m_hWnd);
	CMemoryDC mdc(pdc, pdc.m_ps.rcPaint);

	// ������
	CRect clientRect(pdc.m_ps.rcPaint);
	mdc.FillRect(clientRect, bkgBrush.m_hBrush);

	// ������
	double perPixel = clientRect.Width() * 1.0 / 100.0;
	int pixel = int(round(perPixel * percent));
	int x = clientRect.left, y = clientRect.top, w = pixel, h = clientRect.Height();
	CRect rect(x, y, x + w, y + h);
	if (err.empty()) {
		mdc.FillRect(rect, processBrush.m_hBrush);
	} else {
		mdc.FillRect(rect, errorBrush.m_hBrush);
	}
	

	// ������
	if (percent == 0) {
		return 0;
	}

	std::wstring text = std::to_wstring(val);
	text.append(L"%");

	UINT uFormat = DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS;
	HFONT oldFont = mdc.SelectFont(textFont);
	HPEN oldPen = mdc.SelectPen(textPen);
	x = 5, y = rect.top + 2, w = 80, h = 20;
	CRect textRect(x, y, x + w, y + h);
	COLORREF oldTextColor = mdc.SetTextColor(textColor);
	COLORREF textBkColor =  processColor;
	textBkColor = bkgColor ;
	COLORREF oldTextBkgColor = mdc.SetBkColor(textBkColor);
	mdc.DrawText(text.c_str(), static_cast<int>(text.size()), textRect, uFormat);
	mdc.SetTextColor(oldTextColor);
	mdc.SetBkColor(oldTextBkgColor);
	mdc.SelectPen(oldPen);
	mdc.SelectFont(oldFont);
	return 0;
}

