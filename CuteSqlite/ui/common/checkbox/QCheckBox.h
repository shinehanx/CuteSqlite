/*****************************************************************//**
 * @file   QCheckBox.h
 * @brief  多选按钮
 * @detail    $DETAIL
 * 
 * @author 覃学涵
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
	// 重载COwnerDraw的自定义绘制函数，绘制图像
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	void setTextColor(COLORREF color); // 文字颜色，勾颜色，方框颜色
	void setBkgColor(COLORREF color); // 背景颜色
	void setFontSize(int fontSize); // 文字大小

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