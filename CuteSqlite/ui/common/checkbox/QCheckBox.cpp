#include "stdafx.h"
#include "QCheckBox.h"
#include "core/common/Lang.h"
#include "utils/FontUtil.h"


void QCheckBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CRect clientRect(lpDrawItemStruct->rcItem);
	CRect leftRect = getLeftRect(clientRect);
	CRect rightRect = getRightRect(clientRect);

	CDC dc(lpDrawItemStruct->hDC);
	CMemoryDC mdc(dc, clientRect);
	mdc.FillRect(clientRect, bkgBrush);
	

	// 画左边的方框
	drawLeftArea(mdc, leftRect);
	drawRightText(mdc, rightRect);
}


void QCheckBox::setTextColor(COLORREF color)
{
	textColor = color;
	if (textBrush)  ::DeleteObject(textBrush);
	textBrush = ::CreateSolidBrush(textColor);
	
	if (!textPen.IsNull())  textPen.DeleteObject(); 
	textPen.CreatePen(PS_SOLID, 2, textColor);
	// 刷新窗口
	if (IsWindow()) Invalidate(true);
}


void QCheckBox::setBkgColor(COLORREF color)
{
	bkgColor = color;
	if (bkgBrush)  ::DeleteObject(bkgBrush);
	bkgBrush = ::CreateSolidBrush(bkgColor);

	// 刷新窗口
	if (IsWindow()) Invalidate(true);
}


void QCheckBox::setFontSize(int fontSize)
{
	this->fontSize = fontSize;
	if (textFont) ::DeleteObject(textFont);
	std::wstring fontName = Lang::fontName();
	textFont = FontUtil::getFont(fontSize, false, fontName.c_str());

	// 刷新窗口
	if (IsWindow()) Invalidate(true);

}

/**
 * 选中状态.
 * 
 * @return 
 */
int QCheckBox::getIsCheck()
{
	return isCheck;
}

void QCheckBox::setIsCheck(int isCheck)
{
	this->isCheck = isCheck;
	// 刷新窗口
	if (IsWindow()) Invalidate(true);
}

CRect QCheckBox::getLeftRect(CRect &clientRect)
{
	int x = 4, y = (clientRect.Height() - fontSize) / 2, w = fontSize, h = fontSize;
	return {x, y, x + w, y + h};
}



CRect QCheckBox::getRightRect(CRect &clientRect)
{
	CRect leftRect = getLeftRect(clientRect);
	int x = leftRect.right + 5, y = leftRect.top,  w = clientRect.Width(), h = clientRect.Height();
	return { x, y, x + w, y + h };
}

void QCheckBox::drawLeftArea(CMemoryDC & mdc, CRect &rect)
{
	// 画方框
	rect.DeflateRect(1, 1, 1, 1); //减一像素
	mdc.FillRect(rect, rectBrush);

	rect.InflateRect(1, 1, 1, 1);//增一像素
	HPEN oldPen = mdc.SelectPen(tickPen); // 黑色的外框
	mdc.FrameRect(rect, textBrush); // 黑色的外框
	
	int margin = 2;
	if (rect.Width() < 18) {
		margin = 2;
	}

	// 画打勾
	if (isCheck) {
		int x = rect.left + margin , y = rect.top + rect.Height() / 2;
		mdc.MoveTo(rect.left + margin, rect.top + rect.Height() / 2);
		mdc.LineTo(rect.left + margin + 5, rect.bottom - margin);
		mdc.LineTo(rect.right - margin, rect.top + margin);
	}
	mdc.SelectPen(oldPen);
	
}


void QCheckBox::drawRightText(CMemoryDC & mdc, CRect &rect)
{
	CString text;
	GetWindowText(text);

	
	HFONT oldFont = mdc.SelectFont(textFont);
	int oldMode = mdc.SetBkMode(TRANSPARENT);
	COLORREF oldColor = mdc.SetTextColor(textColor);
	mdc.DrawText(text, text.GetLength(), rect, DT_LEFT | DT_VCENTER);
	mdc.SetTextColor(oldColor);
	mdc.SetBkMode(oldMode);
	mdc.SelectFont(oldFont);
	
}

LRESULT QCheckBox::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!textBrush) textBrush = ::CreateSolidBrush(textColor);
	if (!bkgBrush) bkgBrush = ::CreateSolidBrush(bkgColor);
	if (!rectBrush) rectBrush = ::CreateSolidBrush(rectColor);

	if (tickPen.IsNull()) tickPen.CreatePen(PS_SOLID, 2, tickColor);
	if (textPen.IsNull()) textPen.CreatePen(PS_SOLID, 2, textColor);

	std::wstring fontName = Lang::fontName();
	if (!textFont) textFont = FontUtil::getFont(fontSize, false, fontName.c_str());
	return 0;
}

LRESULT QCheckBox::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (textBrush) ::DeleteObject(textBrush);
	if (bkgBrush) ::DeleteObject(bkgBrush);
	if (rectBrush) ::DeleteObject(rectBrush);
	if (!tickPen.IsNull()) tickPen.DeleteObject();
	if (!textPen.IsNull()) textPen.DeleteObject();
	return 0;
}

BOOL QCheckBox::OnEraseBkgnd(CDCHandle dc)
{
	return TRUE;
}
