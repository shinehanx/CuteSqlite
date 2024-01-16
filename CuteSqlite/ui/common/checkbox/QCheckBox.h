/*****************************************************************//**
 * @file   QCheckBox.h
 * @brief  ��ѡ��ť
 * @detail    $DETAIL
 * 
 * @author ��ѧ��
 * @date   2023-04-09
 *********************************************************************/
#pragma once
#include <string>
#include <atlwin.h>
#include <atlframe.h>
#include <atlcrack.h>
#include <atlgdi.h>
#include <atlctrls.h>
#include <atltypes.h>
#include <atlstr.h>

class QCheckBox : public CWindowImpl<QCheckBox, CButton>, public COwnerDraw<QCheckBox> {
public:

	BEGIN_MSG_MAP_EX(QCheckBox)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		CHAIN_MSG_MAP_ALT(COwnerDraw<QCheckBox>, 1)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	~QCheckBox();
	// ����COwnerDraw���Զ�����ƺ���������ͼ��
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	void setTextColor(COLORREF color); // ������ɫ������ɫ��������ɫ
	void setBkgColor(COLORREF color); // ������ɫ
	void setFontSize(int fontSize); // ���ִ�С

	int getIsCheck();
	void setIsCheck(int isCheck);
protected:
	COLORREF bkgColor = RGB(255, 255, 255);
	HBRUSH bkgBrush = nullptr;
	COLORREF textColor = RGB(0, 0, 0);
	HBRUSH textBrush = nullptr;
	COLORREF rectColor = RGB(255, 255, 255);
	HBRUSH rectBrush = nullptr;
	COLORREF tickColor = RGB(0, 0, 0);
	CPen tickPen;
	CPen textPen;
	HFONT textFont = nullptr;
	int fontSize = 18;
	int isCheck = 0;

	CRect getLeftRect(CRect &clientRect);
	CRect getRightRect(CRect &clientRect);

	void drawLeftArea(CMemoryDC & mdc, CRect &rect);
	void drawRightText(CMemoryDC & mdc, CRect &rect);

	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual BOOL OnEraseBkgnd(CDCHandle dc);
};