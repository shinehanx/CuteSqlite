#include "stdafx.h"
#include "QMessageBox.h"

#include "core/common/Lang.h"
#include "utils/FontUtil.h"
#include "utils/StringUtil.h"
#include "ui/common/QWinCreater.h"
#include "common/AppContext.h"

UINT QMessageBox::confirm(HWND parentHwnd, std::wstring & text, std::wstring & yesBtnText, std::wstring & noBtnText)
{
	QMessageBox win;
	win.setup(text, yesBtnText, noBtnText);
	// win.Create(parentHwnd);
	UINT ret = static_cast<UINT>(win.DoModal());
	return ret;
}

void QMessageBox::setup(std::wstring & text, std::wstring & yesBtnText, std::wstring & noBtnText)
{
	this->text = text;
	int textSize = Lang::fontSize(L"message-text-size");
	std::wstring fontName = Lang::fontName();
	// 计算文本大小
	CSize size = FontUtil::measureTextSize(text.c_str(), textSize, false, fontName.c_str());
	height = 20 + 20 + size.cy + 20 + 20;

	this->yesBtnText = yesBtnText;
	this->noBtnText = noBtnText;

	textFont = Lang::font(L"message-text-size", false);
}

void QMessageBox::setSize(int w, int h)
{
	width = w;
	height = h;
}

void QMessageBox::createOrShowUI()
{
	CRect clientRect;
	GetClientRect(clientRect);
	createOrShowTextEdit(clientRect);
	createOrShowButtons(clientRect);
	noButton.SetFocus();
}


CRect QMessageBox::getInitRect()
{
	// 获得桌面的位置	
	CRect destopRect;
	HWND destopHwnd = ::GetDesktopWindow();
	::GetWindowRect(destopHwnd, destopRect);
	int x = (destopRect.Width() - QMESSAGEBOX_WIDTH) / 2, y = destopRect.top - QMESSAGEBOX_HEIGHT, w = QMESSAGEBOX_WIDTH, h = QMESSAGEBOX_HEIGHT;
	return { x, y, x + w, y + h };
}

void QMessageBox::createOrShowTextEdit(CRect & clientRect)
{
	int x = clientRect.left + 20 , y = clientRect.top + 20, w = clientRect.Width() - 20 * 2 , h = clientRect.Height() - 20 * 2 - 40;
	CRect rect(x, y, x + w, y + h);
	std::wstring showText = StringUtil::replaceBreak(text);
	createOrShowEdit(textEdit, 0, showText, rect, clientRect, ES_MULTILINE | ES_AUTOVSCROLL);

}

void QMessageBox::createOrShowEdit(WTL::CEdit & win, UINT id, std::wstring text, CRect rect, CRect &clientRect, DWORD exStyle, bool readOnly)
{
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		DWORD dwStyle = WS_CHILD | WS_VISIBLE |WS_TABSTOP ;
		if (exStyle) {
			dwStyle |= exStyle;
		}
		win.Create(m_hWnd, rect, text.c_str(), dwStyle , 0, id);
		HFONT hfont = GdiPlusUtil::GetHFONT(14, DEFAULT_CHARSET, true);
		win.SetFont(hfont);
		win.SetReadOnly(readOnly);
		win.SetWindowText(text.c_str());
		::DeleteObject(hfont);

		return;
	} else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
	}
}

void QMessageBox::createOrShowButtons(CRect & clientRect)
{
	int x = clientRect.left + 20, y = clientRect.bottom - 40 - 10, w = 110, h = 30;
	CRect rect(x, y, x + w, y + h);
	yesBtnText = yesBtnText.empty() ?  S(L"yes") : yesBtnText ;
	QWinCreater::createOrShowButton(m_hWnd, yesButton, Config::CUSTOMER_FORM_YES_BUTTON_ID, yesBtnText , rect, clientRect);

	rect.OffsetRect(w + 30, 0); // 向右移动
	noBtnText = noBtnText.empty() ?  noBtnText = S(L"no") : noBtnText ;
	QWinCreater::createOrShowButton(m_hWnd, noButton, Config::CUSTOMER_FORM_NO_BUTTON_ID, noBtnText, rect, clientRect);
}

LRESULT QMessageBox::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HINSTANCE ins = ModuleHelper::GetModuleInstance();

	winRect = QMessageBox::getInitRect();
	MoveWindow(winRect); //  注意，这个winRect必须赋值，不然不显示

	CRect clientRect;
	GetClientRect(clientRect);
	// 圆角窗口
	hRgn = ::CreateRoundRectRgn(0, 0, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, 20, 20);
	::SetWindowRgn(m_hWnd, hRgn, TRUE);

	// 置顶悬浮窗口
	SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); 

	bkgBrush.CreateSolidBrush(bkgColor);
	btnFont = GDI_PLUS_FT(L"setting-button-size");
	yesButton.SetFontColors(RGB(0,0,0), RGB(0x12,0x96,0xdb), RGB(153,153,153));
	yesButton.SetFont(btnFont);
	noButton.SetFontColors(RGB(0,0,0), RGB(0x12,0x96,0xdb), RGB(153,153,153));
	noButton.SetFont(btnFont);

	SetTimer(Config::MSG_TIMER_MOVE_ID, 10, NULL);
	return 0;
}

LRESULT QMessageBox::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (textFont) ::DeleteObject(textFont);
	if (!bkgBrush.IsNull()) bkgBrush.DeleteObject();
	if (btnFont) ::DeleteObject(btnFont);

	if (textEdit.IsWindow()) textEdit.DestroyWindow();
	if (yesButton.IsWindow()) yesButton.DestroyWindow();
	if (noButton.IsWindow()) noButton.DestroyWindow();
	if (hRgn) ::DeleteObject(hRgn);
	return 0;
}

LRESULT QMessageBox::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	createOrShowUI();
	return 0;
}


LRESULT QMessageBox::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{	
	CPaintDC dc(m_hWnd);
	CMemoryDC mdc(dc, dc.m_ps.rcPaint);
	mdc.FillRect(&(dc.m_ps.rcPaint), bkgBrush.m_hBrush);

	return 0;
}

/**
 * 定时器.
 * 
 * @param uMsg
 * @param wParam 计时器标识符 Config::MSG_TIMER_MOVE_ID
 * @param lParam 指向安装计时器时传递给 SetTimer 函数的应用程序定义的回调函数的指针。
 * @param bHandled
 * @return 
 */
LRESULT QMessageBox::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UINT msgId = static_cast<UINT>(wParam);
	if (msgId == Config::MSG_TIMER_MOVE_ID) {
		winRect.OffsetRect(0, 5);
		MoveWindow(winRect);
		if (winRect.top >= 30) {
			KillTimer(Config::MSG_TIMER_MOVE_ID); // 关闭移动			
		}
	}
	return 0;
}


HBRUSH QMessageBox::OnCtlStaticColor(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, bkgColor);
	::SetTextColor(hdc, textColor);
	::SelectObject(hdc, textFont);
	return bkgBrush.m_hBrush;
}

LRESULT QMessageBox::OnClickNoButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	EndDialog(Config::CUSTOMER_FORM_NO_BUTTON_ID);
	return 0;
}

LRESULT QMessageBox::OnClickYesButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	EndDialog(Config::CUSTOMER_FORM_YES_BUTTON_ID);
	return 0;
}

