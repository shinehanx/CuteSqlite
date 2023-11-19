/*****************************************************************//**
 * @file   QImage.h
 * @brief  图片展示的基类
 * @detail    $DETAIL
 * 
 * @author 覃学涵
 * @date   2023-03-09
 *********************************************************************/

#pragma once
#include <atltypes.h>
#include <atlwin.h>
#include <GdiPlus.h>
#include <atlgdi.h>
#include "common/Config.h"
#include "utils/GdiPlusUtil.h"

class QImage : public CWindowImpl<QImage, CStatic>
{
public:
	typedef enum  {
		kImageStretchFitTargetRect, // 图片缩放适应目标：不做任何裁剪，图片填满目标区域。1.如果图片的宽高大于目标区域，压缩图片宽高;2.如果宽高小于目标区域，则拉伸图片宽高
		kImageWidthFixTargetRect, // 按宽度比例缩放；1.如果图片宽度大于目标，则按比例缩小宽度； 2.如果图片宽度小于目标，则按比例放大宽度. 3.高度按宽度的比例缩放
		kImageHeightFixTargetRect, // 按高度比例缩放；1.如果图片高度大于目标，则按比例缩小高度； 2.如果图片高度小于目标，则按比例放大高度. 3.宽度按高度的比例缩放
		kImageCenterFixTargetRect // 中心点对齐显示：不压缩/不裁剪任何图片宽高，直接中心点对齐显示。
	} DisplayMode; //显示方式

	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP(QImage)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
	END_MSG_MAP()

	QImage();
	QImage(CBitmap & _hBitmap);
	QImage(CBitmap * _bitmap);
	QImage(HBITMAP _hBitmap);
	QImage(Gdiplus::Bitmap * bitmap);
	QImage(const wchar_t * path, DWORD compress, bool isCopy=false);

	~QImage();

	// 加载图片
	void load(HBITMAP _hBitmap);
	void load(CBitmap & _hBitmap);
	void load(CBitmap * _hBitmap);
	void load(Gdiplus::Bitmap * _bitmap);
	void load(const wchar_t * path, DWORD compress = BI_RGB, bool isCopy = false);

	// 两种方式加载BITMAP
	HBITMAP loadBitmapWithGdi(const wchar_t * path, bool isCopy);
	HBITMAP loadBitmapWithGdiPlus(const wchar_t * path);
	
	void setDisplayMode(DisplayMode _displayMode);
	DisplayMode getDisplayMode();

	// 配置背景颜色
	void setBkgColor(COLORREF color);

	//画边框
	void setBorder(int size, COLORREF color);

private:
	void paintBitblt(CMemoryDC &mdc, CBitmap & bitmap);
	void paintStretchBlt(CMemoryDC &mdc, CBitmap & bitmap);
	void paintWidthFixStretchBlt(CMemoryDC &mdc, CBitmap & bitmap);
	void paintHeightFixStretchBlt(CMemoryDC &mdc, CBitmap & bitmap);
	void paintCenterFixStretchBlt(CMemoryDC &mdc, CBitmap & bitmap);
protected:
	CBitmap bitmap; // 这个类负责释放hBitmap的内存
	DisplayMode displayMode = kImageCenterFixTargetRect; //默认中心点对齐
	COLORREF bkgColor = RGB(204, 204, 204); //背景颜色
	CBrush bkgBrush; // 背景画刷

	int borderSize = 0;
	COLORREF borderColor = 0;

	// 图片平移和缩放系数
	SIZE translate = { 0, 0 }; // 平移系数,给计算点数计算图形位置使用
	double scale_w = 1.0;	// 缩放系数
	double scale_h = 1.0;	// 缩放系数
	
	CRect imageRect; // 平移缩放后的图片相对位置

	virtual LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	virtual LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	virtual LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	virtual LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	/**
	 * 画面扩展的内容，子类继承QImage可以实现此方法覆盖基类的paintItem, 获得mdc扩展画面内容.
	 * 
	 * @param dc
	 * @param paintRect
	 */
	virtual void paintItem(CDC & dc, CRect & paintRect);	
};



