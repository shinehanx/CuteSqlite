/*****************************************************************//**
 * @file   QPopAnimate.h
 * @brief  ������ʾ��
 * @detail    $DETAIL
 * 
 * @author ��ѧ��
 * @date   2023-04-23
 *********************************************************************/
#pragma once
#include <atlwin.h>
#include <atltypes.h>
#include <atlcrack.h>
#include <string>
#include "resource.h"
#include "core/common/exception/QRuntimeException.h"
#include "core/common/exception/QSqlExecuteException.h"
#include "ui/common/image/QStaticImage.h"
#include "ui/common/button/QImageButton.h"

#define QPOPANIMATE_WIDTH 300
#define QPOPANIMATE_HEIGHT 70
class QPopAnimate : public CDialogImpl<QPopAnimate> {
public:
	BOOL PreTranslateMessage(MSG* pMsg);
	enum { IDD = IDD_QFORM_DIALOG };

	BEGIN_MSG_MAP_EX(QPopAnimate)
		//MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MSG_WM_CTLCOLOREDIT(OnCtlEditColor)
		MSG_WM_CTLCOLORSTATIC(OnCtlEditColor)
		COMMAND_ID_HANDLER_EX(Config::CLOSE_IMAGE_BUTTON_ID, OnClickCloseButton)
		COMMAND_ID_HANDLER_EX(Config::QDIALOG_YES_BUTTON_ID, OnClickCloseButton)
		COMMAND_ID_HANDLER_EX(Config::QDIALOG_NO_BUTTON_ID, OnClickCloseButton)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	typedef enum  {
		POP_COMMON_TEXT,// common text text, close by timer
		POP_REPORT_TEXT, // report code/msg/[sql], close by user
	} PopType;

	static void error(HWND parentHwnd, std::wstring & text);
	static void error(std::wstring & text);

	static void success(HWND parentHwnd, std::wstring & text);
	static void success(std::wstring & text);

	static void warn(HWND parentHwnd, std::wstring & text);
	static void warn(std::wstring & text);

	static void report(HWND parentHwnd, const std::wstring & code, const std::wstring & text);
	static void report(HWND parentHwnd, const QRuntimeException & ex);
	static void report(const QRuntimeException & ex);
	static void report(const QSqlExecuteException & ex);
	
	void setup(const std::wstring & text);
	void setup(const std::wstring & code, const std::wstring &text, const std::wstring & sql = std::wstring(), bool rollBack = false);
	void setSize(int w, int h);
	void createOrShowUI();
	void setImagePath(const std::wstring & imagePath);
	void setCloseImagePath(const std::wstring & closeImagePath);
private:
	int width = QPOPANIMATE_WIDTH;
	int height = QPOPANIMATE_HEIGHT;
	CRect winRect;
	CRect destopRect;

	HRGN hRgn = nullptr;

	PopType popType = POP_COMMON_TEXT;
	std::wstring code;
	std::wstring text;
	std::wstring sql;
	bool rollBack = false;
	std::wstring imagePath;
	std::wstring closeImagePath;

	COLORREF textColor = RGB(8, 8, 8);
	COLORREF bkgColor = RGB(201, 211, 216);
	CBrush bkgBrush;
	HFONT textFont = nullptr;

	HACCEL m_hAccel = nullptr;

	QStaticImage image;
	QImageButton closeButton;
	CEdit textEdit;

	void createOrShowImage(CRect & clientRect);
	void createOrShowCloseButton(CRect & clientRect);
	void createOrShowTextEdit(CRect & clientRect);
	void createOrShowEdit(WTL::CEdit & win, UINT id, std::wstring text, CRect rect, CRect &clientRect, DWORD exStyle = 0);

	CRect getInitRect();
	LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnForwardMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	HBRUSH OnCtlEditColor(HDC hdc, HWND hwnd);
	LRESULT OnClickCloseButton(UINT uNotifyCode, int nID, HWND hwnd);
};