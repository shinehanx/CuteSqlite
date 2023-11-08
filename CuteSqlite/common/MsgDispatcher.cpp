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
 * 分发消息(无返回).
 * 
 * @param msgId 消息ID
 * @param wParam 参数1
 * @param lParam 参数2
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
 * 分发消息(有返回).
 * 
 * @param msgId 消息ID
 * @param wParam 参数1
 * @param lParam 参数2
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
 * 消息订阅者.
 * 
 * @param msgId 消息ID
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

	// 查找是否已存在hwnd
	auto hwndIterator = std::find(hwndList.begin(), hwndList.end(), hwnd);
	if (hwndIterator == hwndList.end()) {
		hwndList.push_back(hwnd);
	}
}

/**
 * 取消订阅.
 * 
 * @param hwnd 窗口句柄
 * @param msgId 消息ID
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

	// 删除存在的窗口句柄
	hwndList.erase(std::remove_if(hwndList.begin(), hwndList.end(), [&hwnd](HWND _hwnd) -> bool {
		return hwnd == _hwnd;
	}), hwndList.end());
}

void MsgDispatcher::unsuscribeAll(HWND hwnd)
{
	if (msgMap.empty()) {
		return ;
	}

	// 遍历msgMap，然后帅选出含有hwnd窗口句柄的列表，逐一删除
	for (auto iterator = msgMap.begin(); iterator != msgMap.end(); iterator++) {
		HWNDLIST & hwndList = (*iterator).second;
		if (hwndList.empty()) {
			return;
		}

		// 删除存在的窗口句柄
		hwndList.erase(std::remove_if(hwndList.begin(), hwndList.end(), [&hwnd](HWND _hwnd) -> bool {
			return hwnd == _hwnd;
		}), hwndList.end());
	}
	Q_DEBUG(L"unsuscribeAll,msgMap.size:{}", msgMap.size());
}
