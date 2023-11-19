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

	// GDI的图片转成HBITMAP
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
	if (hbitmap != nullptr && !isCopy) { // 是否拷贝
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
 * 使用GID+加載图片.
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

	Gdiplus::Bitmap * bmp = Gdiplus::Bitmap::FromStream(stream); //GDI+读取JPG/PNG图像

	//Gdiplus::Bitmap * bmp = Gdiplus::Bitmap::FromFile(path);

	// GDI的图片转成HBITMAP
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
	if (!bkgBrush.IsNull()) bkgBrush.DeleteObject(); // 先卸载，再显示
	bkgBrush.CreateSolidBrush(bkgColor);
}

/**
 * 边框，宽度和颜色.
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
	CMemoryDC mdc(dc, clientRect); //建立双缓冲机制

	// 背景画刷
	HBRUSH oldBrush = mdc.SelectBrush(bkgBrush.m_hBrush);
	mdc.FillRect(clientRect, bkgBrush.m_hBrush);
	mdc.SelectBrush(oldBrush);

	if (bitmap.IsNull()) {
		// 画面扩展内容
		paintItem(mdc, clientRect);
		return 0;
	}		
	
	switch (displayMode)
	{
	case kImageCenterFixTargetRect: //对齐中心点
		paintCenterFixStretchBlt(mdc, bitmap);
		break;
	case kImageWidthFixTargetRect: // 宽度等比缩放
		paintWidthFixStretchBlt(mdc, bitmap);
		break;
	case kImageHeightFixTargetRect: // 高度等比缩放
		paintHeightFixStretchBlt(mdc, bitmap);
		break;
	case kImageStretchFitTargetRect:
	default:
		paintStretchBlt(mdc, bitmap); //压缩显示
		break;
	}
	// 画面扩展的内容，子类继承QImage可以实现此方法,获得mdc扩展画面内容
	paintItem(mdc, clientRect);

	// 画边框
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
 *画面扩展的内容，子类继承QImage可以实现此方法覆盖基类的paintItem, 获得mdc扩展画面内容.
 * 
 * @param dc 
 * @param paintRect
 */
void QImage::paintItem(CDC &dc, CRect & paintRect)
{
	 // 基类中什么都不做
}

void QImage::paintBitblt(CMemoryDC &mdc, CBitmap & bitmap)
{
	CSize size;
	bitmap.GetBitmapDimension(&size);

	CDC memoryDc;
	memoryDc.CreateCompatibleDC(mdc);
	memoryDc.SelectBitmap(bitmap);
	
	// 以K
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
	
	CDC memoryDc; // 内存DC
	memoryDc.CreateCompatibleDC(mdc);
	memoryDc.SelectBitmap(bitmap);
	
	// 
	mdc.SetStretchBltMode(HALFTONE);
	CPoint pointZero(0,0);
	// 使用StretchBlt裁剪
	mdc.StretchBlt(0, 0, mdc.m_rcPaint.right, mdc.m_rcPaint.bottom,
		memoryDc, 0, 0, size.cx, size.cy, SRCCOPY);
	SetBrushOrgEx(mdc, 0,0,NULL);

	int cw = mdc.m_rcPaint.right - size.cx; // 宽度差值 = 目标宽度 - 图片宽度 
	int ch = mdc.m_rcPaint.bottom - size.cy; // 高度差值 = 目标高度 - 图片高度 
	// 缩放系数
	scale_w = mdc.m_rcPaint.right * 1.0 / size.cx;
	scale_h = mdc.m_rcPaint.bottom * 1.0 / size.cy;

	// 计算平移系数
	translate.cx = 0 ; // 宽度差值 * 缩放系数 / 2
	translate.cy = 0 ; // 高度差值 * 缩放系数 / 2
	imageRect = {0, 0, mdc.m_rcPaint.right, mdc.m_rcPaint.bottom}; // 图片位置

	memoryDc.DeleteDC();
	memoryDc.Detach();
}

/**
 * 按宽度比例缩放；1.如果图片宽度大于目标，则按比例缩小宽度； 2.如果图片宽度小于目标，则按比例放大宽度. 3.高度按宽度的比例缩放
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

	int cw = mdc.m_rcPaint.right - size.cx; // 宽度差值 = 目标宽度 - 图片宽度 
	int ch = mdc.m_rcPaint.bottom - size.cy; // 高度差值 = 目标高度 - 图片高度 

	int dst_w = mdc.m_rcPaint.right - mdc.m_rcPaint.left;
	int dst_h = int(round(dst_w * size.cy * 1.0 / size.cx));
	int dst_x = 0;
	int dst_y = mdc.m_rcPaint.top + (mdc.m_rcPaint.bottom - mdc.m_rcPaint.top - dst_h) / 2;
	
	CDC memoryDc; // 内存DC
	memoryDc.CreateCompatibleDC(mdc);
	memoryDc.SelectBitmap(bitmap);
	
	// 使用HALFTONE的模式，则需要调用SetBrushOrgEx
	mdc.SetStretchBltMode(HALFTONE);
	CPoint pointZero(0,0);
	SetBrushOrgEx(mdc, 0,0,NULL);

	// 使用StretchBlt裁剪
	mdc.StretchBlt(dst_x, dst_y, dst_w, dst_h,
		memoryDc, 0, 0, size.cx, size.cy, SRCCOPY);


	// 缩放系数
	scale_w = dst_w * 1.0 / size.cx;
	scale_h = dst_w * 1.0 / size.cx;

	// 计算平移系数
	translate.cx = 0 ; // 仅缩放，不平移
	translate.cy = 0 ; // 仅缩放，不平移
	imageRect = {dst_x, dst_y, dst_x + dst_w, dst_y + dst_h}; // 图片位置
	
	memoryDc.DeleteDC();
	memoryDc.Detach();
}


/**
 * 按高度比例缩放；1.如果图片高度大于目标，则按比例缩小高度； 2.如果图片高度小于目标，则按比例放大高度. 3.宽度按高度的比例缩放
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

	int cw = mdc.m_rcPaint.right - size.cx; // 宽度差值 = 目标宽度 - 图片宽度 
	int ch = mdc.m_rcPaint.bottom - size.cy; // 高度差值 = 目标高度 - 图片高度 

	int dst_w = int(round(( mdc.m_rcPaint.bottom -  mdc.m_rcPaint.top) * size.cx * 1.0 / size.cy ));
	int dst_h = mdc.m_rcPaint.bottom -  mdc.m_rcPaint.top;
	int dst_x =  mdc.m_rcPaint.left + (mdc.m_rcPaint.right - mdc.m_rcPaint.left - dst_w) / 2;
	int dst_y = 0;
	
	CDC memoryDc; // 内存DC
	memoryDc.CreateCompatibleDC(mdc);
	memoryDc.SelectBitmap(bitmap);
	
	// 使用HALFTONE的模式，则需要调用SetBrushOrgEx
	mdc.SetStretchBltMode(HALFTONE);
	CPoint pointZero(0,0);
	SetBrushOrgEx(mdc, 0,0,NULL);

	// 使用StretchBlt裁剪
	mdc.StretchBlt(dst_x, dst_y, dst_w, dst_h,
		memoryDc, 0, 0, size.cx, size.cy, SRCCOPY);


	// 缩放系数
	scale_w = dst_h * 1.0 / size.cy;
	scale_h = dst_h * 1.0 / size.cy;

	// 计算平移系数
	translate.cx = 0 ; // 仅缩放，不平移
	translate.cy = 0 ; // 仅缩放，不平移
	imageRect = {dst_x, dst_y, dst_x + dst_w, dst_y + dst_h}; // 图片位置
	
	memoryDc.DeleteDC();
	memoryDc.Detach();
}

/**
 * 图像对齐目标中心点,只平移，不缩放, 但是需要裁剪图片已便显示
 */
void QImage::paintCenterFixStretchBlt(CMemoryDC &mdc, CBitmap & bitmap)
{
	CSize size;
	if (bitmap.GetSize(size) == false) {
		Q_ERROR("paintStretchBlt has error!");
		return;
	}

	CDC memoryDc; // 内存DC
	memoryDc.CreateCompatibleDC(mdc);
	memoryDc.SelectBitmap(bitmap);
	
	// 使用HALFTONE的模式，则需要调用SetBrushOrgEx
	mdc.SetStretchBltMode(HALFTONE);
	CPoint pointZero(0,0);
	SetBrushOrgEx(mdc, 0,0,NULL);

	int cw = mdc.m_rcPaint.right - size.cx; // 宽度差值 = 目标宽度 - 图片宽度 
	int ch = mdc.m_rcPaint.bottom - size.cy; // 高度差值 = 目标高度 - 图片高度 

	// 目标显示区域
	int dst_x = cw > 0 ? cw / 2 : 0,
		dst_y = ch > 0 ? ch / 2 : 0,
		dst_w = cw > 0 ? size.cx : mdc.m_rcPaint.right,
		dst_h = ch > 0 ? size.cy :  mdc.m_rcPaint.bottom ;

	// 图片区域
	int src_x = cw > 0 ? 0 : -(cw / 2),
		src_y = ch > 0 ? 0 : -(ch / 2),
		src_w = cw > 0 ? size.cx : mdc.m_rcPaint.right,
		src_h = ch > 0 ? size.cy : mdc.m_rcPaint.bottom ;

	if (cw > 0 && ch > 0) {
		// 直接打显卡上，提升性能
		mdc.BitBlt(dst_x, dst_y, dst_w, dst_h,
			memoryDc, 0, 0, SRCCOPY);
	} else {
		// 使用StretchBlt裁剪
		mdc.StretchBlt(dst_x, dst_y, dst_w, dst_h,
			memoryDc, src_x, src_y, src_w, src_h, SRCCOPY);
	}

	// 计算平移系数
	scale_w = 1.0;
	scale_h = 1.0;
	translate.cx = -(cw / 2) ; // 宽度差值 / 2 
	translate.cy = -(ch / 2) ; // 高度差值 / 2
	imageRect = {dst_x, dst_y, dst_x + dst_w, dst_y + dst_h}; // 图片位置
	
	memoryDc.DeleteDC();
	memoryDc.Detach();
}




