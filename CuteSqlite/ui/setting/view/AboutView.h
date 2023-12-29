/*****************************************************************//**
 * @file   AboutView.h
 * @brief  设置-关于页面
 * @detail    $DETAIL
 * 
 * @author 覃学涵
 * @date   2023-04-04
 *********************************************************************/
#pragma once
#include <atlcrack.h>
#include "ui/common/view/QSettingView.h"
#include "ui/common/button/QImageButton.h"

class AboutView : public QSettingView 
{
public:
	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP_EX(AboutView)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MSG_WM_CTLCOLORSTATIC(OnCtlColorStatic)
		CHAIN_MSG_MAP(QSettingView)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	// 三大金刚，都要声明
	virtual void setup();
	virtual void loadWindow();
private:
	CStatic licenseLabel;
	CEdit licenseEdit;

	CStatic contentLabel;
	CEdit contentEdit;
	QImageButton submitButton; 

	CStatic emailLabel;
	CEdit emailEdit;
	virtual void createOrShowUI();
	void createOrShowLicenseElems(CRect &clientRect);
	void createOrShowFeedBackElems(CRect &clientRect);
	virtual void paintItem(CDC &dc, CRect & paintRect);

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual HBRUSH  OnCtlColorStatic(HDC hdc, HWND hwnd);
};
