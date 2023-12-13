#include "stdafx.h"
#include "LeftPanel.h"
#include <atlmisc.h>
#include <sstream>
#include "utils/ResourceUtil.h"
#include "common/AppContext.h"
#include "core/common/Lang.h"

void LeftPanel::createHomeButton()
{
	CRect rect = { 0,0, 48, 48 };
	homeButton.setButtonId(Config::HOME_BUTTON_ID); 
	homeButton.setFont(FT(L"left-panel-size"));
	homeButton.Create(m_hWnd, rect, L"HomeButton", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

	//位置和图片
	CPoint point = { 6, 25 };
	std::vector<wstring> imagePathVector;
	wstring resDirPath = ResourceUtil::getProductBinDir();
	imagePathVector.push_back(resDirPath + L"res\\images\\home\\home-button-normal.png");
	imagePathVector.push_back(resDirPath + L"res\\images\\home\\home-button-pushed.png");
	imagePathVector.push_back(resDirPath + L"res\\images\\home\\home-button-hover.png");
	imagePathVector.push_back(resDirPath + L"res\\images\\home\\home-button-disabled.png");
	
	homeButton.setup(S(L"home"), point, imagePathVector);
}


void LeftPanel::createCustomerButton()
{
	CRect rect = { 0,0, 48, 48 };
	databaseButton.setButtonId(Config::DATABASE_BUTTON_ID);
	databaseButton.setFont(FT(L"left-panel-size"));
	databaseButton.Create(m_hWnd, rect, L"CustomerButton", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

	//位置和图片
	CPoint point = { 6, 35 + 52 + 25 };
	std::vector<wstring> imagePathVector;
	wstring resDirPath = ResourceUtil::getProductBinDir();
	imagePathVector.push_back(resDirPath + L"res\\images\\home\\database-button-normal.png");
	imagePathVector.push_back(resDirPath + L"res\\images\\home\\database-button-pushed.png");
	imagePathVector.push_back(resDirPath + L"res\\images\\home\\database-button-hover.png");
	imagePathVector.push_back(resDirPath + L"res\\images\\home\\database-button-disabled.png");
	databaseButton.setup(S(L"database"), point, imagePathVector); 
	
}

void LeftPanel::createAnalysisButton()
{
	CRect rect = { 0,0, 48, 48 };
	analysisButton.setButtonId(Config::ANALYSIS_BUTTON_ID);
	analysisButton.setFont(FT(L"left-panel-size"));
	analysisButton.Create(m_hWnd, rect, L"AnalysisButton", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

	//位置和图片
	CPoint point = { 6, 35 + (52 + 25)*2 };
	std::vector<wstring> imagePathVector;
	wstring resDirPath = ResourceUtil::getProductBinDir();
	imagePathVector.push_back(resDirPath + L"res\\images\\home\\analysis-button-normal.png");
	imagePathVector.push_back(resDirPath + L"res\\images\\home\\analysis-button-pushed.png");
	imagePathVector.push_back(resDirPath + L"res\\images\\home\\analysis-button-hover.png");
	imagePathVector.push_back(resDirPath + L"res\\images\\home\\analysis-button-disabled.png");
	analysisButton.setup(S(L"analysis"), point, imagePathVector);
}

void LeftPanel::createViewButton()
{
	CRect rect = { 0,0, 48, 48 };
	viewButton.setButtonId(Config::VIEW_BUTTON_ID);
	viewButton.setFont(FT(L"left-panel-size"));
	viewButton.Create(m_hWnd, rect, L"ViewButton", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

	//位置和图片
	CPoint point = { 6, 35 + (52 + 25)*3 };
	std::vector<wstring> imagePathVector;
	wstring resDirPath = ResourceUtil::getProductBinDir();
	imagePathVector.push_back(resDirPath + L"res\\images\\home\\view-button-normal.png");
	imagePathVector.push_back(resDirPath + L"res\\images\\home\\view-button-pushed.png");
	imagePathVector.push_back(resDirPath + L"res\\images\\home\\view-button-hover.png");
	imagePathVector.push_back(resDirPath + L"res\\images\\home\\view-button-disabled.png");
	viewButton.setup(S(L"view"), point, imagePathVector);
}

void LeftPanel::createSettingButton()
{
	CRect rect = { 0,0, 48, 48 };
	settingButton.setButtonId(Config::SETTING_BUTTON_ID);
	settingButton.setFont(FT(L"left-panel-size"));
	settingButton.Create(m_hWnd, rect, L"SettingButton", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

	CRect rectParent;
	GetClientRect(&rectParent);
	//位置和图片
	CPoint point = { 6, rectParent.Height() - (48*2 + 25)};
	std::vector<wstring> imagePathVector;
	wstring resDirPath = ResourceUtil::getProductBinDir();
	imagePathVector.push_back(resDirPath + L"res\\images\\home\\setting-button-normal.png");
	imagePathVector.push_back(resDirPath + L"res\\images\\home\\setting-button-pushed.png");
	imagePathVector.push_back(resDirPath + L"res\\images\\home\\setting-button-hover.png"); 
	imagePathVector.push_back(resDirPath + L"res\\images\\home\\setting-button-disabled.png");
	settingButton.setup(S(L"setting"), point, imagePathVector);
}


void LeftPanel::setup()
{
	
}

/**
 * 选中BUTTONID.
 * 
 * @param buttonId
 */
void LeftPanel::selectButtonId(Config::FrmButtonId buttonId)
{
	this->selButtonId = buttonId;

	for (auto item : buttonMap) {
		if (item.first == this->selButtonId) {
			item.second->setIsSelected(true);
		}else if (item.second->getIsSelected()) {
			// 原来选中，才更改，不然无需改变状态
			item.second->setIsSelected(false);
		}
	}
}


Config::FrmButtonId LeftPanel::getSelButtonId()
{
	return selButtonId;
}

LRESULT LeftPanel::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	bkgBrush.CreateSolidBrush(bkgColor);
	//创建home图形按钮
	createHomeButton();

	//customer按钮
	createCustomerButton();

	//analysis按钮
	createAnalysisButton();

	//view按钮
	// createViewButton();

	//setting按钮
	//createSettingButton();

	
	buttonMap[Config::HOME_BUTTON_ID] = &homeButton;		
	buttonMap[Config::DATABASE_BUTTON_ID] = &databaseButton;
	buttonMap[Config::ANALYSIS_BUTTON_ID] = &analysisButton;
	//buttonMap[Config::VIEW_BUTTON_ID] = &viewButton;
	//buttonMap[Config::SETTING_BUTTON_ID] = &settingButton;

	return 0;
}

LRESULT LeftPanel::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	if (!bkgBrush.IsNull()) bkgBrush.DeleteObject();
	if (homeButton.IsWindow()) homeButton.DestroyWindow();
	if (databaseButton.IsWindow()) databaseButton.DestroyWindow();
	if (analysisButton.IsWindow()) analysisButton.DestroyWindow();
	if (viewButton.IsWindow()) viewButton.DestroyWindow();
	if (settingButton.IsWindow()) settingButton.DestroyWindow();

	return 0;
}

LRESULT LeftPanel::OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CRect rectParent;
	GetClientRect(&rectParent);

	
	if (settingButton.IsWindow()) {
		CRect rect ;
		settingButton.GetClientRect(&rect);
		rect.MoveToXY(6, rectParent.Height() - (48 * 2 + 25));
		settingButton.MoveWindow(rect);
		settingButton.ShowWindow(SW_SHOW);
		settingButton.Invalidate(true);
		settingButton.UpdateWindow();
	}
	
	return 0;
}

LRESULT LeftPanel::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC dc(m_hWnd);
	CMemoryDC mdc(dc, dc.m_ps.rcPaint);
	mdc.FillRect(&dc.m_ps.rcPaint, bkgBrush.m_hBrush);
	return 0;
}


LRESULT LeftPanel::OnEraseBackground(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return TRUE;
}

/**
 * 更改语言.
 * 
 * @param 
 * @param 
 * @param 
 * @param 
 * @return 
 */
LRESULT LeftPanel::OnChangeLang(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	homeButton.setText(S(L"home"));
	databaseButton.setText(S(L"customer"));
	analysisButton.setText(S(L"analysis"));
	if (viewButton.IsWindow()) viewButton.setText(S(L"view"));
	if (settingButton.IsWindow()) settingButton.setText(S(L"setting"));
	
	return 0;
}

BOOL LeftPanel::OnEraseBkgnd(CDCHandle dc)
{
	return true; 
}


