/*****************************************************************//**
 * @file   GdiUtil.h
 * @brief  Gdi������
 * @detail    $DETAIL
 * 
 * @author ��ѧ��
 * @date   2023-04-08
 *********************************************************************/
#pragma once
#include <atlgdi.h>
#include <Windows.h>

class GdiUtil {
public:
	// ͼ�����Ŀ�����ĵ�,ֻƽ�ƣ�������, ������Ҫ�ü�ͼƬ�ѱ���ʾ.
	static void paintCenterFixStretchBlt(CMemoryDC &mdc, CBitmap & bitmap);
	// ��ֵͼ��
	static HBITMAP copyBitmap(HBITMAP hSourcehBitmap);
};
