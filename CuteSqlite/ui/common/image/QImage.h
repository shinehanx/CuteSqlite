/*****************************************************************//**
 * @file   QImage.h
 * @brief  ͼƬչʾ�Ļ���
 * @detail    $DETAIL
 * 
 * @author ��ѧ��
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
		kImageStretchFitTargetRect, // ͼƬ������ӦĿ�꣺�����κβü���ͼƬ����Ŀ������1.���ͼƬ�Ŀ�ߴ���Ŀ������ѹ��ͼƬ���;2.������С��Ŀ������������ͼƬ���
		kImageWidthFixTargetRect, // ����ȱ������ţ�1.���ͼƬ��ȴ���Ŀ�꣬�򰴱�����С��ȣ� 2.���ͼƬ���С��Ŀ�꣬�򰴱����Ŵ���. 3.�߶Ȱ���ȵı�������
		kImageHeightFixTargetRect, // ���߶ȱ������ţ�1.���ͼƬ�߶ȴ���Ŀ�꣬�򰴱�����С�߶ȣ� 2.���ͼƬ�߶�С��Ŀ�꣬�򰴱����Ŵ�߶�. 3.��Ȱ��߶ȵı�������
		kImageCenterFixTargetRect // ���ĵ������ʾ����ѹ��/���ü��κ�ͼƬ��ߣ�ֱ�����ĵ������ʾ��
	} DisplayMode; //��ʾ��ʽ

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

	// ����ͼƬ
	void load(HBITMAP _hBitmap);
	void load(CBitmap & _hBitmap);
	void load(CBitmap * _hBitmap);
	void load(Gdiplus::Bitmap * _bitmap);
	void load(const wchar_t * path, DWORD compress = BI_RGB, bool isCopy = false);

	// ���ַ�ʽ����BITMAP
	HBITMAP loadBitmapWithGdi(const wchar_t * path, bool isCopy);
	HBITMAP loadBitmapWithGdiPlus(const wchar_t * path);
	
	void setDisplayMode(DisplayMode _displayMode);
	DisplayMode getDisplayMode();

	// ���ñ�����ɫ
	void setBkgColor(COLORREF color);

	//���߿�
	void setBorder(int size, COLORREF color);

private:
	void paintBitblt(CMemoryDC &mdc, CBitmap & bitmap);
	void paintStretchBlt(CMemoryDC &mdc, CBitmap & bitmap);
	void paintWidthFixStretchBlt(CMemoryDC &mdc, CBitmap & bitmap);
	void paintHeightFixStretchBlt(CMemoryDC &mdc, CBitmap & bitmap);
	void paintCenterFixStretchBlt(CMemoryDC &mdc, CBitmap & bitmap);
protected:
	CBitmap bitmap; // ����ฺ���ͷ�hBitmap���ڴ�
	DisplayMode displayMode = kImageCenterFixTargetRect; //Ĭ�����ĵ����
	COLORREF bkgColor = RGB(204, 204, 204); //������ɫ
	CBrush bkgBrush; // ������ˢ

	int borderSize = 0;
	COLORREF borderColor = 0;

	// ͼƬƽ�ƺ�����ϵ��
	SIZE translate = { 0, 0 }; // ƽ��ϵ��,�������������ͼ��λ��ʹ��
	double scale_w = 1.0;	// ����ϵ��
	double scale_h = 1.0;	// ����ϵ��
	
	CRect imageRect; // ƽ�����ź��ͼƬ���λ��

	virtual LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	virtual LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	virtual LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	virtual LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	/**
	 * ������չ�����ݣ�����̳�QImage����ʵ�ִ˷������ǻ����paintItem, ���mdc��չ��������.
	 * 
	 * @param dc
	 * @param paintRect
	 */
	virtual void paintItem(CDC & dc, CRect & paintRect);	
};



