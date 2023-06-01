#include "stdafx.h"
#include "ImageTextButton.h"
#include <string>
#include <atltypes.h>
#include <sstream>
#include "utils/ResourceUtil.h"

void ImageTextButton::setButtonId(Config::FrmButtonId btnId)
{
	this->buttonId = btnId;
}

void ImageTextButton::createButton()
{	
	ATLASSERT(buttonId != Config::UNSED_BUTTON_ID);
	button.Create(m_hWnd, 0, L"HomeButton", WS_CHILD | WS_VISIBLE , 0, buttonId);	

	std::wstringstream ss;
	ss << "ImageTextButton::OnClickButton,id:" << buttonId;
	OutputDebugStringW(ss.str().c_str());
}

void ImageTextButton::createLabel()
{
	label.Create(m_hWnd, 0, L"HomeButton", WS_CHILD | WS_VISIBLE , 0);
}

//������͸��
void ImageTextButton::transparent(HWND hwnd)
{
	LONG nExStyle = ::GetWindowLong(hwnd, GWL_EXSTYLE);
	nExStyle |= WS_EX_LAYERED;
	::SetWindowLong(hwnd, GWL_EXSTYLE, nExStyle); 
	SetLayeredWindowAttributes(hwnd, RGB(1,2,3), 128, LWA_COLORKEY);
}

/**
 * ��ʼ��ͼ�ΰ�ť
 * @param imagePathVector ͼƬ·��
 */
void ImageTextButton::setupButton(std::vector<wstring> & imagePathVector,std::wstring & tooltip)
{
	button.SetBitmapButtonExtendedStyle(BMPBTN_HOVER, BMPBTN_HOVER);

	Gdiplus::Bitmap  normalImage(imagePathVector[0].c_str());
	Gdiplus::Bitmap  pushedImage(imagePathVector[1].c_str());
	Gdiplus::Bitmap  hoverImage(imagePathVector[2].c_str());
	Gdiplus::Bitmap  disabledImage(imagePathVector[3].c_str());

	Gdiplus::Color color(RGB(0xff, 0xff, 0xff));	
	normalImage.GetHBITMAP(color, &normalBitmap);	
	pushedImage.GetHBITMAP(color, &pushedBitmap);	
	hoverImage.GetHBITMAP(color, &hoverBitmap);	
	disabledImage.GetHBITMAP(color, &disabledBitmap);

	if (imageList.IsNull()) {
		imageList.Create(normalImage.GetWidth(),normalImage.GetHeight(), ILC_COLOR32, 0, 4);
		imageList.Add(normalBitmap);
		imageList.Add(pushedBitmap);
		imageList.Add(hoverBitmap);
		imageList.Add(disabledBitmap);
	}

	// ѡ����
	if (selectedImageList.IsNull()) {
		selectedImageList.Create(pushedImage.GetWidth(),pushedImage.GetHeight(), ILC_COLOR32, 0, 4);
		selectedImageList.Add(pushedBitmap);
		selectedImageList.Add(pushedBitmap);
		selectedImageList.Add(pushedBitmap);
		selectedImageList.Add(pushedBitmap);
	}
	
	button.SetToolTipText(tooltip.c_str());
	button.SetImageList(imageList);
	// 0-nNormal, 1- nPushed, 2-nFocusOrHover, 3-nDisabled
	button.SetImages(0, 1, 2, 3);	
	button.SizeToImage();

	button.GetClientRect(&buttonRect);
	GetClientRect(&rect);
	
	// ��Ծ�����ʾ
	buttonRect.MoveToXY((rect.Width() - buttonRect.Width())/ 2, 1);
	button.MoveWindow(buttonRect, FALSE);
	// SetBkColor(::GetDC(button.m_hWnd), RGB(0xff, 0xff, 0xff));

	std::wstringstream dd;
	dd << "ImageTextButton rect:w=" <<  rect.Width()  <<  ",h="  <<  rect.Height()  
		<<  ",CBitmapButton rect:w=" <<  buttonRect.Width() <<  ",h="  <<  buttonRect.Height() << endl ;
	::OutputDebugStringW(dd.str().c_str());

}

void ImageTextButton::setupLabel(wstring & text)
{
	
	CClientDC dc(label.m_hWnd);
	dc.SelectFont(font);

	// 1.CStatic ����
	CSize size_control;
	label.SetWindowText(text.c_str());
	label.SetFont(font);
	

	auto length = label.GetWindowTextLength();
	wchar_t* buf = new wchar_t[MAX_PATH]();
	label.GetWindowText(buf, length+1);
	dc.GetTextExtent(buf, static_cast<int>(wcslen(buf)), &size_control);


	label.GetClientRect(&labelRect);
	GetClientRect(&rect);

	// ����ˮƽ��������2����,����̫ӵ��.
	size_control.cx+=2;
	CRect rect_control(CPoint((rect.Width() - size_control.cx)/ 2, buttonRect.Height() + buttonRect.top + 4), size_control);
	label.MoveWindow(rect_control);

	delete buf;
}

BOOL ImageTextButton::PreTranslateMessage(MSG* pMsg)
{
	return FALSE;
}

void ImageTextButton::setText(std::wstring & text)
{
	label.SetWindowText(text.c_str());
}

void ImageTextButton::setFont(HFONT theFont)
{
	if (!font.IsNull()) {
		font.DeleteObject();
	}
	font = theFont;
}

bool ImageTextButton::getIsSelected()
{
	return isSelected;
}

/**
 * ѡ�е�ǰID.
 * 
 * @param isSelected
 */
void ImageTextButton::setIsSelected(bool isSelected)
{
	this->isSelected = isSelected;
	if (isSelected) {
		button.SetImageList(selectedImageList);		
	} else {
		button.SetImageList(imageList);	
	}

	// 0-nNormal, 1- nPushed, 2-nFocusOrHover, 3-nDisabled
	button.SetImages(0, 1, 2, 3);	
	button.SizeToImage();
	button.Invalidate(false);
}


/**
 * ��ʼ��
 * @param text ��ť�·��ı�
 * @param point ������ImageTextButton�ĳ�ʼλ��
 * @param imagePathVector ͼƬ�б�(�±꣺0-nNormal, 1- nPushed, 2-nFocusOrHover, 3-nDisabled)
 * @param id ��ť��ID������¼���
 */
void ImageTextButton::setup(wstring & text, CPoint point, std::vector<wstring> & imagePathVector)
{
	CRect rect = {0, 0, 48, 52};
	rect.MoveToXY(point);
	MoveWindow(rect);

	//��ʼ��δѡ�еİ�ť
	setupButton(imagePathVector, text);

	//��ʼ���ı�
	setupLabel(text);
}

int ImageTextButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	createButton();
	createLabel();

	//
	transparent(m_hWnd);

	//static������ˢ
	ctlColorStaticBrush = ::CreateSolidBrush(RGB(0xff, 0xff, 0xff));

	return TRUE;
}

int ImageTextButton::OnDestroy()
{
	if (font) DeleteObject(font);
	if (ctlColorStaticBrush) DeleteObject(ctlColorStaticBrush);

	// Gdiplus::DllExports::GdipDisposeImage(normalBitmap);
	if (normalBitmap) DeleteObject(normalBitmap);
	if (pushedBitmap) DeleteObject(pushedBitmap);
	if (hoverBitmap) DeleteObject(hoverBitmap);
	if (disabledBitmap) DeleteObject(disabledBitmap);

	if (button.IsWindow()) button.DestroyWindow();
	if (label.IsWindow()) label.DestroyWindow();

	if (!imageList.IsNull() && imageList.GetImageCount() > 0) {
		imageList.RemoveAll();
	}

	if (!selectedImageList.IsNull() && selectedImageList.GetImageCount() > 0) {
		selectedImageList.RemoveAll();
	}
	

	return true;
}

LRESULT ImageTextButton::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC dc(m_hWnd);
	RECT rect;
	GetClientRect(&rect);
	
	//HBRUSH brush = AtlGetStockBrush(BLACK_BRUSH);
	HBRUSH brush = ::CreateSolidBrush(RGB(0xff, 0xff, 0xff));
	dc.FillRect(&dc.m_ps.rcPaint, brush);
	::DeleteObject(brush);
	return 0;
}


HBRUSH ImageTextButton::OnCtlColorStatic(HDC hdc, HWND hwnd)
{
	if(hwnd == label.m_hWnd){
		::SetTextColor(hdc, RGB(0x2c,0x2c,0x2c)); //�ı�����ǰ��ɫ
		//::SetBkColor(hdc, RGB(0,255,0)); // �ı����򱳾�ɫ
		::SetBkColor(hdc, RGB(0xff,0xff,0xff)); // �ı����򱳾�ɫ	
		return ctlColorStaticBrush; // ����CStatic��Client���򱳾�ɫ
	}

	return ctlColorStaticBrush;
}

HBRUSH ImageTextButton::OnCtlColorBtn(HDC hdc, HWND hwnd)
{
	::SetBkColor(hdc, RGB(0xff,0xff,0xff)); // �ı����򱳾�ɫ
	return ctlColorStaticBrush; // ����CStatic��Client���򱳾�ɫ
}

LRESULT ImageTextButton::OnClickButton(UINT /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/)
{
	std::wstringstream ss;
	ss << "ImageTextButton::OnClickButton,buttonId:" << buttonId << ", param wID:" << wID << endl;
	OutputDebugStringW(ss.str().c_str());

	//ImageTextButton->LeftPanel->HomeView->MainFrm,����ȡHomeView������Ϣ������ִ��OnClickLeftPanelButtons��Ϣ
	HWND hwndHomeView = GetParent().GetParent().m_hWnd;
	::PostMessage(hwndHomeView, buttonId, (WPARAM)L"hello",(LPARAM)L"world");
	return 0;
}

BOOL ImageTextButton::OnEraseBkgnd(CDCHandle dc)
{
	return true;
}
