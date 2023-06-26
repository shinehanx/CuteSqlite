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

void QPopAnimate::setup(std::wstring & text)
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
	createOrShowTextEdit(clientRect);	
	image.SetFocus();
}

void QPopAnimate::setImagePath(std::wstring & imagePath)
{
	this->imagePath = imagePath;
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
	int x = clientRect.left + 10, y = y = (clientRect.Height() - 20) / 2, w = 20 , h = 20;
	CRect rect(x, y, x + w, y + h);
	if (!imagePath.empty()) {
		image.load(imagePath.c_str(), BI_RGB);
	}
	image.setDisplayMode(QImage::kImageStretchFitTargetRect);
	QWinCreater::createOrShowImage(m_hWnd, image, 0, rect, clientRect);
}

void QPopAnimate::createOrShowTextEdit(CRect & clientRect)
{
	int fontSize = Lang::fontSize(L"message-text-size");
	std::wstring fontName = Lang::fontName();
	CSize size = FontUtil::measureTextSize(text.c_str(), fontSize, false, fontName.c_str());
	int x = clientRect.left + 10 + 20 + 5, 
		y = (clientRect.Height() - fontSize) / 2 , 
		w = clientRect.Width() - x - 20, 
		h = size.cx % w ? (size.cx / w + 1) * size.cy : size.cx / w * size.cy;
	CRect rect(x, y, x + w, y + h);
	QWinCreater::createOrShowEdit(m_hWnd, textEdit, 0, text, rect, clientRect, textFont, ES_MULTILINE);
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
			SetTimer(Config::MSG_TIMER_DELAY_ID, 3000, nullptr); // 悬停3秒
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

