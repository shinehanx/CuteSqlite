#pragma once


#include <atlwin.h>
#include <atltypes.h>
#include <utility>
#include <string>
#include <atlmisc.h>
#include <atlctrls.h>
#include <atlctrlx.h>
#include <GdiPlus.h>
#include <atlcrack.h>
#include <vector>
#include "common/Config.h"

using namespace std;

//定义
#define IMAGAE_TEXT_BUTTON_ID WM_USER + 1001

class QImageTextButton : public CWindowImpl<QImageTextButton> 
{

public:
	DECLARE_WND_CLASS(NULL)

	BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP_EX(LeftPanel)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		COMMAND_RANGE_CODE_HANDLER_EX(Config::HOME_BUTTON_ID, Config::SETTING_BUTTON_ID, BN_CLICKED, OnClickButton)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MSG_WM_CTLCOLORSTATIC(OnCtlColorStatic)
		MSG_WM_CTLCOLORBTN(OnCtlColorBtn)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()


	void setButtonId(Config::FrmButtonId buttonId);
	void setup(wstring & text, CPoint point, std::vector<wstring> & imagePathVector);
	void setText(std::wstring & text);
	void setFont(HFONT font);
	
	/**
	 * 是否选中.
	 */
	bool getIsSelected();	
	void setIsSelected(bool isSelected);
private:
	WTL::CBitmapButton button;
	WTL::CStatic label;
	WTL::CImageList imageList; // 未选中项图片列表
	WTL::CImageList selectedImageList;// 选中项列表

	HBITMAP normalBitmap = nullptr;
	HBITMAP pushedBitmap = nullptr;
	HBITMAP hoverBitmap = nullptr;
	HBITMAP disabledBitmap = nullptr;

	CRect buttonRect;
	CRect labelRect;
	CRect rect;

	CFont font;
	//CStatic背景画刷，用于OnCtlColorStatic响应方法使用
	CBrush ctlColorStaticBrush;

	Config::FrmButtonId buttonId = Config::UNSED_BUTTON_ID;

	bool isSelected = false;

	void createButton();
	void createLabel();
	// 透明窗体
	void transparent(HWND hwnd);

	void setupButton(std::vector<wstring> & imagePathVector, std::wstring & tooltip);
	void setupSelececImageList(std::vector<wstring> & imagePathVector);
	void setupLabel(std::wstring & text);

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	int OnDestroy();
	virtual LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	HBRUSH OnCtlColorStatic(HDC wParam, HWND lParam);
	HBRUSH OnCtlColorBtn(HDC wParam, HWND lParam);
	LRESULT OnClickButton(UINT /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/);

	BOOL OnEraseBkgnd(CDCHandle dc);
};
