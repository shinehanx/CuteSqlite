#include "stdafx.h"
#include "AppContext.h"
#include <sstream>
AppContext * AppContext::theInstance = nullptr;

AppContext * AppContext::getInstance()
{
	if (AppContext::theInstance == nullptr) {
		AppContext::theInstance = new AppContext();
	}

	return AppContext::theInstance;
}


void AppContext::setMainFrmHwnd(HWND hwnd)
{
	this->mainFrmHwnd = hwnd;
}

HWND AppContext::getMainFrmHwnd()
{
	return mainFrmHwnd;
}

std::wstring AppContext::getMainFrmCaption()
{
	wchar_t cch[1024] = {0};
	HWND hWnd = ::GetForegroundWindow();
	::GetWindowTextW(hWnd, cch, 1024);
	std::wstring text(cch);
	return text;
}

void AppContext::appendMainFrmCaption(const std::wstring & text)
{
	if (text.empty()) {
		return;
	}
	std::wstring caption = L"CuteSqlite - ";
	caption.append(text);
	HWND hWnd = ::GetForegroundWindow();
	::SetWindowTextW(hWnd, caption.c_str());
}

std::wstring AppContext::get(const std::wstring k)
{
	return settings.at(k);
}


int AppContext::getInt(std::wstring k)
{
	std::wstring str = settings.at(k);
	if (str.empty()) {
		return 0;
	}
	int v = std::stoi(str.c_str());
	return v;
}

void AppContext::set(const std::wstring k, const std::wstring v)
{
	settings[k] = v;
}


void AppContext::set(std::wstring k, int v)
{
	std::wstringstream ss; 
	ss << v;
	settings[k] = ss.str();
}

AppContext::AppContext()
{

}

AppContext::~AppContext()
{
	if (AppContext::theInstance) {
		delete AppContext::theInstance;
	}

	clearPopAnimatePtrs();
	clearDialogPtrs();

}

/**
 * ��Ϣ�ַ����޷��أ�.
 * 
 * @param msgId ��ϢID
 * @param wParam ����1
 * @param lParam ����2
 */
void AppContext::dispatch(UINT msgId, WPARAM wParam, LPARAM lParam)
{
	msgDispatcher.dispatch(msgId, wParam, lParam);
}

/**
 * ��Ϣ�ַ����з��أ��ȴ���ɣ�.
 * 
 * @param msgId ��ϢID
 * @param wParam ����1
 * @param lParam ����2
 */
LRESULT AppContext::dispatchForResponse(UINT msgId, WPARAM wParam /*= NULL*/, LPARAM lParam /*= NULL*/)
{
	return msgDispatcher.dispatchForResponse(msgId, wParam, lParam);
}

/**
 * ��Ϣ������.
 * 
 * @param msgId ��ϢID
 */
void AppContext::subscribe(HWND hwnd, UINT msgId)
{
	msgDispatcher.subscribe(hwnd, msgId);
}

/**
 * ȡ������.
 * 
 * @param hwnd ���ھ��
 * @param msgId ��ϢID
 */
void AppContext::unsubscribe(HWND hwnd, UINT msgId)
{
	msgDispatcher.unsuscribe(hwnd, msgId);
}

/**
 * ȡ�����ڵ����ж���.
 * 
 * @param hwnd ���ھ��
 */
void AppContext::unsuscribeAll(HWND hwnd)
{
	msgDispatcher.unsuscribeAll(hwnd);
}

std::vector<CWindow *> & AppContext::getPopAnimatePtrs()
{
	return popAnimatePtrs;
}

void AppContext::addPopAnimatePtr(CWindow * ptr)
{
	popAnimatePtrs.push_back(ptr);
}

void AppContext::erasePopAnimatePtr(CWindow * ptr)
{
	auto iter = popAnimatePtrs.begin();
	for (; iter != popAnimatePtrs.end(); iter++) {
		if ((*iter)->m_hWnd != ptr->m_hWnd) {
			continue;
		}
		popAnimatePtrs.erase(iter);
		break;
	}
}

void AppContext::clearPopAnimatePtrs()
{
	for (auto & ptr : popAnimatePtrs) {
		if (ptr && ptr->IsWindow()) {
			::CloseWindow(ptr->m_hWnd);
			ptr->DestroyWindow();
		}
	}
	popAnimatePtrs.clear();
}

std::vector<CWindow *> & AppContext::getDialogPtrs()
{
	return dialogPtrs;
}

void AppContext::addDialogPtr(CWindow * ptr)
{
	dialogPtrs.push_back(ptr);
}

void AppContext::eraseDialogPtr(CWindow * ptr)
{
	auto iter = dialogPtrs.begin();
	for (; iter != dialogPtrs.end(); iter++) {
		if ((*iter)->m_hWnd != ptr->m_hWnd) {
			continue;
		}
		dialogPtrs.erase(iter);
		break;
	}
}

void AppContext::clearDialogPtrs()
{
	dialogPtrs.clear();
}
