#include "stdafx.h"
#include "GdiUtil.h"
#include <atltypes.h>
#include "Log.h"

/**
 * ͼ�����Ŀ�����ĵ�,ֻƽ�ƣ�������, ������Ҫ�ü�ͼƬ�ѱ���ʾ.
 * 
 * @param mdc ������mdc
 * @param bitmap ͼ��
 */
void GdiUtil::paintCenterFixStretchBlt(CMemoryDC &mdc, CBitmap & bitmap)
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
		dst_h = ch > 0 ? size.cy : mdc.m_rcPaint.bottom ;

	// ͼƬ����
	int src_x = cw > 0 ? 0 : -(cw / 2),
		src_y = ch > 0 ? 0 : -(ch / 2),
		src_w = cw > 0 ? size.cx : mdc.m_rcPaint.right,
		src_h = ch > 0 ? size.cy : mdc.m_rcPaint.bottom;

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
	SIZE translate = { 0, 0 }; // ƽ��ϵ��,�������������ͼ��λ��ʹ��
	float scale = 1.0;	// ����ϵ��
	translate.cx = -(cw / 2) ; // ��Ȳ�ֵ / 2
	translate.cy = -(ch / 2) ; // �߶Ȳ�ֵ / 2
	
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
