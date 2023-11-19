/*****************************************************************//**
 * Copyright 2023 Xuehan Qin 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *   http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and

 * limitations under the License.

 * @file   QDialog.h
 * @brief  The base templcate class of XxxDialog class, sub dialog class will extend this class
 * 
 * @author Xuehan Qin
 * @date   2023-06-07
 *********************************************************************/
#pragma once
#include "stdafx.h"
#include "resource.h"
#include <atltypes.h>
#include <atlcrack.h>
#include "common/Config.h"
#include "ui/common/button/QImageButton.h"
#include "ui/common/checkbox/QCheckBox.h"
#include "ui/common/image/QStaticImage.h"
#include "utils/GdiPlusUtil.h"
#include "utils/ResourceUtil.h"
#include "core/common/Lang.h"

#define QDIALOG_WIDTH		616
#define QDIALOG_HEIGHT		400
#define QDIALOG_MARGIN_TOP	60

#define QDIALOG_LABEL_WIDTH	80
#define QDIALOG_LABEL_HEIGHT	24
#define QDIALOG_LABEL_SPLIT	10

#define QDIALOG_EDIT_WIDTH		150
#define QDIALOG_EDIT_HEIGHT	24
#define QDIALOG_EDIT_SPLIT		10


#define QDIALOG_BUTTON_WIDTH 80
#define QDIALOG_BUTTON_HEIGHT 25

template <class T>
class QDialog :  public CAxDialogImpl<T>, public CUpdateUI<T>,
	public CMessageFilter, public CIdleHandler
{
public:
	enum { IDD = IDD_QDIALOG_DIALOG };
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

	BEGIN_UPDATE_UI_MAP(T)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP_EX(QDialog)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)		
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MSG_WM_CTLCOLORSTATIC(OnCtlColorStatic)
		MSG_WM_CTLCOLORLISTBOX(OnCtlListBoxColor)
		MSG_WM_CTLCOLOREDIT(OnCtlEditColor)
		MSG_WM_CTLCOLORBTN(OnCtlButtonColor)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		COMMAND_HANDLER_EX(Config::QDIALOG_YES_BUTTON_ID, BN_CLICKED, OnClickYesButton)
		COMMAND_HANDLER_EX(Config::QDIALOG_NO_BUTTON_ID, BN_CLICKED, OnClickNoButton)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	virtual void createOrShowUI();
	void setFormSize(int w, int h);
protected:
	COLORREF bkgColor = RGB(238, 238, 238);
	CBrush bkgBrush;

	std::wstring caption;
	CRect transitparentRect; //  注意，这个RECT值在子类必须指定，不然不显示

	CButton yesButton;
	CButton noButton;
	CPen linepen;
	HFONT textFont = nullptr;
	HFONT radioFont = nullptr;
	HFONT buttonFont = nullptr;

	int formWidth = QDIALOG_WIDTH;
	int formHeight = QDIALOG_HEIGHT;

	//CBitmap formBitmap;

	virtual CRect getFormRect(CRect &clientRect);

	void createOrShowYesButton(CButton & win, CRect &clientRect);	
	void createOrShowNoButton(CButton & win, CRect &clientRect);

	void createOrShowFormLabel(CStatic & win, std::wstring text, CRect rect, CRect &clientRect, DWORD exStyle = SS_RIGHT, HFONT  font=nullptr);
	void createOrShowFormImage(QStaticImage &win, UINT id, CRect & rect, CRect & clientRect);
	void createOrShowFormEdit(WTL::CEdit & win, UINT id, std::wstring text, std::wstring cueBanner, CRect rect, CRect &clientRect, DWORD exStyle = 0, bool isReadOnly=false);
	void createOrShowFormComboBox(WTL::CComboBox & win, UINT id, std::wstring text, CRect rect, CRect &clientRect, DWORD exStyle = CBS_DROPDOWNLIST);
	void createOrShowFormCheckBox(QCheckBox & win, UINT id, std::wstring text, CRect rect, CRect &clientRect, DWORD exStyle = BS_OWNERDRAW);
	void createOrShowFormCheckBox(CButton & win, UINT id, std::wstring text, CRect rect, CRect &clientRect, DWORD exStyle = 0);
	void createOrShowFormRadio(CButton & win, UINT id, std::wstring text, CRect rect, CRect &clientRect, DWORD exStyle = 0);
	void createOrShowFormGroupBox(CButton & win, UINT id, std::wstring text, CRect rect, CRect &clientRect, DWORD exStyle = 0);
	void createOrShowFormButton(CButton & win, UINT id, std::wstring text, CRect rect, CRect &clientRect, DWORD exStyle = 0);
	void createOrShowFormListBox(CListBox & win, UINT id, CRect rect, CRect &clientRect, DWORD exStyle = 0, DWORD excludeStyle = 0);

	virtual void paintItem(CDC &dc, CRect &paintRect) {}; //扩展画面内容，子类需要的画就实现它

	virtual LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	virtual LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	virtual LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	virtual LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	virtual HBRUSH OnCtlColorStatic(HDC hdc, HWND hwnd);
	virtual HBRUSH OnCtlListBoxColor(HDC hdc, HWND hwnd);
	virtual HBRUSH OnCtlEditColor(HDC hdc, HWND hwnd);
	virtual HBRUSH OnCtlButtonColor(HDC hdc, HWND hwnd);
	BOOL OnEraseBkgnd(CDCHandle dc);

	//用户定义
	virtual void OnClickYesButton(UINT uNotifyCode, int nID, HWND hwnd);
	virtual void OnClickNoButton(UINT uNotifyCode, int nID, HWND hwnd);
	
};


template <class T>
LRESULT QDialog<T>::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(0);
	return 0;
}

template <class T>
BOOL QDialog<T>::PreTranslateMessage(MSG* pMsg)
{
	return CWindow::IsDialogMessage(pMsg);
}

template <class T>
BOOL QDialog<T>::OnIdle()
{
	UIUpdateChildWindows();
	return FALSE;
}

template <class T>
void QDialog<T>::createOrShowUI()
{
	//界面排版
	CRect clientRect;
	GetClientRect(&clientRect);
	createOrShowYesButton(yesButton, clientRect);
	createOrShowNoButton(noButton, clientRect); 
}

template <class T>
void QDialog<T>::setFormSize(int w, int h)
{
	formWidth = w;
	formHeight = h;
}

template <class T>
LRESULT QDialog<T>::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// center the dialog on the screen
	MoveWindow(0, 0, formWidth, formHeight);
	CenterWindow();

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);
	UIAddChildWindowContainer(m_hWnd);

	if (!caption.empty()) {
		SetWindowText(caption.c_str());
	}

	bkgBrush.CreateSolidBrush(bkgColor);
	textFont = FT(L"form-text-size");
	radioFont = FT(L"radio-size");
	buttonFont = FT(L"image-button-size");

	createOrShowUI();
	return 0;
}

template <class T>
LRESULT QDialog<T>::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	if (!bkgBrush.IsNull()) bkgBrush.DeleteObject();
	if (textFont) ::DeleteObject(textFont);
	if (radioFont) ::DeleteObject(radioFont);
	if (buttonFont) ::DeleteObject(buttonFont);

	//释放窗口
	if (yesButton.IsWindow()) yesButton.DestroyWindow();
	if (noButton.IsWindow()) noButton.DestroyWindow();

	return 0;
}

template <class T>
LRESULT QDialog<T>::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return 0;
}

//获得QDIALOG的RECT
template <class T>
CRect QDialog<T>::getFormRect(CRect &clientRect)
{
	return clientRect;
}

template <class T>
void QDialog<T>::createOrShowYesButton(CButton & win, CRect &clientRect)
{
	CRect rect(clientRect.right - 2 * QDIALOG_BUTTON_WIDTH - 10 - 10, 
		clientRect.bottom - QDIALOG_BUTTON_HEIGHT - 10, 
		clientRect.right  - QDIALOG_BUTTON_WIDTH- 10 - 10, 
		clientRect.bottom - 10);
	createOrShowFormButton(win, Config::QDIALOG_YES_BUTTON_ID, S(L"ok"), rect, clientRect);
}

template <class T>
void QDialog<T>::createOrShowNoButton(CButton & win, CRect &clientRect)
{
	CRect rect(clientRect.right - QDIALOG_BUTTON_WIDTH - 10, 
		clientRect.bottom - QDIALOG_BUTTON_HEIGHT - 10, 
		clientRect.right - 10, clientRect.bottom - 10);

	createOrShowFormButton(win, Config::QDIALOG_NO_BUTTON_ID, S(L"cancel"), rect, clientRect);	
}

template <class T>
void QDialog<T>::createOrShowFormLabel(CStatic & win, std::wstring text, CRect rect, CRect &clientRect, DWORD exStyle, HFONT font)
{
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | SS_NOTIFY ;
		if (exStyle) {
			dwStyle = dwStyle | exStyle;
		}
		win.Create(m_hWnd, rect, text.c_str(), dwStyle, 0);
		if (font) {
			win.SetFont(font);
		}		
		return;
	} else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);		
	}
}

template <class T>
void createOrShowFormImage(QStaticImage &win, UINT id, CRect & rect, CRect & clientRect)
{
	if (::IsWindow(hwnd) && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_VISIBLE |WS_CLIPCHILDREN  | WS_CLIPSIBLINGS | SS_NOTIFY; // SS_NOTIFY - 表示static接受点击事件
		win.Create(hwnd, rect, L"", dwStyle, 0);
		return;
	} else if (::IsWindow(hwnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
	}
}

template <class T>
void QDialog<T>::createOrShowFormEdit(WTL::CEdit & win, UINT id, std::wstring text, std::wstring cueBanner, CRect rect, CRect &clientRect, DWORD exStyle, bool isReadOnly)
{
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_VISIBLE |WS_TABSTOP;
		if (exStyle) {
			dwStyle |= exStyle;
		}
		win.Create(m_hWnd, rect, text.c_str(), dwStyle, WS_EX_CLIENTEDGE, id);
		HFONT hfont = FT(L"form-text-size");
		win.SetFont(hfont);
		::DeleteObject(hfont);
		win.SetCueBannerText(cueBanner.c_str());
		win.SetMargins(2, 2);
		// set the edit as read only
		win.SetReadOnly(isReadOnly);
		return;
	} else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);		
	}
}

template <class T>
void QDialog<T>::createOrShowFormComboBox(WTL::CComboBox & win, UINT id, std::wstring text, CRect rect, CRect &clientRect, DWORD exStyle /* = CBS_DROWDNLIST*/)
{
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | CBS_HASSTRINGS | CBS_AUTOHSCROLL;
		if (exStyle) {
			dwStyle = dwStyle | exStyle;
		}
		win.Create(m_hWnd, rect, text.c_str(), dwStyle , 0, id);
		HFONT hfont = FT(L"form-text-size");
		win.SetFont(hfont);
		::DeleteObject(hfont);
		return;
	} else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);		
	}
}

template <class T>
void QDialog<T>::createOrShowFormCheckBox(QCheckBox & win, UINT id, std::wstring text, CRect rect, CRect &clientRect, DWORD exStyle)
{
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN  |WS_TABSTOP | WS_CLIPSIBLINGS | BS_CHECKBOX ; // 注意 BS_CHECKBOX | BS_OWNERDRAW 才能出发
		if (exStyle) dwStyle = dwStyle | exStyle;
		win.Create(m_hWnd, rect, text.c_str(), dwStyle , 0, id);
		return ;
	} else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
	}
}

template <class T>
void QDialog<T>::createOrShowFormCheckBox(CButton & win, UINT id, std::wstring text, CRect rect, CRect &clientRect, DWORD exStyle)
{
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN  |WS_TABSTOP | WS_CLIPSIBLINGS | BS_CHECKBOX ;
		if (exStyle) dwStyle = dwStyle | exStyle;
		win.Create(m_hWnd, rect, text.c_str(), dwStyle , 0, id);
		win.BringWindowToTop();
		return ;
	} else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
	}
}


template <class T>
void QDialog<T>::createOrShowFormRadio(CButton & win, UINT id, std::wstring text, CRect rect, CRect &clientRect, DWORD exStyle /*= BS_OWNERDRAW*/)
{
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP  | BS_RADIOBUTTON  ;
		if (exStyle) dwStyle = dwStyle | exStyle;
		win.Create(m_hWnd, rect, text.c_str(), dwStyle , 0, id);
		win.SetFont(radioFont);
		return ;
	} else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
	}
}

template <class T>
void QDialog<T>::createOrShowFormGroupBox(CButton & win, UINT id, std::wstring text, CRect rect, CRect &clientRect, DWORD exStyle /*= 0*/)
{
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_VISIBLE  | BS_GROUPBOX ;
		if (exStyle) dwStyle = dwStyle | exStyle;
		win.Create(m_hWnd, rect, text.c_str(), dwStyle , 0, id);
		win.SetFont(radioFont);
		return ;
	} else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
	}
}

template <class T>
void QDialog<T>::createOrShowFormButton(CButton & win, UINT id, std::wstring text, CRect rect, CRect &clientRect, DWORD exStyle /*= 0*/)
{
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN  | WS_CLIPSIBLINGS  | WS_TABSTOP; // 注意 BS_CHECKBOX | BS_OWNERDRAW 才能出发
		if (exStyle) dwStyle = dwStyle | exStyle;
		win.Create(m_hWnd, rect, text.c_str(), dwStyle, 0, id);
		win.SetFont(buttonFont);
		win.BringWindowToTop();
		return ;
	} else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
	}
}

template <class T>
void QDialog<T>::createOrShowFormListBox(CListBox & win, UINT id, CRect rect, CRect &clientRect, DWORD exStyle, DWORD excludeStyle)
{
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_TABSTOP | WS_VSCROLL | LBS_HASSTRINGS; 
		if (exStyle) dwStyle = dwStyle | exStyle;
		if (excludeStyle) dwStyle = dwStyle & ~excludeStyle;
		win.Create(m_hWnd, rect, L"", dwStyle, WS_EX_CLIENTEDGE, id);
		win.BringWindowToTop();
		
		return ;
	} else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
	}
}


template <class T>
LRESULT QDialog<T>::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC dc(m_hWnd);
	CMemoryDC mdc(dc, dc.m_ps.rcPaint);
	mdc.FillRect(&(dc.m_ps.rcPaint), bkgBrush.m_hBrush);

	CRect clientRect;
	GetClientRect(clientRect);
	paintItem(mdc, clientRect);
	return 0;
}

template <class T>
HBRUSH QDialog<T>::OnCtlColorStatic(HDC hdc, HWND hwnd)
{
	//::SetTextColor(hdc, RGB(84, 83, 82)); //文本区域前景色
	::SetBkColor(hdc, bkgColor); //文本区域背景色
	
	// 字体
	::SelectObject(hdc, textFont);
	return bkgBrush.m_hBrush; // 整个CStatic的Client区域背景色
}

template <class T>
HBRUSH QDialog<T>::OnCtlListBoxColor(HDC hdc, HWND hwnd)
{
	// 字体
	::SelectObject(hdc, textFont); 

	return (HBRUSH)AtlGetStockBrush(WHITE_BRUSH);
}

template <class T>
HBRUSH QDialog<T>::OnCtlEditColor(HDC hdc, HWND hwnd)
{
	// 字体
	::SelectObject(hdc, textFont);
	return (HBRUSH)AtlGetStockBrush(WHITE_BRUSH);
}


template <class T>
HBRUSH QDialog<T>::OnCtlButtonColor(HDC hdc, HWND hwnd)
{
	// 字体
	::SelectObject(hdc, textFont);
	return (HBRUSH)AtlGetStockBrush(WHITE_BRUSH);
}

template <class T>
void QDialog<T>::OnClickYesButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	EndDialog(nID);
}

template <class T>
void QDialog<T>::OnClickNoButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	EndDialog(nID);
}


template <class T>
BOOL QDialog<T>::OnEraseBkgnd(CDCHandle dc)
{
	return true;
}
