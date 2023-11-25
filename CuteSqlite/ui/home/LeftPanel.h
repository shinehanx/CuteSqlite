#pragma once
#include <map>
#include <atltypes.h>
#include <utility>
#include <string>
#include <atlmisc.h>
#include <atlctrls.h>
#include <atlcrack.h>
#include <atlctrlx.h>
#include <GdiPlus.h>
#include "common/Config.h"
#include "common/AppContext.h"
#include "ui/common/button/QImageTextButton.h"
#include "ui/common/button/QImageButton.h"

class LeftPanel :public CWindowImpl<LeftPanel>
{
public:
	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP_EX(LeftPanel)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	void setup();

	void selectButtonId(Config::FrmButtonId buttonId);
	Config::FrmButtonId getSelButtonId();
private:

	//左边的按钮ID和按钮的对象指针对应关系
	std::map<Config::FrmButtonId, QImageTextButton *> buttonMap;

	QImageTextButton homeButton;
	QImageTextButton databaseButton;
	QImageTextButton analysisButton;
	QImageTextButton viewButton;
	QImageTextButton settingButton;
	
	// 选中的BUTTONID
	Config::FrmButtonId selButtonId = Config::HOME_BUTTON_ID;

	void createHomeButton();
	void createCustomerButton();
	void createAnalysisButton();
	void createViewButton();
	AppContext * appContext = AppContext::getInstance();
	void createSettingButton();

	virtual LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	virtual LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	virtual LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	virtual LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	virtual LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	virtual LRESULT OnChangeLang(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

	BOOL OnEraseBkgnd(CDCHandle dc);
};