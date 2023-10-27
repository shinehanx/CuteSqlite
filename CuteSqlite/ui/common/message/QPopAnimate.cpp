#include "stdafx.h"
#include "QPopAnimate.h"
#include "core/common/Lang.h"
#include "utils/FontUtil.h"
#include "utils/ResourceUtil.h"
#include "ui/common/QWinCreater.h"
#include "common/AppContext.h"

void QPopAnimate::error(HWND parentHwnd, std::wstring & text)
{
	QPopAnimate * win = new QPopAnimate();
	win->setup(text);
	std::wstring imagePath = ResourceUtil::getProductImagesDir() + L"common\\message\\error.bmp";
	win->setImagePath(imagePath);
	win->Create(parentHwnd);
	win->ShowWindow(SW_SHOW);
}

void QPopAnimate::report(HWND parentHwnd, const std::wstring & code, const std::wstring & text)
{
	QPopAnimate * win = new QPopAnimate();
	win->setup(code, text);
	std::wstring imagePath = ResourceUtil::getProductImagesDir() + L"common\\message\\error.bmp";
	std::wstring closeImagePath = ResourceUtil::getProductImagesDir() + L"common\\message\\close.bmp";
	win->setImagePath(imagePath);
	win->setCloseImagePath(closeImagePath);
	win->Create(parentHwnd);
	win->ShowWindow(SW_SHOW);
}


void QPopAnimate::success(HWND parentHwnd, std::wstring & text)
{
	QPopAnimate * win = new QPopAnimate();
	win->setup(text);
	std::wstring imagePath = ResourceUtil::getProductImagesDir() + L"common\\message\\success.bmp";
	win->setImagePath(imagePath);
	win->Create(parentHwnd);
	win->ShowWindow(SW_SHOW);
}

void QPopAnimate::warn(HWND parentHwnd, std::wstring & text)
{
	QPopAnimate * win = new QPopAnimate();
	win->setup(text);
	std::wstring imagePath = ResourceUtil::getProductImagesDir() + L"common\\message\\warn.bmp";
	win->setImagePath(imagePath);
	win->Create(parentHwnd);
	win->ShowWindow(SW_SHOW);
}

void QPopAnimate::setup(const std::wstring & text)
{
	this->text = text;
	int textSize = Lang::fontSize(L"message-text-size");
	std::wstring fontName = Lang::fontName();
	// 计算文本大小
	CSize size = FontUtil::measureTextSize(text.c_str(), textSize, false, fontName.c_str());
	
	height = 10 + 10 + size.cy + 10 + 10;
	width = 10 + 20 + 5 + size.cx + 20 + 20;

	textFont = Lang::font(L"message-text-size", false);
}

void QPopAnimate::setup(const std::wstring & code, const std::wstring &text)
{
	this->code = code;
	this->text = text;
	int textSize = Lang::fontSize(L"message-text-size");
	
	popType = POP_REPORT_TEXT;
	width = 500;
	height = 250;

	textFont = Lang::font(L"message-text-size", false);
}

void QPopAnimate::setSize(int w, int h)
{
	width = w;
	height = h;
}

void QPopAnimate::createOrShowUI()
{
	CRect clientRect;
	GetClientRect(clientRect);
	createOrShowImage(clientRect);
	createOrShowCloseButton(clientRect);
	createOrShowTextEdit(clientRect);	
	image.SetFocus();
}

void QPopAnimate::setImagePath(const std::wstring & imagePath)
{
	this->imagePath = imagePath;
}

void QPopAnimate::setCloseImagePath(const std::wstring & closeImagePath)
{
	this->closeImagePath = closeImagePath;
}

CRect QPopAnimate::getInitRect()
{
	// 获得桌面的位置	
	CRect destopRect;
	HWND destopHwnd = ::GetDesktopWindow();
	::GetWindowRect(destopHwnd, destopRect);
	int x = (destopRect.Width() - width) / 2, y = destopRect.top - height, w = width, h = height;
	return { x, y, x + w, y + h };
}

void QPopAnimate::createOrShowImage(CRect & clientRect)
{
	CRect rect;
	if (popType == POP_REPORT_TEXT) {
		int x = clientRect.left + 10, y = 30, w = 20 , h = 20;
		rect = { x, y, x + w, y + h };
	} else {
		int x = clientRect.left + 10, y = (clientRect.Height() - 20) / 2, w = 20 , h = 20;
		rect = { x, y, x + w, y + h };
	}
	
	if (!imagePath.empty()) {
		image.load(imagePath.c_str(), BI_RGB);
	}
	image.setDisplayMode(QImage::kImageStretchFitTargetRect);
	QWinCreater::createOrShowImage(m_hWnd, image, 0, rect, clientRect);
}

void QPopAnimate::createOrShowCloseButton(CRect & clientRect)
{	
	if (popType == POP_COMMON_TEXT) {
		return;
	}

	int x = clientRect.right - 21, y = 5, w = 16 , h = 16;
	CRect rect = { x, y, x + w, y + h };
	
	std::wstring normalImagePath = ResourceUtil::getProductImagesDir() + L"common\\message\\close-button-normal.png";
	std::wstring pressedImagePath = ResourceUtil::getProductImagesDir() + L"common\\message\\close-button-pressed.png";
	closeButton.SetIconPath(normalImagePath, pressedImagePath);
	closeButton.SetBkgColors(RGB(238, 238, 238), RGB(238, 238, 238), RGB(238, 238, 238));
	QWinCreater::createOrShowButton(m_hWnd, closeButton, Config::QPOP_ANIMATE_CLOSE_BUTTON_ID, L"", rect, clientRect);
}

void QPopAnimate::createOrShowTextEdit(CRect & clientRect)
{
	CRect rect;
	if (popType == POP_REPORT_TEXT) {
		int x = clientRect.left + 10 + 20 + 5, 
		y = 30 , 
		w = clientRect.Width() - x - 20, 
		h = clientRect.Height() - x - 30;
		rect = { x, y, x + w, y + h};
	} else {
		int fontSize = Lang::fontSize(L"message-text-size");
		std::wstring fontName = Lang::fontName();
		CSize size = FontUtil::measureTextSize(text.c_str(), fontSize, false, fontName.c_str());

		int x = clientRect.left + 10 + 20 + 5, 
		y = (clientRect.Height() - fontSize) / 2 , 
		w = clientRect.Width() - x - 20, 
		h = size.cx % w ? (size.cx / w + 1) * size.cy : size.cx / w * size.cy;
		rect = { x, y, x + w, y + h + 10 };
	}
	
	std::wstring reportText = text;
	if (popType == POP_REPORT_TEXT) {
		reportText.append(L"\r\n\r\n").append(L"Code:").append(code);
	}
	
	QWinCreater::createOrShowEdit(m_hWnd, textEdit, 0, reportText, rect, clientRect, textFont, ES_MULTILINE | ES_AUTOVSCROLL , false);
	::SetWindowLong(textEdit.m_hWnd, GWL_STYLE, ::GetWindowLongW(textEdit.m_hWnd, GWL_STYLE) & ~WS_BORDER);
}

LRESULT QPopAnimate::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	winRect = getInitRect();
	MoveWindow(winRect); //  注意，这个winRect必须赋值，不然不显示

	CRect clientRect;
	GetClientRect(clientRect);
	// 圆角窗口
	HRGN hRgn = ::CreateRoundRectRgn(0, 0, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, 20, 20);
	::SetWindowRgn(m_hWnd, hRgn, TRUE);

	// 置顶悬浮窗口
	SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); 

	bkgBrush = ::CreateSolidBrush(bkgColor);

	SetTimer(Config::MSG_TIMER_MOVE_ID, 10, NULL);  
	return 0;
}

LRESULT QPopAnimate::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (textFont) ::DeleteObject(textFont);
	if (bkgBrush) ::DeleteObject(bkgBrush);

	if (image.IsWindow()) image.DestroyWindow();
	if (closeButton.IsWindow()) closeButton.DestroyWindow();
	if (textEdit.IsWindow()) textEdit.DestroyWindow();
	return 0;
}

LRESULT QPopAnimate::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	createOrShowUI();
	return 0;
}

LRESULT QPopAnimate::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{	
	CPaintDC dc(m_hWnd);
	CMemoryDC mdc(dc, dc.m_ps.rcPaint);
	mdc.FillRect(&(dc.m_ps.rcPaint), bkgBrush);

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
LRESULT QPopAnimate::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	UINT msgId = static_cast<UINT>(wParam);
	if (msgId == Config::MSG_TIMER_MOVE_ID) {
		winRect.OffsetRect(0, 5);
		MoveWindow(winRect);
		if (winRect.top >= 30) {
			KillTimer(Config::MSG_TIMER_MOVE_ID); // 关闭移动
			if (popType == POP_COMMON_TEXT) {
				SetTimer(Config::MSG_TIMER_DELAY_ID, 3000, nullptr); // 悬停3秒
			}			
		}
	} else if (msgId == Config::MSG_TIMER_DELAY_ID) {
		KillTimer(Config::MSG_TIMER_DELAY_ID);
		// 悬停结束，关闭销毁窗口
		::CloseWindow(m_hWnd);
		DestroyWindow();
		m_hWnd = nullptr;
	}
	return 0;
}

HBRUSH QPopAnimate::OnCtlEditColor(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, bkgColor);
	::SetTextColor(hdc, textColor);
	::SelectObject(hdc, textFont);
	return bkgBrush;
}

LRESULT QPopAnimate::OnClickCloseButton(UINT uNotifyCode, int nID, HWND hwnd)
{
	// 悬停结束，关闭销毁窗口
	::CloseWindow(m_hWnd);
	DestroyWindow();
	m_hWnd = nullptr;
	return 0;
}

