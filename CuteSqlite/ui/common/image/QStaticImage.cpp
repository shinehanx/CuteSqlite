#include "stdafx.h"
#include <gdiplus.h>
#include "QStaticImage.h"
#include "utils/Log.h"


BOOL QStaticImage::PreTranslateMessage(MSG* pMsg)
{
	if (tooltipCtrl.IsWindow()) {
		tooltipCtrl.RelayEvent(pMsg);
	}
	return FALSE;
}

QStaticImage::QStaticImage() : QImage()
{

}


QStaticImage::QStaticImage(HBITMAP bitmap) :  QImage(bitmap)
{

}


QStaticImage::QStaticImage(Gdiplus::Bitmap * bitmap) :  QImage(bitmap)
{

}


QStaticImage::QStaticImage(const wchar_t * path, DWORD compress, bool isCopy):  QImage(path, compress, isCopy)
{

}


QStaticImage::QStaticImage(CBitmap &bitmap) : QImage(bitmap)
{

}

/**
 * ���ý����Сͼ��.(���½�)
 * 
 * @param path Сͼ��·��
 * @param compress ѹ��
 */
void QStaticImage::setCornerIcon(const wchar_t * path, DWORD compress/*=BI_RGB*/)
{
	if (path == NULL || sizeof(path) == 0 || _waccess(path, 0) != 0) {
		Q_ERROR(L"load error, image path is not exists.path:{}", path);
		return;
	}

	if (compress == BI_JPEG || compress == BI_PNG) {
		Gdiplus::Bitmap bmp(path); //GDI+��ȡJPG/PNGͼ��
		// GDI��ͼƬת��HBITMAP
		Gdiplus::Color color(Gdiplus::Color::Transparent);
		HBITMAP hbitmap = nullptr;
		if (bmp.GetHBITMAP(color, &hbitmap) != Gdiplus::Ok) {
			Q_ERROR(L"load error, Gdiplus::Bitmap translate to HBIMAP error.path:{}", path);
			ATLASSERT(false);
		}
		cornerIcon.Attach(hbitmap);
	}else {
		HBITMAP hbitmap =(HBITMAP)::LoadImageW(ModuleHelper::GetModuleInstance(), path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

		if (hbitmap == nullptr) {
			Q_ERROR(L"load error, CBitmap load error.path:{}", path);
			ATLASSERT(false);
		}
		cornerIcon.Attach(hbitmap);
	}
}

/**
 * ���Сͼ��λ�ã������ʱ�������Ϊ�ж�.
 * 
 * @return 
 */
CRect QStaticImage::getCornerIconRect()
{
	return cornerIconRect;
}

/**
 * ��ʾ����.
 * 
 * @param path
 * @param font
 */
void QStaticImage::setTip(const wchar_t * text, HFONT font)
{
	tip = text;
	tipFont = font;
}

void QStaticImage::setToolTip(const wchar_t * text)
{
	toolTipText = text;

	if(toolTipText.empty()) {
		if(tooltipCtrl.IsWindow())
			tooltipCtrl.Activate(FALSE);
		return ;
	}

	if(tooltipCtrl.IsWindow()) {
		tooltipCtrl.Pop();
		tooltipCtrl.Activate(TRUE);
		tooltipCtrl.AddTool(this->m_hWnd, toolTipText.c_str());
	}
}

/**
 * ��׽�����Ϣ�ǲο�CBitmapButton��˼·����tooltip.
 * 
 * @param uMsg
 * @param wParam
 * @param lParam
 * @param bHandled
 * @return 
 */
LRESULT QStaticImage::OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	MSG msg = { this->m_hWnd, uMsg, wParam, lParam };
	if(tooltipCtrl.IsWindow())
		tooltipCtrl.RelayEvent(&msg);
	bHandled = FALSE;
	return 1;
}

QStaticImage::~QStaticImage()
{

}

LRESULT QStaticImage::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	QImage::OnCreate(uMsg, wParam, lParam, bHandled);
	bkgIconBrush.CreateSolidBrush(RGB(177, 177, 177));
	bkgIconPen = ::CreatePen(PS_SOLID, 1, RGB(177, 177, 177));

	tooltipCtrl.Create(this->m_hWnd);
	ATLASSERT(tooltipCtrl.IsWindow());
	if(tooltipCtrl.IsWindow() && (toolTipText.empty() != false)) {
		tooltipCtrl.Activate(TRUE);
		tooltipCtrl.AddTool(this->m_hWnd, toolTipText.c_str());
	}
	return true;
}

LRESULT QStaticImage::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	QImage::OnDestroy(uMsg, wParam, lParam, bHandled);

	if (!bkgIconBrush.IsNull()) bkgIconBrush.DeleteObject();
	if (bkgIconPen != nullptr) DeleteObject(bkgIconPen);
	if (!cornerIcon.IsNull()) cornerIcon.DeleteObject();
	if (tipFont) ::DeleteObject(tipFont);

	if(tooltipCtrl.IsWindow()) {
		tooltipCtrl.DestroyWindow();
		tooltipCtrl.m_hWnd = NULL;
	}
	return 0;
}

LRESULT QStaticImage::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	QImage::OnSize(uMsg, wParam, lParam, bHandled);
	return 0;
}

LRESULT QStaticImage::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	QImage::OnPaint(uMsg, wParam, lParam, bHandled);
	return 0;
}

/**
 * �����.
 * 
 * @param uMsg
 * @param wParam
 * @param lParam
 * @param bHandled
 * @return 
 */
LRESULT QStaticImage::OnMousePressed(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt;
	pt.x = GET_X_LPARAM(lParam);
	pt.y = GET_Y_LPARAM(lParam);

	UINT nID = ::GetWindowLong(m_hWnd, GWL_ID); //��ñ�����ID

	// ���QStaticImageͼƬ���½�Сͼ�꣨���յȣ����������ڷ��ʹ���Ϣ wParam���ϱ�����Ĵ���ID, lParam�Ǵ��ھ��
	if (!cornerIcon.IsNull() && cornerIconRect.PtInRect(pt)) {
		Q_DEBUG("call QStaticImage::OnMousePressed, click corner icon");
		UINT nID = ::GetWindowLong(m_hWnd, GWL_ID); //��ñ�����ID
		::PostMessage(GetParent().m_hWnd, Config::MSG_QIMAGE_CONERICON_CLICK_ID, (WPARAM)nID, (LPARAM)m_hWnd);
	}else { // ����֮��
		::PostMessage(GetParent().m_hWnd, Config::MSG_QIMAGE_CLICK_ID, (WPARAM)nID, (LPARAM)m_hWnd);
	}

	return 0;
}

/**
 * ���ǻ����paintItem�� �����OnPain��Ϣ�����������������paintItem�ĺ�����չ��������.
 * 
 * @param dc ����DC
 * @param paintRect ��������ķ���
 */
void QStaticImage::paintItem(CDC & dc, CRect & rect)
{
	// �����½ǵ�Сͼ��
	if (!cornerIcon.IsNull()) {
		// �������ε���
		CPoint points[4] = {
			{rect.right, rect.bottom - 55}, // 
			{rect.right - 55, rect.bottom}, 
			{rect.right, rect.bottom},
			{rect.right, rect.bottom - 55} 
		};
		cornerIconRect = {rect.right - 55, rect.bottom - 55, rect.right, rect.bottom};
		HBRUSH oldBrush = dc.SelectBrush(bkgIconBrush.m_hBrush);
		HPEN oldPen = dc.SelectPen(bkgIconPen);
		dc.Polygon(points, 4); //��������
		dc.SelectBrush(oldBrush);
		dc.SelectPen(oldPen);

		// ��Сͼ��
		CSize size;
		cornerIcon.GetSize(size);
		int ix = rect.right - 3 - size.cx;
		int iy = rect.bottom - 3 - size.cy; 
		CDC mdc;
		mdc.CreateCompatibleDC(dc);
		mdc.SelectBitmap(cornerIcon);

		dc.BitBlt(ix, iy, size.cx, size.cy, mdc, 0, 0, SRCCOPY);
		mdc.DeleteDC();
		mdc.Detach();
	}
	

	if (!tip.empty()) {
		// ������
		HFONT oldFont = dc.SelectFont(tipFont);
		UINT uFormat = DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS;
		CSize lpSize; // GetTextExtentPoint32 ��������ָ���ı��ַ����Ŀ�Ⱥ͸߶ȡ�
		::GetTextExtentPoint32(dc, tip.c_str(), static_cast<int>(tip.size()), &lpSize);
		CRect tipRect(5, 5, 5 + lpSize.cx + 5, 5 + lpSize.cy);
		COLORREF oldTextColor = dc.SetTextColor(RGB(255, 255, 255));
		COLORREF oldTextBkgColor = dc.SetBkColor(RGB(177, 177, 177));
		dc.DrawText(tip.c_str(), static_cast<int>(tip.size()), tipRect, uFormat);
		dc.SetTextColor(oldTextColor);
		dc.SetBkColor(oldTextBkgColor);
		dc.SelectFont(oldFont);
	}
}
