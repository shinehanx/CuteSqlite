#include "stdafx.h"
#include "HomePanel.h"
#include "utils/GdiUtil.h"


void HomePanel::createOrShowUI()
{

}

LRESULT HomePanel::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	
	return 0;
}

LRESULT HomePanel::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
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
	CRect rect;
	GetClientRect(&rect);
	
	//HBRUSH brush = AtlGetStockBrush(GRAY_BRUSH);
	HBRUSH brush = ::CreateSolidBrush(RGB(0x2c, 0x2c, 0x2c));
	mdc.FillRect(&pdc.m_ps.rcPaint, brush);
	::DeleteObject(brush);

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
