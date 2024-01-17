/*****************************************************************//**
 * @file   QMessageBox.h
 * @brief  ÏûÏ¢µ¯´°
 * @detail    $DETAIL
 * 
 * @author ñûÑ§º­
 * @date   2023-04-23
 *********************************************************************/
#pragma once
#include <atlwin.h>
#include <atltypes.h>
#include <atlcrack.h>
#include <string>
#include "common/Config.h"
#include "resource.h"
#include "ui/common/button/QImageButton.h"

#define QMESSAGEBOX_WIDTH 400
#define QMESSAGEBOX_HEIGHT 180
class QMessageBox : public CDialogImpl<QMessageBox> {
public:
	enum { IDD = IDD_QFORM_DIALOG };
	BEGIN_MSG_MAP_EX(QMessageBox)
		//MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MSG_WM_CTLCOLORSTATIC(OnCtlStaticColor)
		COMMAND_ID_HANDLER_EX(Config::CUSTOMER_FORM_NO_BUTTON_ID, OnClickNoButton)
		COMMAND_ID_HANDLER_EX(Config::CUSTOMER_FORM_YES_BUTTON_ID, OnClickYesButton)
		COMMAND_ID_HANDLER_EX(IDOK, OnClickYesButton)
		COMMAND_ID_HANDLER_EX(IDCANCEL, OnClickNoButton)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
	static CRect getInitRect();

	static UINT confirm(HWND parentHwnd, std::wstring & text, std::wstring & yesBtnText = std::wstring(L""), std::wstring & noBtnText = std::wstring(L""));

	void setup(std::wstring & text, std::wstring & yesBtnText = std::wstring(L""), std::wstring & noBtnText = std::wstring(L""));
	void setSize(int w, int h);
	void createOrShowUI();
private:
	int width = QMESSAGEBOX_WIDTH;
	int height = QMESSAGEBOX_HEIGHT;
	CRect winRect;
	CRect destopRect;
	std::wstring text;
	std::wstring yesBtnText;
	std::wstring noBtnText;

	COLORREF textColor = RGB(8, 8, 8);
	COLORREF bkgColor = RGB(219, 219, 219);
	CBrush bkgBrush;
	HFONT textFont = nullptr;
	Gdiplus::Font * btnFont = nullptr;

	HRGN hRgn = nullptr;

	CEdit textEdit;
	QImageButton yesButton;
	QImageButton noButton;
	
	void createOrShowTextEdit(CRect & clientRect);
	void createOrShowEdit(WTL::CEdit & win, UINT id, std::wstring text, CRect rect, CRect &clientRect, DWORD exStyle = 0, bool readOnly = true);
	void createOrShowButtons(CRect & clientRect);

	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	HBRUSH OnCtlStaticColor(HDC hdc, HWND hwnd);

	LRESULT OnClickNoButton(UINT uNotifyCode, int nID, HWND hwnd);
	LRESULT OnClickYesButton(UINT uNotifyCode, int nID, HWND hwnd);
};

