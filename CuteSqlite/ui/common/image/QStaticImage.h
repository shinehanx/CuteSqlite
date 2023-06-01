/*****************************************************************//**
 * @file   QImageViewer.h
 * @brief  ���ھ�̬ͼƬչʾ ���̳���QImage->QStatic��
 * @detail    $DETAIL
 * 
 * @author ��ѧ��
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
	
	// �����Сͼ��
	void setCornerIcon(const wchar_t * path, DWORD compress=BI_RGB);
	//Сͼ��λ��
	CRect getCornerIconRect(); 

	// ��ʾ����
	void setTip(const wchar_t * text, HFONT font);
protected:
	
	CBitmap cornerIcon; // ��С��ͼ��
	HBRUSH bkgIconBrush = nullptr; // ���½�Сͼ��������α���
	HPEN bkgIconPen = nullptr;
	CRect cornerIconRect; //Сͼ������������ж��Ƿ���

	std::wstring tip; // ���Ͻǵ���ʾ����
	HFONT tipFont = nullptr;

	virtual LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	virtual LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	virtual LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	virtual LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	
	// �����
	LRESULT OnMousePressed(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	// ���ǻ����paintItem�� �����OnPain��Ϣ�����������������paintItem�ĺ�����չ��������.
	virtual void paintItem(CDC & dc, CRect & paintRect);
};
