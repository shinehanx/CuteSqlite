#include "stdafx.h"
#include "SettingPanel.h"
#include "core/common/Lang.h"

#define SETTING_MENU_BAR_WIDTH 250

void SettingPanel::setup()
{

}

void SettingPanel::createOrShowUI()
{
	CRect clientRect;
	GetClientRect(&clientRect);

	createOrShowMenuBar(menuBar, clientRect);
	// 右边的视图
	createOrShowViews(clientRect);
}

void SettingPanel::loadWindow()
{
	menuBar.setup();
	menuBar.loadWindow(); //重新刷新子窗口，重新加载数据

	// view loadwindow
	generalSettingsView.loadWindow();
	aboutView.loadWindow();

	int nSelItem = menuBar.getSelItem();
	for (int i = 0; i < (int)settingViews.size(); i++) {
		auto settingView = settingViews.at(i);
		
		if (i == nSelItem) {
			settingView->ShowWindow(SW_SHOW);
		}else {
			settingView->ShowWindow(SW_HIDE);			
		}
	}
}


void SettingPanel::createOrShowMenuBar(SettingMenuBar &win, CRect &clientRect)
{
	CRect rect = getLeftRect(clientRect);
	
	if (::IsWindow(m_hWnd) && !win.IsWindow()) {
		win.Create(m_hWnd, rect, L"", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS );
		return;
	} else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		win.MoveWindow(&rect);
		win.ShowWindow(SW_SHOW);		
	}
}


void SettingPanel::createOrShowViews(CRect &clientRect)
{
	CRect rightRect = getRightRect(clientRect);

	createOrShowView(&generalSettingsView, Config::SETTING_GENERAL_SETTINGS_VIEW_ID, rightRect, clientRect);
	createOrShowView(&aboutView, Config::SETTING_ABOUT_VIEW_ID, rightRect, clientRect);
}


void SettingPanel::createOrShowView(QSettingView * win, UINT id, CRect rect, CRect &clientRect, DWORD exStyle/*=0*/)
{
	int nSelItem = menuBar.getSelItem();
	UINT selId = settingViewIds[nSelItem];
	if (::IsWindow(m_hWnd) && !win->IsWindow()) {
		DWORD dwStyle;
		if (id == selId) {
			dwStyle = WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
		} else {
			dwStyle = WS_CHILD  | WS_CLIPCHILDREN | WS_CLIPSIBLINGS; // WS_VISIBLE不放进去，先不显示
		}		
		if (exStyle) dwStyle = dwStyle | exStyle;
		win->Create(m_hWnd, rect, L"", dwStyle, 0, id);
		return;
	} else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		win->MoveWindow(&rect);
		if (id == selId) {
			win->setIsNeedReload(true);
			win->ShowWindow(SW_SHOW);
		}else {
			win->ShowWindow(SW_HIDE);
		}
	}
}


CRect SettingPanel::getLeftRect(CRect & clientRect)
{
	return { 0, 0, SETTING_MENU_BAR_WIDTH, clientRect.Height() };
}


CRect SettingPanel::getRightRect(CRect & clientRect)
{
	CRect leftRect = getLeftRect(clientRect);
	return CRect(leftRect.right, 0, clientRect.right, clientRect.bottom);
}

LRESULT SettingPanel::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	leftBrush = ::CreateSolidBrush(leftColor);
	rightBrush = ::CreateSolidBrush(rightColor);

	settingViews[0] = (QSettingView *)&generalSettingsView;
	settingViews[1] = (QSettingView *)&aboutView; 	

	settingViewIds[0] = Config::SETTING_GENERAL_SETTINGS_VIEW_ID;
	settingViewIds[1] = Config::SETTING_ABOUT_VIEW_ID;
	

	return 0;
}

LRESULT SettingPanel::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	createOrShowUI();
	return 0;
}

LRESULT SettingPanel::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (leftBrush) ::DeleteObject(leftBrush);
	if (rightBrush) ::DeleteObject(rightBrush);

	if (generalSettingsView.IsWindow()) generalSettingsView.DestroyWindow();
	if (aboutView.IsWindow()) aboutView.DestroyWindow();

	settingViews.clear();
	settingViewIds.clear();

	return 0;
}

LRESULT SettingPanel::OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!wParam) {
		return 0;
	}
	loadWindow();
	return 0;
}

LRESULT SettingPanel::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CPaintDC dc(m_hWnd);
	CMemoryDC mdc(dc, dc.m_ps.rcPaint);
	CRect clientRect(dc.m_ps.rcPaint);
	CRect leftRect = getLeftRect(clientRect);
	mdc.FillRect(leftRect, leftBrush);
	CRect rightRect = getRightRect(clientRect);
	mdc.FillRect(rightRect, rightBrush);
	return 0;
}

LRESULT SettingPanel::OnClickMenuItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int nSelItem = static_cast<int>(wParam);

	// 切换右边视图，左边的视图不动
	CRect clientRect;
	GetClientRect(&clientRect);
	createOrShowViews(clientRect);

	return 0;
}

BOOL SettingPanel::OnEraseBkgnd(CDCHandle dc)
{
	return TRUE;
}

