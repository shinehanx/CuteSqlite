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
}

/**
 * 消息分发.
 * 
 * @param msgId 消息ID
 * @param wParam 参数1
 * @param lParam 参数2
 */
void AppContext::dispatch(UINT msgId, WPARAM wParam, LPARAM lParam)
{
	msgDispatcher.dispatch(msgId, wParam, lParam);
}

/**
 * 消息订阅者.
 * 
 * @param msgId 消息ID
 */
void AppContext::subscribe(HWND hwnd, UINT msgId)
{
	msgDispatcher.subscribe(hwnd, msgId);
}

/**
 * 取消订阅.
 * 
 * @param hwnd 窗口句柄
 * @param msgId 消息ID
 */
void AppContext::unsuscribe(HWND hwnd, UINT msgId)
{
	msgDispatcher.unsuscribe(hwnd, msgId);
}

/**
 * 取消窗口的所有订阅.
 * 
 * @param hwnd 窗口句柄
 */
void AppContext::unsuscribeAll(HWND hwnd)
{
	msgDispatcher.unsuscribeAll(hwnd);
}
