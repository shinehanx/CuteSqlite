#include "stdafx.h"
#include "HomePanel.h"
#include "utils/GdiUtil.h"
#include "core/common/Lang.h"

void HomePanel::createOrShowUI()
{

}

LRESULT HomePanel::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	bkgBrush.CreateSolidBrush(bkgColor);
	titleFont = FT(L"welcome-text-size");
	homeFont = FT(L"home-text-size");
	return 0;
}

LRESULT HomePanel::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (bkgBrush.IsNull()) bkgBrush.DeleteObject();
	if (titleFont) ::DeleteObject(titleFont);
	if (homeFont) ::DeleteObject(homeFont);
	return 0;
}

LRESULT HomePanel::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return 0;
}

LRESULT HomePanel::OnShowWindow(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (!wParam) {
		return 0;
	}
	std::wstring binDir = ResourceUtil::getProductBinDir();
	std::wstring path = settingService->getSysInit(L"home-bkg-image");
	std::wstring newPath = binDir + path;
	if (!path.empty() && newPath != bkgImagePath) {
		bkgImagePath = binDir + path;
		Gdiplus::Bitmap bitmap(bkgImagePath.c_str(), false);
		Gdiplus::Color color(Gdiplus::Color::Transparent);
		HBITMAP hBitmap;
		bitmap.GetHBITMAP(color, &hBitmap);
		bkgBitmap.Attach(hBitmap);
		::DeleteObject(&bitmap);
	}else {
		bkgImagePath = L"";
	}
	return 0;
}

LRESULT HomePanel::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC pdc(m_hWnd);
	CMemoryDC mdc(pdc, pdc.m_ps.rcPaint);
	CRect clientRect;
	GetClientRect(clientRect);	
	mdc.FillRect(&pdc.m_ps.rcPaint, bkgBrush.m_hBrush);

	
	int x = clientRect.left , y = 70, w = clientRect.Width(), h = 70;
	CRect rect(x, y, x + w, y + h);
	int oldMode = mdc.SetBkMode(TRANSPARENT);
	HFONT oldFont = mdc.SelectFont(titleFont);	
	COLORREF oldColor = mdc.SetTextColor(titleColor);
	std::wstring welcome = S(L"welcome-text");
	mdc.DrawText(welcome.c_str(), static_cast<int>(welcome.size()), rect, DT_CENTER | DT_VCENTER);
	mdc.SetTextColor(oldColor);
	mdc.SelectFont(oldFont);

	rect.OffsetRect(0, h + 50);
	rect.bottom = rect.top + 40;
	oldFont = mdc.SelectFont(homeFont);
	oldColor = mdc.SetTextColor(homeColor);
	std::wstring homeText = S(L"home-text1");
	mdc.DrawText(homeText.c_str(), static_cast<int>(homeText.size()), rect, DT_CENTER | DT_VCENTER);

	rect.OffsetRect(0, rect.Height());
	homeText = S(L"home-text2");
	mdc.DrawText(homeText.c_str(), static_cast<int>(homeText.size()), rect, DT_CENTER | DT_VCENTER);

	rect.OffsetRect(0, rect.Height());
	homeText = S(L"home-text3");
	mdc.DrawText(homeText.c_str(), static_cast<int>(homeText.size()), rect, DT_CENTER | DT_VCENTER);

	rect.OffsetRect(0, rect.Height());
	homeText = S(L"home-text4");
	mdc.DrawText(homeText.c_str(), static_cast<int>(homeText.size()), rect, DT_CENTER | DT_VCENTER);

	mdc.SetTextColor(oldColor);
	mdc.SelectFont(oldFont);
	mdc.SetBkMode(oldMode);
	if (!bkgBitmap.IsNull()) {
		GdiUtil::paintCenterFixStretchBlt(mdc, bkgBitmap);
	}
	
	return 0;
}

LRESULT HomePanel::OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (!bkgBitmap.IsNull()) {
		bkgBitmap.DeleteObject();
	}

	return TRUE;
}

BOOL HomePanel::OnEraseBkgnd(CDCHandle dc)
{
	return true;
}
