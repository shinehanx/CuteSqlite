#include "stdafx.h"
#include "AboutView.h"
#include "core/common/Lang.h"
#include "ui/common/QWinCreater.h"
#include "utils/FileUtil.h"

void AboutView::setup()
{

}

void AboutView::createOrShowUI()
{
	CRect clientRect;
	GetClientRect(clientRect);

	createOrShowLicenseElems(clientRect);
	// createOrShowFeedBackElems(clientRect);
}


void AboutView::createOrShowLicenseElems(CRect &clientRect)
{
	int x = 40, y = 355 + 30 + 10, w = 500, h = 200;
	CRect editRect(x, y, x + w, y + h);
	
	std::wstring licenseText;
	if (!licenseEdit.IsWindow()) {
		std::wstring binDir = ResourceUtil::getProductBinDir();
		std::wstring licenseFilePath = binDir + L"\\res\\license.txt";
		licenseText = FileUtil::readFile(licenseFilePath);
		licenseText = StringUtil::replace(licenseText, L"\n", L"\r\n");
	}
	
	createOrShowEdit(licenseEdit, 0, licenseText, L"" , editRect, clientRect, ES_MULTILINE | ES_READONLY | WS_VSCROLL | ES_AUTOVSCROLL);
}

void AboutView::createOrShowFeedBackElems(CRect &clientRect)
{
	int x = 40, y = 355 + 30 + 40, w = 100, h = 30;
	CRect labelRect(x, y, x + w, y + h);
	CRect editRect(x, y, x + w, y + h);
	editRect.OffsetRect(w + 10, 0);
	editRect.InflateRect(0, 0, 300, 150);
	QWinCreater::createOrShowLabel(m_hWnd, contentLabel, S(L"content-text").append(L":"), labelRect, clientRect, SS_RIGHT);
	createOrShowEdit(contentEdit, Config::FEEDBACK_CONTENT_EDIT_ID, L"", L"Your feedback content", editRect, clientRect);

	labelRect.OffsetRect(0, h + 150 + 10);
	editRect.OffsetRect(0, h + 150 + 10);
	editRect.InflateRect(0, 0, 0, -155);
	QWinCreater::createOrShowLabel(m_hWnd, emailLabel, S(L"email-text").append(L":"), labelRect, clientRect, SS_RIGHT);
	createOrShowEdit(emailEdit, Config::FEEDBACK_EMAIL_EDIT_ID, L"", L"such as:alex@gmail.com", editRect, clientRect);

	CRect buttonRect( editRect.right - 120, editRect.bottom + 10, editRect.right, editRect.bottom + 10 + 35); 
	QWinCreater::createOrShowButton(m_hWnd, submitButton, Config::SETTING_FEEDBACK_SUBMIT_BUTTON_ID, S(L"submit"), buttonRect, clientRect, BS_OWNERDRAW);
}

/**
 * 子类实现这个方法，扩展画面内容.
 * 
 * @param dc 上下文DC
 * @param paintRect 窗口的范围
 */
void AboutView::paintItem(CDC &dc, CRect & paintRect)
{
	// 1.aboutus section
	paintSection(dc, CPoint(40, 45), paintRect, S(L"about-us").c_str(), S(L"about-us-description").c_str());

	// 2.version section 
	paintSection(dc, CPoint(40, 200), paintRect, S(L"version-info").c_str(), S(L"version-info-description").c_str());

	// 3.License
	paintSection(dc, CPoint(40, 355), paintRect, S(L"license").c_str(), L"");  

	// 4.FeedBack
	//paintSection(dc, CPoint(40, 355), paintRect, S(L"feedback").c_str(), S(L"feedback-description").c_str()); 
}

void AboutView::loadWindow()
{

}

LRESULT AboutView::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	QSettingView::OnCreate(uMsg, wParam, lParam, bHandled);	

	submitButton.SetFontColors(RGB(0,0,0), RGB(0x12,0x96,0xdb), RGB(153,153,153));
	submitButton.SetFont(buttonFont);
	return 0;
}

LRESULT AboutView::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	QSettingView::OnDestroy(uMsg, wParam, lParam, bHandled);

	if (contentLabel) contentLabel.DestroyWindow();
	if (emailLabel) emailLabel.DestroyWindow();
	if (contentEdit) contentEdit.DestroyWindow();
	if (emailEdit) emailEdit.DestroyWindow();
	if (submitButton) submitButton.DestroyWindow();
	return 0;
}

HBRUSH AboutView::OnCtlColorStatic(HDC hdc, HWND hwnd)
{
	if (hwnd == licenseEdit.m_hWnd) {
		return QSettingView::OnCtlColorEdit(hdc, hwnd);
	}

	return QSettingView::OnCtlColorStatic(hdc, hwnd);
}


