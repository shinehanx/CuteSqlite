/*****************************************************************//**
 * @file   QImageViewer.h
 * @brief  用于静态图片展示 （继承于QImage->QStatic）
 * @detail    $DETAIL
 * 
 * @author 覃学涵
 * @date   2023-03-09
 *********************************************************************/
#pragma once
#include <gdiplus.h>
#include "QImage.h"

class QStaticImage : public QImage {
public:
	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP(QStaticImage)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnMousePressed)
	END_MSG_MAP()

	QStaticImage();
	~QStaticImage();
	QStaticImage(HBITMAP bitmap);
	QStaticImage(Gdiplus::Bitmap * bitmap); 
	QStaticImage(CBitmap &bitmap); 
	QStaticImage(const wchar_t * path, DWORD compress=BI_RGB, bool isCopy=false);
	
	// 角落的小图标
	void setCornerIcon(const wchar_t * path, DWORD compress=BI_RGB);
	//小图标位置
	CRect getCornerIconRect(); 

	// 提示文字
	void setTip(const wchar_t * text, HFONT font);
protected:
	
	CBitmap cornerIcon; // 右小角图标
	HBRUSH bkgIconBrush = nullptr; // 有下角小图标的三角形背景
	HPEN bkgIconPen = nullptr;
	CRect cornerIconRect; //小图标的区域，用来判断是否点击

	std::wstring tip; // 左上角的提示文字
	HFONT tipFont = nullptr;

	virtual LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	virtual LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	virtual LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	virtual LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	
	// 鼠标点击
	LRESULT OnMousePressed(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	// 覆盖基类的paintItem， 基类的OnPain消息处理函数，会调用子类paintItem的函数扩展画面内容.
	virtual void paintItem(CDC & dc, CRect & paintRect);
};
