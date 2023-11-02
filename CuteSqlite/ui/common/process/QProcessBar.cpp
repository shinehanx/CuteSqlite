#include "stdafx.h"
#include <string>
#include "QProcessBar.h"
#include "core/common/Lang.h"

void QProcessBar::run(int percent)
{
	this->percent = percent;
	if (percent <= 5) {
		this->err.clear();
	}
	
	Invalidate(true);
}

void QProcessBar::error(const std::wstring & err)
{
	this->err = err;
	Invalidate(true);
}

/**
 * ��ɫ��������ɫ�ͽ�����ɫ.
 * 
 * @param bkgColor ������ɫ
 * @param processColor ������ɫ.
 */
void QProcessBar::setColors(COLORREF bkgColor, COLORREF processColor)
{
	this->bkgColor = bkgColor;
	this->processColor = processColor;

	if (bkgBrush) DeleteObject(bkgBrush);
	if (processBrush) DeleteObject(processBrush);

	bkgBrush = ::CreateSolidBrush(bkgColor);
	processBrush = ::CreateSolidBrush(processColor);
}

LRESULT QProcessBar::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bkgBrush = ::CreateSolidBrush(bkgColor);
	processBrush = ::CreateSolidBrush(processColor);
	errorBrush = ::CreateSolidBrush(errorColor);
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

LRESULT QProcessBar::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (bkgBrush) ::DeleteObject(bkgBrush);
	if (processBrush) ::DeleteObject(processBrush);
	if (errorBrush) ::DeleteObject(errorBrush);
	if (textFont) ::DeleteObject(textFont);
	if (!textPen.IsNull()) textPen.DeleteObject();
	return 0;
}

LRESULT QProcessBar::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CPaintDC pdc(m_hWnd);
	CMemoryDC mdc(pdc, pdc.m_ps.rcPaint);

	// ������
	CRect clientRect(pdc.m_ps.rcPaint);
	mdc.FillRect(clientRect, bkgBrush);

	// ������
	double perPixel = clientRect.Width() * 1.0 / 100.0;
	int pixel = int(round(perPixel * percent));
	int x = clientRect.left, y = clientRect.top, w = pixel, h = clientRect.Height();
	CRect rect(x, y, x + w, y + h);
	if (err.empty()) {
		mdc.FillRect(rect, processBrush);
	} else {
		mdc.FillRect(rect, errorBrush);
	}
	

	// ������
	if (percent == 0) {
		return 0;
	}

	std::wstring text = std::to_wstring(percent);
	text.append(L"%");

	UINT uFormat = DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS;
	HFONT oldFont = mdc.SelectFont(textFont);
	HPEN oldPen = mdc.SelectPen(textPen);
	x = (clientRect.Width() - 80) / 2, y = rect.top + 2, w = 80, h = 20;
	CRect textRect(x, y, x + w, y + h);
	COLORREF oldTextColor = mdc.SetTextColor(textColor);
	COLORREF textBkColor = err.empty() ? processColor : errorColor;
	textBkColor = percent < 60 ? bkgColor : textBkColor;
	COLORREF oldTextBkgColor = mdc.SetBkColor(textBkColor);
	mdc.DrawText(text.c_str(), static_cast<int>(text.size()), textRect, uFormat);
	mdc.SetTextColor(oldTextColor);
	mdc.SetBkColor(oldTextBkgColor);
	mdc.SelectPen(oldPen);
	mdc.SelectFont(oldFont);
	return 0;
}

