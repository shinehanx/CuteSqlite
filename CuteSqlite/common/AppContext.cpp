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
 * ��Ϣ�ַ�.
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
void AppContext::unsuscribe(HWND hwnd, UINT msgId)
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
