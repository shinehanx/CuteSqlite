/*****************************************************************//**
 * @file   GdiUtil.h
 * @brief  Gdi工具类
 * @detail    $DETAIL
 * 
 * @author 覃学涵
 * @date   2023-04-08
 *********************************************************************/
#pragma once
#include <atlgdi.h>
#include <Windows.h>

class GdiUtil {
public:
	// 图像对齐目标中心点,只平移，不缩放, 但是需要裁剪图片已便显示.
	static void paintCenterFixStretchBlt(CMemoryDC &mdc, CBitmap & bitmap);
	// 赋值图像
	static HBITMAP copyBitmap(HBITMAP hSourcehBitmap);
};
