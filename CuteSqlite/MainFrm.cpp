// MainFrm.cpp : implmentation of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "HomeView.h"
#include "MainFrm.h"
#include "common/AppContext.h"
#include "ui/common/message/QMessageBox.h"
#include "ui/common/message/QPopAnimate.h"
#include "core/common/Lang.h"

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if (m_view.PreTranslateMessage(pMsg)) {
		return TRUE;
	}

	auto & popAnimatePtrs = AppContext::getInstance()->getPopAnimatePtrs();
	for (auto & ptr : popAnimatePtrs) {
		QPopAnimate * popAnimate = (QPopAnimate *)ptr;
		if (popAnimate && popAnimate->IsWindow() 
			&& popAnimate->PreTranslateMessage(pMsg)) {
			return true;
		}
	}


	// Here CFrameWindowImpl will handle the Ctrl+C/V/X key that be defined in the CuteSqlite.rc file
	// CuteSqlite.rc show in VC++ Developer View chain: Resource View - CuteSqlite - Accelerator - IDR_MAINFRAME
	// Define as :
	// 	IDR_MAINFRAME ACCELERATORS
	// 	BEGIN
	// 		"N",            ID_FILE_NEW,            VIRTKEY, CONTROL
	// 		"O",            ID_FILE_OPEN,           VIRTKEY, CONTROL
	// 		"S",            ID_FILE_SAVE,           VIRTKEY, CONTROL
	// 		"P",            ID_FILE_PRINT,          VIRTKEY, CONTROL
	// 		"Z",            ID_EDIT_UNDO,           VIRTKEY, CONTROL
	// 		"X",            ID_EDIT_CUT,            VIRTKEY, CONTROL
	// 		"C",            ID_EDIT_COPY,           VIRTKEY, CONTROL
	// 		"V",            ID_EDIT_PASTE,          VIRTKEY, CONTROL
	// 		VK_BACK,        ID_EDIT_UNDO,           VIRTKEY, ALT
	// 		VK_DELETE,      ID_EDIT_CUT,            VIRTKEY, SHIFT
	// 		VK_INSERT,      ID_EDIT_COPY,           VIRTKEY, CONTROL
	// 		VK_INSERT,      ID_EDIT_PASTE,          VIRTKEY, SHIFT
	// 		VK_F6,          ID_NEXT_PANE,           VIRTKEY 
	// 		VK_F6,          ID_PREV_PANE,           VIRTKEY, SHIFT
	// 	END
	if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
		return TRUE;

	return FALSE;
}

BOOL CMainFrame::OnIdle()
{
	return FALSE;
}

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// 这里去掉凹凸边框的扩展属性(第五个参数)
	m_hWndClient = m_view.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	AppContext::getInstance()->setMainFrmHwnd(m_hWnd);

	SetMenu(NULL);

	::DragAcceptFiles(m_hWnd, true);
	return 0;
}

LRESULT CMainFrame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	bHandled = FALSE;
	return 1;
}

LRESULT CMainFrame::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	LRESULT response = AppContext::getInstance()->dispatchForResponse(Config::MSG_DATA_HAS_CHANGED_ID, NULL, NULL);
	// Data has no changed
	if (response) {
		DestroyWindow();
		return 0;
	}
	
	// Data has changed
	if (QMessageBox::confirm(m_hWnd, S(L"data-has-changed"), S(L"not-save-and-close"), S(L"cancel"))
			== Config::CUSTOMER_FORM_YES_BUTTON_ID) {
		DestroyWindow();
		return 0;
	}

	return 0;
}

void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
	databaseSupplier->dragFilePaths.clear();
	UINT n = ::DragQueryFileW(hDropInfo, 0xFFFFFFFF, NULL, 0);
	for (UINT i = 0; i < n; i++) {
		UINT bufLen = ::DragQueryFileW(hDropInfo, i, NULL, 0) + 1;
		wchar_t * buf = new wchar_t[bufLen]();
		::DragQueryFileW(hDropInfo, i, buf, bufLen);
		databaseSupplier->dragFilePaths.push_back(buf);
		delete[] buf;
	}
	::DragFinish(hDropInfo);
	if (databaseSupplier->dragFilePaths.empty()) {
		return;
	}

	AppContext::getInstance()->dispatch(Config::MSG_DROP_FILES_ID);
}

