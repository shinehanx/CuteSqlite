// View.cpp : implementation of the CView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "HomeView.h"

BOOL HomeView::PreTranslateMessage(MSG* pMsg)
{
	if (databasePanel.IsWindow() && databasePanel.PreTranslateMessage(pMsg)) {
		return TRUE;
	}
	return FALSE;
}

void HomeView::createOrShowUI()
{
	CenterWindow();
	CRect clientRect;
	GetClientRect(&clientRect);
	createOrShowLeftPanel(clientRect);

	// create or show panel
	createOrShowPanel(Config::HOME_PANEL, (CWindowImpl &)homePanel, clientRect);
	createOrShowPanel(Config::DATABASE_PANEL, (CWindowImpl &)databasePanel, clientRect);

	std::wstring initPanel = SettingService::getInstance()->getSysInit(L"init-panel");
	int nPanel = initPanel.empty() ? Config::HOME_PANEL : std::stoi(initPanel);
	
	changePanelByPanelId(nPanel);
	
	return;
		
}

void HomeView::createOrShowLeftPanel(CRect &clientRect)
{
	CRect rect = { 0, 0, 64, 100 };

	//leftPanel ���
	if (::IsWindow(m_hWnd) && !leftPanel.IsWindow()) {
		leftPanel.Create(m_hWnd, rect, L"leftPanel", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
		return;
	}
	else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		rect = { 0, 0, 64, clientRect.bottom - clientRect.top };
		// ����ʾ��Ȼ���ƶ�
		leftPanel.ShowWindow(SW_SHOW);
		leftPanel.MoveWindow(&rect);
	}
}

void HomeView::createOrShowPanel(Config::PanelId panelId, CWindowImpl & panel, CRect &clientRect)
{
	CRect rect( 64, 0, clientRect.Width(), clientRect.Height() );
	// panel �ұ�
	if (::IsWindow(m_hWnd) && !panel.IsWindow()) {
		// ������ʾ/����
		DWORD dwStyle;
		if ((selectedButtonId == Config::UNSED_BUTTON_ID && panelId == Config::HOME_PANEL)
			|| (panelId == buttonPanelRelations[selectedButtonId])) {
			dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
		}
		else {
			dwStyle = WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
		}
		panel.Create(m_hWnd, rect, L"", dwStyle);
		//panel���õ�MAP�����ڿ�����ʾ/����
		panels[panelId] = (CWindowImpl *)&panel;
		return;
	}
	else if (::IsWindow(m_hWnd) && (clientRect.bottom - clientRect.top) > 0) {
		rect = { 64, 0, clientRect.Width(), clientRect.Height() };
		panel.MoveWindow(&rect);

		//������ʾ/����
		(selectedButtonId == Config::UNSED_BUTTON_ID && panelId == Config::HOME_PANEL)
			|| (panelId == selectedPannelId)  // selectedPannelId�������� selectedButtonId
											  //|| (panelId == buttonPanelRelations[selectedButtonId])  // selectedButtonId�ͺ���selectedPannelId
			? panel.ShowWindow(SW_SHOW) : panel.ShowWindow(SW_HIDE);

	}
}

ATL::CWindow * HomeView::changePanelByButtonId(UINT selButtonId)
{
	Config::PanelId panelId;
	Config::FrmButtonId oldButtonId = Config::UNSED_BUTTON_ID;
	// 1.����buttonPanelRelations<buttonId, panelId>��ϵ���ҳ�panelId���ٸ���panelId��λ����һ��panel
	for (auto item : buttonPanelRelations) {
		if (item.first == selButtonId) {
			panelId = item.second;
			selectedButtonId = static_cast<Config::FrmButtonId>(selButtonId); // ѡ��״̬
			oldButtonId = leftPanel.getSelButtonId();
			leftPanel.selectButtonId(selectedButtonId);
			break;
		}
	}

	

	CWindow * selPanel = nullptr;
	// 2.ʹ��panels<panelId, panels> ����panelId��λ��������һ��panel
	for (auto item : panels) {
		if (item.first == panelId) {
			item.second->ShowWindow(SW_SHOW);
			selectedPannelId = static_cast<Config::PanelId>(panelId); // ѡ��״̬
			selPanel = item.second;
			SettingService::getInstance()->setSysInit(L"init-panel",std::to_wstring(panelId));
		} else {
			item.second->ShowWindow(SW_HIDE);
		}
	}
	return selPanel;
}

ATL::CWindow * HomeView::changePanelByPanelId(UINT panelId)
{
	// 2.ʹ��panels<panelId, panels> ����panelId��λ��������һ��panel
	CWindow * selPanel = nullptr;
	for (auto item : panels) {
		if (item.first == panelId) {
			item.second->ShowWindow(SW_SHOW);
			selectedPannelId = static_cast<Config::PanelId>(panelId); // ѡ��״̬
			selPanel = item.second;
		}
		else {
			item.second->ShowWindow(SW_HIDE);
		}
	}

	// ��{buttonId,panelId}��ϵMAP����,�Ҳ���isFound=false
	bool isFound = false;
	for (auto item : buttonPanelRelations) {
		if (item.second == selectedPannelId) {
			leftPanel.selectButtonId(item.first);
			selectedButtonId = item.first;
			SettingService::getInstance()->setSysInit(L"init-panel",std::to_wstring(panelId));
			isFound = true;
			break;
		}
	}

	// ��ϵMAP���Ҳ���,���ֹ��ж�
	return selPanel;
}

LRESULT HomeView::OnCreate(UINT, WPARAM, LPARAM, BOOL &)
{
	//��ߵİ�ťID��panel��id��Ӧ��ϵ
	buttonPanelRelations[Config::HOME_BUTTON_ID] = Config::HOME_PANEL;
	buttonPanelRelations[Config::DATABASE_BUTTON_ID] = Config::DATABASE_PANEL;
	return 0;
}

LRESULT HomeView::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (leftPanel.IsWindow()) leftPanel.DestroyWindow();
	if (homePanel.IsWindow()) homePanel.DestroyWindow();
	if (databasePanel.IsWindow()) databasePanel.DestroyWindow();

	panels.clear();
	buttonPanelRelations.clear();

	return 0;
}

LRESULT HomeView::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	createOrShowUI();
	return 0;
}

LRESULT HomeView::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC dc(m_hWnd);
	dc.FillRect(&(dc.m_ps.rcPaint), RGB(225, 225, 255));
	//TODO: Add your drawing code here

	return 0;
}

LRESULT HomeView::OnClickLeftPanelButtons(UINT uMsg, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	std::wstringstream ss;
	ss << " CHomeView::OnChangePanel,uMsg:" << uMsg << endl;
	::OutputDebugStringW(ss.str().c_str());

	UINT selButtonId = (Config::FrmButtonId)uMsg;
	changePanelByButtonId(selButtonId);

	return 0;
}
