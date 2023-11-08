#include "stdafx.h"
#include "MsgDispatcher.h"
#include <algorithm>
#include "utils/Log.h"

MsgDispatcher::MsgDispatcher()
{
	msgMap.clear();
}

MsgDispatcher::~MsgDispatcher()
{

}

/**
 * �ַ���Ϣ(�޷���).
 * 
 * @param msgId ��ϢID
 * @param wParam ����1
 * @param lParam ����2
 */
void MsgDispatcher::dispatch(UINT msgId, WPARAM wParam, LPARAM lParam)
{
	if (msgMap.empty() || msgMap.find(msgId) == msgMap.end()) {
		return ;
	}

	HWNDLIST & hwndList = msgMap.at(msgId);
	std::for_each(hwndList.begin(), hwndList.end(), [&msgId, &wParam, &lParam](HWND hwnd) {
		::PostMessage(hwnd, msgId, wParam, lParam);
	});
	
}

/**
 * �ַ���Ϣ(�з���).
 * 
 * @param msgId ��ϢID
 * @param wParam ����1
 * @param lParam ����2
 */
LRESULT MsgDispatcher::dispatchForResponse(UINT msgId, WPARAM wParam /*= NULL*/, LPARAM lParam /*= NULL*/)
{
	if (msgMap.empty() || msgMap.find(msgId) == msgMap.end()) {
		return 0;
	}

	HWNDLIST & hwndList = msgMap.at(msgId);
	LRESULT result = 0;
	std::for_each(hwndList.begin(), hwndList.end(), [&msgId, &wParam, &lParam, &result](HWND hwnd) {
		result = ::SendMessage(hwnd, msgId, wParam, lParam);
	});
	return result;
}

/**
 * ��Ϣ������.
 * 
 * @param msgId ��ϢID
 */
void MsgDispatcher::subscribe(HWND hwnd, UINT msgId)
{
	if (msgMap.empty() || msgMap.find(msgId) == msgMap.end()) {
		HWNDLIST hwndList = { hwnd };
		msgMap.insert(std::pair<UINT, HWNDLIST>(msgId, hwndList));
		return ;
	}
	
	HWNDLIST & hwndList = msgMap.at(msgId);
	if (hwndList.empty()) {
		hwndList.push_back(hwnd);
		return ;
	}

	// �����Ƿ��Ѵ���hwnd
	auto hwndIterator = std::find(hwndList.begin(), hwndList.end(), hwnd);
	if (hwndIterator == hwndList.end()) {
		hwndList.push_back(hwnd);
	}
}

/**
 * ȡ������.
 * 
 * @param hwnd ���ھ��
 * @param msgId ��ϢID
 */
void MsgDispatcher::unsuscribe(HWND hwnd, UINT msgId)
{
	if (msgMap.empty()) {
		return ;
	}

	auto iterator = msgMap.find(msgId);
	if (iterator == msgMap.end()) {
		return ;
	}

	HWNDLIST & hwndList = msgMap.at(msgId);
	if (hwndList.empty()) {
		return ;
	}

	// ɾ�����ڵĴ��ھ��
	hwndList.erase(std::remove_if(hwndList.begin(), hwndList.end(), [&hwnd](HWND _hwnd) -> bool {
		return hwnd == _hwnd;
	}), hwndList.end());
}

void MsgDispatcher::unsuscribeAll(HWND hwnd)
{
	if (msgMap.empty()) {
		return ;
	}

	// ����msgMap��Ȼ��˧ѡ������hwnd���ھ�����б���һɾ��
	for (auto iterator = msgMap.begin(); iterator != msgMap.end(); iterator++) {
		HWNDLIST & hwndList = (*iterator).second;
		if (hwndList.empty()) {
			return;
		}

		// ɾ�����ڵĴ��ھ��
		hwndList.erase(std::remove_if(hwndList.begin(), hwndList.end(), [&hwnd](HWND _hwnd) -> bool {
			return hwnd == _hwnd;
		}), hwndList.end());
	}
	Q_DEBUG(L"unsuscribeAll,msgMap.size:{}", msgMap.size());
}
