// CuteSqlite.cpp : main source file for CuteSqlite.exe
//

#include "stdafx.h"

#include "resource.h"

#include "HomeView.h"
#include "aboutdlg.h"
#include "MainFrm.h"

CAppModule _Module;

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	CMainFrame wndMain;

	if(wndMain.CreateEx() == NULL)
	{
		ATLTRACE(_T("Main window creation failed!\n"));
		return 0;
	}

	wndMain.ShowWindow(nCmdShow);

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();
	return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	HRESULT hRes = ::CoInitialize(NULL);
	ATLASSERT(SUCCEEDED(hRes));

	AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls
	ULONG_PTR m_gdiplusToken;
	Gdiplus::GdiplusStartupInput  m_gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL);

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	AtlAxWinInit();

	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
	Gdiplus::GdiplusShutdown(m_gdiplusToken);
	::CoUninitialize();

	return nRet;
}
