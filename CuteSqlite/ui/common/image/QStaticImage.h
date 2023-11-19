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
	BOOL PreTranslateMessage(MSG* pMsg);
	BEGIN_MSG_MAP(QStaticImage)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnMousePressed)
		MESSAGE_RANGE_HANDLER(WM_MOUSEFIRST, WM_MOUSELAST, OnMouseMessage)
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
	void setToolTip(const wchar_t * text);
	
protected:
	
	CBitmap cornerIcon; // ��С��ͼ��
	CBrush bkgIconBrush; // ���½�Сͼ��������α���
	HPEN bkgIconPen = nullptr;
	CRect cornerIconRect; //Сͼ������������ж��Ƿ���
	HFONT tipFont = nullptr;

	std::wstring tip; // ���Ͻǵ���ʾ����
	std::wstring toolTipText; // tool tip ��ʾ
	CToolTipCtrl tooltipCtrl;

	virtual LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	virtual LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	virtual LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	virtual LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	
	// �����
	LRESULT OnMousePressed(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	// ���ǻ����paintItem�� �����OnPain��Ϣ�����������������paintItem�ĺ�����չ��������.
	virtual void paintItem(CDC & dc, CRect & paintRect);
};
