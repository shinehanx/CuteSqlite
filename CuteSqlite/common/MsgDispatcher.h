/*****************************************************************//**
 * @file   MsgDispatcher.h
 * @brief  消息调度器
 * @detail    $DETAIL
 * 
 * @author 覃学涵
 * @date   2023-04-02
 *********************************************************************/
#pragma once
#include <list>
#include <unordered_map>

typedef std::list<HWND> HWNDLIST; // 订阅消息的窗口列表

class MsgDispatcher
{
public:
	MsgDispatcher();
	~MsgDispatcher();

	// 消息分发(无返回)
	void dispatch(UINT msgId, WPARAM wParam = NULL, LPARAM lParam = NULL);
	// 消息分发(有返回)
	LRESULT dispatchForResponse(UINT msgId, WPARAM wParam = NULL, LPARAM lParam = NULL);

	// 消息订阅
	void subscribe(HWND hwnd, UINT msgId);
	void unsuscribe(HWND hwnd, UINT msgId);
	void unsuscribeAll(HWND hwnd);
private:
	/**
	 * 订阅消息的MAP,
	 * @param UINT msgId 消息的ID
	 * @param HWNDLIST hwnd list 订阅msgId消息的窗口列表
	 */
	std::unordered_map<UINT, HWNDLIST> msgMap; 
};
