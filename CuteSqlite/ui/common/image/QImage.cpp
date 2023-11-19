#include "stdafx.h"
// #include <gdiplusgpstubs.h>
// #include <gdiplustypes.h>
// #include <gdiplusenums.h>
// #include <gdipluspixelformats.h>
// #include <gdiplusflat.h>
#include <atlgdi.h>
#include <atlmisc.h>
#include "QImage.h"
#include "utils/Log.h"
#include "utils/GdiPlusUtil.h"
#include "utils/GdiUtil.h"

QImage::QImage()
{

}

QImage::QImage(Gdiplus::Bitmap * theBitmap) 
{
	load(theBitmap);
}

QImage::QImage(HBITMAP _hBitmap) : bitmap(_hBitmap)
{
	
}

QImage::QImage(const wchar_t * path, DWORD compress,bool isCopy) 
{
	load(path, compress,isCopy);
}


QImage::QImage(CBitmap & _hBitmap) :bitmap(HBITMAP(_hBitmap))
{
	
}


QImage::QImage(CBitmap * _bitmap)
{
	bitmap = * _bitmap;
}

QImage::~QImage()
{
	
}

void QImage::load(HBITMAP _hBitmap)
{
	bitmap.Attach(_hBitmap);
}


void QImage::load(Gdiplus::Bitmap * _bitmap)
{
	if (_bitmap == nullptr) {
		Q_ERROR(L"load bitmap is null...");
		return ;
	}

	if (bitmap.IsNull() == false) {
		bitmap.DeleteObject();
		bitmap.Detach();
	}

	// GDI��ͼƬת��HBITMAP
	Gdiplus::Color color(Gdiplus::Color::Transparent);
	HBITMAP hbitmap = nullptr;
	if (_bitmap->GetHBITMAP(color, &hbitmap) != Gdiplus::Ok) {
		ATLASSERT(false);
	}
	bitmap.Attach(hbitmap);
}


void QImage::load(const wchar_t * path, DWORD compress, bool isCopy)
{
	if (bitmap.IsNull() == false) {
		bitmap.DeleteObject();
		bitmap.Detach();
	}
	
	if (path == NULL || sizeof(path) == 0 || _waccess(path, 0) != 0) {
		Q_ERROR(L"load error, image path is not exists.path:{}", path);
		return;
	}

	if (compress == BI_JPEG || compress == BI_PNG) {		
		HBITMAP hbitmap = loadBitmapWithGdiPlus(path);
		if (hbitmap == nullptr) {
			Q_ERROR(L"load error, image path can't be loaded whith gdi+ .path:{}", path);
			return ;
		}
		bitmap.Attach(hbitmap);
		return;
	}else {
		HBITMAP hbitmap = loadBitmapWithGdi(path, isCopy);
		if (hbitmap == nullptr) {
			Q_ERROR(L"load error, image path can't be loaded whith gdi .path:{}", path);
			return ;
		}
		bitmap.Attach(hbitmap);
		return;
	}
}


void QImage::load(CBitmap & _hBitmap)
{
	bitmap.Attach(HBITMAP(_hBitmap));
}

void QImage::load(CBitmap * _hBitmap)
{
	bitmap.Attach(HBITMAP(*_hBitmap));
}


HBITMAP QImage::loadBitmapWithGdi(const wchar_t * path, bool isCopy)
{
	HBITMAP hbitmap = (HBITMAP)::LoadImageW(ModuleHelper::GetModuleInstance(), path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hbitmap != nullptr && !isCopy) { // �Ƿ񿽱�
		return hbitmap;
	}
	if (hbitmap == nullptr) {
		hbitmap = loadBitmapWithGdiPlus(path);
	}
	
	HBITMAP hNewBitmap = GdiUtil::copyBitmap(hbitmap);
	if (hbitmap == nullptr || hNewBitmap == nullptr) {
		return nullptr;
	}
	
	::DeleteObject(hbitmap);
	return hNewBitmap;
}

/**
 * ʹ��GID+���dͼƬ.
 * 
 * @param path
 * @return 
 */
HBITMAP QImage::loadBitmapWithGdiPlus(const wchar_t * path)
{
	CAtlFile file;
	if (S_OK != file.Create(path, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING)) {
		return nullptr;
	}
	ULONGLONG ulBmpSize;
	if (S_OK != file.GetSize(ulBmpSize)) {
		return nullptr;
	}
	HGLOBAL hMemBmp = GlobalAlloc(GMEM_FIXED, ulBmpSize);
	IStream * stream = NULL;
	if (S_OK != CreateStreamOnHGlobal(hMemBmp, FALSE, &stream)) {
		return nullptr;
	}

	BYTE* bytes = (BYTE *)GlobalLock(hMemBmp);
	file.Seek(0, FILE_BEGIN);
	file.Read(bytes, static_cast<DWORD>(ulBmpSize));

	Gdiplus::Bitmap * bmp = Gdiplus::Bitmap::FromStream(stream); //GDI+��ȡJPG/PNGͼ��

	//Gdiplus::Bitmap * bmp = Gdiplus::Bitmap::FromFile(path);

	// GDI��ͼƬת��HBITMAP
	Gdiplus::Color color(Gdiplus::Color::Transparent);
	HBITMAP hbitmap = nullptr;
	if (bmp->GetHBITMAP(color, &hbitmap) != Gdiplus::Ok) {
		return nullptr;
	}
	
	::DeleteObject(bmp);
	stream->Release();
	::GlobalFree(hMemBmp);
	stream = nullptr;
	return hbitmap;
}

void QImage::setDisplayMode(DisplayMode _displayMode)
{
	this->displayMode = _displayMode;
}


QImage::DisplayMode QImage::getDisplayMode()
{
	return displayMode;
}


void QImage::setBkgColor(COLORREF color)
{
	this->bkgColor = color;
	if (!bkgBrush.IsNull()) bkgBrush.DeleteObject(); // ��ж�أ�����ʾ
	bkgBrush.CreateSolidBrush(bkgColor);
}

/**
 * �߿򣬿�Ⱥ���ɫ.
 * 
 * @param size
 * @param color
 */
void QImage::setBorder(int size, COLORREF color)
{
	borderSize = size;
	borderColor = color;
}

LRESULT QImage::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (bkgBrush.IsNull())bkgBrush.CreateSolidBrush(bkgColor);
	return true;
}

LRESULT QImage::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (!bitmap.IsNull()) {
		bitmap.DeleteObject();
		bitmap.Detach();
	}
	if (!bkgBrush.IsNull()) bkgBrush.DeleteObject();
	
	return 0;
}

LRESULT QImage::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return true;
}

LRESULT QImage::OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC dc(m_hWnd);
	CRect clientRect;
	GetClientRect(clientRect);
	CMemoryDC mdc(dc, clientRect); //����˫�������

	// ������ˢ
	HBRUSH oldBrush = mdc.SelectBrush(bkgBrush.m_hBrush);
	mdc.FillRect(clientRect, bkgBrush.m_hBrush);
	mdc.SelectBrush(oldBrush);

	if (bitmap.IsNull()) {
		// ������չ����
		paintItem(mdc, clientRect);
		return 0;
	}		
	
	switch (displayMode)
	{
	case kImageCenterFixTargetRect: //�������ĵ�
		paintCenterFixStretchBlt(mdc, bitmap);
		break;
	case kImageWidthFixTargetRect: // ��ȵȱ�����
		paintWidthFixStretchBlt(mdc, bitmap);
		break;
	case kImageHeightFixTargetRect: // �߶ȵȱ�����
		paintHeightFixStretchBlt(mdc, bitmap);
		break;
	case kImageStretchFitTargetRect:
	default:
		paintStretchBlt(mdc, bitmap); //ѹ����ʾ
		break;
	}
	// ������չ�����ݣ�����̳�QImage����ʵ�ִ˷���,���mdc��չ��������
	paintItem(mdc, clientRect);

	// ���߿�
	if (borderSize > 0) {
		CPen borderPen;
		borderPen.CreatePen(PS_SOLID, borderSize, borderColor);
		HBRUSH borderBrush = (HBRUSH)::CreateSolidBrush(borderColor);
		HPEN oldPen = mdc.SelectPen(HPEN(borderPen));
		oldBrush = mdc.SelectBrush((HBRUSH)::GetStockObject(HOLLOW_BRUSH));
		mdc.Rectangle(&(clientRect));
		mdc.SelectBrush(oldBrush);
		mdc.SelectPen(oldPen);
		borderPen.DeleteObject();
		::DeleteObject(borderBrush);
	}
	return 0;
}

/**
 *������չ�����ݣ�����̳�QImage����ʵ�ִ˷������ǻ����paintItem, ���mdc��չ��������.
 * 
 * @param dc 
 * @param paintRect
 */
void QImage::paintItem(CDC &dc, CRect & paintRect)
{
	 // ������ʲô������
}

void QImage::paintBitblt(CMemoryDC &mdc, CBitmap & bitmap)
{
	CSize size;
	bitmap.GetBitmapDimension(&size);

	CDC memoryDc;
	memoryDc.CreateCompatibleDC(mdc);
	memoryDc.SelectBitmap(bitmap);
	
	// ��K
	mdc.BitBlt(0, 0, mdc.m_rcPaint.right, mdc.m_rcPaint.bottom,
		memoryDc, 0, 0, SRCCOPY);
}

void QImage::paintStretchBlt(CMemoryDC &mdc, CBitmap & bitmap)
{
	CSize size;
	if (bitmap.GetSize(size) == false) {
		Q_ERROR("paintStretchBlt has error!");
		return;
	}
	
	CDC memoryDc; // �ڴ�DC
	memoryDc.CreateCompatibleDC(mdc);
	memoryDc.SelectBitmap(bitmap);
	
	// 
	mdc.SetStretchBltMode(HALFTONE);
	CPoint pointZero(0,0);
	// ʹ��StretchBlt�ü�
	mdc.StretchBlt(0, 0, mdc.m_rcPaint.right, mdc.m_rcPaint.bottom,
		memoryDc, 0, 0, size.cx, size.cy, SRCCOPY);
	SetBrushOrgEx(mdc, 0,0,NULL);

	int cw = mdc.m_rcPaint.right - size.cx; // ��Ȳ�ֵ = Ŀ���� - ͼƬ��� 
	int ch = mdc.m_rcPaint.bottom - size.cy; // �߶Ȳ�ֵ = Ŀ��߶� - ͼƬ�߶� 
	// ����ϵ��
	scale_w = mdc.m_rcPaint.right * 1.0 / size.cx;
	scale_h = mdc.m_rcPaint.bottom * 1.0 / size.cy;

	// ����ƽ��ϵ��
	translate.cx = 0 ; // ��Ȳ�ֵ * ����ϵ�� / 2
	translate.cy = 0 ; // �߶Ȳ�ֵ * ����ϵ�� / 2
	imageRect = {0, 0, mdc.m_rcPaint.right, mdc.m_rcPaint.bottom}; // ͼƬλ��

	memoryDc.DeleteDC();
	memoryDc.Detach();
}

/**
 * ����ȱ������ţ�1.���ͼƬ��ȴ���Ŀ�꣬�򰴱�����С��ȣ� 2.���ͼƬ���С��Ŀ�꣬�򰴱����Ŵ���. 3.�߶Ȱ���ȵı�������
 * 
 * @param mdc
 * @param bitmap
 */
void QImage::paintWidthFixStretchBlt(CMemoryDC &mdc, CBitmap & bitmap)
{
	CSize size;
	if (bitmap.GetSize(size) == false) {
		Q_ERROR("paintStretchBlt has error!");
		return;
	}

	int cw = mdc.m_rcPaint.right - size.cx; // ��Ȳ�ֵ = Ŀ���� - ͼƬ��� 
	int ch = mdc.m_rcPaint.bottom - size.cy; // �߶Ȳ�ֵ = Ŀ��߶� - ͼƬ�߶� 

	int dst_w = mdc.m_rcPaint.right - mdc.m_rcPaint.left;
	int dst_h = int(round(dst_w * size.cy * 1.0 / size.cx));
	int dst_x = 0;
	int dst_y = mdc.m_rcPaint.top + (mdc.m_rcPaint.bottom - mdc.m_rcPaint.top - dst_h) / 2;
	
	CDC memoryDc; // �ڴ�DC
	memoryDc.CreateCompatibleDC(mdc);
	memoryDc.SelectBitmap(bitmap);
	
	// ʹ��HALFTONE��ģʽ������Ҫ����SetBrushOrgEx
	mdc.SetStretchBltMode(HALFTONE);
	CPoint pointZero(0,0);
	SetBrushOrgEx(mdc, 0,0,NULL);

	// ʹ��StretchBlt�ü�
	mdc.StretchBlt(dst_x, dst_y, dst_w, dst_h,
		memoryDc, 0, 0, size.cx, size.cy, SRCCOPY);


	// ����ϵ��
	scale_w = dst_w * 1.0 / size.cx;
	scale_h = dst_w * 1.0 / size.cx;

	// ����ƽ��ϵ��
	translate.cx = 0 ; // �����ţ���ƽ��
	translate.cy = 0 ; // �����ţ���ƽ��
	imageRect = {dst_x, dst_y, dst_x + dst_w, dst_y + dst_h}; // ͼƬλ��
	
	memoryDc.DeleteDC();
	memoryDc.Detach();
}


/**
 * ���߶ȱ������ţ�1.���ͼƬ�߶ȴ���Ŀ�꣬�򰴱�����С�߶ȣ� 2.���ͼƬ�߶�С��Ŀ�꣬�򰴱����Ŵ�߶�. 3.��Ȱ��߶ȵı�������
 * 
 * @param mdc
 * @param bitmap
 */
void QImage::paintHeightFixStretchBlt(CMemoryDC &mdc, CBitmap & bitmap)
{
	CSize size;
	if (bitmap.GetSize(size) == false) {
		Q_ERROR("paintStretchBlt has error!");
		return;
	}

	int cw = mdc.m_rcPaint.right - size.cx; // ��Ȳ�ֵ = Ŀ���� - ͼƬ��� 
	int ch = mdc.m_rcPaint.bottom - size.cy; // �߶Ȳ�ֵ = Ŀ��߶� - ͼƬ�߶� 

	int dst_w = int(round(( mdc.m_rcPaint.bottom -  mdc.m_rcPaint.top) * size.cx * 1.0 / size.cy ));
	int dst_h = mdc.m_rcPaint.bottom -  mdc.m_rcPaint.top;
	int dst_x =  mdc.m_rcPaint.left + (mdc.m_rcPaint.right - mdc.m_rcPaint.left - dst_w) / 2;
	int dst_y = 0;
	
	CDC memoryDc; // �ڴ�DC
	memoryDc.CreateCompatibleDC(mdc);
	memoryDc.SelectBitmap(bitmap);
	
	// ʹ��HALFTONE��ģʽ������Ҫ����SetBrushOrgEx
	mdc.SetStretchBltMode(HALFTONE);
	CPoint pointZero(0,0);
	SetBrushOrgEx(mdc, 0,0,NULL);

	// ʹ��StretchBlt�ü�
	mdc.StretchBlt(dst_x, dst_y, dst_w, dst_h,
		memoryDc, 0, 0, size.cx, size.cy, SRCCOPY);


	// ����ϵ��
	scale_w = dst_h * 1.0 / size.cy;
	scale_h = dst_h * 1.0 / size.cy;

	// ����ƽ��ϵ��
	translate.cx = 0 ; // �����ţ���ƽ��
	translate.cy = 0 ; // �����ţ���ƽ��
	imageRect = {dst_x, dst_y, dst_x + dst_w, dst_y + dst_h}; // ͼƬλ��
	
	memoryDc.DeleteDC();
	memoryDc.Detach();
}

/**
 * ͼ�����Ŀ�����ĵ�,ֻƽ�ƣ�������, ������Ҫ�ü�ͼƬ�ѱ���ʾ
 */
void QImage::paintCenterFixStretchBlt(CMemoryDC &mdc, CBitmap & bitmap)
{
	CSize size;
	if (bitmap.GetSize(size) == false) {
		Q_ERROR("paintStretchBlt has error!");
		return;
	}

	CDC memoryDc; // �ڴ�DC
	memoryDc.CreateCompatibleDC(mdc);
	memoryDc.SelectBitmap(bitmap);
	
	// ʹ��HALFTONE��ģʽ������Ҫ����SetBrushOrgEx
	mdc.SetStretchBltMode(HALFTONE);
	CPoint pointZero(0,0);
	SetBrushOrgEx(mdc, 0,0,NULL);

	int cw = mdc.m_rcPaint.right - size.cx; // ��Ȳ�ֵ = Ŀ���� - ͼƬ��� 
	int ch = mdc.m_rcPaint.bottom - size.cy; // �߶Ȳ�ֵ = Ŀ��߶� - ͼƬ�߶� 

	// Ŀ����ʾ����
	int dst_x = cw > 0 ? cw / 2 : 0,
		dst_y = ch > 0 ? ch / 2 : 0,
		dst_w = cw > 0 ? size.cx : mdc.m_rcPaint.right,
		dst_h = ch > 0 ? size.cy :  mdc.m_rcPaint.bottom ;

	// ͼƬ����
	int src_x = cw > 0 ? 0 : -(cw / 2),
		src_y = ch > 0 ? 0 : -(ch / 2),
		src_w = cw > 0 ? size.cx : mdc.m_rcPaint.right,
		src_h = ch > 0 ? size.cy : mdc.m_rcPaint.bottom ;

	if (cw > 0 && ch > 0) {
		// ֱ�Ӵ��Կ��ϣ���������
		mdc.BitBlt(dst_x, dst_y, dst_w, dst_h,
			memoryDc, 0, 0, SRCCOPY);
	} else {
		// ʹ��StretchBlt�ü�
		mdc.StretchBlt(dst_x, dst_y, dst_w, dst_h,
			memoryDc, src_x, src_y, src_w, src_h, SRCCOPY);
	}

	// ����ƽ��ϵ��
	scale_w = 1.0;
	scale_h = 1.0;
	translate.cx = -(cw / 2) ; // ��Ȳ�ֵ / 2 
	translate.cy = -(ch / 2) ; // �߶Ȳ�ֵ / 2
	imageRect = {dst_x, dst_y, dst_x + dst_w, dst_y + dst_h}; // ͼƬλ��
	
	memoryDc.DeleteDC();
	memoryDc.Detach();
}




