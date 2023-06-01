#include "stdafx.h"
#include "GdiUtil.h"
#include <atltypes.h>
#include "Log.h"

/**
 * 图像对齐目标中心点,只平移，不缩放, 但是需要裁剪图片已便显示.
 * 
 * @param mdc 缓冲区mdc
 * @param bitmap 图像
 */
void GdiUtil::paintCenterFixStretchBlt(CMemoryDC &mdc, CBitmap & bitmap)
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
		dst_h = ch > 0 ? size.cy : mdc.m_rcPaint.bottom ;

	// 图片区域
	int src_x = cw > 0 ? 0 : -(cw / 2),
		src_y = ch > 0 ? 0 : -(ch / 2),
		src_w = cw > 0 ? size.cx : mdc.m_rcPaint.right,
		src_h = ch > 0 ? size.cy : mdc.m_rcPaint.bottom;

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
	SIZE translate = { 0, 0 }; // 平移系数,给计算点数计算图形位置使用
	float scale = 1.0;	// 缩放系数
	translate.cx = -(cw / 2) ; // 宽度差值 / 2
	translate.cy = -(ch / 2) ; // 高度差值 / 2
	
	memoryDc.DeleteDC();
	memoryDc.Detach();
}

HBITMAP GdiUtil::copyBitmap(HBITMAP hSourcehBitmap)
{
	CDC sourcedc;
	CDC destdc;
	sourcedc.CreateCompatibleDC(NULL);
	destdc.CreateCompatibleDC(NULL);
	//the   bitmap   information.
	BITMAP   bm = {0};
	//get   the   bitmap   information.
	::GetObject(hSourcehBitmap,   sizeof(bm),   &bm);
	//   create   a   bitmap   to   hold   the   result
	HBITMAP   hbmresult   =   ::CreateCompatibleBitmap(CClientDC(NULL),   bm.bmWidth ,   bm.bmHeight );
	HBITMAP   hbmoldsource   =   (HBITMAP)::SelectObject(   sourcedc.m_hDC ,   hSourcehBitmap);
	HBITMAP   hbmolddest   =   (HBITMAP)::SelectObject(   destdc.m_hDC ,   hbmresult   );
	destdc.BitBlt(0, 0, bm.bmWidth ,  bm.bmHeight , sourcedc,   0,   0,   SRCCOPY   );

	//   restore   dcs
	::SelectObject(  sourcedc.m_hDC ,   hbmoldsource   );
	::SelectObject(  destdc.m_hDC ,   hbmolddest   );
	::DeleteObject(  sourcedc.m_hDC );
	::DeleteObject(  destdc.m_hDC );

	return   hbmresult;
}
