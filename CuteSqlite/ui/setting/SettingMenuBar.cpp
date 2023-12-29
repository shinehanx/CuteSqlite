#include "stdafx.h"
#include "SettingMenuBar.h"
#include "utils/Log.h"
#include "core/common/Lang.h"

void SettingMenuBar::setup()
{

}

void SettingMenuBar::createOrShowUI()
{
	CRect clientRect;
	GetClientRect(&clientRect);

	// ���õĲ˵�
	createOrShowListMenu(listMenu, clientRect);
}

void SettingMenuBar::loadWindow()
{
	loadListMenu(); //���ز˵��б�
}


int SettingMenuBar::getSelItem()
{
	return listMenu.getSelItem();
}


void SettingMenuBar::setSelItem(int nItem)
{
	listMenu.setSelItem(nItem);
}

void SettingMenuBar::createOrShowListMenu(QListMenu & win, CRect &clientRect)
{
	CRect rect = {0, 40, clientRect.right , clientRect.bottom };
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		win.Create(m_hWnd, rect, L"", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN  | SS_OWNERDRAW , 0, Config::SETTING_MENU_LIST_ID);
		win.setItemHeight(50);
		win.setItemTriangleColor(RGB(118, 118, 118)); // �ұ������ε���ɫ
		createOrShowListMenuItem(win);
		win.setSelItem(0); //�ѵ�һ������ΪĬ��ѡ�е���
		return;
	} else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {		
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);
	}
}

void SettingMenuBar::createOrShowListMenuItem(QListMenu &win)
{
	win.addItem(S(L"general-settings"), Config::SETTING_GENERAL_SETTINGS_MENU_ID);
	win.addItem(S(L"about"), Config::SETTING_ABOUT_MENU_ID);
}

void SettingMenuBar::loadListMenu()
{
	CRect clientRect;
	GetClientRect(clientRect);

	listMenu.setSelItem(0); //�˵���ز�����0����²���
	createOrShowListMenu(listMenu, clientRect); //���²��� 
	listMenu.loadWindow();//����load
}

LRESULT SettingMenuBar::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bkgBrush = ::CreateSolidBrush(bkgColor);
	return 0;
}

LRESULT SettingMenuBar::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (bkgBrush) ::DeleteObject(bkgBrush);

	if (listMenu.IsWindow()) listMenu.DestroyWindow();
	return 0;
}

LRESULT SettingMenuBar::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	createOrShowUI();
	return 0;
}

LRESULT SettingMenuBar::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CPaintDC dc(m_hWnd);
	dc.FillRect(&(dc.m_ps.rcPaint), bkgBrush);
	return 0;
}

LRESULT SettingMenuBar::OnClickMenuItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	Q_DEBUG(L"CALL SettingMenuBar::OnClickMenuItem, nItem:{}", wParam);
	int nSelItem = static_cast<int>(wParam);
	if (nSelItem != getSelItem()) { // �������ȣ������ò˵����
		setSelItem(nSelItem);
	}

	// ʹ��·��1��QListMenu->SettingMenuBar->SettingPanel->HomeView->MainFrm ������ȡ��һ����SettingPanel
	::PostMessage(GetParent().m_hWnd, Config::MSG_MENU_ITEM_CLICK_ID, nSelItem, (LPARAM)nullptr);
	return 0;
}

